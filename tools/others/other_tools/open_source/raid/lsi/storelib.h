
/****************************************************************
 * FILE        : storelib.h
 * DESCRIPTION : StoreLib provides an abstraction to interact with "New Firmware Interface" controllers.
 *********************************************************************************
 *  Copyright (c) 2014-2016 Avago Technologies. All rights reserved
 *********************************************************************************/


#ifndef __STORELIB_H__
#define __STORELIB_H__

#include "types.h"      //data type definitions
#include "mr.h"         //firmware headers
#include "event.h"      //firmware event info
#include "slevent.h"    // IR firmware / Warhawk specific event info
#include "mr_drv.h"    //driver headers
#include "mfistat.h"    //firmware status
#include "slerrors.h"   //storelib error definitions
#include "smp.h"        // SMP PASSTHRU definitions

#pragma pack(1)
/****************************************************************************
*
*        GLOBAL CONSTANTS
*
****************************************************************************/
#ifdef MCTP_PCIE_INTERFACE
	#define SL_MAX_CONTROLLERS		8
#else
	#define SL_MAX_CONTROLLERS		64          
#endif

#ifndef MAX_LOGICAL_DRIVES_LIMIT_64
	#define MAX_LOGICAL_DRIVES		MAX_API_LOGICAL_DRIVES_EXT
	#define MAX_ARRAYS			    MAX_API_ARRAYS_EXT
	#define MAX_TARGET_ID           MAX_API_TARGET_ID_EXT
#endif

#define SL_MAX_OS_DRIVE_COUNT      300

/****************************************************************************
*
*        VD/LD DDF States
*
****************************************************************************/
enum SL_VD_STATE { VD_OPTIMAL=0, VD_DEGRADED, VD_DELETED, VD_MISSING, VD_FAILED, VD_STATE_MASK=0x07 };
enum SL_VD_STATE_MORPHING { VD_MORPHING_ON=0x08, VD_MORPHING_OFF = 0x00, VD_MORPHING_MASK=0x08 };
enum SL_VD_STATE_CONSISTENT { VD_NOT_CONSISTENT = 0x10, VD_CONSISTENT=0x00,  VD_CONSISTENT_MASK=0x10 };

/****************************************************************************
*
*        VD/LD RAID Levels
*
****************************************************************************/
enum SL_VD_RAID_LEVEL { VD_RAID0=0x00, VD_RAID1=0x01, VD_RAID5=0x05, VD_JBOD=0x0F, VD_RAID1E=0x11, VD_RAID6=0x06 };

/****************************************************************************
*
*        MFI TRANSFER DIRECTION FLAGS
*
****************************************************************************/
typedef enum _SL_DIR
{
    SL_DIR_NONE        = 0,            /* no data transfer */
    SL_DIR_WRITE       = 1,            /* transfer is from host */
    SL_DIR_READ        = 2,            /* transfer is to host  */
    SL_DIR_BOTH        = 3,            /* transfer is both read and write, or unknown */
} SL_DIR;

/****************************************************************************
*
*        FLASH TYPE
*
****************************************************************************/
typedef enum _SL_FLASH_TYPE
{
    SL_FLASH_TYPE_FW            = 0,            /* Firmware Flash Image type */
    SL_FLASH_TYPE_BIOS          = 1,            /* BIOS Flash Image type. Not applicable for MegaRAID SAS ctrl  */
    SL_FLASH_TYPE_FCODE         = 2,            /* SPARK BIOS Flash Image type. Not applicable for MegaRAID SAS ctrl  */
    SL_FLASH_TYPE_EFI_BIOS      = 3,            /* EFI BIOS Flash Image type. Not applicable for MegaRAID SAS ctrl  */
    SL_FLASH_TYPE_COMPLETE      = 4,
    SL_FLASH_TYPE_CPLD		    = 5,			/*CPLD flash image type*/
	SL_FLASH_TYPE_COMBINED_BIOS = 6 
} SL_FLASH_TYPE;



/****************************************************************************
*
*        
*
****************************************************************************/
typedef enum _SL_FW_FLASH_MASK
{
	SL_FW_FLASH_MASK_NO_CHECK		= 0x01,		/* Force not to do some validations */
	SL_FW_FLASH_MASK_ERASE_CONFIG	= 0x02,		/* perform ToolBox Clean and preserve ManPage 0 and 1*/
	SL_FW_FLASH_MASK_ERASE_ALL		= 0x04,		/* perform ToolBox Clean */

}SL_FW_FLASH_MASK;

/****************************************************************************
*
*        COMMAND TYPE DEFINITION
*
****************************************************************************/
enum SL_CMD_TYPE
{
    SL_SYSTEM_CMD_TYPE   = 0,
    SL_CTRL_CMD_TYPE     = 1,
    SL_PD_CMD_TYPE       = 2,
    SL_LD_CMD_TYPE       = 3,
    SL_CONFIG_CMD_TYPE   = 4,
    SL_BBU_CMD_TYPE      = 5,
    SL_PASSTHRU_CMD_TYPE = 6,
    SL_EVENT_CMD_TYPE    = 7,
    SL_ENCL_CMD_TYPE     = 8,
    SL_OOB_CMD_TYPE       = 9,
    SL_WH_CMD_TYPE      = 10, //OOB
    SL_CCOH_CMD_TYPE     = 11,         //CCOH commands
    SL_NYTRO_CMD_TYPE    = 12,         //Nytro commands
    //@@@@@ Always add new commands here @@@@@//

    SL_MAX_CMD_TYPE = 0xFF,
    //@@@@@ Do NOT add new cmd type below!!@@@@@//
};


/****************************************************************************
*
*        COMMAND DEFINITION
*
****************************************************************************/
/****************************************************************************
*
* NOTE: Error codes which are specific to each command are mentioned below.
* System related commands *across* controllers
*
****************************************************************************/
/****************************************************************************
*
*        SYSTEM COMMANDS (ACROSS CONTROLLERS)
*
****************************************************************************/
enum SL_SYSTEM_CMD
{
   /*
    * SL_INIT_LIB
    * Desc:
    *   1. Discovers controllers and creates a list of controllers found
    *   2. Creates Mutexes/Semaphores
    *   3. Initializes internal data structures
    *   4. In case of OOB, apps may fill in SL_CTRL_LIST_T with the controller location details
    *      Storelib will check if controller is responding on the specified addresses and update
    *      SL_CTRL_LIST_T accordingly. Apps need to check SL_CTRL_LIST once SL_INIT_LIB returns to 
    *      get list of controllers.
    * Input:
    *   pData: Pointer to SL_CTRL_LIST_T struct to accept controller details
    *   dataSize: SL_CTRL_LIST_S
    * Output:
    *   pData: SL_CTRL_LIST_T
    *   dataSize: SL_CTRL_LIST_S
    * StoreLib Error Codes:
    *    SL_ERR_ACCESS_DENIED
    *    SL_ERR_INIT_FAILED
    *    SL_ERR_INVALID_INPUT_PARAMETER
    */
    SL_INIT_LIB = 0,

   /*
    * SL_REGISTER_AEN
    * Desc:
    *   Registers for receiving Asynchronous Event Notification (AEN). In case of OOB storelib will poll 
    *   for events over i2c link. Poll interval is specified in LIB_PARAMETERS and can be changed via SL_SET_LIB_PARAMETERS
    * Input:
    *   pCmdParam: Pointer to SL_REG_AEN_INPUT_T struct
    *   pData: Pointer to SL_REG_AEN_OUTPUT_T struct
    *   dataSize: SL_REG_AEN_OUTPUT_S
    * Output:
    *   pData: SL_REG_AEN_OUTPUT_T
    *   dataSize: SL_REG_AEN_OUTPUT_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INIT_NOT_DONE
    *   SL_ERR_COM_INIT_FAILED
    *   SL_ERR_WMI_INIT_FAILED
    *   SL_ERR_INVALID_EVENT_MASK
    *   SL_ERR_WMI_REGISTRATION_FAILED
    *   SL_ERR_CMD_TIMEOUT
    *   SL_ERR_MEMORY_ALLOC_FAILED
    */
    SL_REGISTER_AEN,

   /*
    * SL_UNREGISTER_AEN
    * Desc:
    *   Function used to unregister from receiving AEN.
    * Input:
    *   cmdParam_4b[0]: Unique registration identifier that was returned by RegisterAEN
    *   pData: None
    * Output:
    *   pData: None
    * StoreLib Error Codes:
    *   SL_ERR_WMI_UNREGISTRATION_FAILED
    */
    SL_UNREGISTER_AEN,

   /*
    * SL_GET_CTRL_LIST
    * Desc:
    *   Gets number of "MFI-Aware" controllers in the system
    * Input:
    *   pData: Pointer to SL_CTRL_LIST_T struct to accept controller details. 
	*          For OOB apps need to set certain fields. Refer to SL_CTRL_LIST_T data struct definition
    *   dataSize: SL_CTRL_LIST_S
    * Output:
    *   pData: SL_CTRL_LIST_T
    *   dataSize: SL_CTRL_LIST_S
    * StoreLib Error Codes:
    *   None
    */
    SL_GET_CTRL_LIST,

    /*
    * SL_GET_LIB_VER
    * Desc:
    *   Gets version information of StoreLib
    * Input:
    *   pData: Pointer to buffer of type SL_LIB_VER_T to accept StoreLib version info
    *   dataSize: SL_LIB_VER_S
    * Output:
    *   pData: SL_LIB_VER_T
    *   dataSize: SL_LIB_VER_S
    * StoreLib Error Codes:
    *   None
    */
    SL_GET_LIB_VER,

   /*
    * SL_GET_SYSTEM_HEALTH
    * Desc:
    *   Gets health status for all controllers present within the system
    * Input:
    *   pData: Pointer to buffer of type SL_SYSTEM_HEALTH_T
    *   dataSize: SL_SYSTEM_HEALTH_S
    * Output:
    *   pData: SL_SYSTEM_HEALTH_T
    *   dataSize: SL_SYSTEM_HEALTH_S
    * StoreLib Error Codes:
    *   None
    */
    SL_GET_SYSTEM_HEALTH,

   /*
    * SL_GET_LIB_PARAMETERS
    * Desc:
    *   Gets StoreLib parameters at run time. Parameters include
    *   debug information.
    * Input:
    *   pData: Pointer to buffer of type SL_LIB_PARAMETERS_T
    *   dataSize: SL_LIB_PARAMETERS_S
    * Output:
    *   pData: SL_GET_LIB_PARAMETERS
    *   dataSize: SL_LIB_PARAMETERS_S
    * StoreLib Error Codes:
    *   None
    */
    SL_GET_LIB_PARAMETERS,

   /*
    * SL_SET_LIB_PARAMETERS
    * Desc:
    *   Allows to set StoreLib parameters at run time. Parameters include
    *   debug information.
    * Input:
    *   pData: Pointer to buffer of type SL_LIB_PARAMETERS_T
    *   dataSize: SL_LIB_PARAMETERS_S
    * Output:
    *   pData: SL_LIB_PARAMETERS_T
    *   dataSize: SL_LIB_PARAMETERS_S
    * StoreLib Error Codes:
    *   None
    */
    SL_SET_LIB_PARAMETERS,


   /*
    * SL_GET_PARTITION_INFO
    * Desc:
    *   Gets the partition information for the specified mapped O/S drive
    * Input:
    *   ctrlId: Controller Id
    *   cmdParam_1b[0]: OS drive number (osDriveNumber from SL_DRIVE_DISTRIBUTION_T, returned by SL_LD_DISTRIBUTION/SL_PD_DISTRIBUTION cmd(s))
    *   cmdParam_1b[1]: Logical drive number corresponding to the OS drive number above
    *   pData: Pointer to SL_PARTITION_INFO_T data structure
    *   dataSize: Minimum size is SL_MIN_PART_SIZE.
    * Output:
    *   pData: SL_PARTITION_INFO_T
    *   dataSize: same as input dataSize
    * StoreLib Error Codes:
    *   SL_ERR_GET_PARTITION_FAILED
    *   SL_ERR_LINUX_DEVICE_OPEN_FAILED
    */
    SL_GET_PARTITION_INFO,

   

   /*
    * SL_EXIT_LIB
    * Desc:
    *   Cleans up O/S specific resources that was allocated during SL_INIT_LIB.
    *   Applications must call this cmd before exiting to enable library to free
    *   these resources.
    * Input:
    *   None
    * Output:
    *   None
    * StoreLib Error Codes:
    *   None
    */
    SL_EXIT_LIB,

   /*
    * SL_GET_VIRT_CTRL_LIST
    * Desc:
    *   Gets the List of "Virtual" controllers in the system
    * Input:
    *   pData: Pointer to SL_CTRL_LIST_T struct to accept controller details
    *   dataSize: SL_CTRL_LIST_S
    * Output:
    *   pData: SL_CTRL_LIST_T
    *   dataSize: SL_CTRL_LIST_S
    * StoreLib Error Codes:
    *   SL_ERR_CCOH_NOT_INITIALIZED
    *   SL_ERR_INCORRECT_DATA_SIZE
    */
    SL_GET_VIRT_CTRL_LIST,
	
	
	/*
    * SL_GET_PARTITION_INFO_EXT
    * Desc:
    *   Gets the partition information for the specified mapped O/S drive
    * Input:
    *   ctrlId: Controller Id
    *   cmdParam_2b[0]: OS drive number (osDriveNumber from SL_DRIVE_DISTRIBUTION_T, returned by SL_LD_DISTRIBUTION/SL_PD_DISTRIBUTION cmd(s))
    *   cmdParam_2b[1]: Logical drive number corresponding to the OS drive number above
    *   pData: Pointer to SL_PARTITION_INFO_T data structure
    *   dataSize: Minimum size is SL_MIN_PART_SIZE.
    * Output:
    *   pData: SL_PARTITION_INFO_T
    *   dataSize: same as input dataSize
    * StoreLib Error Codes:
    *   SL_ERR_GET_PARTITION_FAILED
    *   SL_ERR_LINUX_DEVICE_OPEN_FAILED
    */
	
    SL_GET_PARTITION_INFO_EXT,

	SL_GET_LIB_VER_EXT,


    //@@@@@ Always add new commands at the end @@@@@//
};

/****************************************************************************
*
*        CONTROLLER COMMANDS
*
****************************************************************************/
enum SL_CTRL_CMD
{
   /*
    * SL_GET_CTRL_INFO
    * Desc:
    *   Gets controller information.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_CTRL_INFO type to accept data
    *   dataSize: sizeof(MR_CTRL_INFO)
    * Output:
    *   pData: MR_CTRL_INFO
    *   dataSize: sizeof(MR_CTRL_INFO)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_CTRL_INFO = 0,

   /*
    * SL_GET_CTRL_PROPERTIES
    * Desc:
    *   Gets controller properties.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_CTRL_PROP struct to accept data
    *   dataSize: sizeof(MR_CTRL_PROP)
    * Output:
    *   pData: MR_CTRL_PROP
    *   dataSize: sizeof(MR_CTRL_PROP)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_CTRL_PROPERTIES,

   /*
    * SL_SET_CTRL_PROPERTIES
    * Desc:
    *   Sets controller properties.
    * Input:
    *   ctrlId: Controller Id
    *   genRef.ref_2b[0]: Controller sequence number
    *   pData: Pointer to buffer of MR_CTRL_PROP struct with data to be set
    *   dataSize: sizeof(MR_CTRL_PROP)
    * Output:
    *   None
    * StoreLib Error Codes:
    *    SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_SET_CTRL_PROPERTIES,

   /*
    * SL_FLASH_CTRL_FIRMWARE
    * Desc:
    *   Flashes controller firmware.
    * Input:
    *   ctrlId: Controller Id
    *   cmdParam_1b[0]: SL_FLASH_TYPE - type of image that is being supported.
    *   pData: Pointer to buffer containing binary image in Host/OOB 
    *
    *             In EFI: This will contain buffer of type SL_FW_DOWNLOAD_T
    *    
    *             ImageType;      // Type of Image to be flashed
    *             TotalImageSize; // Total Size of the firmware Image 
    *             ImageSize;      // Size of the region to be flashed
    *             ImageOffset;    // Offset to be read from the buffer passed in pData, Mostly ZERO yet will change based on the changes in boot loader.
    *             *pData;         // Pointer to buffer to send/receive data
    *   dataSize: size of binary image
    *   Flashing Raw FW, BIOS and FCode is not supported for WarpDrive.
    *   Application can do it forcefully by setting following parameters
    *   cmdParam_1b[5]: 0x0A
    *   cmdParam_1b[6]: 0x0F
    *   cmdParam_1b[7]: 0x0D
	*   Also for WarpDrive FW or FW Package flash some of the validation 
	*   will be bypassed if Apps set following:
	*   cmdParam_1b[4]: SL_FW_FLASH_MASK_NO_CHECK.
	*   ToolBox Clean command is issued and ManPage 0 and 1 shall be preserved
	*    if Apps set following:
	*   cmdParam_1b[4]: SL_FW_FLASH_MASK_ERASE_CONFIG.
	*   ToolBox Clean command is issued if Apps set following:
	*   cmdParam_1b[4]: SL_FW_FLASH_MASK_ERASE_CONFIG.
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_FW_FLASH_IMAGE
    * AVAGO SAS IT/IR Controllers: Flashing BIOS as a separate image is supported for this ctrl
    */
    SL_FLASH_CTRL_FIRMWARE,

   /*
    * SL_GET_PD_LIST
    * Desc:
    *   Gets the list of physical devices in the controller.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_PD_LIST struct to accept PD list
    *   dataSize: sizeof(MR_PD_LIST)
    * Output:
    *   pData: MR_PD_LIST
    *   dataSize: sizeof(MR_PD_LIST)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_PD_LIST,

    /*
    * SL_GET_ENCL_LIST
    * Desc:
    *   Gets the list of enclosures and details for all enclosures
    *   connected to the controller.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_ENCL_LIST struct to accept enclosure list
    *   dataSize: sizeof(MR_ENCL_LIST)
    * Output:
    *   pData: MR_ENCL_LIST
    *   dataSize: sizeof(MR_ENCL_LIST)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_ENCL_LIST,

   /*
    * SL_GET_TOPOLOGY_INFO
    * Desc:
    *   Gets the topology information.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer to accept topology info
    *   dataSize: SL_TOPOLOGY_BUFFER_SIZE
    * Output:
    *   pData: Topology info data structure
    *   dataSize: Actual size of topology data buffer
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * Note: The Buffer returned has the Topology Data structure, if command succeeds.
    *   The topology Data structure Starts with the SL_TOPOLOGY_HEAD_NODE_T type data structure.
    *   The tree can be traversed from here using the device offsets in the data structure.
    *   Refer to Storelib API Doc and StorelibTest for more information.
    */
    SL_GET_TOPOLOGY_INFO,

   /*
    * SL_GET_ALARM
    * Desc:
    *   Gets current alarm status
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of U8 type to accept alarm status
    *   dataSize: sizeof(U8)
    * Output:
    *   pData: Alarm status
    *   dataSize: sizeof(U8)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_ALARM,

   /*
    * SL_ENABLE_ALARM
    * Desc:
    *   Enables alarm
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_ENABLE_ALARM,

   /*
    * SL_DISABLE_ALARM
    * Desc:
    *   Disables alarm
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_DISABLE_ALARM,

   /*
    * SL_SILENCE_ALARM
    * Desc:
    *   Silences alarm
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_SILENCE_ALARM,


   /*
    * SL_GET_PCI_INFO
    * Desc:
    *   Gets the PCI information. Under Linux this cmd is supported only for kernel   2.5 or greater.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_DRV_PCI_INFORMATION type to accept PCI Info
    *   dataSize: MR_DRV_PCI_INFORMATION_SIZE
    * Output:
    *   pData: MR_DRV_PCI_INFORMATION
    *   dataSize: MR_DRV_PCI_INFORMATION_SIZE
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD - returned in Linux if kernel version is older than 2.5
    * OOB : SL_ERR_INVALID_CMD
    */

    SL_GET_PCI_INFO,

   /*
    * SL_LD_DISTRIBUTION
    * Desc:
    *   Gets the O/S drive map information
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_DRIVE_DISTRIBUTION_T type
    *   dataSize: SL_DRIVE_DISTRIBUTION_S
    * Output:
    *   pData: SL_DRIVE_DISTRIBUTION_T data
    *   dataSize: SL_DRIVE_DISTRIBUTION_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * OOB : SL_ERR_INVALID_CMD
    */
    SL_LD_DISTRIBUTION,

   /*
    * SL_GET_LD_LIST
    * Desc:
    *   Gets list of logical drives
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_LD_LIST to accept LD list
    *   dataSize: sizeof(MR_LD_LIST)
    * Output:
    *   pData: MR_LD_LIST
    *   dataSize: sizeof(MR_LD_LIST)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_LD_LIST,

    //Patrol Read Commands
   /*
    * SL_GET_PATROL_READ_STATUS_INFO
    * Desc:
    *   Gets Patrol Read Status Information
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_PR_STATUS struct
    *   dataSize: sizeof(MR_PR_STATUS)
    * Output:
    *   pData: MR_PR_STATUS
    *   dataSize: sizeof(MR_PR_STATUS)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_PATROL_READ_STATUS_INFO,

   /*
    * SL_GET_PATROL_READ_PROPERTIES
    * Desc:
    *   Gets Patrol Read Properties
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_PR_PROPERTIES struct
    *   dataSize: sizeof(MR_PR_PROPERTIES)
    * Output:
    *   pData: MR_PR_PROPERTIES
    *   dataSize: sizeof(MR_PR_PROPERTIES)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_PATROL_READ_PROPERTIES,

   /*
    * SL_SET_PATROL_READ_PROPERTIES
    * Desc:
    *   Sets Patrol Read Properties
    * Input:
    *   ctrlId: Controller Id
    *   genRef.ref_2b[0]: Controller Sequence Number
    *   pData: Pointer to buffer of MR_PR_PROPERTIES struct with data to be set
    *   dataSize: sizeof(MR_PR_PROPERTIES)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_SET_PATROL_READ_PROPERTIES,

   /*
    * SL_START_PATROL_READ_OPERATION
    * Desc:
    *   Starts Patrol Read Operation
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_START_PATROL_READ_OPERATION,

   /*
    * SL_STOP_PATROL_READ_OPERATION
    * Desc:
    *   Stops Patrol Read Operation
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_STOP_PATROL_READ_OPERATION,

    /*
    * SL_GET_DRIVER_VER
    * Desc:
    *   Gets version of driver installed in the system
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of type MR_DRV_DRIVER_VERSION to accept driver version info
    *   dataSize: sizeof(MR_DRV_DRIVER_VERSION)
    * Output:
    *   pData: MR_DRV_DRIVER_VERSION
    *   dataSize: sizeof(MR_DRV_DRIVER_VERSION)
    * StoreLib Error Codes:
    *   None
    */
    SL_GET_DRIVER_VER,

   /*
    * SL_GET_CTRL_HEALTH
    * Desc:
    *   Gets health status for the specified controller
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of type SL_CTRL_HEALTH_T
    *   dataSize: SL_CTRL_HEALTH_S
    * Output:
    *   pData: SL_CTRL_HEALTH_T
    *   dataSize: SL_CTRL_HEALTH_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_CTRL_HEALTH,

   /*
    * SL_PD_DISTRIBUTION
    * Desc:
    *   Gets the O/S drive map information for SCSI devices exposed to O/S
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_DRIVE_DISTRIBUTION_T type
    *   dataSize: SL_DRIVE_DISTRIBUTION_S
    * Output:
    *   pData: SL_DRIVE_DISTRIBUTION_T data
    *   dataSize: SL_DRIVE_DISTRIBUTION_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * MegaRAID SAS Controllers: SL_ERR_INVALID_CMD
    * OOB : SL_ERR_INVALID_CMD
    */
    SL_PD_DISTRIBUTION,

   /*
    * SL_CTRL_SELF_CHECK_DIAGNOSTIC
    * Desc:
        Run the self-check diagnostics for the given number of seconds using the supplied buffer.
    * Input:
    *   ctrlId: Controller Id
    *   cmdParam_4b[0]: Time in Seconds
    *   pData: None
    *   dataSize: 0
    * Output:
    *   pData: None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_CTRL_SELF_CHECK_DIAGNOSTIC,

   /*
    * SL_CTRL_START_BATTERY_RETENTION_TEST
    * Desc:
    *   Start a battery retention test
    * Input:
    *   ctrlId: Controller Id
    *   pData: None
    *   dataSize: 0
    * Output:
    *   pData: None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
//    SL_CTRL_START_BATTERY_RETENTION_TEST,

   /*
    * SL_GET_ALL_LDS_ALLOWED_OPERATION
    * Desc:
    *   Gets the allowed operation (init, cc, etc) on all the lds present in the ctrl
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to SL_ALL_LDS_ALLOWED_OPER_T data structure
    *   dataSize: SL_ALL_LDS_ALLOWED_OPER_S
    * Output:
    *   pData: SL_ALL_LDS_ALLOWED_OPER_T
    *   dataSize: SL_ALL_LDS_ALLOWED_OPER_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_ALL_LDS_ALLOWED_OPERATION,

   /*
    * SL_LD_DISTRIBUTION_EX
    * Desc:
    *  Gets the O/S drive map information. This commands returns OS name upto 255 bytes
    * Input:
    *  ctrlId: Controller Id
    *  pData: Pointer to buffer of SL_DRIVE_DISTRIBUTION_EX_T type
    *  dataSize: SL_DRIVE_DISTRIBUTION_EX_S
    * Output:
    *  pData: SL_DRIVE_DISTRIBUTION_EX_T data
    *  dataSize: SL_DRIVE_DISTRIBUTION_EX_S
    * StoreLib Error Codes:
    *  SL_ERR_INVALID_CTRL
    * OOB : SL_ERR_INVALID_CMD
    */
    SL_LD_DISTRIBUTION_EX,
   /*
    *  *SL_PD_DISTRIBUTION_EX
    * Desc:
    *  Gets the O/S drive map information for SCSI devices exposed to O/S. This commands returns OS name upto 255 bytes
    * Input:
    *  ctrlId: Controller Id
    *  pData: Pointer to buffer of SL_DRIVE_DISTRIBUTION_EX_T type
    *  dataSize: SL_DRIVE_DISTRIBUTION_EX_S
    * Output:
    *  pData: SL_DRIVE_DISTRIBUTION_EX_T data
    *  dataSize: SL_DRIVE_DISTRIBUTION_EX_S
    * StoreLib Error Codes:
    *  SL_ERR_INVALID_CTRL
    * MegaRAID SAS Controllers: SL_ERR_INVALID_CMD
    * OOB : SL_ERR_INVALID_CMD
    */
    SL_PD_DISTRIBUTION_EX,

    /*
    * SL_RESET_CONTROLLER
    * Desc:
    *   Resets the controller
    * Input:
    *   ctrlId: Controller Id
    * Output:
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_RESET_CONTROLLER,

   /*
    * SL_DIAGNOSTIC_BUFFER_QUERY
    * Desc:
    *   The controller will be queried for information about the Diagnostic
    *   Buffer.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_DIAGNOSTIC_BUFFER_QUERY_T
    *   dataSize: SL_DIAGNOSTIC_BUFFER_QUERY_S
    * Output:
    *   pData: SL_DIAGNOSTIC_BUFFER_QUERY_T
    *   dataSize: SL_DIAGNOSTIC_BUFFER_QUERY_S
    * StoreLib Error Codes:
    *    SL_SUCCESS
    *    SL_ERR_CMD_FAILED_BY_OS
    *    SL_ERR_CMD_TIMEDOUT
    *    SL_ERR_MEMORY_ALLOC_FAILED
    *    SL_ERR_DIAG_BUFFER_INVALID_BUFFER_TYPE
    */
    SL_DIAGNOSTIC_BUFFER_QUERY,

   /*
    * SL_DIAGNOSTIC_BUFFER_REGISTER
    * Desc:
    *   Register a Diagnostic Buffer. For now the only usage of this command is
    *   to pass in a new set of ProductSpecific information, since the buffer
    *   will have been allocated by the OS Drivers at system startup
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_DIAGNOSTIC_BUFFER_REGISTER_T
    *   dataSize: SL_DIAGNOSTIC_BUFFER_REGISTER_S
    * Output:
    *   pData: SL_DIAGNOSTIC_BUFFER_REGISTER_T
    *   dataSize: SL_DIAGNOSTIC_BUFFER_REGISTER_S
    * StoreLib Error Codes:
    *    SL_SUCCESS
    *    SL_ERR_DIAG_BUFFER_INVALID_PARAM
    *    SL_ERR_CMD_FAILED_BY_OS
    *    SL_ERR_CMD_TIMEDOUT
    *    SL_ERR_MEMORY_ALLOC_FAILED
    *    SL_ERR_DIAG_BUFFER_INVALID_BUFFER_TYPE
    */
    SL_DIAGNOSTIC_BUFFER_REGISTER,

   /*
    * SL_DIAGNOSTIC_BUFFER_READ
    * Desc:
    *   Specified diagnostic buffer will be extracted
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_DIAGNOSTIC_BUFFER_READ_T
    *   dataSize: SL_DIAGNOSTIC_BUFFER_READ_S
    * Output:
    *   pData: SL_DIAGNOSTIC_BUFFER_READ_T
    *   dataSize: SL_DIAGNOSTIC_BUFFER_READ_S
    * StoreLib Error Codes:
    *    SL_SUCCESS
    *    SL_ERR_CMD_FAILED_BY_OS
    *    SL_ERR_CMD_TIMEDOUT
    *    SL_ERR_MEMORY_ALLOC_FAILED
    *    SL_ERR_DIAG_BUFFER_INVALID_BUFFER_TYPE
    */
    SL_DIAGNOSTIC_BUFFER_READ,

   /*
    * SL_DIAGNOSTIC_BUFFER_UNREGISTER
    * Desc:
    *   Requests that the OS driver clean up (free up if possible) any memory
    *   allocated for the Diagnostic Buffer and free up any associated resources.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_DIAGNOSTIC_BUFFER_UNREGISTER_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_UNREGISTER_S
    * Output:
    *   pData: SL_DIAGNOSTIC_BUFFER_UNREGISTER_T
    *   dataSize: SL_DIAGNOSTIC_BUFFER_UNREGISTER_S
    * StoreLib Error Codes:
    *    SL_SUCCESS
    *    SL_ERR_CMD_FAILED_BY_OS
    *    SL_ERR_CMD_TIMEDOUT
    *    SL_ERR_MEMORY_ALLOC_FAILED
    *    SL_ERR_DIAG_BUFFER_INVALID_BUFFER_TYPE
    */
    SL_DIAGNOSTIC_BUFFER_UNREGISTER,

   /*
    * SL_DIAGNOSTIC_BUFFER_RELEASE
    * Desc:
    *   Requests that the IOC release the buffer of the specified type that was
    *   previously posted.  This returns ownership of the buffer back to the
    *   host, allowing applications to read the buffer without having data
    *   changed by the IOC.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_DIAGNOSTIC_BUFFER_RELEASE_T
    *   dataSize: SL_DIAGNOSTIC_BUFFER_RELEASE_S
    * Output:
    *   pData: SL_DIAGNOSTIC_BUFFER_RELEASER_T
    *   dataSize: SL_DIAGNOSTIC_BUFFER_RELEASE_S
    * StoreLib Error Codes:
    *    SL_SUCCESS
    *    SL_ERR_CMD_FAILED_BY_OS
    *    SL_ERR_CMD_TIMEDOUT
    *    SL_ERR_MEMORY_ALLOC_FAILED
    *    SL_ERR_DIAG_BUFFER_INVALID_BUFFER_TYPE
    */
    SL_DIAGNOSTIC_BUFFER_RELEASE,

       /*
        * SL_SET_MASTER_TRIGGER
        * Desc:
        *   Set various bits as defined in the Master Trigger Flag field
        * Input:
        *   ctrlId: Controller Id
        *   masterTriggerFlag: Value defining the Master Triggers enabled for
        *                      this controller
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_SET_MASTER_TRIGGER,

       /*
        * SL_GET_MASTER_TRIGGER
        * Desc:
        *   Get various bits as defined in the Master Trigger Flag field
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   masterTriggerFlag: Value defining the Master Triggers enabled for
        *                      this controller
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_GET_MASTER_TRIGGER,

       /*
        * SL_CLEAR_MASTER_TRIGGER
        * Desc:
        *   Clear various bits as defined in the Master Trigger Flag field
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_CLEAR_MASTER_TRIGGER,

       /*
        * SL_SET_EVENT_TRIGGERS
        * Desc:
        *   Set Event Trigger values
        * Input:
        *   ctrlId:    Controller Id
        *   pData:     SL_WH_EVENT_TRIGGERS_T
        *   dataSize:  SL_WH_EVENT_TRIGGERS_S
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_SET_EVENT_TRIGGERS,

       /*
        * SL_GET_EVENT_TRIGGERS
        * Desc:
        *   Get Event Trigger values
        * Input:
        *   ctrlId:    Controller Id
        *   pData:     SL_WH_EVENT_TRIGGERS_T
        *   dataSize:  SL_WH_EVENT_TRIGGERS_S
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_GET_EVENT_TRIGGERS,

       /*
        * SL_CLEAR_EVENT_TRIGGERS
        * Desc:
        *   Have the OS Driver clear all the Event Triggers defined for this
        *   controller
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_CLEAR_EVENT_TRIGGERS,

       /*
        * SL_SET_SCSI_TRIGGERS
        * Desc:
        *   Set SCSI Trigger values
        * Input:
        *   ctrlId:    Controller Id
        *   pData:     SL_WH_SCSI_TRIGGERS_T
        *   dataSize:  SL_WH_SCSI_TRIGGERS_S
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_SET_SCSI_TRIGGERS,

       /*
        * SL_GET_SCSI_TRIGGERS
        * Desc:
        *   Get SCSI Trigger values
        * Input:
        *   ctrlId:    Controller Id
        * Output:
        *   pData:     SL_WH_EVENT_TRIGGERS_T
        *   dataSize:  SL_WH_EVENT_TRIGGERS_S
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_GET_SCSI_TRIGGERS,

       /*
        * SL_CLEAR_SCSI_TRIGGERS
        * Desc:
        *   Have the OS Driver clear all the SCSI Triggers defined for this
        *   controller
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_CLEAR_SCSI_TRIGGERS,

       /*
        * SL_SET_MPI_TRIGGERS
        * Desc:
        *   Set MPI Trigger values
        * Input:
        *   ctrlId:    Controller Id
        *   pData:     SL_WH_MPI_TRIGGERS_T
        *   dataSize:  SL_WH_MPI_TRIGGERS_S
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_SET_MPI_TRIGGERS,

       /*
        * SL_WH_GET_MPI_TRIGGERS
        * Desc:
        *   Get MPI Trigger values
        * Input:
        *   ctrlId:    Controller Id
        * Output:
        *   pData:     SL_WH_MPI_TRIGGERS_T
        *   dataSize:  SL_WH_MPI_TRIGGERS_S
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_GET_MPI_TRIGGERS,

       /*
        * SL_CLEAR_MPI_TRIGGERS
        * Desc:
        *   Have the OS Driver clear all the MPI Triggers defined for this
        *   controller
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_CLEAR_MPI_TRIGGERS,

       /*
        * SL_UPLOAD_CTRL_FLASH_IMAGES
        * Desc:
        *   Uploades the controller flash images to a file.
        * Input:
        *   ctrlId: Controller Id
        *   cmdParam_1b[0]: Type of flash image that is being supported.
        *   pData: Name of file to which requested image is to be uploaded.
        *   
        * Output:
        *   User requested File is created in which the requested image is downloaded from flash 
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   SL_ERR_MEMORY_ALLOC_FAILED
        *   SL_ERR_FLASH_IMAGE_NOT_PRESENT
        *   SL_SUCCESS
        */  
        SL_UPLOAD_CTRL_FLASH_IMAGES,

       /*
        * SL_CLEAR_CTRL_FLASH
        * Desc:
        *   Clears the following controller flash images. 
		*	1	NVSRAM
		*	2	BACKUP FW
		*	3	PERSISTENT PAGES      
		*	4	MAN PERSISTENT PAGES  
		*	5	BOOT SERVICES     
		*	6	CLEAN FLASH       
		*	7	ALL               
		*	8	MEGARAID FW       
        * Input:
        *   ctrlId: Controller Id
        *   cmdParam_1b[0]: TOOLBOX CLEAN flag (One of the above).
        *   
        * Output:
        *   Clears the requested flash region.
		*
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   SL_ERR_MEMORY_ALLOC_FAILED
        *   SL_ERR_FLASH_IMAGE_NOT_PRESENT
        *   SL_SUCCESS
        */
        SL_CLEAR_CTRL_FLASH,
        
   /*
	  * SL_SPD_PROGRAMMING
	  * Desc:
	  *   Allows SPD data reading or writing. 
	  
	  * Input:
	  *   ctrlId: Controller Id
	  *   cmdParam_1b[0]: direction (SPD Read/ SPD write).
	  
	  * values for this filed
        * MPI2_TOOL_ISTWI_ACTION_READ_DATA             (0x01)
        * MPI2_TOOL_ISTWI_ACTION_WRITE_DATA           (0x02)
	  *   
	  * Output:
	  *   Either reads pr programs the SPD data.
	  *
	  * StoreLib Error Codes:
	  *   SL_ERR_INVALID_CTRL
	  *   SL_ERR_MEMORY_ALLOC_FAILED
	  *   SL_SUCCESS
	  */

        SL_SPD_PROGRAMMING,

   /*
	  * SL_SPD_PROGRAMMING
	  * Desc:
	  *   Allows SPD data reading or writing. 
	  
	  * Input:
	  *   ctrlId: Controller Id
	  *   cmdParam_1b[0]: direction (SPD Read/ SPD write).
	  
	  * values for this filed
        * MPI2_TOOL_ISTWI_ACTION_READ_DATA             (0x01)
        * MPI2_TOOL_ISTWI_ACTION_WRITE_DATA           (0x02)
	  *   
	  * Output:
	  *   Either reads pr programs the SPD data.
	  *
	  * StoreLib Error Codes:
	  *   SL_ERR_INVALID_CTRL
	  *   SL_ERR_MEMORY_ALLOC_FAILED
	  *   SL_SUCCESS
	  */

        SL_SBR_PROGRAMMING,

   		SL_PHY_LINK_RESET,

      /*
       * SL_UPDATE_CONTROLLER_CACHE 
       * Desc:
       * When server system tries to execute a superise shutdown, then
       * user application can send refresh controller command to re-new
       * the controller device list manually
       */
       SL_UPDATE_CONTROLLER_CACHE,
	   
    //@@@@@ Always add new commands at the end @@@@@//
};

/****************************************************************************
*
*        PHYSICAL DEVICE COMMANDS
*
****************************************************************************/
enum SL_PD_CMD
{
   /*
    * SL_GET_PD_INFO
    * Desc:
    *   Gets physical device information
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceId: Device Id of the physical device
    *   pData: Pointer to buffer to accept pd info
    *   dataSize: sizeof(MR_PD_INFO)
    * Output:
    *   pData: MR_PD_INFO
    *   dataSize: sizeof(MR_PD_INFO)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_PD_INFO = 0,

   /*
    * SL_MAKE_ONLINE
    * Desc:
    *   Attempts to make physical drive online
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_MAKE_ONLINE,

   /*
    * SL_MAKE_OFFLINE
    * Desc:
    *   Attempts to make physical drive offline
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_MAKE_OFFLINE,

   /*
    * SL_REPLACE_MISSING_PD
    * Desc:
    *   Attempts to replace a 'missing' pd in an array
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    *   cmdParam_2b[0]:array index
    *   cmdParam_1b[2]:array row number
    *   pData:None
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_REPLACE_MISSING_PD,

   /*
    * SL_MARK_PD_MISSING
    * Desc:
    *   Attempts to mark a physical drive as missing
    *   from an existing array.
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_MARK_PD_MISSING,

   /*
    * SL_START_REBUILD
    * Desc:
    *   Attempts to start rebuild
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_START_REBUILD,

   /*
    * SL_CANCEL_REBUILD
    * Desc:
    *   Attempts to cancel ongoing rebuild
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_CANCEL_REBUILD,

   /*
    * SL_GET_PD_PROGRESS
    * Desc:
    *   Gets the progress of ongoing operation on pd
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceId: Device Id of the physical device
    *   pData: Pointer to MR_PD_PROGRESS buffer
    *   dataSize: sizeof(MR_PD_PROGRESS)
    * Output:
    *   pData: MR_PD_PROGRESS
    *   dataSize: sizeof(MR_PD_PROGRESS)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_PD_PROGRESS,

   /*
    * SL_START_LOCATE_PD
    * Desc:
    *   Locates the pd by blinking LED on the pd. The blinking duration in seconds may
    *   be specified in the input. If 0 is passed, blinking will continue till LOCATE_STOP
    *   command is send.
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceId: Device Id of the physical device
    *   cmdParam_1b[0]:duration of locate (in seconds; 0 for indefinite)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_START_LOCATE_PD,

   /*
    * SL_STOP_LOCATE_PD
    * Desc:
    *   Stops blinking LED on the pd
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceId: Device Id of the physical device
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_STOP_LOCATE_PD,

   /*
    * SL_MAKE_GLOBAL_HOTSPARE
    * Desc:
    *   Attempts to make a drive as Global Hotspare
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_MAKE_GLOBAL_HOTSPARE,

   /*
    * SL_MAKE_DEDICATED_HOTSPARE
    * Desc:
    *   Attempts to assign a drive as Dedicated Hotspare
    *   dedicated to an array
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    *   pData: SL_ARRAY_LIST_T - one or more arrays that this hotspare will belong to.
    *   dataSize: SL_ARRAY_LIST_S
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_MAKE_DEDICATED_HOTSPARE,

   /*
    * SL_GET_DEDICATED_HOTSPARE_INFO
    * Desc:
    *   Gets information on dedicated hotspare
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceId: Device Id of the physical device
    *   pData: Pointer to MR_SPARE data structure containing dedicated HSP info
    *   dataSize: sizeof(MR_SPARE)
    * Output:
    *   pData: MR_SPARE
    *   dataSize: sizeof(MR_SPARE)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_DEDICATED_HOTSPARE_INFO,

   /*
    * SL_REMOVE_HOTSPARE
    * Desc:
    *   Attempts to change a hotspare to ready drive
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_REMOVE_HOTSPARE,

   /*
    * SL_PREPARE_PD_FOR_REMOVAL
    * Desc:
    *   Prepares a physical disk for removal. F/W spins down
    *   the disk and performs other necessary action for removal.
    *   Once this call succeeds, no IO is possible on the drive.
    *   A bit is set to mark the pd in this state
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_PREPARE_PD_FOR_REMOVAL,

   /*
    * SL_UNDO_PREPARE_PD_FOR_REMOVAL
    * Desc:
    *   Undoes removal preparation. Spins up the drive, clears removal
    *   bit from state and allows application to perform IO on the drive.
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_UNDO_PREPARE_PD_FOR_REMOVAL,

   /*
    * SL_GET_LD_OF_PD
    * Desc:
    *   Gets logical drives created from specified pd
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceId: Device Id of the physical device
    *   pData: Pointer to SL_LD_OF_PD_T
    *   dataSize: SL_LD_OF_PD_S
    * Output:
    *   pData: SL_LD_OF_PD_T
    *   dataSize: SL_LD_OF_PD_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_LD_OF_PD,

   /*
    * SL_FORMAT_PD
    * Desc:
    *   Formats the physical drive by writing 0's.
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical drive
    *   pData: None
    * Output:
    *   pData: None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_FORMAT_PD,

   /*
    * SL_ABORT_FORMAT_PD
    * Desc:
    *   Aborts formatting the physical drive
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical drive
    *   pData: None
    * Output:
    *   pData: None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_ABORT_FORMAT_PD,

   /*
    * SL_SET_PD_STATE
    * Desc:
    *   Attempts to change the state of the pd
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical drive
    *   cmdParam_2b[0]:MR_PD_STATE
    *   pData: None
    * Output:
    *   pData: None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_SET_PD_STATE,

   /*
    * SL_GET_PD_ALLOWED_OPERATION
    * Desc:
    *   Gets the allowed operation (make online, offline, etc) on the pd
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceId: Device Id of the physical drive
    *   pData: Pointer to SL_PD_ALLOWED_OPER_T data structure
    *   dataSize: SL_PD_ALLOWED_OPER_S
    * Output:
    *   pData: SL_PD_ALLOWED_OPER_T
    *   dataSize: SL_PD_ALLOWED_OPER_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_PD_ALLOWED_OPERATION,

   /*
    * SL_MAKE_HOTSPARE
    * Desc:
    *   Attempts to assign a drive as Hotspare (Dedicated or Global)
    *   with assigned attributes.
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.seqNum: Sequence number of the pd
    *   pdRef.deviceId: Device Id of the physical device
    *   pData: pointer to MR_SPARE data structure.
    *   dataSize: sizeof(MR_SPARE).
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_NULL_DATA_PTR
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_MAKE_HOTSPARE,

    //@@@@@ Always add new commands at the end @@@@@//
};

/****************************************************************************
*
*        LOGICAL DRIVE COMMANDS
*
****************************************************************************/
enum SL_LD_CMD
{
   /*
    * SL_GET_LD_INFO
    * Desc:
    *   Gets logical device information
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive id
    *   pData: Pointer to buffer of MR_LD_INFO struct
    *   dataSize: sizeof(MR_LD_INFO)
    * Output:
    *   pData: MR_LD_INFO
    *   dataSize: sizeof(MR_LD_INFO)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_LD_INFO = 0,

   /*
    * SL_GET_LD_PROPERTIES
    * Desc:
    *   Gets ld properties.
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    *   pData: Pointer to buffer of MR_LD_PROPERTIES struct
    *   dataSize: sizeof(MR_LD_PROPERTIES)
    * Output:
    *   pData: MR_LD_PROPERTIES
    *   dataSize: sizeof(MR_LD_PROPERTIES)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_LD_PROPERTIES,

   /*
    * SL_SET_LD_PROPERTIES
    * Desc:
    *   Sets ld properties.
    * Input:
    *   ctrlId: Controller Id, MR_CTRL_PROP
    *   ldRef.seqNum: Logical drive sequence number
    *   ldRef.targetId: Logical drive Id
    *   pData: Pointer to buffer of MR_LD_PROPERTIES struct with data to be set
    *   dataSize: sizeof(MR_LD_PROPERTIES)
    *   Setting LD Properties is not supported for WarpDrive.
    *   Application can do it forcefully by setting following parameters
    *   cmdParam_1b[5]: 0x0A
    *   cmdParam_1b[6]: 0x0F
    *   cmdParam_1b[7]: 0x0D
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_SET_LD_PROPERTIES,

   /*
    * SL_DELETE_LD
    * Desc:
    *   Attempts to delete the ld.
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    *   ldRef.seqNum: Sequence Number of Logical drive Id
    *   pData: None
    *   Deleting LD is not supported for WarpDrive.
    *   Application can do it forcefully by setting following parameters
    *   cmdParam_1b[5]: 0x0A
    *   cmdParam_1b[6]: 0x0F
    *   cmdParam_1b[7]: 0x0D
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_DELETE_LD,

   /*
    * SL_START_LOCATE_LD
    * Desc:
    *   Locates the ld by blinking LED on the constituent pd of the ld.
    *   The blinking duration in seconds may be specified in the input.
    *   If 0 is passed, blinking will continue till LOCATE_STOP command is send.
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: logical drive Id
    *   cmdParam_1b[0]:duration of locate (in seconds; 0 for indefinite)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_START_LOCATE_LD,

   /*
    * SL_STOP_LOCATE_LD
    * Desc:
    *   Stops blinking LED on the constituent pd of the ld
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: logical drive Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_STOP_LOCATE_LD,

   /*
    * SL_START_CHECK_CONSISTENCY
    * Desc:
    *   Starts check consistency
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_START_CHECK_CONSISTENCY,

   /*
    * SL_CANCEL_CHECK_CONSISTENCY
    * Desc:
    *   Cancels check consistency
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_CANCEL_CHECK_CONSISTENCY,

   /*
    * SL_GET_LD_PROGRESS
    * Desc:
    *   Gets logical drive operation progress
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    *   pData: Pointer to MR_LD_PROGRESS struct
    *   dataSize: sizeof(MR_LD_PROGRESS)
    * Output:
    *   pData: MR_LD_PROGRESS
    *   dataSize: sizeof(MR_LD_PROGRESS)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_LD_PROGRESS,


   /*
    * SL_START_INITIALIZATION
    * Desc:
    *   Starts foreground initialization
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
	*	ldRef.seqNum  : Logical drive sequence number
    *   cmdParam_1b[0] : 0-Slow Init (Full Init), 1-Fast Init
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_START_INITIALIZATION,

   /*
    * SL_CANCEL_INITIALIZATION
    * Desc:
    *   Cancels foreground initialization
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_CANCEL_INITIALIZATION,

   /*
    * SL_GET_PD_IN_LD
    * Desc:
    *   Gets pds participating in the ld
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    *   pData: Pointer to SL_PD_IN_LD_T
    *   dataSize: SL_PD_IN_LD_S
    * Output:
    *   pData: SL_PD_IN_LD_T
    *   dataSize: SL_PD_IN_LD_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_PD_IN_LD,

   /*
    * SL_START_RECONSTRUCTION
    * Desc:
    *   Attempts to perform reconstruction
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    *   pData: Pointer to buffer of MR_RECON struct containing details of pd to be reconst
    *   dataSize: sizeof(MR_RECON)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_START_RECONSTRUCTION,

   /*
    * SL_GET_RECONSTRUCTION_INFO
    * Desc:
    *   Attempts to get reconstruction progress info
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    *   pData: Pointer to buffer of MR_RECON struct containing details of pd to be reconst
    *   dataSize: sizeof(MR_RECON)
    * Output:
    *   pData: MR_RECON
    *   dataSize: sizeof(MR_RECON)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_RECONSTRUCTION_INFO,

   /*
    * SL_GET_LD_ALLOWED_OPERATION
    * Desc:
    *   Gets the allowed operation (init, cc, etc) on the ld
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    *   pData: Pointer to SL_LD_ALLOWED_OPER_T data structure
    *   dataSize: sizeof(SL_LD_ALLOWED_OPER_T)
    * Output:
    *   pData: SL_LD_ALLOWED_OPER_T
    *   dataSize: sizeof(SL_LD_ALLOWED_OPER_T)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_LD_ALLOWED_OPERATION,

    //@@@@@ Always add new commands at the end @@@@@//
};

/****************************************************************************
*
*        CONFIG COMMANDS
*
****************************************************************************/
enum SL_CONFIG_CMD
{
   /*
    * SL_READ_CONFIG
    * Desc:
    *   Gets current config. The size field inside MR_CONFIG_DATA has the full
    *   size of the config data structure
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to MR_CONFIG_DATA buffer
    *   dataSize: sizeof(MR_CONFIG_DATA)
    * Output:
    *   pData: MR_CONFIG_DATA data
    *   dataSize: sizeof(MR_CONFIG_DATA)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_READ_CONFIG = 0,

   /*
    * SL_ADD_CONFIG
    * Desc:
    *   Adds specified config to current config.
    *   If new config is desired, then existing config needs to be cleared first
    *   prior to using this command.
    * Input:
    *   ctrlId: Controller Id
    *   pData: MR_CONFIG_DATA buffer containing config data.
    *   dataSize: sizeof(MR_CONFIG_DATA)
    *   Adding Config is not supported for WarpDrive.
    *   Application can do it forcefully by setting following parameters
    *   cmdParam_1b[5]: 0x0A
    *   cmdParam_1b[6]: 0x0F
    *   cmdParam_1b[7]: 0x0D
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_ADD_CONFIG,

   /*
    * SL_CLEAR_CONFIG
    * Desc:
    *   Clears existing config
    * Input:
    *   ctrlId: Controller Id
    *   Clearing Config is not supported for WarpDrive.
    *   Application can do it forcefully by setting following parameters
    *   cmdParam_1b[5]: 0x0A
    *   cmdParam_1b[6]: 0x0F
    *   cmdParam_1b[7]: 0x0D
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_CLEAR_CONFIG,

   /*
    * SL_SCAN_FOR_FOREIGN_CONFIG
    * Desc:
    *   Scans for foreign configs
    * Input:
    *   ctrlId: Controller Id
    *   pData: Buffer to hold list of max foreign config GUIDS.
    * Output:
    *   pData: List of foreign config GUIDS
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_SCAN_FOR_FOREIGN_CONFIG,

   /*
    * SL_READ_FOREIGN_CONFIG
    * Desc:
    *   Reads config as existing in the foreign disk(s).
    * Input:
    *   ctrlId: Controller Id
    *   configGUIDRef.GUIDIndex: Index of foreign config GUID in DDF Header
    *   pData: Pointer to buffer of MR_CONFIG_DATA to receive foreign config
    * Output:
    *   pData: MR_CONFIG_DATA data
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_READ_FOREIGN_CONFIG,

   /*
    * SL_READ_CONFIG_AFTER_IMPORT
    * Desc:
    *   Reads F/W generated config based on available disks
    *   in the foreign config. This config will be the potential config if imported
    * Input:
    *   ctrlId: Controller Id
    *   configGUIDRef.GUIDIndex: Index of foreign config GUID in DDF Header, 0xFF to view all possible configurations
    *   pData: Pointer to buffer of MR_CONFIG_DATA to receive foreign config
    * Output:
    *   pData: MR_CONFIG_DATA data
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_READ_CONFIG_AFTER_IMPORT,

   /*
    * SL_IMPORT_FOREIGN_CONFIG
    * Desc:
    *   Imports the foreign config and merges with existing config.
    * Input:
    *   ctrlId: Controller Id
    *   configGUIDRef.GUIDIndex: Index of foreign config GUID in DDF Header, 0xFF to view all possible configurations
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_IMPORT_FOREIGN_CONFIG,

   /*
    * SL_CLEAR_FOREIGN_CONFIG
    * Desc:
    *   Clears the config.from a set of foreign drives
    * Input:
    *   ctrlId: Controller Id
    *   configGUIDRef.GUIDIndex: Index of foreign config GUID in DDF Header
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_CLEAR_FOREIGN_CONFIG,

   /*
    * SL_GET_ARRAY_INFO
    * Desc:
    *   Gets information for the specified array.
    * Input:
    *   ctrlId: Controller Id
    *   arrayRef.ref: Array id
    *   pData: Pointer to buffer of SL_ARRAY_INFO_T to accept array information
    *   dataSize: SL_ARRAY_INFO_S
    * Output:
    *   pData: SL_ARRAY_INFO_T
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_ARRAY_INFO,

   /*
    * SL_GET_CONFIG_ALLOWED_OPERATION
    * Desc:
    *   Gets the config allowed operation (addConfig, clearConfig).
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to SL_CONFIG_ALLOWED_OPER_T data structure
    *   dataSize: sizeof(SL_CONFIG_ALLOWED_OPER_T)
    * Output:
    *   pData: SL_CONFIG_ALLOWED_OPER_T
    *   dataSize: sizeof(SL_CONFIG_ALLOWED_OPER_T)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_CONFIG_ALLOWED_OPERATION,

   /*
    * SL_RESIZE_ARRAY
    * Desc:
    *   Change the size of the given array.  The supplied array must match FW's data for this array.
    *   In other words, the drive counts and states must match FW's current state.  The new drive
    *   size must be large enough to handle any configured LDs.  It must also be smaller than the
    *   smallest member disk.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to a single MR_ARRAY structure containing desired new size
    *   dataSize: sizeof(MR_ARRAY)
    * Output:
    *   pData: None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_RESIZE_ARRAY,

    //@@@@@ Always add new commands at the end @@@@@//
};

/****************************************************************************
*
*        BATTERY COMMANDS
*
****************************************************************************/
enum SL_BBU_CMD
{
   /*
    * SL_GET_BBU_STATUS
    * Desc:
    *   Gets the current status information for the battery backup unit (BBU)
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_BBU_STATUS struct
    *   dataSize: sizeof(MR_BBU_STATUS)
    * Output:
    *   pData: MR_BBU_STATUS
    *   dataSize: sizeof(MR_BBU_STATUS)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_BBU_STATUS,

   /*
    * SL_GET_BBU_CAPACITY_INFO
    * Desc:
    *   Gets the capacity of the BBU
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_BBU_CAPACITY_INFO struct
    *   dataSize: sizeof(MR_BBU_CAPACITY_INFO)
    * Output:
    *   pData: MR_BBU_CAPACITY_INFO
    *   dataSize: sizeof(MR_BBU_CAPACITY_INFO)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_BBU_CAPACITY_INFO,

   /*
    * SL_GET_BBU_DESIGN_INFO
    * Desc:
    *   Gets the design parameters of the BBU
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_BBU_DESIGN_INFO struct
    *   dataSize: sizeof(MR_BBU_DESIGN_INFO)
    * Output:
    *   pData: MR_BBU_DESIGN_INFO
    *   dataSize: sizeof(MR_BBU_DESIGN_INFO)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_BBU_DESIGN_INFO,

   /*
    * SL_BBU_LEARN
    * Desc:
    *   Starts a Learning cycle on the BBU
    * Input:
    *   ctrlId: Controller Id
    *   pData: None
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_BBU_LEARN,

   /*
    * SL_GET_BBU_PROPERTIES
    * Desc:
    *   Gets the BBU properties
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_BBU_PROPERTIES struct
    *   dataSize: sizeof(MR_BBU_PROPERTIES)
    * Output:
    *   pData: MR_BBU_PROPERTIES
    *   dataSize: sizeof(MR_BBU_PROPERTIES)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_GET_BBU_PROPERTIES,

   /*
    * SL_SET_BBU_PROPERTIES
    * Desc:
    *   Sets the BBU properties
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_BBU_PROPERTIES struct
    *   dataSize: sizeof(MR_BBU_PROPERTIES)
    * Output:
    *   pData: None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_SET_BBU_PROPERTIES,

   /*
    * SL_BBU_SLEEP
    * Desc:
    *   Place the battery into low-power storage mode.  The battery will automatically
    *   exit this state after five seconds.
    * Input:
    *   ctrlId: Controller Id
    *   pData: None
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_BBU_SLEEP,
};

/****************************************************************************
*
*        PASSTHRU COMMANDS
*
****************************************************************************/
enum SL_PASSTHRU_CMD
{
   /*
    * SL_SCSI_PASSTHRU
    * Desc:
    *   SCSI Pass through
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceId: Device Id of the physical device
    *   pData: Pointer to buffer of SL_SCSI_PASSTHRU_T struct
    *   dataSize: sizeof(SL_SCSI_PASSTHRU_T)+dataSize
    * Output:
    *   pData: SL_SCSI_PASSTHRU_T
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_SCSI_PASSTHRU = 0,

   /*
    * SL_SMP_PASSTHRU
    * Desc:
    *   Command to fire SMP Pass through to SAS Expanders
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_SMP_PASSTHRU_T struct
    *   dataSize: SL_SMP_PASSTHRU_S
    * Output:
    *   pData: SL_SMP_PASSTHRU_T
    *   dataSize: SL_SMP_PASSTHRU_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_SMP_PASSTHRU,

   /*
    * SL_STP_PASSTHRU
    * Desc:
    *   Command to fire STP Pass through to SATA devices
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_STP_PASSTHRU_T struct
    *   dataSize: SL_STP_PASSTHRU_S
    * Output:
    *   pData: SL_STP_PASSTHRU_T
    *   dataSize: SL_STP_PASSTHRU_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_STP_PASSTHRU,

   /*
    * SL_DCMD_PASSTHRU
    * Desc:
    *   Direct Commands (DCMD) Pass through. This command may be used
    *   to send any DCMD to the firmware
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_DCMD_INPUT_T struct
    *   dataSize: SL_DCMD_INPUT_S
    * Output:
    *   pData: SL_DCMD_INPUT_T
    *   dataSize: SL_DCMD_INPUT_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_DCMD_PASSTHRU,

   /*
    * SL_NVME_ENCAP_PASSTHRU 
    * Desc:
    *   NVME Pass through. This command may be used
    *   to send commands to NVME devices
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_DCMD_INPUT_T struct
    *   dataSize: Needs to create //KC:
    * Output:
    *   pData: 
    *   dataSize:
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */

	SL_NVME_ENCAP_PASSTHRU,
};

/****************************************************************************
*
*        EVENT COMMANDS
*
****************************************************************************/
enum SL_EVENT_CMD
{
   /*
    * SL_GET_EVENT_SEQUENCE_INFO
    * Desc:
    *   Gets event sequence information
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of MR_EVT_LOG_INFO struct
    *   dataSize: sizeof(MR_EVT_LOG_INFO)
    * Output:
    *   pData: MR_EVT_LOG_INFO
    *   dataSize: sizeof(MR_EVT_LOG_INFO)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_EVENT_SEQUENCE_INFO = 0,

   /*
    * SL_GET_EVENT_DETAILS
    * Desc:
    *   Gets event detail starting from specified sequence number
    * Input:
    *   ctrlId: Controller Id
    *   evtRef.seqNum: Starting seq num
    *   cmdParam_4b[0]: MR_EVT_CLASS_LOCALE containing desired class/locale values
    *   pData: Pointer to buffer of MR_EVT_LIST struct
    *   dataSize: sizeof(MR_EVT_LIST) + (count-1) * sizeof(MR_EVT_DETAIL)
    * Output:
    *   pData: MR_EVT_DETAIL list
    * Notes:
    *   F/W will copy max available events that the buffer can hold starting from the
    *   specified sequence number.
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_EVENT_DETAILS,

   /*
    * SL_CLEAR_EVENTS
    * Desc:
    *   Clears the event log. Note that F/W event log is not really cleared.
    *   This is a logical clear.
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_CLEAR_EVENTS,
};

//Enclosure related commands
enum SL_ENCL_CMD
{
   /*
    * SL_GET_ENCL_CONFIG
    * Desc:
    *   Gets enclosure configuration information
    * Input:
    *   ctrlId: Controller Id
    *   enclRef.deviceId: deviceId of the enclosure
    *   pData: Pointer to buffer of SL_ENCL_CONFIG_T struct
    *   dataSize: SL_ENCL_CONFIG_S
    * Output:
    *   pData: SL_ENCL_CONFIG_T
    *   dataSize: SL_ENCL_CONFIG_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_ENCL_CONFIG,

   /*
    * SL_GET_ENCL_STATUS
    * Desc:
    *   Gets enclosure status information
    * Input:
    *   ctrlId: Controller Id
    *   enclRef.deviceId: deviceId of the enclosure
    *   pData: Pointer to buffer of SL_ENCL_STATUS_T struct
    *   dataSize: SL_MAX_ENCL_BUFFER_SIZE
    * Output:
    *   pData: SL_ENCL_STATUS_T
    *   dataSize: Total size of the data
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_ENCL_STATUS,

   /*
    * SL_SET_ENCL_ELEMENT
    * Desc:
    *   Sets enclosure element data
    * Input:
    *   ctrlId: Controller Id
    *   enclRef.deviceId: deviceId of the enclosure
    *   enclRef.elementIndex: Encl element index. Not used for setting temp threshold values
    *   cmdParam_1b[0]: elementType (of type SL_ENCL_ELEM_TYPE e.g. SL_SES_ELMT_TEMPERATURE_SENSOR)
    *   pData: Pointer to buffer of SL_ENCL_SET_ELEM_DATA_T struct with data to be set
    *   dataSize: SL_ENCL_SET_ELEM_DATA_S
    * Output:
    *   None
    * Boundary Conditions:
    *   Only temperature within Low Critical and High Critical can be set. Element Index
    *   is not used while setting temp threshold values
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_SET_ENCL_ELEMENT,

   /*
    * SL_LOCATE_ENCL
    * Desc:
    *   Locates the enclosure
    * Input:
    *   ctrlId: Controller Id
    *   enclRef.deviceId: deviceId of the enclosure
    *   pData: None
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
    */
    SL_LOCATE_ENCL,

   /*
    * SL_SHUTDOWN_ENCL
    * Desc:
    *   Initiates enclosure shutdown
    * Input:
    *   ctrlId: Controller Id
    *   enclRef.deviceId: deviceId of the enclosure
    *   pData: None
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_SHUTDOWN_ENCL,

    // Structure: Voltage Sensor
    /*
    * SL_GET_ENCL_STATUS_EX
    * Desc:
    *   Gets enclosure status information for the SES-2 elements defined in SES2r15 spec
    * Input:
    *   ctrlId: Controller Id
    *   enclRef.deviceId: deviceId of the enclosure
    *   pData: Pointer to buffer of SL_ENCL_STATUS_EX_T struct
    *   dataSize: SL_MAX_ENCL_BUFFER_SIZE
    * Output:
    *   pData: SL_ENCL_STATUS_EX_T
    *   dataSize: Total size of the data
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_GET_ENCL_STATUS_EX,

    // Structure: Voltage Sensor

   /*
    * SL_GET_ENCL_ELEMENT
    * Desc:
    *   Gets enclosure element data
    * Input:
    *   ctrlId: Controller Id
    *   enclRef.deviceId: deviceId of the enclosure
    *   cmdParam_1b[0]: elementType (of type SL_ENCL_ELEM_TYPE e.g. SL_SES_ELMT_TEMPERATURE_SENSOR)
    *   cmdParam_1b[1]: elementIndex (zero based index of type SL_ENCL_ELEM_TYPE e.g. SL_SES_ELMT_TEMPERATURE_SENSOR)
    *   pData: Pointer to buffer of SL_ENCL_GET_ELEM_DATA_T struct where data is returned
    *   dataSize: SL_ENCL_GET_ELEM_DATA_S
    * Output:
    *   pData: SL_ENCL_GET_ELEM_DATA_T struct.
    * Boundary Conditions:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_INVALID_INPUT_PARAMETER
    *   SL_ERR_MEMORY_ALLOC_FAILED
    */
    SL_GET_ENCL_ELEMENT,
    //@@@@@ Always add new commands at the end @@@@@//
};

//OOB related commands
enum SL_OOB_CMD
{
/*
    * SL_OOB_STATUS_POLL
    * Desc: Returns list of objects and events that have occurred since the last poll.
    * Restriction: Only one app is polling at a time
    * Input:
    *   cmdParam_1b[0]: max number of response packets; 0x00 = No limit
    *   cmdParam_1b[1]: 1 - send all information; 0 - send incremental information
    *   pData: Pointer to SL_OOB_EVT_OBJ_LIST_T struct
    *   dataSize: size of buffer to hold objects and events. Since data is dynamic,
    *   it is recommended to allocate a larger buffer such as 8k and send it down
    * Output:
    *   pData: Pointer to SL_OOB_EVT_OBJ_LIST_T struct
    *   dataSize: Actual size of data returned
    * StoreLib Error Codes:
    *   MFI_STAT_INVALID_STATUS - Data not available try again
    *   SL_ERR_INVALID_INPUT_PARAMETER - The input parameter to the command was invalid
    * AVAGO non OOB environment: SL_ERR_INVALID_CMD
    */
    SL_OOB_STATUS_POLL = 0,

    /*
    * SL_OOB_REGISTER_OBJ_INFO
    * Desc:
    *   Register for change in objects from FW. Storelib will poll for change in obj and
    *   Notify via callback provided by application. Changed object will be send as parameter
    *   in callback.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_REG_OOB_OBJ_INPUT_T struct
    *   dataSize: SL_REG_OOB_OBJ_INPUT_S
    * Output:
    *   pData: SL_REG_OOB_OBJ_OUTPUT_T
    *   dataSize: SL_REG_OOB_OBJ_OUTPUT_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_INPUT_PARAMETER
    * AVAGO non OOB environment: SL_ERR_INVALID_CMD (OOB only)
    */
    SL_OOB_REGISTER_OBJ_INFO = 1,

   /*
    * SL_OOB_UNREGISTER_OBJ_INFO
    * Desc:
    *   Function used to unregister from receiving object updates.
    * Input:
    *   cmdParam_4b[0]: Unique registration identifier that was returned by SL_REG_OOB_OBJ_INFO in its output (SL_REG_OOB_OBJ_OUTPUT_T)
    *   pData: None
    * Output:
    *   pData: None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_INPUT_PARAMETER
    * AVAGO non OOB environment: SL_ERR_INVALID_CMD (OOB only)
    */
    SL_OOB_UNREGISTER_OBJ_INFO = 2,  
	
   /*
    * SL_OOB_SUSPENDRESUME
    * Desc:
    *   Command used to suspend or resume all OOB functionalities maybe used when controller is temporarily not available but OOB is up and running.
    *   After this cmd is fired, all successive commands to controller will return SL_ERR_OOB_SUSPENDED_STATE. Apps must fire SL_OOB_SUSPENDRESUME with cmdParam_1b[0] = 0 to resume
    *   or fire SL_INIT_LIB which will again reset the suspended state and storelib will discover ctrl from ctrl list
    * Input:
    *   cmdParam_1b[0]: 1 - Suspend; 0 - Resume
    *   pData: None
    * Output:     
    *   pData: None
    * StoreLib Error Codes:
    *   SL_ERR_OOB_INVALID_STATE - returned when storelib is already in suspended state
    * AVAGO non OOB environment: SL_ERR_INVALID_CMD (OOB only)
    */
    SL_OOB_SUSPENDRESUME = 3,

   /*
    * SL_OOB_GET_TTY_LOG
    * Desc:
    *   Gets TTY logs into a file.
    *   Input:
	*   ctrlId: Controller Id
    *   pData: File pointer
    * Output:     
    *   pData: File pointer
    * StoreLib Error Codes:
    *   SL_ERR_OOB_INVALID_CTRL 
    */
    SL_OOB_GET_TTY_LOG = 4,

};

//WarHawk Specific commands
enum SL_WH_CMD
{
    // Begin
   /*
    * SL_WH_FORMAT_SSD
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
    SL_WH_FORMAT_SSD,

   /*
    * SL_WH_FORMAT_DD
    * Desc:
    *   The DirectDrive volume will be formatted
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   NA
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_DD_FORMAT_FAILED
    */
    SL_WH_FORMAT_DD,

   /*
    * SL_WH_GET_DD_INFO
    * Desc:
    *   Get DirectDrive information for the selected DirectDrive Controller.
    *   This will give details about the DD controller
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_WH_DD_INFO_T
    *   dataSize: SL_WH_DD_INFO_S
    * Output:
    *   pData: SL_WH_DD_INFO_T
    *   dataSize: SL_WH_DD_INFO_S
    * StoreLib Error Codes:
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_MEMORY_ALLOC_FAILED
    *   SL_ERR_INVALID_CTRL
    */
    SL_WH_GET_DD_INFO,

   /*
    * SL_WH_GET_SSD_INFO
    * Desc:
    *   Get SSD drive information for the selected SSD on DirectDrive
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Device ID of the physical drive
    *   pData: Pointer to buffer of the type SL_WH_SSD_INFO_T
    *   dataSize: SL_CTRL_WH_SSD_INFO_S
    * Output:
    *   pData: Pointer to buffer of the type SL_WH_SSD_INFO_T
    *   dataSize: SL_CTRL_WH_SSD_INFO_S
    * StoreLib Error Codes:
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_MEMORY_ALLOC_FAILED
    *   SL_ERR_IR_DEVICE_NOT_FOUND
    *   SL_ERR_INVALID_CTRL
    */
    SL_WH_GET_SSD_INFO,

   /*
    * SL_WH_DIAGNOSTIC_BUFFER_QUERY
    * Desc:
    *   The controller will be queried for information about the Diagnostic
    *   Buffer.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_WH_DD_DIAGNOSTIC_BUFFER_QUERY_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_QUERY_S
    * Output:
    *   pData: SL_WH_DD_DIAGNOSTIC_BUFFER_QUERY_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_QUERY_S
    * StoreLib Error Codes:
    *    SL_SUCCESS
    *    SL_ERR_CMD_FAILED_BY_OS
    *    SL_ERR_CMD_TIMEDOUT
    *    SL_ERR_MEMORY_ALLOC_FAILED
    *    SL_ERR_WH_DIAG_BUFFER_INVALID_BUFFER_TYPE
    */
    SL_WH_DIAGNOSTIC_BUFFER_QUERY,

   /*
    * SL_WH_DIAGNOSTIC_BUFFER_REGISTER
    * Desc:
    *   Register a Diagnostic Buffer. For now the only usage of this command is
    *   to pass in a new set of ProductSpecific information, since the buffer
    *   will have been allocated by the OS Drivers at system startup
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_WH_DD_DIAGNOSTIC_BUFFER_REGISTER_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_REGISTER_S
    * Output:
    *   pData: SL_WH_DD_DIAGNOSTIC_BUFFER_REGISTER_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_REGISTER_S
    * StoreLib Error Codes:
    *    SL_SUCCESS
    *    SL_ERR_WH_DIAG_BUFFER_INVALID_PARAM
    *    SL_ERR_CMD_FAILED_BY_OS
    *    SL_ERR_CMD_TIMEDOUT
    *    SL_ERR_MEMORY_ALLOC_FAILED
    *    SL_ERR_WH_DIAG_BUFFER_INVALID_BUFFER_TYPE
    */
    SL_WH_DIAGNOSTIC_BUFFER_REGISTER,

   /*
    * SL_WH_DIAGNOSTIC_BUFFER_READ
    * Desc:
    *   Specified diagnostic buffer will be extracted
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_WH_DD_DIAGNOSTIC_BUFFER_READ_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_READ_S
    * Output:
    *   pData: SL_WH_DD_DIAGNOSTIC_BUFFER_READ_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_READ_S
    * StoreLib Error Codes:
    *    SL_SUCCESS
    *    SL_ERR_CMD_FAILED_BY_OS
    *    SL_ERR_CMD_TIMEDOUT
    *    SL_ERR_MEMORY_ALLOC_FAILED
    *    SL_ERR_WH_DIAG_BUFFER_INVALID_BUFFER_TYPE
    */
    SL_WH_DIAGNOSTIC_BUFFER_READ,

   /*
    * SL_WH_DIAGNOSTIC_BUFFER_UNREGISTER
    * Desc:
    *   Requests that the OS driver clean up (free up if possible) any memory
    *   allocated for the Diagnostic Buffer and free up any associated resources.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_WH_DD_DIAGNOSTIC_BUFFER_UNREGISTER_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_UNREGISTER_S
    * Output:
    *   pData: SL_WH_DD_DIAGNOSTIC_BUFFER_UNREGISTER_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_UNREGISTER_S
    * StoreLib Error Codes:
    *    SL_SUCCESS
    *    SL_ERR_CMD_FAILED_BY_OS
    *    SL_ERR_CMD_TIMEDOUT
    *    SL_ERR_MEMORY_ALLOC_FAILED
    *    SL_ERR_WH_DIAG_BUFFER_INVALID_BUFFER_TYPE
    */
    SL_WH_DIAGNOSTIC_BUFFER_UNREGISTER,

   /*
    * SL_WH_DIAGNOSTIC_BUFFER_RELEASE
    * Desc:
    *   Requests that the IOC release the buffer of the specified type that was
    *   previously posted.  This returns ownership of the buffer back to the
    *   host, allowing applications to read the buffer without having data
    *   changed by the IOC.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_WH_DD_DIAGNOSTIC_BUFFER_RELEASE_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_RELEASE_S
    * Output:
    *   pData: SL_WH_DD_DIAGNOSTIC_BUFFER_RELEASER_T
    *   dataSize: SL_WH_DD_DIAGNOSTIC_BUFFER_RELEASE_S
    * StoreLib Error Codes:
    *    SL_SUCCESS
    *    SL_ERR_CMD_FAILED_BY_OS
    *    SL_ERR_CMD_TIMEDOUT
    *    SL_ERR_MEMORY_ALLOC_FAILED
    *    SL_ERR_WH_DIAG_BUFFER_INVALID_BUFFER_TYPE
    */
    SL_WH_DIAGNOSTIC_BUFFER_RELEASE,

   /*
    * SL_WH_GET_DFF_HEALTH_INFO
    * Desc:
    *   Get DFF heatlh information for all DFFs
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_WH_DFF_HEALTH_INFO_T
    *   dataSize: SL_WH_DFF_HEALTH_INFO_S
    * Output:
    *   pData: Pointer to buffer of the type SL_WH_DFF_HEALTH_INFO_T
    *   dataSize: SL_WH_DFF_HEALTH_INFO_S
    * StoreLib Error Codes:
    *   SL_SUCCESS
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_MEMORY_ALLOC_FAILED
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_NULL_DATA_PTR
    */
    SL_WH_GET_DFF_HEALTH_INFO,

    /*
     * SL_WH_QUERY_PANIC
     * Desc:
     *   Query a specific SSD to determine if it has any Panic Logs. There can
     *   be up to 4 Panic Logs stored in the SSD by the SSD Firmware. The Panic
     *   Logs are a Binary File that can be collected and sent to SandForce for
     *   analysis.
     *
     * Input:
     *   ctrlId: Controller Id
     *   pdRef.deviceID: Slot number of the physical drive
     *   pData: Pointer to empty buffer of size SL_WH_DD_PANIC_INFO_S
     *   dataSize: SL_WH_DD_PANIC_INFO_S
     * Output:
     *   pData: Pointer to buffer of the type SL_WH_DD_PANIC_INFO_T
     *   dataSize: SL_WH_DD_PANIC_INFO_S
     * StoreLib Error Codes:
     *   SL_SUCCESS
     *   SL_ERR_MEMORY_ALLOC_FAILED
     */
     SL_WH_QUERY_PANIC,

     /*
      * SL_WH_EXTRACT_PANIC
      * Desc:
      *   Extract the specified Panic Log
      *
      * Input:
      *   ctrlId: Controller Id
      *   pdRef.deviceID: Slot number of the physical drive
      *   cmdParam_1b[0]: Panic Entry to extract.  There are up to 4 panic entries.
      *                   This parameter will be 0-3
      *   pData: Pointer to empty buffer, at least as large as what is returned
      *          by the SL_WH_QUERY_PANIC command
      *   dataSize: Size of the Panic Buffer(pData).
      * Output:
      *   pData: Pointer to data buffer allocated by application to hold the
      *          panic information
      *   dataSize: Size of the Panic Buffer. This buffer should be at least as
      *             large as what is returned by the SL_WH_QUERY_PANIC command
      * StoreLib Error Codes:
      *   SL_SUCCESS
      *   SL_ERR_MEMORY_ALLOC_FAILED
      */
      SL_WH_EXTRACT_PANIC,

      /*
       * SL_WH_ERASE_PANIC
       * Desc:
       *   Erase the specified Panic Log. Panics should be erased from highest
       *   slot to lowest because of SF firmware bug
       *
       * Input:
       *   ctrlId: Controller Id
       *   pdRef.deviceID: Slot number of the physical drive
       *   cmdParam_1b[0]: Panic Entry to erase.  There are up to 4 panic entries.
       *                   This parameter will be 0-3
       * Output:
       *   None
       * StoreLib Error Codes:
       *   SL_SUCCESS
       */
       SL_WH_ERASE_PANIC,

      /*
       * SL_WH_DUMP_CONFIG_PAGES
       * Desc:
       *   Fetch all MPI Configuration Pages, ATA IDENTIFY DEVICE Response, ATA
       *   SMART Logs and Toolbox Diagnostic Data Upload data and write into a file.
       * Input:
       *   ctrlId: Controller Id
       *   pData : Pointer to file-name string
       * Output:
       *   pData: None
       * StoreLib Error Codes:
       *   SL_ERR_INVALID_CTRL
       * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
       */
       SL_WH_DUMP_CONFIG_PAGES,

      /*
       * SL_WH_SMART_EVENT_LOG
       * Desc:
       *   Dump SMART Event Logs into a file
       * Input:
       *   ctrlId: Controller Id
       *   pData: Pointer to SL_FILE_NAME_PARAMS_T
       *   dataSize: sizeof(SL_FILE_NAME_PARAMS_T)
       * Output:
       *   pData: None
       * StoreLib Error Codes:
       *   SL_ERR_INVALID_CTRL
       * AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
       */
       SL_WH_SMART_EVENT_LOG,

       /*
        * SL_WH_SET_MASTER_TRIGGER
        * Desc:
        *   Set various bits as defined in the Master Trigger Flag field
        * Input:
        *   ctrlId: Controller Id
        *   masterTriggerFlag: Value defining the Master Triggers enabled for
        *                      this controller
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_SET_MASTER_TRIGGER ,

       /*
        * SL_WH_GET_MASTER_TRIGGER
        * Desc:
        *   Get various bits as defined in the Master Trigger Flag field
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   masterTriggerFlag: Value defining the Master Triggers enabled for
        *                      this controller
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_GET_MASTER_TRIGGER ,

       /*
        * SL_WH_CLEAR_MASTER_TRIGGER
        * Desc:
        *   Clear various bits as defined in the Master Trigger Flag field
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_CLEAR_MASTER_TRIGGER ,

       /*
        * SL_WH_SET_EVENT_TRIGGERS
        * Desc:
        *   Set Event Trigger values
        * Input:
        *   ctrlId:    Controller Id
        *   pData:     SL_WH_EVENT_TRIGGERS_T
        *   dataSize:  SL_WH_EVENT_TRIGGERS_S
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_SET_EVENT_TRIGGERS ,

       /*
        * SL_WH_GET_EVENT_TRIGGERS
        * Desc:
        *   Get Event Trigger values
        * Input:
        *   ctrlId:    Controller Id
        *   pData:     SL_WH_EVENT_TRIGGERS_T
        *   dataSize:  SL_WH_EVENT_TRIGGERS_S
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_GET_EVENT_TRIGGERS ,

       /*
        * SL_WH_CLEAR_EVENT_TRIGGERS
        * Desc:
        *   Have the OS Driver clear all the Event Triggers defined for this
        *   controller
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_CLEAR_EVENT_TRIGGERS ,

       /*
        * SL_WH_SET_SCSI_TRIGGERS
        * Desc:
        *   Set SCSI Trigger values
        * Input:
        *   ctrlId:    Controller Id
        *   pData:     SL_WH_SCSI_TRIGGERS_T
        *   dataSize:  SL_WH_SCSI_TRIGGERS_S
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_SET_SCSI_TRIGGERS ,

       /*
        * SL_WH_GET_SCSI_TRIGGERS
        * Desc:
        *   Get SCSI Trigger values
        * Input:
        *   ctrlId:    Controller Id
        * Output:
        *   pData:     SL_WH_EVENT_TRIGGERS_T
        *   dataSize:  SL_WH_EVENT_TRIGGERS_S
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_GET_SCSI_TRIGGERS ,

       /*
        * SL_WH_CLEAR_SCSI_TRIGGERS
        * Desc:
        *   Have the OS Driver clear all the SCSI Triggers defined for this
        *   controller
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_CLEAR_SCSI_TRIGGERS ,

       /*
        * SL_WH_SET_MPI_TRIGGERS
        * Desc:
        *   Set MPI Trigger values
        * Input:
        *   ctrlId:    Controller Id
        *   pData:     SL_WH_MPI_TRIGGERS_T
        *   dataSize:  SL_WH_MPI_TRIGGERS_S
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_SET_MPI_TRIGGERS ,

       /*
        * SL_WH_GET_MPI_TRIGGERS
        * Desc:
        *   Get MPI Trigger values
        * Input:
        *   ctrlId:    Controller Id
        * Output:
        *   pData:     SL_WH_MPI_TRIGGERS_T
        *   dataSize:  SL_WH_MPI_TRIGGERS_S
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_GET_MPI_TRIGGERS ,

       /*
        * SL_WH_CLEAR_MPI_TRIGGERS
        * Desc:
        *   Have the OS Driver clear all the MPI Triggers defined for this
        *   controller
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   None
        * StoreLib Error Codes:
        *   SL_ERR_INVALID_CTRL
        *   AVAGO SAS IT/IR Controllers: SL_ERR_INVALID_CMD
        *   SL_SUCCESS
        *   SL_ERR_MEMORY_ALLOC_FAILED
        */
       SL_WH_CLEAR_MPI_TRIGGERS ,

       /*
        * SL_WH_PFK_SAFEID_GET
        * Desc:
        *   The controller will be queried for SAFE ID
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   pData: NULL terminated SAFE id string
        * StoreLib Error Codes:
        *    SL_ERR_INCORRECT_DATA_SIZE
        *    SL_ERR_CMD_FAILED_BY_OS
        *    SL_ERR_MEMORY_ALLOC_FAILED
        */
        SL_WH_PFK_SAFEID_GET ,

       /*
        * SL_WH_PFK_FEATURE_GET
        * Desc:
        *   Gets premium features list
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   pData: Pointer to structure MR_PF_LIST
        * StoreLib Error Codes:
        *    SL_ERR_INCORRECT_DATA_SIZE
        *    SL_ERR_CMD_FAILED_BY_OS
        *    SL_ERR_MEMORY_ALLOC_FAILED
        */
        SL_WH_PFK_FEATURE_GET ,

       /*
        * SL_WH_ACTIVATE_LICENSE
        * Desc:
        *   Sends feature key/license key to the firmware sent from the
        *   application
        * Input:
        *   ctrlId: Controller Id
        *   pData: Pointer to structure MR_PF_FEATURE_SET_PARAMS
        * StoreLib Error Codes:
        *    SL_ERR_INCORRECT_DATA_SIZE
        *    SL_ERR_CMD_FAILED_BY_OS
        *    SL_ERR_MEMORY_ALLOC_FAILED
        */
        SL_WH_ACTIVATE_LICENSE ,

       /*
        * SL_WH_RECOVER_VOLUME
        * Desc:
        *   Recovers locked volume
        * Input:
        *   ctrlId: Controller Id
        * Output:
        *   None
        * StoreLib Error Codes:
        *    SL_SUCCESS
        *    SL_ERR_CMD_FAILED_BY_OS
        *    SL_ERR_MEMORY_ALLOC_FAILED
        *    SL_ERR_INVALID_CMD
        */
       SL_WH_RECOVER_VOLUME ,

       /*
        * SL_WH_HEALTH
        * Desc:
        *   Provide Health information of the WD controller
        * Input:
        *   ctrlId: Controller Id
		*   pData: Pointer to buffer of the type SL_WH_CTRL_HEALTH_T
		*   dataSize: SL_WH_DD_INFO_S
		* Output:
		*   pData: SL_WH_CTRL_HEALTH_T
		*   dataSize: SL_WH_CTRL_HEALTH_S
        * StoreLib Error Codes:
        *    SL_SUCCESS
        *    SL_ERR_CMD_FAILED_BY_OS
        *    SL_ERR_MEMORY_ALLOC_FAILED
        *    SL_ERR_INVALID_CMD
        */
       SL_WH_GET_HEALTH_INFO ,


    //End
    //@@@@@ Always add new commands at the end @@@@@//
};
/****************************************************************************
*
*        CCOH COMMANDS
*
****************************************************************************/
enum SL_CCOH_CMD
{
    /*
    * SL_GET_CG_LIMITS
    * Desc:
    *   Send IOCTL to CCoH Cfg Driver to get CG limits.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to the buffer SL_CCOH_CG_LIMITS_T struct
    *   dataSize: sizeof(SL_CCOH_CG_LIMITS_T)
    * Output:
    *   pData: Pointer to the buffer SL_CCOH_CG_LIMITS_T struct
    *   dataSize: sizeof(SL_CCOH_CG_LIMITS_T)
    * StoreLib Error Codes:
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    */
    SL_GET_CG_LIMITS = 0,

   /*
    * SL_GET_CACHE_DEVS
    * Desc:
    *   Gets the list of cache devices present in the system.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_CCOH_DEV_LIST_T struct
    *   dataSize: sizeof(SL_CCOH_DEV_LIST_T)
    * Output:
    *   pData: SL_CCOH_DEV_LIST_T
    *   dataSize: sizeof(SL_CCOH_DEV_LIST_T)
    * StoreLib Error Codes:
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    */
    SL_GET_CACHE_DEVS,

    /*
    * SL_GET_VIRTUAL_DRIVES
    * Desc:
    *   ctrlId: Controller Id
    *   Gets the list of virtual drives present in the system.
    *   pData: Pointer to buffer of SL_CCOH_DEV_LIST_T struct
    *   dataSize: sizeof(SL_CCOH_DEV_LIST_T)
    * Output:
    *   pData: SL_CCOH_DEV_LIST_T
    *   dataSize: sizeof(SL_CCOH_DEV_LIST_T)
    * StoreLib Error Codes:
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    */
    SL_GET_VIRTUAL_DRIVES,

    /*
    * SL_GET_CONFIG
    * Desc:
    *   Gets the CCoH configuration.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_CCOH_CONFIG_T struct
    *   dataSize: sizeof(SL_CCOH_CONFIG_T)
    * Output:
    *   pData: SL_CCOH_CONFIG_T
    *   dataSize: sizeof(SL_CCOH_CONFIG_T)
    * StoreLib Error Codes:
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    *   SL_ERR_NO_VALID_CFG
    *   SL_ERR_CCOH_FW_VERSION_NOT_SUPPORTED
    */
    SL_GET_CONFIG,

    /*
    * SL_ACTIVATE_CONFIG
    * Desc:
    *   Activate the CCoH config.
    *   Send IOCTL's to CCoH Cfg Driver to create
    *   in-memory configuration in the host.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_CCOH_CONFIG_T struct
    *   dataSize: sizeof(SL_CCOH_CONFIG_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    *   SL_ERR_MEMORY_ALLOC_FAILED
    *   SL_ERR_CCOH_LICENSE_NOT_FOUND
    */
    SL_ACTIVATE_CONFIG,

   /*
    * SL_CLR_CONFIG
    * Desc:
    *   Clear the CCoH Config.
    *   Gets the CCoH Config data from the warpdrive.
    *   Clears the in-memory configuration by issuing IOCTL
    *   to CCoH Cfg driver to remove VD/CDEV/CG.
    * Input:
    *   Updates the config pages and writes to the warpdrive.
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of SL_CCOH_CONFIG_T struct
    *   dataSize: sizeof(SL_CCOH_CONFIG_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    */
    SL_CLR_CONFIG,

    /*
    * SL_ADD_CACHE_DEVICE
    * Desc:
    *   Add a cache device to the cache group.
    *   Read the config page from WH.
    *   Update the config page and write to WH.
    *   Send IOCTL to CCoH Cfg Driver to add cache device.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to the buffer SL_CCOH_DEV_T struct
    *   dataSize: sizeof(SL_CCOH_DEV_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    *   SL_ERR_MEMORY_ALLOC_FAILED
    *   SL_ERR_CCOH_DEV_BUSY
    *   SL_ERR_CCOH_LICENSE_NOT_FOUND
    */
    SL_ADD_CACHE_DEVICE,

    /*
    * SL_REMOVE_CACHE_DEVICE
    * Desc:
    *   Remove cache device from a cache group.
    *   Send IOCTL to CCoH Cfg Driver to remove cache device.
    *   Read the config page from WH.
    *   Update the config page and write to WH.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to the buffer SL_CCOH_DEV_T struct
    *   dataSize: sizeof(SL_CCOH_DEV_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    *   SL_ERR_CCOH_DEV_BUSY
    *   SL_ERR_DEV_NOT_EXIST
    *   SL_ERR_VDS_CONFIGURED
    */
    SL_REMOVE_CACHE_DEVICE,

    /*
    * SL_ADD_VIRTUAL_DRIVE
    * Desc:
    *   Add virtual drive to the cache group.
    *   Read the config page from WH.
    *   Update the config page and write to WH.
    *   Send IOCTL to CCoH Cfg Driver to add virtual device.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to the buffer SL_CCOH_DEV_T struct
    *   dataSize: sizeof(SL_CCOH_DEV_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    *   SL_ERR_NO_CACHE_DEV
    *   SL_ERR_ADD_VD_FAILED
    *   SL_ERR_CCOH_DEV_BUSY
    */
    SL_ADD_VIRTUAL_DRIVE,

    /*
    * SL_REMOVE_VIRTUAL_DRIVE
    * Desc:
    *   Remove virtual drivee from a cache group.
    *   Send IOCTL to CCoH Cfg Driver to remove cache device.
    *   Read the config page from WH.
    *   Update the config page and write to WH.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to the buffer SL_CCOH_DEV_T struct
    *   dataSize: sizeof(SL_CCOH_DEV_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    *   SL_ERR_NO_CACHE_DEV
    *   SL_ERR_REMOVE_VD_FAILED
    *   SL_ERR_CCOH_DEV_BUSY
    */
    SL_REMOVE_VIRTUAL_DRIVE,

    /*
    * SL_GET_CG_STATS
    * Desc:
    *   Send IOCTL to CCoH Cfg Driver to get CG statistics.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to the buffer SL_CCOH_CG_STATS_T struct
    *   dataSize: sizeof(SL_CCOH_CG_STATS_T)
    * Output:
    *   pData: Pointer to the buffer SL_CCOH_CG_STATS_T struct
    *   dataSize: sizeof(SL_CCOH_CG_STATS_T)
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    */
    SL_GET_CG_STATS,

   /*
    * SL_DEV_STATE_CHANGE
    * Desc:
    *   Send IOCTL to CCoH Cfg Driver to intimate about the
    *   device(VD/CDEV) state change.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to the buffer SL_CCOH_DEV_STATE_CHANGE_T struct
    *   dataSize: sizeof(SL_CCOH_DEV_STATE_CHANGE_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    */
    SL_DEV_STATE_CHANGE,

   /*
    * SL_ONLINE_VD
    * Desc:
    *   When the VD which was configured but was OFFLINE, comes Online, the
    *   application gets the event READY_FOR_ONLINE from the driver.
    *   This command is expected to be sent when the application recieves the
    *   event READY_FOR_ONLINE for that VD.
    *   This command removes the dev mapper entry and recreates the dm node
    *
    *
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to the buffer SL_CCOH_DEV_T struct
    *   dataSize: sizeof(SL_CCOH_DEV_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    *   SL_ERR_NO_CACHE_DEV
    *   SL_ERR_REMOVE_VD_FAILED
    *   SL_ERR_CCOH_DEV_BUSY
    * Output:
    *   None
    */
    SL_ONLINE_VD,
   /*
    * SL_GET_LICENSE_STATUS
    * Desc:
    *   Returns whether a valid CCoH Licence is installed on the system.
    *
    *
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   return value: SL_SUCCESS if command success and license found
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    *   SL_ERR_CCOH_LICENSE_NOT_FOUND
    */
    SL_GET_LICENSE_STATUS,
   /*
    * SL_GET_DEV_STATS
    * Desc:
    *   Returns Statistics for a given Device
    *
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to SL_CCOH_DEV_STATS_T struct
    *   dataSize: sizeof(SL_CCOH_DEV_STATS_T)
    * Output:
    *   return value: SL_SUCCESS if command success and VD Stats populated
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_CCOH_NOT_INITIALIZED
    */
    SL_GET_DEV_STATS,
    /*
    * SL_ADD_CACHE_DEVICE_WITH_MAP
    * Desc:
    *   Add a cache device to the cache group with Map.
    *   Read the config page from WH.
    *   Update the config page and write to WH.
    *   Send IOCTL to CCoH Cfg Driver to add cache device.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to the buffer SL_CCOH_DEV_T struct
    *   dataSize: sizeof(SL_CCOH_DEV_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_NULL_DATA_PTR
    *   SL_ERR_INCORRECT_DATA_SIZE

    *   SL_ERR_CCOH_NOT_INITIALIZED
    *   SL_ERR_MEMORY_ALLOC_FAILED
    *   SL_ERR_CCOH_DEV_BUSY
	*   SL_ERR_CCOH_LICENSE_NOT_FOUND
    */
    SL_ADD_CACHE_DEVICE_WITH_MAP

};

/****************************************************************************
*
*        DATA STRUCTURE DEFINITIONS
*
****************************************************************************/
/****************************************************************************
*
*        STORELIB VERSION INFORMATION
*
****************************************************************************/
#define SL_MINOR_VER_LEN    7
#define SL_MAJOR_VER_LEN    8
typedef struct _SL_LIB_VER_T
{
    S8     minor[SL_MINOR_VER_LEN];
    S8     major[SL_MAJOR_VER_LEN];
} SL_LIB_VER_T;
#define SL_LIB_VER_S sizeof(SL_LIB_VER_T)


/****************************************************************************
*
*        STORELIB EXTENTED VERSION INFORMATION
*
****************************************************************************/
#define SL_EXT_MINOR_VER_LEN    4
#define SL_EXT_MAJOR_VER_LEN    4
#define SL_EXT_PROGRAM_VER_LEN  4
#define SL_EXT_PRODUCT_VER_LEN  4
#define SL_EXT_OEM_VER_LEN      4
#define SL_EXT_PATCH_VER_LEN    4
#define SL_EXT_DEBUG_VER_LEN    4
 
 
typedef struct _SL_LIB_VER_EXT_T
{
    S8     major[SL_EXT_MAJOR_VER_LEN];
    S8     program[SL_EXT_PROGRAM_VER_LEN];
    S8     minor[SL_EXT_MINOR_VER_LEN];
    S8     product[SL_EXT_PRODUCT_VER_LEN];
    S8     oem[SL_EXT_OEM_VER_LEN];
    S8     patch[SL_EXT_PATCH_VER_LEN];
    S8     debug[SL_EXT_DEBUG_VER_LEN];
} SL_LIB_VER_EXT_T;
#define SL_LIB_VER_EXT_S sizeof(SL_LIB_VER_EXT_T)


/****************************************************************************
*
*        CONTROLLER LIST INFORMATION
*
****************************************************************************/
//lib type definition
#define SL_LIB_TYPE_STORELIB                0
#define SL_LIB_TYPE_STORELIBIR              1
#define SL_LIB_TYPE_STORELIBIR_2            2
#define SL_LIB_TYPE_STORELIB_OOB            3
#define SL_LIB_TYPE_STORELIBIR_3            4
#define SL_LIB_TYPE_STORELIBIT              5

//flags definition
#define SL_FLAGS_NONE                       0
#define SL_FLAGS_SIMULATION                 1
#define SL_FLAGS_OOB                        2
#define SL_FLAGS_MCTP                       3

//mask for libType
#define     SL_LIB_TYPE_MASK                0xFF000000
//mask for ctrlId
#define     SL_CTRL_ID_MASK                 0x00FFFFFF
typedef struct _SL_CTRL_LIST_T
{
    U16     count;
    U8      flags;      //values as defined above; set to SL_FLAGS_OOB for OOB
    U8      reserved;
    union
    {
        U32     ctrlId[SL_MAX_CONTROLLERS];
        struct
        {
#if defined(SPARC) || defined(_PPC_STORELIB)
            U32 libType:8;  //Upper 1 byte: type of library - StoreLib/StoreLibIR - use hash defined from above
            U32 ctrlId:24;  //Lower 3 bytes: controller Id - 0 based
#else

            U32 ctrlId:24;  //Lower 3 bytes: controller Id - 0 based
            U32 libType:8;  //Upper 1 byte: type of library - StoreLib/StoreLibIR - use hash defined from above
#endif
        } cl[SL_MAX_CONTROLLERS];
		struct
		{
			U8 channel;       //channel where controller is connected
			U8 deviceAddress; //device address where controller is connected
#if defined(SPARC) || defined(_PPC_STORELIB)
			U8 reserved:7;
			U8 reDiscover:1;
#else
			U8 reDiscover:1;
			U8 reserved:7;
#endif
			U8 libType;       //will be filled in as SL_LIB_TYPE_STORELIB_OOB for OOB
		} oob_cl[SL_MAX_CONTROLLERS];
    };
} SL_CTRL_LIST_T;
#define SL_CTRL_LIST_S sizeof(SL_CTRL_LIST_T)

/****************************************************************************
*
*        PD Allowed Operations
*
****************************************************************************/

typedef MR_PD_ALLOWED_OPS SL_PD_ALLOWED_OPER_T;

/*
typedef struct _SL_PD_ALLOWED_OPER_T
{
    U32 makeOnline:1;
    U32 makeOffline:1;
    U32 makeFailed:1;
    U32 makeBad:1;
    U32 makeUncofigured:1;
    U32 makeSpare:1;
    U32 removeSpare:1;
    U32 replaceMissing:1;
    U32 markMissing:1;
    U32 startRebuild:1;
    U32 stopRebuild:1;
    U32 locate:1;
    U32 pdClear:1;
    U32 reserved:19;
} SL_PD_ALLOWED_OPER_T;
*/
#define SL_PD_ALLOWED_OPER_S sizeof(SL_PD_ALLOWED_OPER_T)
#define SL_PD_BLOCK_SIZE    512

/****************************************************************************
*
*        LD Allowed Operations
*
****************************************************************************/
typedef MR_LD_ALLOWED_OPS SL_LD_ALLOWED_OPER_T;

/*
typedef struct _SL_LD_ALLOWED_OPER_T
{
    U32 startFGI:1;
    U32 stopFGI:1;
    U32 startCC:1;
    U32 stopCC:1;
    U32 startRecon:1; //1 when no Spanning, Single LD array, No long operation
    U32 deleteLD:1;
    U32 reserved:26;
} SL_LD_ALLOWED_OPER_T;
*/
#define SL_LD_ALLOWED_OPER_S sizeof(SL_LD_ALLOWED_OPER_T)

typedef struct _SL_ALLOWED_OPER_WITH_ID_T
{
    U32 targetId; //logical drive id
    SL_LD_ALLOWED_OPER_T allowedOper;
} SL_ALLOWED_OPER_WITH_ID_T;
#define SL_ALLOWED_OPER_WITH_ID_S sizeof(SL_ALLOWED_OPER_WITH_ID_T)

typedef struct _SL_ALL_LDS_ALLOWED_OPER_T
{
    U32 count;
    SL_ALLOWED_OPER_WITH_ID_T ldAllowedOper[MAX_LOGICAL_DRIVES];
} SL_ALL_LDS_ALLOWED_OPER_T;
#define SL_ALL_LDS_ALLOWED_OPER_S sizeof(SL_ALL_LDS_ALLOWED_OPER_T)

/****************************************************************************
*
*        Config Allowed Operations
*
****************************************************************************/

typedef MR_CONFIG_ALLOWED_OPS SL_CONFIG_ALLOWED_OPER_T;
/*
typedef struct _SL_CONFIG_ALLOWED_OPER_T
{
    U32 addConfig:1;
    U32 clearConfig:1;
    U32 reserved:30;
} SL_CONFIG_ALLOWED_OPER_T;
*/
#define SL_CONFIG_ALLOWED_OPER_S sizeof(SL_CONFIG_ALLOWED_OPER_T)

/****************************************************************************
*
*        Dump SMART Event Logs
*
****************************************************************************/
#define MAX_PATH_LENGTH  256
typedef struct _SL_SMART_EVENT_LOG_T
{
    U8 SlotFlag;             /* True if request is for all slots, else false */
    U8 SlotNum;              /*
                              * Specify slot number here when request is for
                              * a specific slot
                              */
    U8 Path[MAX_PATH_LENGTH];/* Path where the dump files need to be created */
} SL_SMART_EVENT_LOG_T;

#define SL_SMART_EVENT_LOG_S sizeof(SL_SMART_EVENT_LOG_T)

#define SL_SLOT_FLAG_ALL    1
#define SL_SLOT_FLAG_SINGLE 0

/****************************************************************************
*
*        LOGICAL DRIVE DISTRIBUTION
*
****************************************************************************/
#define SL_MAX_OS_DISK_NAME_LEN     16
typedef struct _SL_DRIVE_DISTRIBUTION_T
{
    S32 mappedDevicesCount;
    struct {
        union
        {
            U32 ldNumber;   //logical drive #
            U16 deviceId;   //deviceId for the PD
        };
        U32 osDriveNumber;
        U32 osDriveChannel;
        U32 osDriveTarget;
        U8  osDiskName[SL_MAX_OS_DISK_NAME_LEN]; //name of the os disk e.g. Win - Disk 1; Linux - /dev/sda
        U32 reserved;
    } mappedDriveInfo[SL_MAX_OS_DRIVE_COUNT];
} SL_DRIVE_DISTRIBUTION_T;
#define SL_DRIVE_DISTRIBUTION_S sizeof(SL_DRIVE_DISTRIBUTION_T)

#define SL_MAX_OS_DISK_NAME_LEN_EX 256

typedef struct _SL_DRIVE_DISTRIBUTION_EX_T
{
   S32 mappedDevicesCount;
   struct {
       union
       {
         U32 ldNumber; //logical drive #
         U16 deviceId; //deviceId for the PD
       };
       U32 osDriveNumber;
       U32 osDriveChannel;
       U32 osDriveTarget;
       U8  osDiskName[SL_MAX_OS_DISK_NAME_LEN_EX]; //name of the os disk e.g. Win - Disk 1; Linux - /dev/sda, VMware ESX(i) - naa.60022190b40c3d0011a4df4d04e8e2f0
       U32 reserved;
       } mappedDriveInfo[SL_MAX_OS_DRIVE_COUNT];
 }SL_DRIVE_DISTRIBUTION_EX_T;
#define SL_DRIVE_DISTRIBUTION_EX_S sizeof(SL_DRIVE_DISTRIBUTION_EX_T)



/****************************************************************************
*
*        ARRAYS
*
****************************************************************************/
typedef struct _SL_ARRAY_LIST_T
{
    U8      count;                         // number of arrays
    U16     ref[MAX_ARRAYS_DEDICATED];     // array reference
} SL_ARRAY_LIST_T;
#define SL_ARRAY_LIST_S sizeof(SL_ARRAY_LIST_T)

/****************************************************************************
*
*        SCSI PASSTHRU
*
****************************************************************************/
typedef enum _SL_CMD_OP {
    SL_CMD_OP_LD_SCSI     = 0,         /* issue a SCSI CDB to a logical drive */
    SL_CMD_OP_PD_SCSI     = 1,         /* issue a SCSI CDB to a physical drive */
} SL_CMD_OP;

#define SL_SCSI_MAX_SENSE_LENGTH    32
#define SL_PCIE_MAX_ERR_RSP_LENGTH    32
typedef struct _SL_SCSI_PASSTHRU_T
{
    U16     targetId;               /* target Identifier */
    U8      cmd;                    /* command (type SL_CMD_OP) */
    U8      lun;                    /* lun */
    U8      dir;                    /* dir of the command */
    U8      reserved[3];
    U16     timeout;                /* timeout in secs */
    U8      scsiStatus;             /* returned SCSI status for CDB commands */
    U8      cdbLength;              /* SCSI CDB length */
    U8      cdb[16];
    U8      pRequestSenseData[SL_SCSI_MAX_SENSE_LENGTH];  /* request sense data*/
    U32     dataSize;               /* length of data */
    U8      data[1];                /* data buffer of dynamic size to hold requested data. Data starts here */
} SL_SCSI_PASSTHRU_T;
#define SL_SCSI_PASSTHRU_S (sizeof(SL_SCSI_PASSTHRU_T) - sizeof(U8))


typedef enum _SL_SMP_STAT {
    SL_SMP_STAT_OK                                = 0x00,  /* Command completed successfully */
    SL_SMP_STAT_UNKNOWN_ERROR                     = 0x01,  /* Unknown error */
    SL_SMP_STAT_INVALID_FRAME                     = 0x02,  /* Invalid frame */
    SL_SMP_STAT_UTC_BAD_DEST                      = 0x03,  /* Unable to Connect (Bad Destination) */
    SL_SMP_STAT_UTC_BREAK_RECEIVED                = 0x04,  /* Unable to Connect (Break Received) */
    SL_SMP_STAT_UTC_CONNECT_RATE_NOT_SUPPORTED    = 0x05,  /* Unable to Connect (Connect Rate Not Supported) */
    SL_SMP_STAT_UTC_PORT_LAYER_REQUEST            = 0x06,  /* Unable to Connect (Port Layer Request) */
    SL_SMP_STAT_UTC_PROTOCOL_NOT_SUPPORTED        = 0x07,  /* Unable to Connect (Protocol Not Supported) */
    SL_SMP_STAT_UTC_STP_RESOURCES_BUSY            = 0x08,  /* Unable to Connect (STP Resources Busy) */
    SL_SMP_STAT_UTC_WRONG_DESTINATION             = 0x09,  /* Unable to Connect (Wrong Destination) */
    SL_SMP_STAT_SHORT_INFORMATION_UNIT            = 0x0A,  /* Information Unit too short */
    SL_SMP_STAT_LONG_INFORMATION_UNIT             = 0x0B,  /* Information Unit too long */
    SL_SMP_STAT_XFER_RDY_INCORRECT_WRITE_DATA     = 0x0C,  /* XFER_RDY Incorrect Write Data */
    SL_SMP_STAT_XFER_RDY_REQUEST_OFFSET_ERROR     = 0x0D,  /* XFER_RDY Request Offset Error */
    SL_SMP_STAT_XFER_RDY_NOT_EXPECTED             = 0x0E,  /* XFER_RDY Not Expected */
    SL_SMP_STAT_DATA_INCORRECT_DATA_LENGTH        = 0x0F,  /* DATA Incorrect Data Length */
    SL_SMP_STAT_DATA_TOO_MUCH_READ_DATA           = 0x10,  /* DATA Too much read data */
    SL_SMP_STAT_DATA_OFFSET_ERROR                 = 0x11,  /* DATA Offset Error */
    SL_SMP_STAT_SDSF_NAK_RECEIVED                 = 0x12,  /* Service Delivery Subsystem Failure – NAK received */
    SL_SMP_STAT_SDSF_CONNECTION_FAILED            = 0x13,  /* Service Delivery Subsystem Failure – Connection failed */
    SL_SMP_STAT_INITIATOR_RESPONSE_TIMEOUT        = 0x14   /* Initiator response timeout */
} SL_SMP_STAT;

typedef struct _SL_SMP_PASSTHRU_T {
    U64         sasAddr;            /* device SAS address (0 = use hdr.targetID) */
    U8          dir;                /* dir of the command (SL_DIR) */
    U8          connectionStatus;   /* SAS Connection Status (SL_SMP_STAT) */
    U16         timeout;            /* command timeout (in seconds) */
    U32         reserved;
    U32         responseLen;        /* length of the response buffer */
    U32         requestLen;         /* length of the request buffer */
    U8          responseBuffer[1];  /* data buffer of dynamic size to hold SMP response buffer */
    U8          requestBuffer[1];   /* data buffer of dynamic size to hold SMP request buffer */
} SL_SMP_PASSTHRU_T;
//Total size of SL_SMP_PASSTHRU_T = SL_SMP_PASSTHRU_S + requestLen + responseLen
#define SL_SMP_PASSTHRU_S (sizeof(SL_SMP_PASSTHRU_T) - 2 * sizeof(U8))
//NOTE: In order to access requestBuffer, use the macro SL_ADDR_OF_SMP_REQUEST_BUFF defined below
#define SL_ADDR_OF_SMP_REQUEST_BUFF(s/*pointer to variable of SL_SMP_PASSTHRU_T type*/) (s->responseBuffer + s->responseLen)

// STP Flags
#define CSMI_SAS_STP_READ           0x00000001
#define CSMI_SAS_STP_WRITE          0x00000002
#define CSMI_SAS_STP_UNSPECIFIED    0x00000004
#define CSMI_SAS_STP_PIO            0x00000010
#define CSMI_SAS_STP_DMA            0x00000020
#define CSMI_SAS_STP_PACKET         0x00000040
#define CSMI_SAS_STP_DMA_QUEUED     0x00000080
#define CSMI_SAS_STP_EXECUTE_DIAG   0x00000100
#define CSMI_SAS_STP_RESET_DEVICE   0x00000200

typedef struct _SL_STP_PASSTHRU_T {
    U8          targetId;           /* Device Id of the SATA device */
    U8          dir;                /* dir of the command (SL_DIR) */
    U16         timeout;            /* command timeout (in seconds) */
    U32         stpFlags;           /* STP flags - CSMI_SAS_STP_*** defined above */
    U16         fis[10];            /* STP FIS block */
    U8          connectionStatus;   /* SAS Connection Status (SL_SMP_STAT) */
    U8          reserved[3];
    U32         fisResponseLen;     /* length of the FIS response buffer */
    U32         dataLen;            /* length of the data buffer */
    U8          fisResponseBuffer[1];   /* data buffer of dynamic size to hold FIS response buffer */
    U8          data[1];            /* data buffer of dynamic size to hold STP data buffer */
} SL_STP_PASSTHRU_T;
//Total size of SL_STP_PASSTHRU_T = SL_STP_PASSTHRU_S + fisResponseLen + dataLen
#define SL_STP_PASSTHRU_S (sizeof(SL_STP_PASSTHRU_T) - 2 * sizeof(U8))
//NOTE: In order to access data, use the macro SL_ADDR_OF_STP_DATA_BUFF defined below
#define SL_ADDR_OF_STP_DATA_BUFF(s/*pointer to variable of SL_STP_PASSTHRU_T type*/) (s->fisResponseBuffer + s->fisResponseLen)

#define SL_NVME_MAX_MPT_ERROR_REPLY 48
#define SL_NVME_MAX_ERR_RSP_LENGTH  32
#define SL_NVME_MAX_CMD_LENGTH     128

typedef struct _SL_NVME_ENCAPSULATION_T
{
    U16     targetId;               /* target Identifier */
    U8      dir;                    /* dir of the command  (SL_DIR)*/
                                    /* Read/write/Bi-directional */
    U8      reserved[3];
    U16     timeout;                /* timeout in secs */

/* Length of NVMe command, not including length of any scatter/gather
 *  entries.  The value MUST be a multiple of 4. The NVMe command will NOT
 *  include the length of any scatter/gather entries located elsewhere, such 
 *  as those reference by PRP List or SGL descriptors.
*/  
    U16     encapsulatedCmdLength;  

/* Encapsulated NVME command
 *  Layers below this library will be responsible for filling in the 
 *  appropriate PRP/SGL information. This means that the Data Pointer [DPTR]
 *  field of the NVMe encapsulated command (CDW6-CDW9) MUST be zeroed in the
 * following structure. This equates to encapsulatedNVMeCmd[39..24]
 * 
 * Note: Current NVMe commands are all 64 bytes in length. Following buffer
 *       has room for growth, the encapsulatedCmdLength will define how much 
 *       of this buffer has valid data 
*/ 
    U8      encapsulatedNVMeCmd[SL_NVME_MAX_CMD_LENGTH];
  
/* Flags that affect processing of request message 
 * This structure will be a mirror of Flags field in the NVMe 
 * Encapsulated Request Message
 * Bitfields
 *     Bits[1:0] Data Direction
 *                 0b00 = No Data Transfer
 *                 0b01 = Write
 *                 0b10 = Read
 *                 0b11 = Bidirectional Data Transfer
 *     Bits[3:2] Error Response Address Space
 *                 0 = System (PCI)  (Host interface MUST set these bits to 0
 *                 All other values are reserved 
 *     Bit 4     Submission Queue Type 
 *                 0 = I/O Submission Queue
 *                 1 = Admin Submission Queue
 *     Bit 5     Force Admin Error Response
 *                 0 = Write the NVMe completion queue entry to the error 
 *                     response buffer only when a non-successful response 
 *                     is returned by the NVMe device
 *                 1 = Write the NVMe completion queue entry to the error
 *                     response buffer when either successful or
 *                     non-successful response is returned by the
 *                     NVMe device.  
 *               NOTE: Bit 5 applies only to requests submitted to the Admin
 *                     Submission queue (Bit 4 == 1)
 *    ALL OTHER BITS ARE RESERVED AND MUST BE SET TO 0 
*/ 
    U16     encapsulatedNVMeFlags;

/* The length, in bytes, of the buffer allocated for NVMe completion queue
 * entry.  Current all NVMe completion queue entries are 16 bytes in length
*/
    U16    errorResponseAllocationLength;

/* Error Response buffer. 
 * Note: All current NVMe completion queue entries are 16 bytes in length,
 *       thus the errorResponseAllocationLength should be set to 16. 
 *       Buffer will be 32 bytes for potential future growth, at which point
 *       errorResponseAllocationLength will be adjusted accordingly
 *
 *       See explanation of bit 5 in encapsulatedNVMeFlags for case where the
 *       errorResponseData will have information even in the case of the
 *       NVMe command succeeding
*/
    U8      errorResponseData[SL_NVME_MAX_ERR_RSP_LENGTH];

/* The count of error response bytes received.
 * If a successful response is included in the errorResponseData, 
 * the length returned in this field will be the length filled into the 
 * errorResponseAllocationLength field of this structure
*/
    U16     errorResponseCount;  

/* Product specific error status information, this information comes from the
 * PL FW in the NVMe Encapsulated Error Reply Message and is useful in 
 * debugging issues with invalid NVMe Encapsulated commands. 
 * Current size is 24 bytes, but extra space added to account for potential 
 * growth of the MPT error reply. 
 * 
 * This information should just be printed out when errors occur, especially 
 * the IOCStatus and IOCLogInfo data
*/
   U8       encapsulatedMPTErrorResponse[SL_NVME_MAX_MPT_ERROR_REPLY];

/* Number of bytes of data to transfer. This value must not be greater than
 * the value specified in the MaximumDataTransferSize field of PCIe Device Page 2
 *  
 * Note: dataSize MUST be a multiple of 4
 *  TBD: How would application get this MaximumDataTransferSize value?
*/
    U32     dataSize;               /* length of data */
    U8      data[1];                /* data buffer of dynamic size to hold requested data. Data starts here */
} SL_NVME_ENCAPSULATION_T;
#define SL_NVME_ENCAPSULATION_S (sizeof(SL_NVME_ENCAPSULATION_T) - sizeof(U8))


/****************************************************************************
*
*        EVENT DETAIL INFORMATION
*
****************************************************************************/
#define SL_EVT_PERSISTENT           0
#define SL_EVT_NON_PERSISTENT       1
#define SL_EVT_STORELIB_GENERATED   1
typedef struct _SL_EVENT_DETAIL_T
{
    U16 status;                 //status of the execution. SL_SUCCESS or suitable error code
    struct
    {
        U8 isNonPersistent : 1;
        U8 isStorelibGenerated :1;
        U8 reserved        : 6;
    };
    U8 reserved1;
    U32 ctrlId;                  //controller number
    U32 registrationId;         //unique registration id
    union
	{
    MR_EVT_DETAIL evtDetail;    //event details from firmware
        SL_EVT_DETAIL slEvtDetail;  //event details from Storelib
    };
} SL_EVENT_DETAIL_T;
#define SL_EVENT_DETAIL_S sizeof(SL_EVENT_DETAIL_T)

/****************************************************************************
*
*        ENCLOSURE RELATED INFORMATION
*
****************************************************************************/
//enclosure elements
enum SL_ENCL_ELEM_TYPE
{
    SL_SES_ELMT_DEVICE                          =0x01,
    SL_SES_ELMT_POWER_SUPPLY                    =0x02,
    SL_SES_ELMT_COOLING                         =0x03,
    SL_SES_ELMT_TEMPERATURE_SENSOR              =0x04,
    SL_SES_ELMT_DOOR_LOCK                       =0x05,
    SL_SES_ELMT_AUDIBLE_ALARM                   =0x06,
    SL_SES_ELMT_SES_CONTROLLER_ELECTRONICS      =0x07,
    SL_SES_ELMT_SCC_CONTROLLER_ELECTRONICS      =0x08,
    SL_SES_ELMT_NONVOLATILE_CACHE               =0x09,
    SL_SES_ELMT_INVALID_OPERATION_REASON        =0x0A,
    SL_SES_ELMT_UNINTERRUPTABLE_POWER_SUPPLY    =0x0B,
    SL_SES_ELMT_DISPLAY                         =0x0C,
    SL_SES_ELMT_KEY_PAD_ENTRY                   =0x0D,
    SL_SES_ELMT_ENCLOSURE                       =0x0E,
    SL_SES_ELMT_SCSI_TRANSCEIVER_PORT           =0x0F,
    SL_SES_ELMT_LANGUAGE                        =0x10,
    SL_SES_ELMT_COMMUNICATION_PORT              =0x11,
    SL_SES_ELMT_VOLTAGE_SENSOR                  =0x12,
    SL_SES_ELMT_CURRENT_SENSOR                  =0x13,
    SL_SES_ELMT_SCSI_TARGET_PORT                =0x14,
    SL_SES_ELMT_SCSI_INITIATOR_PORT             =0x15,
    SL_SES_ELMT_SIMPLE_SUB_ENCLOSURE            =0x16,
    SL_SES_ELMT_ARRAY_DEVICE                    =0x17,
    // Structure: Voltage Sensor
    SL_SES_ELMT_SAS_EXPANDER                    =0x18,
    SL_SES_ELMT_SAS_CONNECTOR                   =0x19,
    SL_SES_ELMT_RESERVED_START_01               =0x1A,
    SL_SES_ELMT_RESERVED_END_01                 =0x7F,
    SL_SES_ELMT_VENDOR_SPECIFIC_START_01        =0x80,
    SL_SES_ELMT_VENDOR_SPECIFIC_END_01          =0xFF
    // Structure: Voltage Sensor
    //SL_SES_ELMT_VENDOR_SPECIFIC_START_01        =0x80,
};

//Enclosure element status from SES-2 specs pg-47
enum SL_ENCL_ELEM_STATUS
{
    SL_ENCL_ELEM_STATUS_UNSUPPORTED = 0x00,    //0x0 Element is unsupported. new for SES spec.
    SL_ENCL_ELEM_STATUS_OK = 0x01,             //0x1 Element is installed and no error conditions are known.
    SL_ENCL_ELEM_STATUS_CRITICAL = 0x02,       //0x2 Critical condition is detected.
    SL_ENCL_ELEM_STATUS_NON_CRITICAL = 0x03,   //0x3 Noncritical condition is detected.
    SL_ENCL_ELEM_STATUS_UNRECOVERABLE = 0x04,  //0x4 Unrecoverable condition is detected.
    SL_ENCL_ELEM_STATUS_NOT_INSTALLED = 0x05,  //0x5 Element is not installed in enclosure.
    SL_ENCL_ELEM_STATUS_UNKNOWN = 0x06,        //0x6 Sensor has failed or element status is not available.
    SL_ENCL_ELEM_STATUS_NOT_AVAILABLE = 0x07,  //0x7 Element installed, no known errors, but the element has not been turned on or set into operation.
	SL_ENCL_ELEM_STATUS_NO_ACCESS_ALLOWED =0x08, // 0x8 The enclosure is expected to respond with “No_Access_Allowed” status
};

//Slot status
typedef struct _SL_SLOT_STATUS_T
{
    U32     sesSlotStatusPage;   //SES Status page from the enclosure
    U32     slotStatusVal;
} SL_SLOT_STATUS_T;
#define SL_SLOT_STATUS_S sizeof(SL_SLOT_STATUS_T)

//Power supply status
typedef struct _SL_POWER_SUPPLY_STATUS_T
{
    U32     sesPowerSupplyStatusPage;   //SES Status page from the enclosure
    U32     psStatusVal;
} SL_POWER_SUPPLY_STATUS_T;
#define SL_POWER_SUPPLY_STATUS_S sizeof(SL_POWER_SUPPLY_STATUS_T)

//Fan status
typedef struct _SL_FAN_STATUS_T
{
    U32     sesFanStatusPage;   //SES Status page from the enclosure
    U32     fanStatusVal;
} SL_FAN_STATUS_T;
#define SL_FAN_STATUS_S sizeof(SL_FAN_STATUS_T)

//Temp Sensor status
typedef struct _SL_TEMP_SENSOR_STATUS_T
{
    U32     sesTempSensorStatusPage;   //SES Status page from the enclosure
    U32     tempSensorStatusVal;
} SL_TEMP_SENSOR_STATUS_T;
#define SL_TEMP_SENSOR_STATUS_S sizeof(SL_TEMP_SENSOR_STATUS_T)

//Alarm status
typedef struct _SL_ALARM_STATUS_T
{
    U32     sesAlarmStatusPage;   //SES Status page from the enclosure
    U32     alarmStatusVal;
} SL_ALARM_STATUS_T;
#define SL_ALARM_STATUS_S sizeof(SL_ALARM_STATUS_T)

//SIM status
typedef struct _SL_SIM_STATUS_T
{
    U32     sesSIMPage;   //SES Status page from the enclosure
    U32     simStatusVal;
} SL_SIM_STATUS_T;
#define SL_SIM_STATUS_S sizeof(SL_SIM_STATUS_T)

//Maximum size of enclosure buffer
#define SL_MAX_ENCL_BUFFER_SIZE     2048

typedef struct _SL_ENCL_CONFIG_DPH_T
{
    U8          pageCode;           /*Page Code*/
    U8          subEnclCnt;         /*Number of Sub-Enclosures*/
    U16         pageLen;            /*Page Length*/
    U32         generationCode;     /*Generation Code*/
} SL_ENCL_CONFIG_DPH_T;
#define SL_ENCL_CONFIG_DPH_S sizeof(SL_ENCL_CONFIG_DPH_T);

typedef struct _SL_ENCL_DSRPT_HDR_T
{
    U8          serviceProcsCnt         :3; /*number of enclosure service processes*/
    U8          rsvd_1                  :1;
    U8          relEnclServiceProcsID   :3; /*relative enclosure service process identifier*/
    U8          rsvd_2                  :1;
    U8          subEnclID;                  /*subenclosure identifier*/
    U8          elmtTypeCnt;                /*number of element types supported*/
    U8          dsrptLen;                   /*enclosure descriptor length*/
}SL_ENCL_DSRPT_HDR_T;
#define SL_ENCL_DSRPT_HDR_S sizeof(SL_ENCL_DSRPT_HDR_T);

typedef struct _SL_ENCL_DSRPT_T
{
    U8          logicalID[8];
    U8          vendorID[8];
    U8          productID[16];
    U8          productRevLevel[4];
}SL_ENCL_DSRPT_T;
#define SL_ENCL_DSRPT_S sizeof(SL_ENCL_DSRPT_T);

typedef struct _SL_TYPE_DSRPT_HDR_T
{
    U8          elmtType;           //type of element defined by enum SL_ENCL_ELEM_TYPE above
    U8          possibleElmtCnt;
    U8          subEnclId;
    U8          typeDsrptTextLen;
} SL_TYPE_DSRPT_HDR_T;
#define SL_TYPE_DSRPT_HDR_S sizeof(SL_TYPE_DSRPT_HDR_T);

typedef struct _SL_ENCL_CONFIG_T
{
    U32 size;                           //total size of the data structure
    U32 vendorSpecificLen;              //length of vendor specific data
    U32 typeDsrptHdrLen;                //length of descriptor header
    U32 typeDsrptTxtLen;                //length of descriptor text
    union
    {
        struct
        {
            SL_ENCL_CONFIG_DPH_T    configPageHdr;
            SL_ENCL_DSRPT_HDR_T     dsrptHdr;
            SL_ENCL_DSRPT_T         dsrpt;
            U8                      vendorSpecificInfo[1];
            SL_TYPE_DSRPT_HDR_T     typeDsrptHdr[1];
            U8                      typeDsrptTxt[1];
        };
        U8 configPageBuffer[SL_MAX_ENCL_BUFFER_SIZE - (sizeof(U32) * 4)];
    };
                                         //and typeDsrptTextLen defines the length of each of this
} SL_ENCL_CONFIG_T;
#define SL_ENCL_CONFIG_S sizeof(SL_ENCL_CONFIG_T)
//Helper Macros for SL_ENCL_CONFIG_T data structure
#define SL_ADDR_OF_TYPE_DSRPT_HDR_ELMT(s/*ptr to struct of type SL_ENCL_CONFIG_T*/, elemIdx/*index of element*/) \
            ((s)->vendorSpecificInfo + (s)->vendorSpecificLen + elemIdx * sizeof(SL_TYPE_DSRPT_HDR_T))
#define SL_ADDR_OF_TYPE_DSRPT_TXT(s/*ptr to struct of type SL_ENCL_CONFIG_T*/) \
            ((s)->vendorSpecificInfo + (s)->vendorSpecificLen + (s)->dsrptHdr.elmtTypeCnt * sizeof(SL_TYPE_DSRPT_HDR_T))

typedef struct _SL_ENCL_STATUS_T
{
    U32  size;                                  //total size of the data structure
    U32  slotCount;
    U32  psCount;                               //number of power supplies present
    U32  fanCount;
    U32  tsCount;
    U32  alarmCount;
    U32  simCount;
    SL_SLOT_STATUS_T              slotStatus[1];
    SL_POWER_SUPPLY_STATUS_T      psStatus[1];  //power supply status for each ps. Max -> psStatus x psCount
    SL_FAN_STATUS_T               fanStatus[1];
    SL_TEMP_SENSOR_STATUS_T       tsStatus[1];
    SL_ALARM_STATUS_T             alarmStatus[1];
    SL_SIM_STATUS_T               simStatus[1];
} SL_ENCL_STATUS_T;
//Helper Macros for SL_ENCL_STATUS_T data structure
//Power Supply
#define SL_ADDR_OF_PS_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_T*/, elemIdx/*index of element*/) \
            ((U8*)s->slotStatus + s->slotCount * sizeof(SL_SLOT_STATUS_T) + elemIdx * sizeof(SL_POWER_SUPPLY_STATUS_T))
//Fan
#define SL_ADDR_OF_FAN_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_T*/, elemIdx/*index of element*/) \
            ((U8*)s->slotStatus + s->slotCount * sizeof(SL_SLOT_STATUS_T) + s->psCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + elemIdx * sizeof(SL_FAN_STATUS_T))

//Temp Sensor
#define SL_ADDR_OF_TEMP_SENSOR_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_T*/, elemIdx/*index of element*/) \
            ((U8*)s->slotStatus + s->slotCount * sizeof(SL_SLOT_STATUS_T) + s->psCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->fanCount * sizeof(SL_FAN_STATUS_T) + elemIdx * sizeof(SL_TEMP_SENSOR_STATUS_T))
//Alarm
#define SL_ADDR_OF_ALARM_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_T*/, elemIdx/*index of element*/) \
            ((U8*)s->slotStatus + s->slotCount * sizeof(SL_SLOT_STATUS_T) + s->psCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->fanCount * sizeof(SL_FAN_STATUS_T) + s->tsCount * sizeof(SL_TEMP_SENSOR_STATUS_T) \
            + elemIdx * sizeof(SL_ALARM_STATUS_T))
//SIM
#define SL_ADDR_OF_SIM_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_T*/, elemIdx/*index of element*/) \
            ((U8*)s->slotStatus + s->slotCount * sizeof(SL_SLOT_STATUS_T) + s->psCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->fanCount * sizeof(SL_FAN_STATUS_T) + s->tsCount * sizeof(SL_TEMP_SENSOR_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T) + elemIdx * sizeof(SL_SIM_STATUS_T))

typedef struct _SL_ENCL_TEMP_THRESHOLD_T
{
    U8      highWarning;        //High Warning Threshold
    U8      lowWarning;         //Low Warning Threshold
    U8      reserved[30];
} SL_ENCL_TEMP_THRESHOLD_T;
#define SL_ENCL_TEMP_THRESHOLD_S sizeof(SL_ENCL_TEMP_THRESHOLD_T)

typedef struct _SL_ENCL_SET_ELEM_DATA_T
{
    union
    {
        SL_ENCL_TEMP_THRESHOLD_T    tsThreshold;
        U8                          dataBuffer[32];
    };
} SL_ENCL_SET_ELEM_DATA_T;
#define SL_ENCL_SET_ELEM_DATA_S sizeof(SL_ENCL_SET_ELEM_DATA_T)

typedef struct _SL_ENCL_GET_ELEM_DATA_T
{
    U8      highWarning;        //High Warning Threshold
    U8      lowWarning;         //Low Warning Threshold
    U8      highCritical;        //High Critical Threshold
    U8      lowCritical;         //Low Critical Threshold
    U8      reserved[28];
} SL_ENCL_GET_ELEM_DATA_T;
#define SL_ENCL_GET_ELEM_DATA_S sizeof(SL_ENCL_GET_ELEM_DATA_T)

//Partition Info data structures
#define SL_MAX_MBR_PART 4
#define SL_MAX_GPT_PART 128

typedef enum _SL_PARTITION_STYLE {
  SL_PARTITION_STYLE_MBR,
  SL_PARTITION_STYLE_GPT,
  SL_PARTITION_STYLE_RAW
} SL_PARTITION_STYLE;

typedef struct _SL_PARTITION_DETAILS_T
{
  U8   bootIndicator;      //0 - Non boot, 1 - Boot
  U8   systemId;           //Defines the volume type e.g 0x07->NTFS, 0x0B->FAT32, 0xEE->GPT Partition, etc...
  U8   reserved[6];
  U64  numSectors;         //number of sectors in partition
  U64  sizeInMB;           //size of partition in MB
  U8   guid[16];           //GPT partition only: Identifies the type of partition e.g. 0x28732AC11FF8D211BA4B00A0C93EC93B, 0xA2A0D0EBE5B9334487C068B6B72699C7, etc
  U16  name[36];           //GPT partition only: A 36-character Unicode string that can be used to name the partition.e.g. EFI System partition, Microsoft Reserved partition, etc
} SL_PARTITION_DETAILS_T;
#define SL_PARTITION_DETAILS_S sizeof(SL_PARTITION_DETAILS_T)

typedef struct _SL_PARTITION_INFO_T
{
    SL_PARTITION_STYLE      partitionStyle;         //MBR, GPT, RAW
    U32                     partitionCount;         //number of valid partitions
    SL_PARTITION_DETAILS_T  partitionDetails[1];    //dynamically allocated. For MBR max is 4 and for GPT max is 128
} SL_PARTITION_INFO_T;
#define SL_PARTITION_INFO_S sizeof(SL_PARTITION_INFO_T)
#define SL_MIN_PART_SIZE (SL_PARTITION_INFO_S - SL_PARTITION_DETAILS_S)

typedef struct _SL_PD_IN_LD_T
{
    U32 count;
    U16 deviceId[MAX_PHYSICAL_DEVICES];
} SL_PD_IN_LD_T;
#define SL_PD_IN_LD_S sizeof(SL_PD_IN_LD_T)


typedef struct _SL_LD_OF_PD_T
{
    U32 count;
    U8 targetId[MAX_LOGICAL_DRIVES];
} SL_LD_OF_PD_T;
#define SL_LD_OF_PD_S sizeof(SL_LD_OF_PD_T)

typedef struct _SL_LD_INFO_T
{
    U8      targetId;
    U8      PRL;     //primary raid level
    U8      reserved[6];
    U64     startBlock;
    U64     numBlocks;
} SL_LD_INFO_T;
#define SL_LD_INFO_S sizeof(SL_LD_INFO_T)

#define SL_MAX_HOLES_IN_ARRAY   32
typedef struct _SL_ARRAY_INFO_T
{
    MR_ARRAY    array;
    U64         totalBlocks;
    U64         usedBlocks;
    U32         reserved;
    U32         freeCount;
    struct
    {
        U64     startBlock;
        U64     numBlocks;
    } freeInfo[SL_MAX_HOLES_IN_ARRAY];
    U32         reserved1;
    U32         ldCount;
    SL_LD_INFO_T ldInfo[MAX_LOGICAL_DRIVES];

} SL_ARRAY_INFO_T;
#define SL_ARRAY_INFO_S sizeof(SL_ARRAY_INFO_T)

// Extented array structure

typedef struct _SL_ARRAY_INFO_EXT_T
{
    MR_ARRAY_EXT    array;
    U64         totalBlocks;
    U64         usedBlocks;
    U32         reserved;
    U32         freeCount;
    struct
    {
        U64     startBlock;
        U64     numBlocks;
    } freeInfo[SL_MAX_HOLES_IN_ARRAY];
    U32         reserved1;
    U32         ldCount;
    SL_LD_INFO_T ldInfo[MAX_LOGICAL_DRIVES];
 
} SL_ARRAY_INFO_EXT_T;

#define SL_ARRAY_INFO_EXT_S sizeof(SL_ARRAY_INFO_EXT_T)


//Helper Macros for Config data structure
//MR_LD_CONFIG Address
#define SL_ADDR_OF_MR_LD_CONFIG(s/*ptr to struct of type MR_CONFIG_DATA*/) (s->array + s->arrayCount)
//MR_SPARE Address
#define SL_ADDR_OF_MR_SPARE(s/*ptr to struct of type MR_CONFIG_DATA*/) ((U8*)s->array + s->arrayCount * s->arraySize + s->logDrvCount * s->logDrvSize)

/****************************************************************************
*
*        Topology
*
****************************************************************************/
#define SL_TOPOLOGY_PAGE_NUMBER         0

/* The topology on the customers setup was too large for 16K SL_TOPOLOGY_BUFFER_SIZE.
 * So increasing the SL_TOPOLOGY_BUFFER_SIZE to 32K
 */
#define SL_TOPOLOGY_BUFFER_SIZE         32 * 1024 //32K
#define SL_TOPOLOGY_MAX_IOC_PHY         16
#define SL_TOPOLOGY_MAX_EXPANDERS       128
#define SL_TOPOLOGY_MAX_EXPANDER_PHYS   128

typedef enum _SL_TOPOLOGY_DEVICE_TYPE{
        SL_NO_DEVICE = 0,
        SL_IOC,
        SL_EDGE_EXPANDER,
        SL_FANOUT_EXPANDER,
        SL_END_DEVICE,
		SL_NVME_DEVICE
}SL_TOPOLOGY_DEVICE_TYPE;

typedef union{                          //Detailed Device Type Information Bits
    struct{
        U32 SSPInitiator:1;
        U32 STPInitiator:1;
        U32 SMPInitiator:1;
        U32 SATAHost:1;
        U32 SSPTarget:1;
        U32 STPTarget:1;
        U32 SMPTarget:1;
        U32 SATADevice:1;
        U32 reserved:24;
    };
    U32 deviceWord;
}SL_TOPOLOGY_DEVICE_BITMAP;

typedef struct _SL_TOPOLOGY_HEAD_PHY_T
{
    U8  attachedPhyId;              //The Phy ID in the attached device
    U8  attachedDeviceType;
    U16 attachedDeviceOffset;       //The device that is attached to this phy.
                                    //The memory offset where its node starts
    U8  isDisabled  :1;             //0->enabled, 1->disabled 
    U8  rsrvd       :7;
    U8  reserved[3];
}SL_TOPOLOGY_HEAD_PHY_T;
#define SL_TOPOLOGY_HEAD_PHY_S sizeof(SL_TOPOLOGY_HEAD_PHY_T)

typedef struct _SL_TOPOLOGY_HEAD_NODE_T
{
    U8  deviceType;                     //Device type of the IOC
    U8  reserved;
    U16 totalSize;                      //Total Size of the whole topology return tree
    U8  reserved1[3];
    U8  numPhy;                         //Number of phy's in the IOC
    SL_TOPOLOGY_HEAD_PHY_T  phyList[MAX_PHYS_PER_CONTROLLER]; //The list of phy's
}SL_TOPOLOGY_HEAD_NODE_T;
#define SL_TOPOLOGY_HEAD_NODE_S sizeof(SL_TOPOLOGY_HEAD_NODE_T)


typedef struct _SL_TOPOLOGY_EXPANDER_PHY_T
{
    U8 attachedDeviceType;          //type of device attached to this phy
    U8 attachedPhyId;               //the phy ID of the attached device
    U8 reserved[2];
    U16 attachedDeviceOffset;       //The memory offset where the node
                                    //of the device that is attached to this phy starts
    U16 reserved1;
}SL_TOPOLOGY_EXPANDER_PHY_T;
#define SL_TOPOLOGY_EXPANDER_PHY_S sizeof(SL_TOPOLOGY_EXPANDER_PHY_T)

typedef struct _SL_TOPOLOGY_EXPANDER_NODE_T
{
    U8                          deviceType;     //device type of this expander
    U8                          numPhy;         //number of phy's the expander has
    U8                          reserved[2];
    SL_TOPOLOGY_DEVICE_BITMAP   deviceBitMap;   //Detailed Device Type Information Bits
    U64                         sasAddr;        //the SAS address of the expander
    SL_TOPOLOGY_EXPANDER_PHY_T  phyList[1];
}SL_TOPOLOGY_EXPANDER_NODE_T;
#define SL_TOPOLOGY_EXPANDER_NODE_S (sizeof(SL_TOPOLOGY_EXPANDER_NODE_T)-sizeof(U8))

typedef struct _SL_TOPOLOGY_END_DEVICE_PHY_T
{
    U64 sasAddr;                    //sasAddr associated with this phy
    U32 reserved[2];
}SL_TOPOLOGY_END_DEVICE_PHY_T;
#define SL_TOPOLOGY_END_DEVICE_PHY_S sizeof(SL_TOPOLOGY_END_DEVICE_PHY_T)

typedef struct _SL_TOPOLOGY_END_DEVICE_NODE_T
{
    U8                          deviceType;         //Device Type of this end device
    U8                          numPhy;             //Number of Phy in the device
    U16                         deviceId;           //The ID used in storelib and controller firmware
    SL_TOPOLOGY_DEVICE_BITMAP   deviceBitMap;       //Detailed Device Type Information Bits
    U32                         reserved;
    SL_TOPOLOGY_END_DEVICE_PHY_T    phyList[2];
}SL_TOPOLOGY_END_DEVICE_NODE_T;
#define SL_TOPOLOGY_END_DEVICE_NODE_S sizeof(SL_TOPOLOGY_END_DEVICE_NODE_T)

//system/ctrl health structures
typedef struct _SL_ENCL_ELEM_HEALTH_T
{
    U16 optimalCount;
    U16 criticalCount;
    U16 nonCriticalCount;
    U16 unrecoverableCount;
    U16 notInstalledCount;
    U16 unknownCount;
    U16 notAvailableCount;
    U16 reserved;
} SL_ENCL_ELEM_HEALTH_T;
#define SL_ENCL_ELEM_HEALTH_S sizeof(SL_ENCL_ELEM_HEALTH_T)

typedef struct _SL_ENCL_HEALTH_T
{
    SL_ENCL_ELEM_HEALTH_T powerSupply;
    SL_ENCL_ELEM_HEALTH_T fan;
    SL_ENCL_ELEM_HEALTH_T tempSensor;
    SL_ENCL_ELEM_HEALTH_T sim;
} SL_ENCL_HEALTH_T;
#define SL_ENCL_HEALTH_S sizeof(SL_ENCL_HEALTH_T)

typedef struct _SL_BBU_HEALTH_T
{
        U16     packMissing         : 1;
        U16     voltageLow          : 1;
        U16     temperatureHigh     : 1;
        U16     reserved            : 13;
} SL_BBU_HEALTH_T;
#define SL_BBU_HEALTH_S sizeof(SL_BBU_HEALTH_T)

typedef struct _SL_CTRL_HEALTH_T
{
    //ld Health
    U16                 ldOptimalCount;
    U16                 ldCriticalCount;
    U16                 ldOfflineCount;
    U16                 reserved;
    //pd Health
    U16                 pdOptimalCount;
    U16                 pdPredFailCount;
    U16                 pdFailedCount;
    //encl Health
    U16                 enclCount;
    SL_ENCL_HEALTH_T    enclHealth[MAX_ENCLOSURES];
    //bbu health
    U16 bbuPresent; //Is BBU present
    SL_BBU_HEALTH_T     bbuHealth;
    //nvram error
    U16                 nvramUnCorrectableErrorCount;
} SL_CTRL_HEALTH_T;
#define SL_CTRL_HEALTH_S sizeof(SL_CTRL_HEALTH_T)

typedef struct _SL_SYSTEM_HEALTH_T
{
    U16                 ctrlCount;
    U16                 reserved;
    SL_CTRL_HEALTH_T    ctrlHealth[SL_MAX_CONTROLLERS];
} SL_SYSTEM_HEALTH_T;
#define SL_SYSTEM_HEALTH_S sizeof(SL_SYSTEM_HEALTH_T)

//debugLevel
#define SL_DBG_NONE         0 //Disable debug output
#define SL_DBG_LEVEL_1      1 //Log level 1 (Function entries & exits)
#define SL_DBG_LEVEL_2      2 //Log level 2 (Full debug info)
#define SL_DBG_MAX_LEVELS   3 //Number of debug levels

// Define method to mask off specific debug messages when have Log Level 2
// Useful when looking at specific areas
// All Debug strings will be of type SL_DBG_LEVEL_1 (Entry/Exit) OR
// they will have one of the specific values below 
#define SL_DBG_RESERVED_DBG         (0x00000001) /* Reserved for SL_DBG_LEVEL_1 */
#define SL_DBG_ERROR_DBG            (0x00000002)
#define SL_DBG_PD_DBG               (0x00000004)
#define SL_DBG_LD_DBG               (0x00000008)
#define SL_DBG_I2C_DBG              (0x00000010)
#define SL_DBG_PCIE_DBG             (0x00000020)
#define SL_DBG_CONFIG_DBG           (0x00000040)
#define SL_DBG_SCSI_DBG             (0x00000080)
#define SL_DBG_AEN_REGISTER_DBG     (0x00000100)
#define SL_DBG_AEN_PROCESS_DBG      (0x00000200)
#define SL_DBG_MCTP_DBG             (0x00000400)
#define SL_DBG_DEBUG_DBG            (0x00000800)
#define SL_DBG_TOPOLOGY_DBG         (0x00001000)
#define SL_DBG_LINUX_DBG            (0x00002000)
#define SL_DBG_CONFIG_PAGE_DBG      (0x00004000)
#define SL_DBG_EVENT_DBG            (0x00008000)
#define SL_DBG_CTRL_DBG             (0x00010000)
#define SL_DBG_I2C_DISCOVERY_DBG    (0x00020000)
#define SL_DBG_COMMON_DBG           (0x00040000)
//overwriteDebugFile
#define SL_DBG_APPEND               (0)       //Append to debug file
#define SL_DBG_OVERWRITE            (1)       //Overwrite debug contents
#define SL_MAX_DBG_DIR_PATH         (1024)   // Max path length for debug file 

//OOB
#define SL_OOB_FALSE      0 //Indicates that environment is not OOB/BMC
#define SL_OOB_TRUE       1 //Indicates OOB/BMC environment for out of band management

#define SL_DEVICE_ID_ARRAY_COUNT 10 /* number of different controller Device IDs supported by library */

typedef struct _SL_LIB_PARAMETERS_T
{
    U8 debugLevel;                  //default->SL_DBG_NONE
    U8 overwriteDebugFile   : 1;    //default->SL_DBG_APPEND
    U8 oobEnabled           : 1;    //default->SL_OOB_FALSE
    U8 reserved1            : 6;
    struct
    {
        U8 pollInterval;        //Interval in seconds between poll to BMC for status
        U8 maxWriteSize;        //max data that can be written over i2c link
        U8 maxReadSize;         //max data that can be read over i2c link
        U8 maxReadWriteSize;    //max data that can be read/written over i2c link
        U8 reserved1[3];
    } oob;
    U8 debugDirPath[SL_MAX_DBG_DIR_PATH];   // dir where debug file will created. Appplicable only when debugLevel is non-zero
    U8 libBmcDirPath[SL_MAX_DBG_DIR_PATH];  // dir where libbmc file will reside. (This is for I2C)
    struct
    {
        U16 MaxDrvrBufSize;     /* Maximum VDM Buffer size */
        U16 VendorID;           /* Vendor ID of card supported by this library (0x1000 is LSI/Avago)*/
        U32 DeviceIDCount;      /* Number of devices in the DeviceIDArray */
        U16 DeviceIDArray[SL_DEVICE_ID_ARRAY_COUNT];  /* May need to make this dynamic. */
    } MCTP;
	U16 enclosurePollTime;
	U16 uploadMessageSize;
    U8 reserved[480];
} SL_LIB_PARAMETERS_T;
#define SL_LIB_PARAMETERS_S sizeof(SL_LIB_PARAMETERS_T)

/****************************************************************************
*
*        Input to DCMD commands
*
****************************************************************************/
typedef struct _SL_DCMD_INPUT_T
{
    U32         dataTransferLength; //length of data
    U32         opCode;             //F/W command
    union
    {
        U32     controlCode;        //0: Send to Firmware, 1~60 : reserved for CSMI, MR_DRIVER_IOCTL_WINDOWS : Handle by Driver
        struct { //OOB
            U8 controlCmd;          // type MR_OOB_CONTROL_CMD
            U8 reserved;
            U16 payload;            // payload of control command */
        } controlCommand;
    };
    U8          flags;              //dir of the command (SL_DIR)
    U8          reserved[3];
    union                   //mailbox data
    {
            U8     b[12];   //byte reference to mailbox
            U16    s[6];    //short reference to mailbox
            U32    w[3];    //word access to mailbox data
    } mbox;
    void*           pData;      //pointer to data buffer
} SL_DCMD_INPUT_T;
#define SL_DCMD_INPUT_S sizeof(SL_DCMD_INPUT_T)

/****************************************************************************
*
*        LIB CMD PARAM DATA STRUCTURE DEFINITION
*
****************************************************************************/
/****************************************************************************
*
*        PHYSICAL DEVICE ADDRESSING INFO
*
****************************************************************************/
typedef struct _SL_PD_REF
{
    U16     deviceId;               //Device Id
    U16     seqNum;                 //PD Sequence Number. State counter to keep track of changes
    U8      rsvd[4];
} SL_PD_REF;

/****************************************************************************
*
*        ENCLOSURE DEVICE ADDRESSING INFO
*
****************************************************************************/
typedef struct _SL_ENCL_REF
{
    U16     deviceId;              //Enclosure device Id
    U8      elementIndex;          //Index of enclosure element (Fan, PS, TS, etc.). NA for enclosures itself
    U8      rsvd[3];
} SL_ENCL_REF;

/****************************************************************************
*
*        ARRAY ADDRESSING INFO
*
****************************************************************************/
typedef struct _SL_ARRAY_REF
{
    U16     seqNum;                 //Array Sequence Number. State counter to keep track of changes
    U16     ref;                    //Array Reference Number
    U8      rsvd[4];
} SL_ARRAY_REF;

/****************************************************************************
*
*        LOGICAL DEVICE ADDRESSING INFO
*
****************************************************************************/
typedef struct _SL_LD_REF
{
    U16     seqNum;                 //LD Sequence Number. State counter to keep track of changes
    U8      targetId;               //Unique sparse logical drive Id.
    U8      rsvd[5];
} SL_LD_REF;

/****************************************************************************
*
*        EVENT ADDRESSING INFO
*
****************************************************************************/
typedef struct _SL_EVENT_REF
{
    U32     seqNum;    //Event Sequence Number to uniquely identify each event
    U8      reserved[4];
} SL_EVENT_REF;

/****************************************************************************
*
*        FOREIGN CONFIG ADDRESSING INFO
*
****************************************************************************/
typedef struct _SL_CONFIG_GUID_REF
{
    U8     GUIDIndex;
    U8     reserved[7];
} SL_CONFIG_GUID_REF;

/****************************************************************************
*
*        GENERIC ADDRESSING INFO
*
****************************************************************************/
typedef struct _SL_GENERIC_REF
{
    union
    {
        U8      ref_1b[8];    //1 byte usage
        U16     ref_2b[4];    //2 bytes usage
        U32     ref_4b[2];    //4 bytes usage
        U64     ref_8b;       //8 bytes usage
    };
} SL_GENERIC_REF;

/****************************************************************************
* Used by ProcessLibCommand to fire commands to the controller.
* The command type, actual command and controller Id needs to be specified.
* The addressing info will depend upon the device to be addressed.
* The datasize contains the size of the data buffer passed in pData field.
****************************************************************************/
typedef struct _SL_LIB_CMD_PARAM_T
{
    U8      cmdType;        //Type of the command. Use cmd type from SL_CMD_TYPE
    U8      cmd;            //Actual command to be executed
    U8	    AppsSupportEXTConfig; //This suggests that the Apps have the 240 headers packaged. This will differentiate between the old and new apps and for Backward compatibility.
    U8      reserved0;		//1 Bit used for 240 Support
    U32     ctrlId;          //Controller number
    union   //Address to identify devices
    {
        SL_PD_REF           pdRef;
        SL_ENCL_REF         enclRef;
        SL_ARRAY_REF        arrayRef;
        SL_LD_REF           ldRef;
        SL_EVENT_REF        evtRef;
        SL_CONFIG_GUID_REF  configGUIDRef;
        SL_GENERIC_REF      genRef;
    };
    union
    {
        U8      cmdParam_1b[8];    //1 byte usage
        U16     cmdParam_2b[4];    //2 bytes usage
        U32     cmdParam_4b[2];    //4 bytes usage
        U64     cmdParam_8b;       //8 bytes usage
        void    *pCmdParam;        //ptr usage
    };
    U32     reserved1;
    U32     dataSize;       //Length of the buffer passed in pData below.
                            //For receive data, dataSize will be updated with size of data returned
    void    *pData;         //Pointer to buffer to send/receive data
} SL_LIB_CMD_PARAM_T;
#define SL_LIB_CMD_PARAM_S (sizeof(SL_LIB_CMD_PARAM_T) - sizeof(void*))

//Register AEN data structures
#define SL_MAX_AEN_REGISTRATIONS 64
typedef struct _SL_REG_AEN_INFO_T
{
    U32                 ctrlId;
    MR_EVT_CLASS_LOCALE eventMask;
    U32                 evtSeqNum;
} SL_REG_AEN_INFO_T;
#define SL_REG_AEN_INFO_S sizeof(SL_REG_AEN_INFO_T)

typedef int (*pCallBackFunc) (SL_EVENT_DETAIL_T *pEventDetail);
typedef struct _SL_REG_AEN_INPUT_T
{
    U32                 count;
    SL_REG_AEN_INFO_T   regAenInfo[SL_MAX_CONTROLLERS];
    pCallBackFunc       pFunc;
} SL_REG_AEN_INPUT_T;
#define SL_REG_AEN_INPUT_S sizeof(SL_REG_AEN_INPUT_T)

typedef struct _SL_REG_AEN_OUTPUT_T
{
    U32 regCtrlCount;
    U32 regCtrlId[SL_MAX_CONTROLLERS];
    U32 uniqueId;
} SL_REG_AEN_OUTPUT_T;
#define SL_REG_AEN_OUTPUT_S sizeof(SL_REG_AEN_OUTPUT_T)

//Start CR Structure: Voltage Sensor

// 1. Device Status
typedef struct _SL_DEVICE_STATUS_T
{
    U32     sesDeviceStatusPage;   //SES Status page from the enclosure
    U32     deviceStatusVal;
} SL_DEVICE_STATUS_T;
#define SL_DEVICE_STATUS_S sizeof(SL_DEVICE_STATUS_T)
/*
//2 .Power supply status
typedef struct _SL_POWER_SUPPLY_STATUS_T
{
    U32     sesPowerSupplyStatusPage;   //SES Status page from the enclosure
    U32     psStatusVal;
} SL_POWER_SUPPLY_STATUS_T;
#define SL_POWER_SUPPLY_STATUS_S sizeof(SL_POWER_SUPPLY_STATUS_T)
*/
//3 Cooling status
typedef struct _SL_COOLING_STATUS_T
{
    U32     sesCoolingStatusPage;   //SES Status page from the enclosure
    U32     coolingStatusVal;
} SL_COOLING_STATUS_T ;
#define SL_COOLING_STATUS_S sizeof(SL_COOLING_STATUS_T)
/*
//4 Temp Sensor status
typedef struct _SL_TEMP_SENSOR_STATUS_T
{
    U32     sesTempSensorStatusPage;   //SES Status page from the enclosure
    U32     tempsensorStatusVal;
}SL_TEMP_SENSOR_STATUS_T ;
#define SL_TEMP_SENSOR_STATUS_S sizeof(SL_TEMP_SENSOR_STATUS_T)
*/
//5 Door Lock status
typedef struct _SL_DOOR_LOCK_STATUS_T
{
    U32     sesDoorLockStatusPage;   //SES Status page from the enclosure
    U32     doorlockStatusVal;
}SL_DOOR_LOCK_STATUS_T ;
#define SL_DOOR_LOCK_STATUS_S sizeof(SL_DOOR_LOCK_STATUS_T)
/*
//6 Alarm status
typedef struct _SL_ALARM_STATUS_T
{
    U32     sesAlarmStatusPage;   //SES Status page from the enclosure
    U32     alarmStatusVal;
}SL_ALARM_STATUS_T;
#define SL_ALARM_STATUS_S sizeof(SL_ALARM_STATUS_T)
*/
//7 Ses Ctrl Electric status
typedef struct _SL_SES_CTRL_ELEC_STATUS_T
{
    U32     sesCtrlElecStatusPage;   //SES Status page from the enclosure
    U32     sesctrlelecStatusVal;
}SL_SES_CTRL_ELEC_STATUS_T;
#define SL_SES_CTRL_ELEC_STATUS_S sizeof(SL_SES_CTRL_ELEC_STATUS_T)

// 8 SCC Ctrl Electric status
typedef struct _SL_SCC_CTRL_ELEC_STATUS_T
{
    U32     sccCtrlElecStatusPage;   //SES Status page from the enclosure
    U32     sccctrlelecStatusVal;
}SL_SCC_CTRL_ELEC_STATUS_T;
#define SL_SCC_CTRL_ELEC_STATUS_S sizeof(SL_SCC_CTRL_ELEC_STATUS_T)

// 9 Non volatile Cache status
typedef struct _SL_NON_VOLATILE_CACHE_STATUS_T
{
    U32     nonvolatilecacheStatusPage;   //SES Status page from the enclosure
    U32     nonvolatilecacheStatusVal;
}SL_NON_VOLATILE_CACHE_STATUS_T;
#define SL_NON_VOLATILE_CACHE_STATUS_S sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)

// 10 Invalid op reason status
typedef struct _SL_INVALID_OP_REASON_STATUS_T
{
    U32     invalidopreasonStatusPage;   //SES Status page from the enclosure
    U32     InvalidOpReasonStatusVal;
}SL_INVALID_OP_REASON_STATUS_T;
#define SL_INVALID_OP_REASON_STATUS_S sizeof(SL_INVALID_OP_REASON_STATUS_T)

//11 UPS status
typedef struct _SL_UPS_STATUS_T
{
    U32     upsStatusPage;   //SES Status page from the enclosure
    U32     UPSStatusVal;
}SL_UPS_STATUS_T;
#define SL_UPS_STATUS_S sizeof(SL_UPS_STATUS_T)

//12 Display status
typedef struct _SL_DISPLAY_STATUS_T
{
    U32     DisplayStatusPage;   //SES Status page from the enclosure
    U32     DisplayStatusVal;
}SL_DISPLAY_STATUS_T;
#define SL_DISPLAY_STATUS_S sizeof(SL_DISPLAY_STATUS_T)

//13 Key Pad Entry Status
typedef struct _SL_KEY_PAD_ENTRY_STATUS_T
{
    U32     KeyPadEntryStatusPage;   //SES Status page from the enclosure
    U32     KeyPadEntryStatusVal;
}SL_KEY_PAD_ENTRY_STATUS_T ;
#define SL_KEY_PAD_ENTRY_STATUS_S sizeof(SL_KEY_PAD_ENTRY_STATUS_T)


//14 Encl Status
typedef struct _SL_ENCL_ENTRY_STATUS_T
{
    U32     EnclStatusPage;   //SES Status page from the enclosure
    U32     EnclStatusVal;
}SL_ENCL_ENTRY_STATUS_T;
#define SL_ENCL_ENTRY_STATUS_S sizeof(SL_ENCL_ENTRY_STATUS_T)


//15 SCSI TRANSCEIVER Status
typedef struct _SL_SCSI_TRANSCEIVER_STATUS_T
{
    U32     ScsitransStatusPage;   //SES Status page from the enclosure
    U32     ScsitransStatusVal;
}SL_SCSI_TRANSCEIVER_STATUS_T;
#define SL_SCSI_TRANSCEIVER_STATUS_S sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)

//16 Language Status
typedef struct _SL_LANGUAGE_STATUS_T
{
    U32     LangauageStatusPage;   //SES Status page from the enclosure
    U32     LanguageStatusVal;
}SL_LANGUAGE_STATUS_T;
#define SL_LANGUAGE_STATUS_S sizeof(SL_LANGUAGE_STATUS_T)
/*
// Language Status
typedef struct _SL_LANGUAGE_STATUS_EX_T
{
    U32     LangauageStatusPage;   //SES Status page from the enclosure
    U32     LanguageStatusVal;
}SL_SCSI_TRANSCEIVER_STATUS_EX_T;
#define SL_LANGUAGE_STATUS_EX_S sizeof(SL_LANGUAGE_STATUS_EX_T)
*/

//17 COM Port Status
typedef struct _SL_COM_PORT_STATUS_T
{
    U32     ComPortStatusPage;   //SES Status page from the enclosure
    U32     ComPortStatusVal;
}SL_COM_PORT_STATUS_T;
#define SL_COM_PORT_STATUS_S sizeof(SL_COM_PORT_STATUS_T)

//18 Voltage Sensor Status
typedef struct _SL_VOLTAGE_SENSOR_STATUS_T
{
    U32     VoltageSensorStatusPage;   //SES Status page from the enclosure
    U32     VoltageSensorStatusVal;
}SL_VOLTAGE_SENSOR_STATUS_T;
#define SL_VOLTAGE_SENSOR_STATUS_S sizeof(SL_VOLTAGE_SENSOR_STATUS_T)

//19 Current Sensor Status
typedef struct _SL_CURRENT_SENSOR_STATUS_T
{
    U32     CurrentSensorStatusPage;   //SES Status page from the enclosure
    U32     CurrentSensorStatusVal;
}SL_CURRENT_SENSOR_STATUS_T;
#define SL_CURRENT_SENSOR_STATUS_S sizeof(SL_CURRENT_SENSOR_STATUS_T)

//20 SCSI target Port Status
typedef struct _SL_SCSI_TARGET_PORT_STATUS_T
{
    U32     ScsiTargetPortStatusPage;   //SES Status page from the enclosure
    U32     ScsiTargetPortStatusVal;
}SL_SCSI_TARGET_PORT_STATUS_T;
#define SL_SCSI_TARGET_PORT_STATUS_S sizeof(SL_SCSI_TARGET_PORT_STATUS_T)

//21 SCSI Initiator Port Status
typedef struct _SL_SCSI_INITIATOR_PORT_STATUS_T
{
    U32     ScsiInitiatorPortStatusPage;   //SES Status page from the enclosure
    U32     ScsiInitiatorPortStatusVal;
}SL_SCSI_INITIATOR_PORT_STATUS_T;
#define SL_SCSI_INITIATOR_PORT_STATUS_S sizeof(SL_SCSI_INITIATOR_PORT_STATUS_T)

//22 Simple Sub Encl Status
typedef struct _SL_SIMPLE_SUB_ENCL_STATUS_T
{
    U32     SimpleSubEnclStatusPage;   //SES Status page from the enclosure
    U32     SimpleSubEnclStatusVal;
}SL_SIMPLE_SUB_ENCL_STATUS_T;
#define SL_SIMPLE_SUB_ENCL_STATUS_S sizeof(SL_SIMPLE_SUB_ENCL_STATUS_T)

 //23 Array device Status
typedef struct _SL_ARRAY_DEVICE_STATUS_T
{
    U32     ArrayDeviceStatusPage;   //SES Status page from the enclosure
    U32     ArrayDeviceStatusVal;
}SL_ARRAY_DEVICE_STATUS_T;
#define SL_ARRAY_DEVICE_STATUS_S sizeof(SL_ARRAY_DEVICE_STATUS_T)

  //24 SAS Expander Status
typedef struct _SL_SAS_EXPANDER_STATUS_T
{
    U32     SasExpanderStatusPage;   //SES Status page from the enclosure
    U32     SasExpanderStatusVal;
}SL_SAS_EXPANDER_STATUS_T;
#define SL_SAS_EXPANDER_STATUS_S sizeof(SL_SAS_EXPANDER_STATUS_T)

 //25 SAS Connector Status
typedef struct _SL_SAS_CONNECTOR_STATUS_T
{
    U32     SasConnectorStatusPage;   //SES Status page from the enclosure
    U32     SasConnectorStatusVal;
}SL_SAS_CONNECTOR_STATUS_T;
#define SL_SAS_CONNECTOR_STATUS_S sizeof(SL_SAS_CONNECTOR_STATUS_T)


//Structure: Voltage Sensor

//Structure: Voltage Sensor

typedef struct _SL_ENCL_STATUS_EX_T
{
    U32 size;                                  //total size of the data structure
    U8  reserved1[2];                          //padding for alignment
    U8  deviceCount;                           //Device
    U8  powerSupplyCount;                      //Power supply
    U8  coolingCount;                          //Cooling
    U8  tempSensorCount;                       //Temperature Sensor
    U8  doorLockCount;                         //Door Lock
    U8  alarmCount;                            //audible alarm
    U8  sesCtrlElecCount;                      //Enclosure Services Controller
    U8  sccCtrlElecCount;                      //SCC Controller Electronics
    U8  nonVolatileCacheCount;                 //Non Volatile Cache
    U8  invalidOpReasonCount;                   //Invalid Operation Reason
    U8  upsCount;                               //Uninterruptible Power Supply
    U8  displayCount;                           //Display
    U8  keyPadEntryCount;                       //Key Pad Entry
    U8  enclEntryCount;                         //Enclosure
    U8  scsiTransceiverPortCount;               //SCSI Transreceiver Port
    U8  languageCount;                          //Language
    U8  comPortCount;                           //Communication Port
    U8  voltageSensorCount;                 //Voltage Sensor
    U8  currentSensorCount;                 //Current Sensor
    U8  scsiTargetPortCount;                    //SCSI Target Port
    U8  scsiInitiatorPortCount;             //SCSI Initiator Port
    U8  simpleSubEnclCount;                 //Simple SubEnclosure
    U8  arrayDeviceCount;                       //Array Device
    U8  sasExpanderCount;                       //SAS Expander
    U8  sasConnectorCount;                      //SAS Connector
    U8  reserved2[0x7F-0x1A];
    SL_DEVICE_STATUS_T                  deviceStatus[1];
    SL_POWER_SUPPLY_STATUS_T            powerSupplyStatus[1];  //power supply status for each ps. Max -> psStatus x psCount
    SL_COOLING_STATUS_T                 coolingStatus[1];
    SL_TEMP_SENSOR_STATUS_T             tempSensorStatus[1];
    SL_DOOR_LOCK_STATUS_T               doorLockStatus[1];
    SL_ALARM_STATUS_T                   alarmStatus[1];
    SL_SES_CTRL_ELEC_STATUS_T           sesCtrlElecStatus[1];
    SL_SCC_CTRL_ELEC_STATUS_T           sccCtrlElecStatus[1];
    SL_NON_VOLATILE_CACHE_STATUS_T      nonVolatileCacheStatus[1];
    SL_INVALID_OP_REASON_STATUS_T       invalidOpReasonStatus[1];
    SL_UPS_STATUS_T                     upsStatus[1];
    SL_DISPLAY_STATUS_T                 displayStatus[1];
    SL_KEY_PAD_ENTRY_STATUS_T           keyPadEntryStatus[1];
    SL_ENCL_ENTRY_STATUS_T              enclStatus[1];
    SL_SCSI_TRANSCEIVER_STATUS_T        scsiTransceiverStatus[1];
    SL_LANGUAGE_STATUS_T                languageStatus[1];
    SL_COM_PORT_STATUS_T                comPortStatus[1];
    SL_VOLTAGE_SENSOR_STATUS_T          voltageSensorStatus[1];
    SL_CURRENT_SENSOR_STATUS_T          currentSensorStatus[1];
    SL_SCSI_TARGET_PORT_STATUS_T        scsiTargetPortStatus[1];
    SL_SCSI_INITIATOR_PORT_STATUS_T     scsiInitiatorPortStatus[1];
    SL_SIMPLE_SUB_ENCL_STATUS_T         simpleSubEnclStatus[1];
    SL_ARRAY_DEVICE_STATUS_T            arrayDeviceStatus[1];
    SL_SAS_EXPANDER_STATUS_T            sasExpanderStatus[1];
    SL_SAS_CONNECTOR_STATUS_T           sasConnectorStatus[1];
} SL_ENCL_STATUS_EX_T;
#define SL_ENCL_STATUS_EX_S sizeof(SL_ENCL_STATUS_EX_T)

//Helper Macros for SL_ENCL_STATUS_EX_T data structure
//1. Power Supply
#define SL_ADDR_OF_PS_STATUS_EX(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/, elemIdx/*index of element*/) \
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) + elemIdx * sizeof(SL_POWER_SUPPLY_STATUS_T))
//2. COOLING
#define SL_ADDR_OF_COOLING_STATUS_EX(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/, elemIdx/*index of element*/) \
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + elemIdx * sizeof(SL_COOLING_STATUS_T))

//3. Temp Sensor
#define SL_ADDR_OF_TEMP_SENSOR_STATUS_EX(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/, elemIdx/*index of element*/) \
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + elemIdx * sizeof(SL_TEMP_SENSOR_STATUS_T))

//4. Door Lock
#define SL_ADDR_OF_DOOR_LOCK_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T) \
            + elemIdx * sizeof(SL_DOOR_LOCK_STATUS_T))

//5.  Alarm
#define SL_ADDR_OF_ALARM_STATUS_EX(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T)\
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + elemIdx * sizeof(SL_ALARM_STATUS_T))

//6.  SES Ctrl Elec
#define SL_ADDR_OF_SES_CTRL_ELEC_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T)\
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + elemIdx * sizeof(SL_SES_CTRL_ELEC_STATUS_T))

//7. SCC Ctrl Elec
#define SL_ADDR_OF_SCC_CTRL_ELEC_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            +  s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T)\
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + elemIdx * sizeof(SL_SCC_CTRL_ELEC_STATUS_T))

 //8.  NON VOLATILE  CACHE

 #define SL_ADDR_OF_NON_VOLATILE_CACHE_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T)\
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T)\
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + elemIdx * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T))

 //9.  INVALID OP REASON

 #define  SL_ADDR_OF_INVALID_OP_REASON_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T)\
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + elemIdx * sizeof(SL_INVALID_OP_REASON_STATUS_T))

 //10.  UPS
 #define  SL_ADDR_OF_UPS_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + elemIdx * sizeof(SL_UPS_STATUS_T))

 //11. Display
 #define  SL_ADDR_OF_DISPLAY_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + elemIdx * sizeof(SL_DISPLAY_STATUS_T))

//12. keyPadEntryCount
 #define  SL_ADDR_OF_KEY_PAD_ENTRY_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + elemIdx * sizeof(SL_KEY_PAD_ENTRY_STATUS_T))

//13. Encl Status
    #define  SL_ADDR_OF_ENCL_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + elemIdx * sizeof(SL_ENCL_ENTRY_STATUS_T))

//14. SCSI_TRANSCEIVER
    #define  SL_ADDR_OF_SCSI_TRANSCEIVER_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + elemIdx * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T))

// 15. LANGUAGE STATUS
    #define  SL_ADDR_OF_LANGUAGE_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + elemIdx * sizeof(SL_LANGUAGE_STATUS_T))

    // 16. COM PORT
    #define  SL_ADDR_OF_COM_PORT_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + s->languageCount * sizeof(SL_LANGUAGE_STATUS_T)\
            + elemIdx * sizeof(SL_COM_PORT_STATUS_T))

    // 17. Voltage Sensor
    #define  SL_ADDR_OF_VOLTAGE_SENSOR_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + s->languageCount * sizeof(SL_LANGUAGE_STATUS_T)\
            + s->comPortCount * sizeof(SL_COM_PORT_STATUS_T)\
            + elemIdx * sizeof(SL_VOLTAGE_SENSOR_STATUS_T))

    // 18. Current Sensor
    #define  SL_ADDR_OF_CURRENT_SENSOR_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + s->languageCount * sizeof(SL_LANGUAGE_STATUS_T)\
            + s->comPortCount * sizeof(SL_COM_PORT_STATUS_T)\
            + s->voltageSensorCount * sizeof(SL_VOLTAGE_SENSOR_STATUS_T)\
            + elemIdx * sizeof(SL_CURRENT_SENSOR_STATUS_T))

    // 19. SCSI Target Port
    #define  SL_ADDR_OF_SCSI_TARGET_PORT_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + s->languageCount * sizeof(SL_LANGUAGE_STATUS_T)\
            + s->comPortCount * sizeof(SL_COM_PORT_STATUS_T)\
            + s->voltageSensorCount * sizeof(SL_VOLTAGE_SENSOR_STATUS_T)\
            + s->currentSensorCount * sizeof(SL_CURRENT_SENSOR_STATUS_T)\
            + elemIdx * sizeof(SL_SCSI_TARGET_PORT_STATUS_T))

    // 20.  SCSI Initiator Port
    #define  SL_ADDR_OF_SCSI_INITIATOR_PORT_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + s->languageCount * sizeof(SL_LANGUAGE_STATUS_T)\
            + s->comPortCount * sizeof(SL_COM_PORT_STATUS_T)\
            + s->voltageSensorCount * sizeof(SL_VOLTAGE_SENSOR_STATUS_T)\
            + s->currentSensorCount * sizeof(SL_CURRENT_SENSOR_STATUS_T)\
            + s->scsiTargetPortCount *sizeof(SL_SCSI_TARGET_PORT_STATUS_T)\
            + elemIdx * sizeof(SL_SCSI_INITIATOR_PORT_STATUS_T))

    // 21.  Simple Sub Encl
    #define  SL_ADDR_OF_SIMPLE_SUB_ENCL_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + s->languageCount * sizeof(SL_LANGUAGE_STATUS_T)\
            + s->comPortCount * sizeof(SL_COM_PORT_STATUS_T)\
            + s->voltageSensorCount * sizeof(SL_VOLTAGE_SENSOR_STATUS_T)\
            + s->currentSensorCount * sizeof(SL_CURRENT_SENSOR_STATUS_T)\
            + s->scsiTargetPortCount * sizeof(SL_SCSI_TARGET_PORT_STATUS_T)\
            + s->scsiInitiatorPortCount *sizeof(SL_SCSI_INITIATOR_PORT_STATUS_T)\
            + elemIdx * sizeof(SL_SIMPLE_SUB_ENCL_STATUS_T))

// 22.  Simple Sub Encl
    #define  SL_ADDR_OF_ARRAY_DEVICE_STATUS_EX(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + s->languageCount * sizeof(SL_LANGUAGE_STATUS_EX_T)\
            + s->comPortCount * sizeof(SL_COM_PORT_STATUS_T)\
            + s->voltageSensorCount * sizeof(SL_VOLTAGE_SENSOR_STATUS_T)\
            + s->currentSensorCount * sizeof(SL_CURRENT_SENSOR_STATUS_T)\
            + s->scsiTargetPortCount * sizeof(SL_SCSI_TARGET_PORT_STATUS_T)\
            + s->scsiInitiatorPortCount * sizeof(SL_SCSI_INITIATOR_PORT_STATUS_T)\
            + s->simpleSubEnclCount * sizeof(SL_SIMPLE_SUB_ENCL_STATUS_T)\
            + elemIdx * sizeof(SL_ARRAY_DEVICE_STATUS_T))

// 23.  Array device
    #define  SL_ADDR_OF_ARRAY_DEVICE_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + s->languageCount * sizeof(SL_LANGUAGE_STATUS_T)\
            + s->comPortCount * sizeof(SL_COM_PORT_STATUS_T)\
            + s->voltageSensorCount * sizeof(SL_VOLTAGE_SENSOR_STATUS_T)\
            + s->currentSensorCount * sizeof(SL_CURRENT_SENSOR_STATUS_T)\
            + s->scsiTargetPortCount * sizeof(SL_SCSI_TARGET_PORT_STATUS_T)\
            + s->scsiInitiatorPortCount * sizeof(SL_SCSI_INITIATOR_PORT_STATUS_T)\
            + s->simpleSubEnclCount * sizeof(SL_SIMPLE_SUB_ENCL_STATUS_T)\
            + elemIdx * sizeof(SL_ARRAY_DEVICE_STATUS_T))

// 24.  SAS Expander
    #define  SL_ADDR_OF_SAS_EXPANDER_STATUS(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + s->languageCount * sizeof(SL_LANGUAGE_STATUS_T)\
            + s->comPortCount * sizeof(SL_COM_PORT_STATUS_T)\
            + s->voltageSensorCount * sizeof(SL_VOLTAGE_SENSOR_STATUS_T)\
            + s->currentSensorCount * sizeof(SL_CURRENT_SENSOR_STATUS_T)\
            + s->scsiTargetPortCount * sizeof(SL_SCSI_TARGET_PORT_STATUS_T)\
            + s->scsiInitiatorPortCount * sizeof(SL_SCSI_INITIATOR_PORT_STATUS_T)\
            + s->simpleSubEnclCount * sizeof(SL_SIMPLE_SUB_ENCL_STATUS_T)\
            + s->arrayDeviceCount * sizeof(SL_ARRAY_DEVICE_STATUS_T)\
            + elemIdx * sizeof(SL_SAS_EXPANDER_STATUS_T))

// 25.  SAS Connector
    #define  SL_ADDR_OF_SAS_CONNECTOR_STATUS_EX(s/*ptr to struct of type SL_ENCL_STATUS_EX_T*/,elemIdx/*index of element*/)\
            ((U8*)s->deviceStatus + s->deviceCount * sizeof(SL_DEVICE_STATUS_T) \
            + s->powerSupplyCount * sizeof(SL_POWER_SUPPLY_STATUS_T) \
            + s->coolingCount * sizeof(SL_COOLING_STATUS_T) \
            + s->tempSensorCount * sizeof(SL_TEMP_SENSOR_STATUS_T)\
            + s->doorLockCount * sizeof(SL_DOOR_LOCK_STATUS_T) \
            + s->alarmCount * sizeof(SL_ALARM_STATUS_T)\
            + s->sesCtrlElecCount * sizeof(SL_SES_CTRL_ELEC_STATUS_T)\
            + s->sccCtrlElecCount * sizeof(SL_SCC_CTRL_ELEC_STATUS_T)\
            + s->nonVolatileCacheCount * sizeof(SL_NON_VOLATILE_CACHE_STATUS_T)\
            + s->invalidOpReasonCount * sizeof(SL_INVALID_OP_REASON_STATUS_T)\
            + s->upsCount *sizeof(SL_UPS_STATUS_T)\
            + s->displayCount* sizeof(SL_DISPLAY_STATUS_T)\
            + s->keyPadEntryCount * sizeof(SL_KEY_PAD_ENTRY_STATUS_T)\
            + s->enclEntryCount * sizeof(SL_ENCL_ENTRY_STATUS_T)\
            + s->scsiTransceiverPortCount * sizeof(SL_SCSI_TRANSCEIVER_STATUS_T)\
            + s->languageCount * sizeof(SL_LANGUAGE_STATUS_T)\
            + s->comPortCount * sizeof(SL_COM_PORT_STATUS_T)\
            + s->voltageSensorCount * sizeof(SL_VOLTAGE_SENSOR_STATUS_T)\
            + s->currentSensorCount * sizeof(SL_CURRENT_SENSOR_STATUS_T)\
            + s->scsiTargetPortCount * sizeof(SL_SCSI_TARGET_PORT_STATUS_T)\
            + s->scsiInitiatorPortCount * sizeof(SL_SCSI_INITIATOR_PORT_STATUS_T)\
            + s->simpleSubEnclCount * sizeof(SL_SIMPLE_SUB_ENCL_STATUS_T)\
            + s->arrayDeviceCount * sizeof(SL_ARRAY_DEVICE_STATUS_T)\
            + s->sasExpanderCount *  sizeof(SL_SAS_EXPANDER_STATUS_T)\
            + elemIdx * sizeof(SL_SAS_CONNECTOR_STATUS_T))
//CR Structure: Voltage Sensor

typedef struct _CFGI_PROD_ID

{

    U32 Signature;

    U8  VendorId[8];          /* ASCII Vendor ID */

    U8  ProductId[16];        /* ASCII Product ID */

    U8  ProductRevision[4];   /* Product Revision */

    U32 Reserved1;

    U32 Reserved2;

    U16 PCIVendorID;          /* indicates the exact NVDATA image for which it is intended */

                              /* 0xFFFF = matches any combination of PCI Vendor ID, PCI Device ID, */

                              /*          PCI Subsystem Vendor ID, and PCI Subsystem ID            */

    U16 PCIDeviceID;          /* Indicates the exact NVDATA image for which this image is intended */

    U16 PCISubsystemVendorID; /* Indicates the exact NVDATA image for which this image is intended */

    U16 PCISubsystemID;       /* Indicates the exact NVDATA image for which this image is intended */

    U8  PCIRevisionID;        /* Indicates the exact NVDATA image for which this image is intended */

                              /* 0xFF, indicates this image may be used with any of the chips indicated by the four fields above */

    U8  OEMID;                /* Application specified 8-bit binary field */

	U8  MaxPCIRevisionID;     /* If non-zero indicates the maximum supported PCI Revision ID */

    U8 Reserved5;

    U32 Reserved6;

    U32 Reserved7;

    U32 Reserved8;

} CFGI_PROD_ID, *PTR_CFGI_PROD_ID;



/* NVDATA Structure information */



typedef struct _CFGI_DIR_HEADER

{

    U32         Signature;                  /* 0x00 */

    U8          State;                      /* 0x04 */

    U8          Reserved1;                  /* 0x05 Reserved for Checksum */

    U16         TotalBytes;                 /* 0x06 Seeprom only */

    U32         NvdataVersion;              /* 0x08 NVDATA version  */

    U8          CdhSize;                    /* 0x0C CFGI_DIR_HEADER size in DWORDS */

    U8          CdeSize;                    /* 0x0D CFGI_DIR_ENTRY size in DWORDS */

    U8          PphSize;                    /* 0x0E CFGI_PERSISTENT_PAGE_HEADER size in DWORDS */

    U8          ProdIdSize;                 /* 0x0F CFGI_PROD_ID size in DWORDS */

    U32         NbrDirEntries;              /* 0x10 # of Dir Entries */

    U32         NbrPersistDirEntries;       /* 0x14 # of Dir Entries that have PERSISTENT Attribute */

    U32         Reserved3;                  /* 0x18 */

    U16         ProductIdOffset;            /* 0x1C Offset to CFGI_PROD_ID in DWORDS */

    U16         DirEntryOffset;             /* 0x1E Offset to CFGI_DIR_ENTRY in DWORDS */

    U32         VendorNvdataVersion;        /* 0x20 Vendor's NVDATA Version */

} CFGI_DIR_HEADER, *PTR_CFGI_DIR_HEADER;


#define CFGI_DIR_HEADER_SIGNATURE              (0x4E69636B)

/***************************************************************************
*
*       EFI Specific 
*
***************************************************************************/

typedef struct _SL_FW_DOWNLOAD_T {
	U8   ImageType;      // Type of Image to be flashed
	U8   isLastImage;    // Set this field only if the image being sent is the last component during noPad image download. 
	U8   performReset;   // Set this field only if the reset is required post firmware flash
	U32  TotalImageSize; // Total Size of the firmware Image 
	U32  ImageSize;      // Size of the region to be flashed
	U32  ImageOffset;    // Offset to be read from the buffer passed in pData, Mostly ZERO yet will change based on the changes in boot loader.
	void *pData;         // Pointer to buffer to send/receive data
}SL_FW_DOWNLOAD_T;

#define SL_FW_DOWNLOAD_S (sizeof(SL_FW_DOWNLOAD_T) - sizeof(void*))

typedef enum _SL_SBR_OP
{
	READ_SBR = 0,
	READ_SBR_BACKUP,
	WRITE_SBR_BACKUP,
	WRITE_SBR
}SL_SBR_OP;

/****************************************************************************
*
*        OOB Management
*
****************************************************************************/

typedef struct _SL_OOB_OBJ_INFO_T {
    U32 length; //size of entire data struct including length field
    U8 objType; //type SL_OOB_OBJ_TYPE
    U8 reserved[3];
    union {
       struct {                       // MR_OOB_OBJ_EVT_INFO
            U32  newestEventSeqNum;
            U32  newestEventSeqNumOOB;
       } eventInfo;

       struct {                       // MR_OOB_OBJ_CTRL_ADAPTER_STATUS
           U16  adapterStatus;              // MR_CTRL_INFO.adapterStatus
           U16  reserved;
       } adapterInfo;

       struct {                       // MR_OOB_OBJ_CTRL_TEMP_INFO
           U8   ctrltemp;                  // Temp in degrees C
           U8   ctrlROCTemp;               // Temp in degrees C
       } ctrlTemp;

       struct {                       // MR_OOB_OBJ_BBU_STATUS
           U16  voltage;                    // accurate battery terminal voltage in mV
           S16  current;                    // current being supplied (or accepted) through the battery’s terminals
           U32  fwStatus;                   // battery staus (type MR_BBU_FW_STATUS)
           U8   temperature;                // Temp in degrees C
           U8   reserved[3];                     // to compile
       } bbuStatus;

       struct {                       // MR_OOB_OBJ_PD_TEMP_INFO
           U8 count;                    //number of pds
           U8 reserved;
           struct {
               U8  deviceId;             // device id of pd
               U8   temperature;         // temperature of pd
           } pd [1];                     // variable array of status for count pds
       } pdTempInfo;

       struct {                       // MR_OOB_OBJ_PD_INFO
           U8 count;                    //number of pds
           U8 reserved;
           struct {
               U8  deviceId;             // device id of pd
               U8   state;               // fw state of the device (MR_PD_STATE)
               U8   reserved;
               U8   powerState  : 4;         // power condition of the device (MR_PD_POWER_STATE)
               U8   predFailure : 1;         // device is reporting a predictive failure
               U8   otherErr    : 1;        // transient error(s) were detected in communicating with this device
               U8   locate      : 1;
               U8   reserved2   : 1;         // For future use
           } pd [1];                     // variable array of status for count pds
       } pdInfo;


       MR_OOB_TYPE_BITMAP pdRemoved; // MR_OOB_OBJ_PD_REMOVED - bitmap of PDs removed since last poll

       struct {                       // MR_OOB_OBJ_LD_INFO
           U8 count;                    //number of pds
           U8 reserved;
           struct {
               U8   targetId;            // target id of ld
               U8   state;               // state of the LD (MR_LD_STATE)
               U8   currentCachePolicy;  // current cache based on battery state (MR_LD_CACHE)
               U8   powerState;          // power condition of the LD (MR_LD_PS_POLICY)
           } ld [1];                     // variable array of status for count pds
       } ldInfo;
    } record; //union
} SL_OOB_OBJ_INFO_T;


typedef struct _SL_OBJ_DETAIL_T
{
    U16 status;
    U8 reserved[2];
    U32 ctrlId;
    U32 registrationId;
    SL_OOB_OBJ_INFO_T objInfo;
}SL_OBJ_DETAIL_T;
#define SL_OBJ_DETAIL_S sizeof(SL_OBJ_DETAIL_T)


typedef struct _SL_OOB_OBJ_LIST_T
{
    U8 countObjects;
    U8 pad[3];

    SL_OOB_OBJ_INFO_T objInfo[1]; //a variable-length set of records for each object, which changed since last poll
} SL_OOB_OBJ_LIST_T;

typedef struct _SL_OOB_EVT_OBJ_LIST_T
{
    U8 countEvents;
    U8 countObjects;
    U8 pad[2];

    SL_OOB_OBJ_INFO_T objInfo[1]; //a variable-length set of records for each object, which changed since last poll
    MR_EVT_DETAIL events[1];      //a variable-length set of records for each event, which occurred since last poll
} SL_OOB_EVT_OBJ_LIST_T;


//OOB
typedef int (*pObjCallBackFunc) (SL_OBJ_DETAIL_T *pObj);
typedef struct _SL_REG_OOB_OBJ_INFO_T
{
    U32 ctrlId;
    MR_OOB_OBJ_TYPE  objType;
} SL_REG_OOB_OBJ_INFO_T;
#define SL_REG_OOB_OBJ_INFO_S sizeof(SL_REG_OOB_OBJ_INFO_T)

typedef struct _SL_REG_OOB_OBJ_INPUT_T
{
    U32 count;
    SL_REG_OOB_OBJ_INFO_T regObjInfo[SL_MAX_CONTROLLERS];
    pObjCallBackFunc pFunc;
} SL_REG_OOB_OBJ_INPUT_T;
#define SL_REG_OOB_OBJ_INPUT_S sizeof(SL_REG_OOB_OBJ_INPUT_T)

typedef struct _SL_REG_OOB_OBJ_OUTPUT_T
{
    U32 regCtrlCount;
    U32 regCtrlId[SL_MAX_CONTROLLERS];
    U32 uniqueId;
} SL_REG_OOB_OBJ_OUTPUT_T;
#define SL_REG_OOB_OBJ_OUTPUT_S sizeof(SL_REG_OOB_OBJ_OUTPUT_T)

/* Defines for erasing */
#define ERASE_NVSRAM            1
#define ERASE_BACKUP_FW         2
#define ERASE_PERSIS_PAGES      3
#define ERASE_MAN_PERSIS_PAGES  4
#define ERASE_BOOT_SERVICES     5
#define ERASE_CLEAN_FLASH       6
#define ERASE_ALL               7
#define ERASE_MEGARAID_FW       8
#define ERASE_ALL_BUT_MPB       9
#define ERASE_ENTIRE_FLASH      10


/*############################################################################
*
*        WARHAWK Specific definitions
*
############################################################################*/

/****************************************************************************
*
*        WARHAWK Direct Drive (DD) Controller INFO Structure
*
****************************************************************************/
typedef struct _SL_WH_DD_INFO_T
{
    U16         AuxilaryPowerPresent;       /* 0x00 This confirms the presence of Auxiliary power.
                                             *          0 - Aux Power not present
                                             *          1 - Aux Power Present
                                             *          0xFF - Aux Power not supported
                                             * This information will be fetched from the GPIOVal value in IO Unit Page3 */

    U16         LifeCurveStatus;            /* 0x02     100 - No throttling being done
                                             *          90 - One (or more) of the SSDs are being throttled */

    U32         SSDLifeLeft;                /* 0x04 Percentage of SSD life left.
                                             * This field reports worst case of all the SSD drives attached to the DD controller */

    U32         DDStatus;                   /* 0x08 Overall DD status */

    U8          LifeLeftWarningThreshold;   /* 0x0C SSD Life left warning threshold value
                                                    This is also called Life left yellow level
                                                    This will be read from MFG Page 10 */

    U8          LifeLeftErrorThreshold;     /* 0x0D SSD Life left error threshold value
                                                    This is also called Life left Red level
                                                    This will be read from MFG Page 10 */

    U16          TemperatureWarningThreshold;/* 0x0E This is temperature yellow level
                                                    This will be read from IO Unit Page 8 */

    U16          TemperatureErrorThreshold;  /* 0x10 This is temperature red level
                                                    This will be read from IO Unit Page 8 */

    U16         Temperature;                /* 0x12 Temperature of each SSD */

    U8         ReadOnlyState;                  /* 0x14 Readonly State */
    
    U8         Reserved1;                  /* 0x15 Reserved */

} SL_WH_DD_INFO_T, *PTR_SL_WH_DD_INFO_T;

#define SL_WH_DD_INFO_S sizeof(SL_WH_DD_INFO_T)


/****************************************************************************
*
*        WARHAWK SSD INFO Structure
*
****************************************************************************/

typedef struct _SL_WH_SSD_INFO_T
{
    U8          Cage;                                       /* 0x00 Current Cage that holds the SSD */

    U8          Location;                                   /* 0x01 Current location of SSD 0 - Upper, 1 - Lower */

    U16         Temperature;                                /* 0x02 Temperature of each SSD */

    U16         FlashType;                                  /* 0x04 Flash Type being used (SLC / MLC / EMLC) */

    U16         LifeCurveStatus;                            /* 0x06     100 - No throttling being done
                                                             *          90 - SSD is being throttled */

    U32         SSDLifeLeft;                                /* 0x08 Percentage of SSD life left */

    U8          LifeLeftWarningThreshold;                   /* 0x0C SSD Life left warning threshold value
                                                                    This is also called Life left yellow level
                                                                    This will be read from MFG Page 10 */

    U8          LifeLeftErrorThreshold;                     /* 0x0D SSD Life left error threshold value
                                                                    This is also called Life left Red level
                                                                    This will be read from MFG Page 10 */

    U16          TemperatureWarningThreshold;               /* 0x0E This is temperature yellow level
                                                                    This will be read from IO Unit Page 8 */

    U16          TemperatureErrorThreshold;                 /* 0x10 This is temperature red level
                                                                    This will be read from IO Unit Page 8 */

    U8          ReadOnlyState;                  /* 0x12 Readonly State */
    
    U8          Reserved1;                  /* 0x13 Reserved */

} SL_WH_SSD_INFO_T, *PTR_SL_WH_SSD_INFO_T;

#define SL_WH_SSD_INFO_S sizeof(SL_WH_SSD_INFO_T)

typedef struct _SL_WH_CURRENT_ATTRIBUTES
{
    U64  RawReadErrorRate;       /* Bytes Read */
    U32  SATACRCErrorCount;      /* SATA R-Errors (CRC) Error Count */
    U32  ECCOnFlyCount;          /* Uncorrectable ECC Errors */
    U8   Temperature;            /* Current Temperature */
    U8   UncorrectableErrors;    /* Uncorrectable RAISE Errors */
    U8   WearRangeDelta;         /* Wear Range Delta */
    U8   LifeCurveStatus;        /* Drive Life Protection Status */
} SL_WH_CURRENT_ATTRIBUTES, *PTR_SL_WH_CURRENT_ATTRIBUTES;

typedef struct _SL_WH_CUMULATIVE_ATTRIBUTES
{
    U32   RetiredBlockCount;                  /* Retired Block Count */
    U32   POH;                                /* Power-On Hours */
    U32   DevPowerCycleCount;                 /* Device Power Cycle Count */
    U32   UnxepectedPowerLossCount;           /* Unexpected Power Loss Count */
    U32   IOErrorDetectionRate;               /* I/O Error Detection Code Rate */
    U32   ReservedBlockCount;                 /* Reserved (over-provisioned) Blocks */
    U32   ProgramFailCount1;                  /* Program Fail Count */
    U32   EraseFailCount1;                    /* Erase Fail Count */
    U32   SoftReadErrorRate;                  /* Soft Read Error Count */
    U8    UncorrectableErrors;                /* Uncorrectable RAISE Errors */
    U8    LifeLeft;                           /* SSD Life Left */
	U8	  MaxTemperature;					  /* Highest temperature sampled over the lifetime of the device */
    U32   LifeTimeWrites;                     /* Total Writes From Host */
    U32   LifeTimeReads;                      /* Total Reads To Host */
    U32   PECycles;                           /* Gigabytes Erased */
    float WriteAmplification;                 /* Write amplification is not applicable if the value in this field is 0 */
	U32	  CacheDataAge;						  /* Cached SMART Data Age in seconds */
	U32	  POMS;								  /* Power-On MilliSecond */
} SL_WH_CUMULATIVE_ATTRIBUTES, *PTR_SL_WH_CUMULATIVE_ATTRIBUTES;

typedef struct _SL_WH_DFF_HEALTH
{
    U8                          Slot;
    U8                          Cage;
    U8                          Location;
    SL_WH_CURRENT_ATTRIBUTES    Current;
    SL_WH_CUMULATIVE_ATTRIBUTES Cumulative;
} SL_WH_DFF_HEALTH, *PTR_SL_WH_DFF_HEALTH;

typedef struct _SL_WH_SAS_CURRENT_ATTRIBUTES
{
    U64  RawReadErrorRate;       /* Bytes Read */
    U32  eccRate;                /* ECC Rate */
    U32  errPrimCount;           /* Error Primitive Count */
    U32  uncorrectableErrCnt;    /* Uncorrectable Error Count */
    U32  Temperature;            /* Current Temperature */
} SL_WH_SAS_CURRENT_ATTRIBUTES, *PTR_SL_WH_SAS_CURRENT_ATTRIBUTES;

typedef struct _SL_WH_SAS_CUMULATIVE_ATTRIBUTES
{
    U32   reallocBlkCnt;                      /* Reallocated Block Count */
    U32   POH;                                /* Power-On Hours */
    U32   accuStartStopCycles;                /* Accumulated Start Stop Cycles */
    U32   ReservedBlockCount;                 /* Unused Reserved Block Count */
    U32   totalUncorrectedWriteErr;           /* Total Uncorrected Write Errors */
    U32   eraseFailCnt;                       /* Erase Fail Count */
    U32   maxLifetimeTemp;                    /* Maximum Lifetime Temperature */
    U8    LifeLeft;                           /* SSD Life Left */
    U64   gbErased;                           /* Gigabytes Erased */
    U64   LifeTimeWrites;                     /* Total Writes From Host */
    U64   LifeTimeReads;                      /* Total Reads To Host */
    float WriteAmplification;                 /* Write amplification is not applicable if the value in this field is 0 */
} SL_WH_SAS_CUMULATIVE_ATTRIBUTES, *PTR_SL_WH_SAS_CUMULATIVE_ATTRIBUTES;

#define LIFE_CURVE_STATUS_NOT_SUPPORTED 0xFF

typedef struct _SL_WH_SAS_DFF_HEALTH
{
    U8                              Slot;
    U8                              Cage;
    U8                              Location;
    SL_WH_SAS_CURRENT_ATTRIBUTES    Current;
    SL_WH_SAS_CUMULATIVE_ATTRIBUTES Cumulative;
} SL_WH_SAS_DFF_HEALTH, *PTR_SL_WH_SAS_DFF_HEALTH;

#define MAX_BAY (8)

/****************************************************************************
*
*        WARHAWK DFF Health INFO Structure
*
****************************************************************************/
typedef struct _SL_WH_DFF_HEALTH_INFO_T
{
    U8               NumDFFs;
    SL_WH_DFF_HEALTH DFFHealth[MAX_BAY];

} SL_WH_DFF_HEALTH_INFO_T, *PTR_SL_WH_DFF_HEALTH_INFO_T;

#define SL_WH_DFF_HEALTH_INFO_S sizeof(SL_WH_DFF_HEALTH_INFO_T)

typedef struct _SL_WH_SAS_DFF_HEALTH_INFO_T
{
    U8                    NumDFFs;
    SL_WH_SAS_DFF_HEALTH  DFFHealth[MAX_BAY];

} SL_WH_SAS_DFF_HEALTH_INFO_T, *PTR_SL_WH_SAS_DFF_HEALTH_INFO_T;

#define SL_WH_SAS_DFF_HEALTH_INFO_S sizeof(SL_WH_SAS_DFF_HEALTH_INFO_T)

#define PRODUCT_SPECIFIC_DWORDS (23)

/****************************************************************************
*
*    WARHAWK Direct Drive (DD) Controller Diagnostic Buffer Query Structure
*
****************************************************************************/
typedef struct _SL_WH_DD_DIAGNOSTIC_BUFFER_QUERY_T
{
    U8          BufferType;                       /* 0x00 Currently only TRACE buffer is supported, but long term may support SNAPSHOT & EXTENTED */
    U16         Flags;                            /* 0x01 Flags indicating BUFFER_VALID / BUFFER_OWNED_BY_APP / BUFFER_OWNED_BY_FIRMWARE */
    U32         Size;                             /* 0x03 Buffer size in bytes, if buffer is a valid buffer */
} SL_WH_DD_DIAGNOSTIC_BUFFER_QUERY_T, *PTR_SL_WH_DD_DIAGNOSTIC_BUFFER_QUERY_T;

#define SL_WH_DD_DIAGNOSTIC_BUFFER_QUERY_S sizeof(SL_WH_DD_DIAGNOSTIC_BUFFER_QUERY_T)

/*****************************************************************************
*
*    WARHAWK Direct Drive (DD) Controller Diagnostic Buffer Register Structure
*
*******************************************************************************/
typedef struct _SL_WH_DD_DIAGNOSTIC_BUFFER_REGISTER_T
{
    U8          BufferType;                                    /* 0x00 Currently only TRACE buffer is supported, but long term may support SNAPSHOT & EXTENTED */
    U8          ExtendedType;                                  /* 0x01 Not supported initially. This will be used when Extended Buffer Type is supported */
    U32         BufferSize;                                    /* 0x02 Size, in bytes, of data buffer to register. Minimum buffer size is 20 kB = 20 * 1024 = 20480 bytes. Register command fails below this. */
    U32         DiagnosticFlags;                               /* 0x06 Defines processing of the Diagnostic Buffer. ReleaseOnFull - If Trace Buffer / ImmediateRelease - If Snapshot buffer */
    U32         ProductSpecific[PRODUCT_SPECIFIC_DWORDS]; /* 0x0A Array of 92 bytes of product specific data */
} SL_WH_DD_DIAGNOSTIC_BUFFER_REGISTER_T, *PTR_SL_WH_DD_DIAGNOSTIC_BUFFER_REGISTER_T;

#define SL_WH_DD_DIAGNOSTIC_BUFFER_REGISTER_S sizeof(SL_WH_DD_DIAGNOSTIC_BUFFER_REGISTER_T)

/****************************************************************************
*
*    WARHAWK Direct Drive (DD) Controller Diagnostic Buffer Read Structure
*
****************************************************************************/
typedef struct _SL_WH_DD_DIAGNOSTIC_BUFFER_READ_T
{
    U8          BufferType;    /* Currently only TRACE buffer is supported, but long term may support SNAPSHOT & EXTENTED */
    U32*        PtrDataBuffer; /* Pointer to data buffer of size specified by PtrBufferSize. The calling application will allocate memory for the data buffer equal to the size input in PtrBufferSize, but the actual size of data copied by API may be less, as returned by PtrBufferSize. */
    U32*        PtrBufferSize; /* Pointer to the size in bytes of data buffer to read. The API will return the size of actual data buffer copied back to calling application and may be less than the input size. */
} SL_WH_DD_DIAGNOSTIC_BUFFER_READ_T, *PTR_SL_WH_DD_DIAGNOSTIC_BUFFER_READ_T;

#define SL_WH_DD_DIAGNOSTIC_BUFFER_READ_S sizeof(SL_WH_DD_DIAGNOSTIC_BUFFER_READ_T)

/*******************************************************************************
*
*    WARHAWK Direct Drive (DD) Controller Diagnostic Buffer Unregister Structure
*
*******************************************************************************/
typedef struct _SL_WH_DD_DIAGNOSTIC_BUFFER_UNREGISTER_T
{
    U8          BufferType;                                    /* 0x00 Currently only TRACE buffer is supported, but long term may support SNAPSHOT & EXTENTED */
} SL_WH_DD_DIAGNOSTIC_BUFFER_UNREGISTER_T, *PTR_SL_WH_DD_DIAGNOSTIC_BUFFER_UNREGISTER_T;

#define SL_WH_DD_DIAGNOSTIC_BUFFER_UNREGISTER_S sizeof(SL_WH_DD_DIAGNOSTIC_BUFFER_UNREGISTER_T)

/*****************************************************************************
*
*    WARHAWK Direct Drive (DD) Controller Diagnostic Buffer Release Structure
*
******************************************************************************/
typedef struct _SL_WH_DD_DIAGNOSTIC_BUFFER_RELEASE_T
{
    U8          BufferType;                                    /* 0x00 Currently only TRACE buffer is supported, but long term may support SNAPSHOT & EXTENTED */
} SL_WH_DD_DIAGNOSTIC_BUFFER_RELEASE_T, *PTR_SL_WH_DD_DIAGNOSTIC_BUFFER_RELEASE_T;

#define SL_WH_DD_DIAGNOSTIC_BUFFER_RELEASE_S sizeof(SL_WH_DD_DIAGNOSTIC_BUFFER_RELEASE_T)



typedef enum _SL_WH_SSD_CAGE
{
    SL_WH_SSD_CAGE_1        = 1,
    SL_WH_SSD_CAGE_2        = 2,
    SL_WH_SSD_CAGE_3        = 3,
    SL_WH_SSD_CAGE_4        = 4,
    SL_WH_SSD_CAGE_INVALID  = 0xFF

}SL_WH_SSD_CAGE;

typedef enum _SL_WH_SSD_LOCATION
{
    SL_WH_SSD_LOCATION_LOWER    = 0,
    SL_WH_SSD_LOCATION_UPPER    = 1,
    SL_WH_SSD_LOCATION_INVALID  = 0xFF

}SL_WH_SSD_LOCATION;

typedef enum _SL_WH_DD_AUX_POWER
{
    SL_WH_DD_AUX_POWER_NOT_PRESENT = 0,
    SL_WH_DD_AUX_POWER_PRESENT = 1,
    SL_WH_DD_AUX_POWER_NOT_SUPPORTED = 0xFF

}SL_WH_DD_AUX_POWER;

#define IR_DRIVE_LIFE_SSD_THROTTLING_OFF    (100)      /* Throttling disabled */
#define IR_DRIVE_LIFE_SSD_THROTTLING_ON     (90)       /* Throttling enabled  */

typedef enum _SL_WH_SSD_FLASH_TYPE
{
    SL_WH_SSD_FLASHTYPE_SLC,
    SL_WH_SSD_FLASHTYPE_MLC,
    SL_WH_SSD_FLASHTYPE_EMLC,
    SL_WH_SSD_FLASHTYPE_CMLC,
    SL_WH_SSD_FLASHTYPE_UNKNOWN = 0xFF

}SL_WH_FLASH_TYPE;

typedef enum _SL_WH_DD_STATUS
{
    SL_WH_DD_STATUS_GOOD,
    SL_WH_DD_STATUS_WARNING,
    SL_WH_DD_STATUS_ERROR,
    SL_WH_DD_STATUS_UNKNOWN = 0xFF

}SL_WH_DD_STATUS;

/****************************************************************************
*
*        WARHAWK SSD Panic Log Structure
*
****************************************************************************/
#define MAX_PANIC_SLOTS                 4

typedef struct _SL_WH_DD_PANIC_INFO_SLOT_T
{
    U16     ValidEntry;
    U32     HeaderPrefixLow;
    U32     HeaderPrefixHigh;
    U32     TransactionNumber;
    U32     SyncCode;
    U32     HeaderSize;
    U32     DataSize;
} SL_WH_DD_PANIC_INFO_SLOT_T;

typedef struct _SL_WH_DD_PANIC_INFO_T
{
    SL_WH_DD_PANIC_INFO_SLOT_T  PanicInfo[MAX_PANIC_SLOTS];
} SL_WH_DD_PANIC_INFO_T;

#define SL_WH_DD_PANIC_INFO_S sizeof(SL_WH_DD_PANIC_INFO_T)

/****************************************************************************
*
*        WARHAWK Trigger Definition Structures
*
****************************************************************************/

#define NUM_VALID_ENTRIES  20

/*
 * Event Trigger(MPI Events and MPI Logs)
 */

typedef struct _SL_WH_EVENT_TRIGGER_T
{
    U16          EventValue;
    U16          LogEntryQualifier;
} SL_WH_EVENT_TRIGGER_T;

typedef struct _SL_WH_EVENT_TRIGGERS_T
{
    U32                   ValidEntries;
    SL_WH_EVENT_TRIGGER_T EventTriggerEntry[NUM_VALID_ENTRIES];
} SL_WH_EVENT_TRIGGERS_T;
#define SL_WH_EVENT_TRIGGERS_S sizeof(SL_WH_EVENT_TRIGGERS_T)

/*
 * SCSI Trigger
 */

typedef struct _SL_WH_SCSI_TRIGGER_T
{
    U8 ASCQ;
    U8 ASC;
    U8 SenseKey;
    U8 Reserved;
} SL_WH_SCSI_TRIGGER_T;

typedef struct _SL_WH_SCSI_TRIGGERS_T
{
    U32                   ValidEntries;
    SL_WH_SCSI_TRIGGER_T  SCSITriggerEntry[NUM_VALID_ENTRIES];
} SL_WH_SCSI_TRIGGERS_T;
#define SL_WH_SCSI_TRIGGERS_S sizeof(SL_WH_SCSI_TRIGGERS_T)

/*
 * MPI Triggers
 */

typedef struct _SL_WH_MPI_TRIGGER_T
{
    U16 IOCStatus;
    U16 Reserved;
    U32 IOCLogInfo;
} SL_WH_MPI_TRIGGER_T;

typedef struct _SL_WH_MPI_TRIGGERS_T
{
    U32                   ValidEntries;
    SL_WH_MPI_TRIGGER_T   MPITriggerEntry[NUM_VALID_ENTRIES];
} SL_WH_MPI_TRIGGERS_T;
#define SL_WH_MPI_TRIGGERS_S sizeof(SL_WH_MPI_TRIGGERS_T)


/****************************************************************************
*
*        WARHAWK Health Structure
*
****************************************************************************/

typedef struct _SL_WH_DFF_NON_SMART_INFO_T
{
    U8          FlashType;                                  /* Flash Type being used (SLC / MLC / EMLC) */
	U8			DeviceId;
    U8          DevProtocol;                                /* 0-Unknown, 1-parallel SCSI, 2-SAS, 3-SATA, 4-FC
                                                             						  * derived from   MR_PD_INFO.interfaceType */
	U8			LockedUp;                                   /* 1-SSD is locked up, 0-SSD is not locked up */
	U8			AccessStatus;								/* if 1 then in ReadOnlyMode */
	U16			FWState;
	U8			PartOfVD;
	U8			Reserved1;
	U8			SerialNumber[20];
	U32			Reserved2[5];
} SL_WH_DFF_NON_SMART_INFO_T, *PTR_SL_WH_DFF_NON_SMART_INFO_T;

typedef struct _SL_WH_CTRL_HEALTH_T
{
    U64         sasAddress;
    U8          FWMode;						// IT - 0, IR - 1
    U8          LifeLeftErrorThreshold;
    U8          LifeLeftWarningThreshold;
	U8			Reserved1;
    U16         TemperatureWarningThreshold;
    U16         TemperatureErrorThreshold;
    U8          AuxPowerStatus;
    U8          Ioc6MaxDrivesR0;
	U16			Reserved2;
	U32			Reserved3;
	U64         SlotTrimCount[MAX_BAY];
    U32         ldCount;
    U8          ldStatus;    //Volume is ACTIVE or INACTIVE
    U8          ldState;                //same as ldConfig.params.state
	U16			Reserved4;
    U8          BoardName[16];
	U8			BoardType;
    SL_WH_DFF_HEALTH_INFO_T DFFHealthInfo;  //All the fields are needed from this existing structure
	SL_WH_DFF_NON_SMART_INFO_T DFFNonSmartInfo[MAX_BAY];
    U32         Reserved5[10];

} SL_WH_CTRL_HEALTH_T, *PTR_SL_WH_CTRL_HEALTH_T;

#define SL_WH_CTRL_HEALTH_S sizeof(SL_WH_CTRL_HEALTH_T)


/****************************************************************************
*
*        CCOH DATA STRUCTURES
*
****************************************************************************/
#define SL_CCOH_CONTROLLER_ID    0x00FFFFFF
#define SL_CCOH_MAX_DEVICES      256
#define SL_CCOH_MAX_NAME_LEN     256
#define SL_CCOH_MAX_DEV_ID_LEN   96      
#define SL_CCOH_MAX_MEMBER_LEN   64
#define SL_CCOH_ARR_BIT_MAP_SIZE 1024

// CCoH Device Type
typedef enum _SL_CCOH_DEV_TYPE
{
    SL_CCOH_CACHE_DEV = 0,
    SL_CCOH_VIRTUAL_DEV
} SL_CCOH_DEV_TYPE;

// CCoH Device Topology
typedef enum _SL_CCOH_DEV_TOPOLOGY
{
    SL_CCOH_SAN_DEVICE = 0,
    SL_CCOH_DAS_DEVICE = 1,
    SL_CCOH_UNKNOWN_DEVICE = 0xFF /*used only in the case of configured and offline devices */
} SL_CCOH_DEV_TOPOLOGY;

// CCoH Device State.
typedef enum _SL_CCOH_DEV_STATE
{
    SL_CCOH_DEV_ONLINE = 0,
    SL_CCOH_DEV_OFFLINE,
    SL_CCOH_DEV_READY_FOR_ONLINE,
    SL_CCOH_DEV_UNKNOWN = 0xFF
} SL_CCOH_DEV_STATE;

// CCoH Device Configuration State.
typedef enum _SL_CCOH_DEV_CONFIG_STATE
{
    SL_CCOH_DEV_CONFIGURED = 0,
    SL_CCOH_DEV_UNCONFIGURED
} SL_CCOH_DEV_CONFIG_STATE;

// CCoH Device Cachin Mode.
typedef enum _SL_CCOH_DEV_CACHING_MODE
{
    SL_CCOH_DEV_READ_CACHE = 0,
    SL_CCOH_DEV_WRITE_THROUGH,
    SL_CCOH_DEV_WRITE_BACK
}SL_CCOH_DEV_CACHING_MODE;
// CCoH Device Content.
typedef enum _SL_CCOH_DEV_CONTENT
{
    SL_CCOH_DEV_NO_CONTENT_DETECTED = (0x0 << 0),
    SL_CCOH_DEV_CONTENT_UNKNOWN     = (0x1 << 0), /*used only in the case of configured and offline devices */
    SL_CCOH_DEV_MBR                 = (0x1 << 1),
    SL_CCOH_DEV_SWAP                = (0x1 << 2),
    SL_CCOH_DEV_LVM2_MEMBER         = (0x1 << 3),
    SL_CCOH_DEV_FILE_SYSTEM         = (0x1 << 4),
    SL_CCOH_DEV_FOREIGN             = (0x1 << 5)
} SL_CCOH_DEV_CONTENT;

// CCoH Device parameters. Data returned by StoreLibIR2 to clients.
typedef struct _SL_CCOH_DEV_T
{
    U8              devPath[SL_CCOH_MAX_NAME_LEN];          // Device node path
    U8              devMapperPath[SL_CCOH_MAX_NAME_LEN];    // Device mapper path, valid for a configured VD.
    U8              devPathOS[SL_CCOH_MAX_MEMBER_LEN];      // OS dependent node name (/dev/sda)
    U8              vendor[SL_CCOH_MAX_MEMBER_LEN];         // Vendor Name
    U8              model[SL_CCOH_MAX_MEMBER_LEN];          // Model Name
    U8              devId[SL_CCOH_MAX_DEV_ID_LEN];          // Unique device Id
    U8              devType;                                // Device Type (SL_CCOH_DEV_TYPE)
    U8              devState;                               // Device State (SL_CCOH_DEV_STATE)
    U8              devTopology;                            // Device Topology (SL_CCOH_DEV_TOPOLOGY)
    U8              devConfigState;                         // Device configuration state (SL_CCOH_DEV_CONFIG_STATE)
    U32             devContent;                             // Device content (SL_CCOH_DEV_CONTENT)
    U64             capacity;                               // Device Capacity in LBA
    U64             resvd[2];                               // Two fields kept as reserved
} SL_CCOH_DEV_T;
#define SL_CCOH_DEV_S sizeof(SL_CCOH_DEV_T)

// CCoH Device list. Used by StoreLibIR2 to return list of devices to the clients.
typedef struct _SL_CCOH_DEV_LIST_T
{
    U32             count;                                  // number of active devices in the list.
    SL_CCOH_DEV_T   deviceList[SL_CCOH_MAX_DEVICES];        // device list
    U8              pad[4];                                 // Pad bytes
    U64             resvd[2];                               // Two fields kept as reserved
} SL_CCOH_DEV_LIST_T;
#define SL_CCOH_DEV_LIST_S sizeof(SL_CCOH_DEV_LIST_T)

// CCoH Config Parameters. Used by StoreLibIR2 to return the CCoH Config to the clients.
// Note: CCoH info is from software layer hence no Big Endian repreasentation for configActive.
typedef struct _SL_CCOH_CONFIG_T
{
    U32                         magicNum;
    U64                         seqNum;
    U8                          capability;
    U8                          versionNum;
    U8                          cacheGroupName[SL_CCOH_MAX_NAME_LEN];
    SL_CCOH_DEV_LIST_T          cacheDeviceList;
    SL_CCOH_DEV_LIST_T          virtualDriveList;
    U8                          configActive : 1;          // configuration status, 1=Active, 0=Inactive
    U8                          reserved     : 7;
    U8                          pad;                       // Pad bytes
    U64                         resvd[2];                  // Two fields kept as reserved
} SL_CCOH_CONFIG_T;
#define SL_CCOH_CONFIG_S sizeof(SL_CCOH_CONFIG_T)

// CCoH cache group statistics, Used by StoreLibIR2 to return the cache group stats
typedef struct _SL_CCOH_CG_STATS_T
{
    /* Device IO Statistics */
    U8                 cacheGroupName[SL_CCOH_MAX_NAME_LEN];    // Cache group name
    U32                numVds;           // Number of VDs
    U32                numCdevs;         // Number of cache devices
    U32                queueDepth;       // Queue Depth
    U64                totalCacheSize;   // Total cache size in blocks
    U64                cacheSizeInUse;   // Cache size currently in use
    U64                cacheLineSize;    // Cache line size in blocks
    U64                cacheWindowSize;  // Cache window size in blocks
    U64                reads;            // Number of reads
    U64                writes;           // Number of writes
    U64                cacheHits;        // Number of cache hits
    U64                cacheMisses;      // Number of cache misses
    U64                hitCount;         // Number of hits before a region
                                         // is considered hot
    U64                hitTime;          // Time period in IOs during which
                                         // the region needs to be accessed
                                         // to qualify as hot data
    U8                 cachingMode;  // Cache Mode
    U8                 pad[3];           // Pad bytes
    U64                resvd[2];         // Two fields kept as reserved
} SL_CCOH_CG_STATS_T;
#define SL_CCOH_CG_STATS_S sizeof(SL_CCOH_CG_STATS_T)

// Cache Group limits. Used by StoreLibIR2 to return the cache group max limits supported
typedef struct __SL_CCOH_CG_LIMITS_T
{
    U32    maxCacheDevicesSupported;    // max cache devices supported
    U32    maxVirtualDrivesSupported;   // max VDs supported
    U64    maxCacheCapacitySupported;   // max cache capacity supported in LBAs
    U64    resvd[2];                    // Two fields kept as reserved
} SL_CCOH_CG_LIMITS_T;
#define SL_CCOH_CG_LIMITS_S sizeof(SL_CCOH_CG_LIMITS_T)

// Cache Group limits. Used by StoreLibIR2 to set the new device state
typedef struct __SL_CCOH_DEV_STATE_CHANGE_T
{
    SL_CCOH_DEV_T   device;         // Device
    U8              newDevState;    // New device state
    U8              pad[7];         // Pad bytes
    U64             resvd[2];       // Two fields kept as reserved
} SL_CCOH_DEV_STATE_CHANGE_T;
#define SL_CCOH_DEV_STATE_CHANGE_S sizeof(SL_CCOH_DEV_STATE_CHANGE_T)

// VD Perf Stats. Used to provide Performance Stats per VD
typedef struct __SL_CCOH_DEV_STATS_T
{ 
    /**< device id */
    U64         dev_id;
    /**< device type */
    U8          dev_type;
    /**< device state */
    U8          dev_state;
    /**< alignment filler */
    U16         dev_props_filler;
    U8          pad[4];
    /**< device Path */
    char        dev_path[SL_CCOH_MAX_NAME_LEN];
    /**< device WWN ID */
    char        dev_wwn_id[SL_CCOH_MAX_DEV_ID_LEN];
    /**< no of cache windows used by this dev*/
    U64         num_cwcbs;
    /**< no of read requests on this ccoh dev*/
    U64         reads;
    /**< no of write requests on this ccoh dev*/
    U64         writes;
    /**< no of read blocks on this ccoh dev*/
    U64         read_blks;
    /** no of write blocks on this ccoh dev*/
    U64         write_blks;
    /**< no of cache hits on this ccoh dev */
    U64         cache_hits;
    /**< no of cache misses on this ccoh dev */
    U64         cache_misses;
    /**< no of cache read fill reqs on this ccoh dev */
    U64         cache_read_fill;
    /**< no of cache hit blocks on this ccoh dev */
    U64         cache_hit_blks;
    /**< no of cache miss blocks on this ccoh dev */
    U64         cache_miss_blks;
    /**< no of cache write hits on this ccoh dev */
    U64         write_hits;
    /**< total amount of time spent by requests in buffer wait queue*/
    U64         acc_buf_wait_time;
    /**< total number of buffer waiters*/
    U64         buf_waiters;
    /**< total number of multi cache window reads*/
    U32         multi_cw_rds;
    /**< total number of multi cache window writes*/
    U32         multi_cw_wrts;
    /**< total cache windows for multi-cache window read requests*/
    U32         acc_multi_cw_rds;
    /**< total cache windows for multi-cache window write requests*/
    U32         acc_multi_cw_wrts;
    /**< total number of read errors*/
    U64         rd_err_cnt;
    /**< total number of write errors*/
    U64         wr_err_cnt;
    /**< no of reads sent to backend disk*/
    U64         be_reads;
    /**< no of writes sent to backend disk*/
    U64         be_writes;
    /**< no of read blocks sent to backend disk*/
    U64         be_read_blks;
    /**< no of write blocks sent to backend disk*/
    U64         be_write_blks;
    /**< total amount of time spent by read reqs on backend disk */
    U64         be_acc_read_lat;
    /**< total amount of time spent by write reqs on backend disk */
    U64         be_acc_write_lat;
    /**< number of low level reads at the driver level*/
    U64         be_ll_rds;
    /**< number of low level read blocks at the driver level*/
    U64         be_ll_rd_blks;
    /**< Total accumulated low level read latency*/
    U64         be_ll_rd_lat;

    /**< number of low level writes at the driver level*/
    U64         be_ll_wrs;
    /**< number of low level write blocks at the driver level*/
    U64         be_ll_wr_blks;
    /**< Total accumulated low level write latency*/
    U64         be_ll_wr_lat;
    /** < reserved for future */
    U64         resvd[6];

} SL_CCOH_DEV_STATS_T;
#define SL_CCOH_DEV_STATS_S sizeof(SL_CCOH_DEV_STATS_T)

// CCoH Cache Device Bit Map, Sender should keep on sending in SL_CCOH_ARR_BIT_MAP_SIZE packet size 
// last packet would be less than SL_CCOH_ARR_BIT_MAP_SIZE or a complete zeroes.
typedef struct __SL_CCOH_DEV_MAP_T 
{
    SL_CCOH_DEV_T   cacheDev;
    U64             bitMapArrSize;                          // should indicate the size of the bitmap being send right now
    U64             bitMapTotSize;                          // Total Bitmap Size to be transfered
    U64             changeCacheBySize;                      // Optional used in case of Shrink Cache
    U64             mapUpdateIndex;                         // Map Update Index
    U8              cacheBitMap[SL_CCOH_ARR_BIT_MAP_SIZE];  // Send/Recv in SL_CCOH_ARR_BIT_MAP_SIZE
    U64             resvd[2];                               // Two fields kept as reserved
} SL_CCOH_DEV_MAP_T;
#define SL_CCOH_DEV_MAP_S sizeof(SL_CCOH_DEV_MAP_T)

/*
 * Size of Safe ID String. This is WarpDrive specific.
 */
#define SAFE_ID_STRING_SIZE (40)

#pragma pack() //@@@ Add all data structures above this inside pragma pack decl @@@@
#endif //__STORELIB_H__
