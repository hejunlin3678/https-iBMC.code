
/****************************************************************
 * FILE        : slerrors.h
 * DESCRIPTION : List of StoreLib errors
 *****************************************************************
 *  Copyright (c) 2014-2016 Avago Technologies. All rights reserved
 *****************************************************************

 ******************************************************************************
 * Disclaimer:
 * ----------
 * The definitions in this file are still in development stages
 * and is subject to change.
 *******************************************************************************/

#ifndef __SLERRORS_H__
#define __SLERRORS_H__

#define SL_SUCCESS                                      0 //Command was executed successfully

#define SL_ERR_MAIN                                     0x8000 //Storelib Error Code Offset
#define SL_ERR_IR_MAIN                                  0x8100 //StorelibIR Error Code Offset
#define SL_ERR_IR_MEGARAID_MAIN                         0x8200 //StorelibIR MegaRAID Error Code Offset
#define SL_ERR_WH_MAIN                                  0x8300 //StorelibIR2 WarHawk Error Code Offset
#define SL_ERR_IR_FW_MAIN                               0x4000 //IR Firmware Error Code Offset
#define SL_ERR_WH_FW_MAIN                               0x5000 //WH Firmware Error Code Offset
#define SL_ERR_CCOH_MAIN                                0x6000 //CCoH Error code offset
#define SL_ERR_MAIN_MCTP                                0x7000 //MCTP error code offset
#define SL_ERR_NYTRO_MAIN                               0x8400 //Nytro Error code offset
#define SL_ERR_MAIN_I2C                                 0x9000
#define SL_ERR_INIT_NOT_DONE                            SL_ERR_MAIN + 0x01 //Initialization not completed.
                                                                           //Will be returned when any SASLib API
                                                                           //is called prior to calling InitLib
#define SL_ERR_INIT_FAILED                              SL_ERR_MAIN + 0x02 //Initialization failed. If any one of the initialization steps fails.
#define SL_ERR_WMI_INIT_FAILED                          SL_ERR_MAIN + 0x03 //WMI Initialization failed
#define SL_ERR_LINUX_AEN_INIT_FAILED                    SL_ERR_MAIN + 0x03 //Linux AEN Initialization failed
#define SL_ERR_AEN_INIT_FAILED                          SL_ERR_MAIN + 0x03 //AEN Initialization failed
#define SL_ERR_WMI_REGISTRATION_FAILED                  SL_ERR_MAIN + 0x04 //Failed to register with WMI
#define SL_ERR_LINUX_AEN_REGISTRATION_FAILED            SL_ERR_MAIN + 0x04 //Linux AEN Registration failed
#define SL_ERR_WMI_UNREGISTRATION_FAILED                SL_ERR_MAIN + 0x05 //Failed to unregister from WMI
#define SL_ERR_AEN_UNREGISTRATION_FAILED                SL_ERR_MAIN + 0x05 //Failed AEN unregistration
#define SL_ERR_COM_INIT_FAILED                          SL_ERR_MAIN + 0x06 //COM Initialization failed
#define SL_ERR_INVALID_EVENT_MASK                       SL_ERR_MAIN + 0x07 //The event mask is invalid
#define SL_ERR_SIMULATION_FILE_MISSING                  SL_ERR_MAIN + 0x08 //The dll required to provide simulation functionalities is missing
#define SL_ERR_CMD_TIMEDOUT                             SL_ERR_MAIN + 0x09 //Command to driver timed out
#define SL_ERR_INVALID_CTRL                             SL_ERR_MAIN + 0x0A //Incorrect controller number
#define SL_ERR_NULL_DATA_PTR                            SL_ERR_MAIN + 0x0B //The data ptr was passed in as NULL
#define SL_ERR_INCORRECT_DATA_SIZE                      SL_ERR_MAIN + 0x0C //The data size did not match the minimum buffer size required by the command
#define SL_ERR_INVALID_CMD_TYPE                         SL_ERR_MAIN + 0x0D //The command type was invalid
#define SL_ERR_INVALID_CMD                              SL_ERR_MAIN + 0x0E //The command was invalid
#define SL_ERR_INVALID_FW_FLASH_IMAGE                   SL_ERR_MAIN + 0x0F //The firmware flash image is invalid
#define SL_ERR_INVALID_FLASH_IMAGE_SIGNATURE            SL_ERR_MAIN + 0x10 //The firmware flash image signature is invalid
#define SL_ERR_INVALID_FLASH_IMAGE_SIZE                 SL_ERR_MAIN + 0x11 //The firmware flash image size is invalid
#define SL_ERR_INVALID_FLASH_IMAGE_FW_SECTION           SL_ERR_MAIN + 0x12 //The firmware flash image firmware section is invalid
#define SL_ERR_INVALID_FLASH_IMAGE_BIOS_SECTION         SL_ERR_MAIN + 0x13 //The firmware flash image BIOS section is invalid
#define SL_ERR_INVALID_FLASH_IMAGE_BOOTBLOCK_SECTION    SL_ERR_MAIN + 0x14 //The firmware flash image Boot Block section is invalid
#define SL_ERR_MEMORY_ALLOC_FAILED                      SL_ERR_MAIN + 0x15 //Failed to allocate memory
#define SL_ERR_MEMORY_FREE_FAILED                       SL_ERR_MAIN + 0x16 //Failed to free memory
#define SL_ERR_CMD_FAILED_BY_OS                         SL_ERR_MAIN + 0x17 //Command failed by O/S
#define SL_ERR_INIT_MUTEX_FAILED                        SL_ERR_MAIN + 0x18 //Mutex creation failed
#define SL_ERR_INVALID_INPUT_PARAMETER                  SL_ERR_MAIN + 0x19 //The input parameter to the command was invalid
#define SL_ERR_INIT_NOT_COMPLETED                       SL_ERR_MAIN + 0x1A //Initialization was not yet completed.
#define SL_ERR_MAX_EVENTS_TO_RETRIEVE_EXCEEDED          SL_ERR_MAIN + 0x1B //The maximum number of events that can be retrieved has exceeded
#define SL_ERR_OS_MAP_INCOMPLETE                        SL_ERR_MAIN + 0x1C //The LD to OS drive mapping is not complete. This will happen when fw reports
#define SL_ERR_CREATE_THREAD_FAILED                     SL_ERR_MAIN + 0x1D //Thread creation failed
#define SL_ERR_LINUX_LIBSYSFS_NOT_PRESENT               SL_ERR_MAIN + 0x1E //libsysfs is not present in the system
#define SL_ERR_LINUX_SYSFS_NOT_MOUNTED                  SL_ERR_MAIN + 0x1F //sysfs is not mounted in the system
#define SL_ERR_LINUX_SCSI_DEV_TO_ADDR_MAP_FAILED        SL_ERR_MAIN + 0x20 //The SCSI device to scsi address mapping failed
#define SL_ERR_LINUX_SYSFS                              SL_ERR_MAIN + 0x21 //An error occurred while accessing sysfs
#define SL_ERR_LINUX_GET_SCSI_DEV_NAME_FAILED           SL_ERR_MAIN + 0x22 //Failed to get SCSI device name
#define SL_ERR_GET_PARTITION_FAILED                     SL_ERR_MAIN + 0x23 //Failed to get partition. Partition may not exist.
#define SL_ERR_LINUX_DEVICE_OPEN_FAILED                 SL_ERR_MAIN + 0x24 //Failed to open the device. Possible cause may be due to system delay in exposing device to kernel
#define SL_ERR_DESTROY_MUTEX_FAILED                     SL_ERR_MAIN + 0x25 //Failed to destroy mutex
#define SL_ERR_RELEASE_MUTEX_FAILED                     SL_ERR_MAIN + 0x26 //Failed to release mutex
#define SL_ERR_ACQUIRE_MUTEX_FAILED                     SL_ERR_MAIN + 0x27 //Failed to acquire mutex
#define SL_ERR_SIMULATION_LOAD_FAILED                   SL_ERR_MAIN + 0x28 //LoadLibrary function failed for simlib.dll
#define SL_ERR_SIMULATION_GET_PROC_ADDRESS_FAILED       SL_ERR_MAIN + 0x29 //GetProcAddress failed to get entry point function address
#define SL_ERR_WBEM_CONTEXT_SET_FAILED                  SL_ERR_MAIN + 0x2A //Setting the context fields for 64-bit architecture failed
#define SL_ERR_ACCESS_DENIED                            SL_ERR_MAIN + 0x2B //User Access level incorrect, administrator privilege required.
#define SL_ERR_LINUX_LIBPCI_NOT_PRESENT                 SL_ERR_MAIN + 0x2C //libpci is not present in the system
//Error codes specific to OOB environment
#define SL_ERR_OOB_NOT_READY                            SL_ERR_MAIN + 0x2D //FW is not ready to communicate over OOB, retry later
#define SL_ERR_OOB_WRONG_STATE                          SL_ERR_MAIN + 0x2E //FW is not in a state to process the CMD sent to it over OOB
#define SL_ERR_OOB_CANNOT_RESUME                        SL_ERR_MAIN + 0x2F //FW cannot resume the CMD over OOB since it is not active
#define SL_ERR_OOB_SUSPENDED_STATE                      SL_ERR_MAIN + 0x30 //All OOB functionalities are suspended.
#define SL_ERR_OOB_CRC_ERROR                            SL_ERR_MAIN + 0x31 //CRC is different from FW and calculated.

//Error code specific to OOB storelib I2c 
#define SL_ERR_OOB_I2C_LIBRARY_LOAD_FAILED              SL_ERR_MAIN + 0x32 //Loading of the storelib I2c library failed.
#define SL_ERR_OOB_I2C_LIBRARY_GET_PROC_ADDRESS_FAILED  SL_ERR_MAIN + 0x33 //GetProcAddress failed to get entry point function address for storelib i2c library.

#define SL_ERR_OOB_FAILED_TO_OPEN_DRIVER_HANDLE			SL_ERR_MAIN + 0x34 // Unable to open i2c driver handle
#define SL_ERR_OOB_FAILED_TO_CLOSE_DRIVER_HANDLE		SL_ERR_MAIN + 0x35 // Failed to close i2c driver handle

#define SL_ERR_UNSUPPORTED_CCVD_CMD                     SL_ERR_MAIN + 0x36 // Invalid command for CacheCade VD
#define SL_ERR_INVALID_DATA                             SL_ERR_MAIN + 0x37 // Validation failed  
#define SL_ERR_OOB_I2C_READ_FAILED                      SL_ERR_MAIN + 0x38 //I2C read failed
#define SL_ERR_OOB_I2C_WRITE_FAILED                     SL_ERR_MAIN + 0x39 //I2C write failed
#define SL_ERR_OOB_PACKET_SEQUENCE_MISMATCH             SL_ERR_MAIN + 0x40 //OOB packet sequence mismatch
#define SL_ERR_OOB_PCIE_LIBRARY_GET_PROC_ADDRESS_FAILED SL_ERR_MAIN + 0x41 //GetProcAddress failed to get entry point function address for storelib PCIE library.
#define SL_ERR_ENCL_CONFIG_DIAG_PAGE_READ_FAILED        SL_ERR_MAIN + 0x42 //Receive diagnostic config page failed 
#define SL_ERR_NOT_64BIT_DRIVER		                    SL_ERR_MAIN + 0x43 // Driver is not built for 64 bit applications

//Error Codes Specific to StorelibIR
#define SL_ERR_IR_INVALID_SPAN_DEPTH                    SL_ERR_IR_MAIN + 0x01 //Incorrect Span Depth,  should always be 1
#define SL_ERR_IR_CMD_FAILED                            SL_ERR_IR_MAIN + 0x02 //Command to MPT failed
#define SL_ERR_IR_MAX_PHYSDISK_EXCEEDED                 SL_ERR_IR_MAIN + 0x03 //IOC cannot support more Physical Disks
#define SL_ERR_INVALID_DIR                              SL_ERR_IR_MAIN + 0x04 //Invalid direction was passed in
#define SL_ERR_IR_RESYNC_PROGRESS                       SL_ERR_IR_MAIN + 0x05 //Resync operation in progress
#define SL_ERR_IR_NOT_USED_1                            SL_ERR_IR_MAIN + 0x06 //Not yet used
#define SL_ERR_IR_DEVICE_NOT_IN_ENCLOSURE               SL_ERR_IR_MAIN + 0x07 //device not within an enclosure
#define SL_ERR_IR_PD_NOT_IN_LD                          SL_ERR_IR_MAIN + 0x08 //pd is not part of a volume
#define SL_ERR_IR_INVALID_FW_FLASH_IMAGE_CHECK_SUM      SL_ERR_IR_MAIN + 0x09 //The firmware flash image checksum is invalid
#define SL_ERR_IR_INVALID_FW_FLASH_IMAGE_PRODUCT_ID     SL_ERR_IR_MAIN + 0x0A //The firmware flash image product Id is invalid
#define SL_ERR_IR_INVALID_FW_FLASH_IMAGE_NVDATA         SL_ERR_IR_MAIN + 0x0B //The firmware flash image attached NVDATA version is older than the NVDATA version of the image already existing on the controller
#define SL_ERR_IR_INVALID_BIOS_FLASH_IMAGE_SIGNATURE    SL_ERR_IR_MAIN + 0x0C //The BIOS flash image signature is invalid
#define SL_ERR_IR_INVALID_BIOS_FLASH_IMAGE_CHECK_SUM    SL_ERR_IR_MAIN + 0x0D //The BIOS flash image checksum is invalid
#define SL_ERR_IR_INVALID_BIOS_FLASH_IMAGE              SL_ERR_IR_MAIN + 0x0E //The BIOS flash image is invalid. Possible reasons may be due to invalid PCI Device Id, PCI type or PCIR offset
#define SL_ERR_FW_FLASH_IMAGE_INCOMPATIBLE_CHIP_AND_REVISION SL_ERR_IR_MAIN + 0x0F //Chip and revision is not matching.
#define SL_ERR_FW_FLASH_IMAGE_INCOMPATIBLE_FLASH_MEMORY SL_ERR_IR_MAIN + 0x10 //Flash memory size is not matching.
#define SL_ERR_IR_INVALID_CONFIG_PAGE_REQUESTED         SL_ERR_IR_MAIN + 0x11 // Invalid configuration page was requested and request failed by StorelibIR
#define SL_ERR_IR_DEVICE_ENCLOSURE_UNKNOWN_TYPE         SL_ERR_IR_MAIN + 0x12 //device is within an enclosure but enclosure management type is unknown, hence can not send request to enclosure.
#define SL_ERR_IR_INVALID_FW_FLASH_IMAGE_FW_VERSION     SL_ERR_IR_MAIN + 0x13 //The firmware version of the image being flashed is older than existing image on the controller
#define SL_ERR_IR_INVALID_FW_FLASH_IMAGE_SAME_VERSION   SL_ERR_IR_MAIN + 0x14 //The firmware version and the NVDATA version of the image being flashed is same as the image already existing on the controller
#define SL_ERR_DEVICE_NOT_FORMATABLE                    SL_ERR_IR_MAIN + 0x15 //Device specified cannot be formatted.
#define SL_ERR_VOLUME_NAME_STRING_NOT_VALID             SL_ERR_IR_MAIN + 0x16 // The volume name string sent by app is not valid
#define SL_ERR_BLOCK_SIZE_MISMATCH						SL_ERR_IR_MAIN + 0x17 // Devices blocksize mismatch while creating volume.
#define SL_ERR_PORT_NUMBER_MISMATCH                     SL_ERR_IR_MAIN + 0x18 // Device path does not belong to this controller.
#define SL_ERR_OPERATION_NOT_SUPPORTED                  SL_ERR_IR_MAIN + 0x19
#define SL_ERR_DIAG_BUFFER_INVALID_BUFFER_TYPE          SL_ERR_IR_MAIN + 0x20
#define SL_ERR_DIAG_BUFFER_INVALID_PARAM                SL_ERR_IR_MAIN + 0x21
#define SL_ERR_FILE_OPERATION_FAILED                    SL_ERR_IR_MAIN + 0x22
#define SL_ERR_DIAG_INVALID_FUNCTION                    SL_ERR_IR_MAIN + 0x23
#define SL_ERR_DIAG_INVALID_PARAMS_REC_LEN              SL_ERR_IR_MAIN + 0x24
#define SL_ERR_DIAG_INVALID_TRIGGER                     SL_ERR_IR_MAIN + 0x25
#define SL_ERR_DIAG_INVALID_NUM_OF_TRIGGER_ENTRIES      SL_ERR_IR_MAIN + 0x26
#define SL_ERR_DIAG_INVALID_SIGN                        SL_ERR_IR_MAIN + 0x27
#define SL_ERR_FLASH_IMAGE_NOT_PRESENT                  SL_ERR_IR_MAIN + 0x28 // Requested Flash image type is not present in flash
#define SL_ERR_EFI_LOCATE_HANDLE                        SL_ERR_IR_MAIN + 0x29
#define SL_ERR_EFI_OPEN_PROTOCOL                        SL_ERR_IR_MAIN + 0x30
#define SL_ERR_EFI_PCI_READ                             SL_ERR_IR_MAIN + 0x31
#define SL_ERR_EFI_GET_PCIO_ATTRIBUTES                  SL_ERR_IR_MAIN + 0x32
#define SL_ERR_EFI_ENABLE_PCIO_ATTRIBUTES               SL_ERR_IR_MAIN + 0x33
#define SL_ERR_EFI_GET_LOCATION                         SL_ERR_IR_MAIN + 0x34
#define SL_ERR_EFI_ADAPTER_IN_INVALID_STATE             SL_ERR_IR_MAIN + 0x35
#define SL_ERR_EFI_MEM_READ                             SL_ERR_IR_MAIN + 0x36
#define SL_ERR_EFI_MEM_WRITE                            SL_ERR_IR_MAIN + 0x37
#define SL_ERR_EFI_MEM_ALLOC_INIT                       SL_ERR_IR_MAIN + 0x38
#define SL_ERR_EFI_ADAPTER_INIT                         SL_ERR_IR_MAIN + 0x39
#define SL_ERR_EFI_TIME_OUT                             SL_ERR_IR_MAIN + 0x40
#define SL_ERR_EFI_PORT_ENABLE_REQUEST_FAILED           SL_ERR_IR_MAIN + 0x41
#define SL_ERR_EFI_REPLY_FRAME_ADDR_MISMATCH            SL_ERR_IR_MAIN + 0x42
#define SL_ERR_EFI_REPLY_DESCRIPTOR_SMID_MISMATCH       SL_ERR_IR_MAIN + 0x43
#define SL_ERR_EFI_REPLY_DESCRIPTOR_INVALID             SL_ERR_IR_MAIN + 0x44
#define SL_ERR_EFI_INVALID_CMD                          SL_ERR_IR_MAIN + 0x45
#define SL_ERR_OPERATION_UNSUPPORTED                    SL_ERR_IR_MAIN + 0x46
#define SL_ERR_CTRL_IN_FAULT_OR_RESET_STATE             SL_ERR_IR_MAIN + 0x47
#define SL_ERR_INVALID_ERASE_FLAG                       SL_ERR_IR_MAIN + 0x48  
#define SL_ERR_EFI_IR_FIRMWARE_FLASHING_UNSUPPORTED     SL_ERR_IR_MAIN + 0x49  
#define SL_ERR_INVALID_FIRMWARE_INITIALIZATION_IMAGE    SL_ERR_IR_MAIN + 0x50  
#define SL_ERR_INVALID_FIRMWARE_BOOTLOADER_IMAGE        SL_ERR_IR_MAIN + 0x51
#define SL_ERR_EFI_FILE_TOO_LARGE                       SL_ERR_IR_MAIN + 0x52  
#define SL_ERR_EFI_DIAG_WRITE_ENABLE_NOT_SET            SL_ERR_IR_MAIN + 0x53
#define SL_ERR_EFI_SENDING_DOORBELL_FAILED              SL_ERR_IR_MAIN + 0x54   //SL_ERR_EFI_SENDING_DOORBELL_FAILED + message function code
#define SL_ERR_IR_INVALID_FW_FLASH_IMAGE_SIGNATURE      SL_ERR_IR_MAIN + 0x55
#define SL_ERR_EFI_DOORBELL_FAILED                      SL_ERR_IR_MAIN + 0x56
#define SL_ERR_CTRL_RESET_FAILED                        SL_ERR_IR_MAIN + 0x57
#define SL_ERR_EXT_IMAGE_NOT_FOUND                      SL_ERR_IR_MAIN + 0x58
#define SL_ERR_PORT_ENABLE_NOT_SET                      SL_ERR_IR_MAIN + 0x59

//#define SL_ERR_EFI_DISCOVERY_FAILURE                    SL_ERR_IR_MAIN + 0x32

// Error Codes specific to Warhawk
#define SL_ERR_WH_FLASH_PACKAGE_INVALID_PACKAGE_SIGNATURE           SL_ERR_WH_FW_MAIN + 0x0A //The WH Package signature is invalid
#define SL_ERR_WH_FLASH_PACKAGE_INVALID_PACKAGE_CHECK_SUM           SL_ERR_WH_FW_MAIN + 0x0B //The WH Package Checksum is invalid
#define SL_ERR_WH_FLASH_PACKAGE_INCOMPATIBLE_PACKAGE_TYPE           SL_ERR_WH_FW_MAIN + 0x0C //The WH Package signature is incompatible
#define SL_ERR_WH_FLASH_PACKAGE_INVALID_PACKAGE_CONTENTS            SL_ERR_WH_FW_MAIN + 0x0D //The WH Package contents are invalid
#define SL_ERR_WH_FLASH_PACKAGE_INVALID_PACKAGE_SIZE                SL_ERR_WH_FW_MAIN + 0x0E //The WH Package size is invalid
#define SL_ERR_WH_FLASH_PACKAGE_INCOMPATIBLE_DEVICE_ID              SL_ERR_WH_FW_MAIN + 0x0F //The WH Package is incompatible with WH board device ID
#define SL_ERR_WH_FLASH_PACKAGE_INVALID_IMAGE                       SL_ERR_WH_FW_MAIN + 0x10 //One or more physical drives did not allow firmware download
#define SL_ERR_WH_FLASH_PACKAGE_DOWNGRADE_NOT_ALLOWED               SL_ERR_WH_FW_MAIN + 0x11 //The WH Package Downgrade not allowed
#define SL_ERR_WH_FLASH_PD_FW_DOWNLOAD_NOT_ALLOWED                  SL_ERR_WH_FW_MAIN + 0x12 //One or more physical drives did not allow firmware download
#define SL_ERR_WH_FLASH_PACKAGE_INCOMPATIBLE_PACKAGE_HEADER_VER     SL_ERR_WH_FW_MAIN + 0x13 //The WH Package is not compatible with current release. StorelibIR23 could not understand the package contents.
#define SL_ERR_WH_FLASH_PACKAGE_INCOMPATIBLE_PACKAGE_TYPE_SUB_ID    SL_ERR_WH_FW_MAIN + 0x14 //WH Package Type is not compatible with board's sub system ID
#define SL_ERR_WH_FLASH_PACKAGE_INCOMPATIBLE_PACKAGE_NAME           SL_ERR_WH_FW_MAIN + 0x15 //WH Package Name is not compatible with Package Name of the existing package on board

// Error Codes specific to Diagnostic Buffer for WarHawk 
#define SL_ERR_WH_DIAG_BUFFER_INVALID_PARAM                         SL_ERR_WH_FW_MAIN + 0x16 //Buffer size sent by the application is invalid. 
#define SL_ERR_WH_DIAG_BUFFER_INVALID_BUFFER_TYPE                   SL_ERR_WH_FW_MAIN + 0x17 //Buffer type sent by the application is invalid.

#define SL_ERR_WH_FILE_OPERATION_FAILED                             SL_ERR_WH_FW_MAIN + 0x18 //File open or file write or file close failed
#define SL_ERR_WH_OPERATION_NOT_SUPPORTED                           SL_ERR_WH_FW_MAIN + 0x19 //Requested operation not supported on DFF1 4k
#define SL_ERR_WH_PD_LIST_NOT_FOUND                                 SL_ERR_WH_FW_MAIN + 0x1A //Ctrl Info does not contain PD Info data
#define SL_ERR_WH_PD_NOT_FOUND                                      SL_ERR_WH_FW_MAIN + 0x1B //Requested PD not found in PD Info data
#define SL_ERR_WH_PANIC_SLOT_INVALID                                SL_ERR_WH_FW_MAIN + 0x1C //Requested Panic Slot value invalid/out of range
#define SL_ERR_WH_SMART_STATUS_FAILURE                              SL_ERR_WH_FW_MAIN + 0x1D //Read SMART status command reported failure
#define SL_ERR_WH_MIXED_PROTOCOL_DEVICES                            SL_ERR_WH_FW_MAIN + 0x1E //Mixed Protocol Devices are found

#define SL_ERR_WH_FLASH_PACKAGE_SUPPORTED_DEVICES_EXT_IMAGE         SL_ERR_WH_FW_MAIN + 0x1F //Failed to find Supported Devices Extended Image.
#define SL_ERR_WH_FLASH_PACKAGE_INVALID_INITALIZATION_IMAGE         SL_ERR_WH_FW_MAIN + 0x20 //Firmware Image does not have a valid Initialization image.
#define SL_ERR_WH_FLASH_PACKAGE_INVALID_NVDATA_SIGNATURE            SL_ERR_WH_FW_MAIN + 0x21 //Invalid NVDATA Signature in Firmware image from File.
#define SL_ERR_WH_FLASH_PACKAGE_INVALID_NVDATA_PROD_ID              SL_ERR_WH_FW_MAIN + 0x22 //Invalid NVDATA PROD_ID Signature in Firmware image from File
#define SL_ERR_WH_FLASH_PACKAGE_INCOMPATIBLE_NVDATA_DEV_ID          SL_ERR_WH_FW_MAIN + 0x23 //NVDATA Image does not match Controller Device ID.
#define SL_ERR_WH_FLASH_PACKAGE_INCOMPATIBLE_NVDATA_SUB_ID          SL_ERR_WH_FW_MAIN + 0x24 //NVDATA Image does not match Controller Sub System ID.


//StorelibIR mapped MegaRAID error codes (See mfistat.h for error desc)
#define SL_ERR_IR_LD_WRONG_RAID_LEVEL                   SL_ERR_IR_MEGARAID_MAIN + 0x1e
#define SL_ERR_IR_DEVICE_NOT_FOUND                      SL_ERR_IR_MEGARAID_MAIN + 0x0c
#define SL_ERR_IR_WRONG_STATE                           SL_ERR_IR_MEGARAID_MAIN + 0x32
#define SL_ERR_IR_LD_MAX_CONFIGURED                     SL_ERR_IR_MEGARAID_MAIN + 0x1a

//StorelibIR CCoH error codes
#define SL_ERR_CCOH_NOT_INITIALIZED                    SL_ERR_CCOH_MAIN + 0x01 // CCoH not initialized/installed on the system
#define SL_ERR_NO_VALID_CFG                            SL_ERR_CCOH_MAIN + 0x02 // No valid CCoH config exists on the system
#define SL_ERR_NO_CACHE_DEV                            SL_ERR_CCOH_MAIN + 0x03 // No cache device has been configured or found
#define SL_ERR_ADD_VD_FAILED                           SL_ERR_CCOH_MAIN + 0x04 // Failed to add VD to cache group. Check if the device is already active
#define SL_ERR_REMOVE_VD_FAILED                        SL_ERR_CCOH_MAIN + 0x05 // Failed to remove VD to cache group. Check if the device is already active
#define SL_ERR_CCOH_INIT_FAILED                        SL_ERR_CCOH_MAIN + 0x06 // Failed to add CCOH as controller type, CCoH drivers may not be loaded.
#define SL_ERR_INVALID_OPERATION                       SL_ERR_CCOH_MAIN + 0x07 // Invalid Operation
#define SL_ERR_DEV_EXIST                               SL_ERR_CCOH_MAIN + 0x08 // Device already exists.
#define SL_ERR_MAX_LIMIT                               SL_ERR_CCOH_MAIN + 0x09 // Maximum limit (cache device / virtual drive / cache capacity) for reached.
#define SL_ERR_CCOH_LICENSE_NOT_FOUND                  SL_ERR_CCOH_MAIN + 0x0a // CCOH license not found.
#define SL_ERR_CCOH_DEV_BUSY                           SL_ERR_CCOH_MAIN + 0x0b // Device is in use and hence remove operation failed.
#define SL_ERR_CCOH_UNIQUEID_NOT_SUPPORTED             SL_ERR_CCOH_MAIN + 0x0c // Host not having unique id (BIOS UUID).
#define SL_ERR_CCOH_FW_VERSION_NOT_SUPPORTED           SL_ERR_CCOH_MAIN + 0x0d // FW version on cache device not supported.
#define SL_ERR_DEV_NOT_EXIST                           SL_ERR_CCOH_MAIN + 0x0e // Device does not exist in configuration.
#define SL_ERR_VDS_CONFIGURED                          SL_ERR_CCOH_MAIN + 0x0f // VDs existing in configuration.

//StorelibIR Nytro error codes
#define SL_ERR_NYTRO_CMD_NOT_SUPPORTED                 SL_ERR_NYTRO_MAIN + 0x01 // Invalid command for the controller
#define SL_ERR_NYTRO_DEVICE_NOT_SAS_OR_SATA            SL_ERR_NYTRO_MAIN+ 0x02 // Not a SATA or SAS Device
#define SL_ERR_NYTRO_PD_LIST_NOT_FOUND                 SL_ERR_NYTRO_MAIN+ 0x03//Ctrl Info does not contain PD Info data
#define SL_ERR_NYTRO_PANIC_SLOT_INVALID                SL_ERR_NYTRO_MAIN + 0x04 //Requested Panic Slot value invalid/out of range
#define SL_ERR_NYTRO_PD_NOT_FOUND                      SL_ERR_NYTRO_MAIN+ 0x05 //Requested PD not found in PD Info data
#define SL_ERR_NYTRO_OPERATION_NOT_SUPPORTED           SL_ERR_NYTRO_MAIN+ 0x06 //Requested operation not supported on DFF1 4k
#define SL_ERR_NYTRO_API_FAILED                        SL_ERR_NYTRO_MAIN+0x07 //Api in Nytro lib failed
#define SL_ERR_NYTRO_POWER_NOT_IN_RANGE                SL_ERR_NYTRO_MAIN+0x08 //Power Value not in Range
#define SL_ERR_NYTRO_CTRL_NOT_SUPPORTED                SL_ERR_NYTRO_MAIN + 0x09 // Not a Nytro controller or Device
#define SL_ERR_NYTRO_DEVICE_NOT_SUPPORTED              SL_ERR_NYTRO_MAIN + 0xa // device not supported
//MCTP Related Errors

/* Derived from the driver codes */
#define SL_ERR_MCTP_CC_INVALID_PARAMS                               (SL_ERR_MAIN_MCTP+0x01)
#define SL_ERR_MCTP_CC_INVALID_EID                                  (SL_ERR_MAIN_MCTP+0x02)
#define SL_ERR_MCTP_CC_LENGTH_EXCEEDED                              (SL_ERR_MAIN_MCTP+0x03)
#define SL_ERR_MCTP_CC_TIMEOUT                                      (SL_ERR_MAIN_MCTP+0x04)
#define SL_ERR_MCTP_CC_EID_BUSY                                     (SL_ERR_MAIN_MCTP+0x05)
#define SL_ERR_MCTP_CC_SEND_ERROR                                   (SL_ERR_MAIN_MCTP+0x06)
#define SL_ERR_MCTP_CC_RECV_OVERRUN                                 (SL_ERR_MAIN_MCTP+0x07)
#define SL_ERR_MCTP_CC_DRVR_FAULT                                   (SL_ERR_MAIN_MCTP+0x08)
#define SL_ERR_MCTP_CC_SYS_NOT_READY                                (SL_ERR_MAIN_MCTP+0x09)
#define SL_ERR_MCTP_CC_MAX                                          (SL_ERR_MAIN_MCTP+0x0A)
/* Developed to handle MCTP */
#define SL_ERR_MCTP_DRVR_INVALID_EID                                (SL_ERR_MAIN_MCTP+0x80)
#define SL_ERR_MCTP_DRVR_UNEXPECTED_FAILURE                         (SL_ERR_MAIN_MCTP+0x81)
#define SL_ERR_MCTP_DRVR_FAILED_TO_UNREGISTER                       (SL_ERR_MAIN_MCTP+0x82)
#define SL_ERR_MCTP_DRVR_EXCESS_REQUEST_SIZE                        (SL_ERR_MAIN_MCTP+0x83)

#define SL_ERR_MCTP_FAILED_MEMORY_ALLOC                             (SL_ERR_MAIN_MCTP+0x90)
#define SL_ERR_MCTP_FAILED_TO_INIT                                  (SL_ERR_MAIN_MCTP+0x91)
#define SL_ERR_MCTP_CMD_QUEUE_FULL                                  (SL_ERR_MAIN_MCTP+0x92)
#define SL_ERR_MCTP_REQ_RSP_BUFFERS_IN_USE                          (SL_ERR_MAIN_MCTP+0x93)
#define SL_ERR_MCTP_CTRL_IN_USE                                     (SL_ERR_MAIN_MCTP+0x94)
#define SL_ERR_MCTP_REQ_RSP_EXCEEDS_BUFFER_SIZE                     (SL_ERR_MAIN_MCTP+0x95)
#define SL_ERR_MCTP_REQUIRES_INIT_CMD                               (SL_ERR_MAIN_MCTP+0x96)
#define SL_ERR_MCTP_EVENT_BUF_OVF                                   (SL_ERR_MAIN_MCTP+0x97)
#define SL_ERR_MCTP_INVALID_PARAM                                   (SL_ERR_MAIN_MCTP+0x98)


/* Due to a packet field. */
#define SL_ERR_MCTP_PE_OFFSET                                       (SL_ERR_MAIN_MCTP+0x100)
#define SL_ERR_MCTP_RXED_PE_GENERAL                                 (SL_ERR_MAIN_MCTP+0x100)
#define SL_ERR_MCTP_RXED_PE_OOO                                     (SL_ERR_MAIN_MCTP+0x101)
#define SL_ERR_MCTP_RXED_PE_PEC_FAIL                                (SL_ERR_MAIN_MCTP+0x102)
#define SL_ERR_MCTP_RXED_PE_INVALID_PARAM                           (SL_ERR_MAIN_MCTP+0x103)
#define SL_ERR_MCTP_RXED_PE_BUSY_RETRY                              (SL_ERR_MAIN_MCTP+0x104)
#define SL_ERR_MCTP_RXED_PE_UNAVAILABLE                             (SL_ERR_MAIN_MCTP+0x105)
#define SL_ERR_MCTP_RXED_PE_AMT_IN_USE                              (SL_ERR_MAIN_MCTP+0x106)
#define SL_ERR_MCTP_RXED_PE_FW_IN_WRONG_STATE                       (SL_ERR_MAIN_MCTP+0x107)
#define SL_ERR_MCTP_UNEXPECTED_PE                                   (SL_ERR_MAIN_MCTP+0x10F)
#define SL_ERR_MCTP_CMDSTATUS_OFFSET                                (SL_ERR_MAIN_MCTP+0x11F)
#define SL_ERR_MCTP_CMDSTATUS_INVALID_CMD                           (SL_ERR_MAIN_MCTP+0x111)
#define SL_ERR_MCTP_CMDSTATUS_INVALID_PARAMETER                     (SL_ERR_MAIN_MCTP+0x112)
#define SL_ERR_MCTP_CMDSTATUS_BUSY                                  (SL_ERR_MAIN_MCTP+0x113)
#define SL_ERR_MCTP_CMDSTATUS_NOT_READY                             (SL_ERR_MAIN_MCTP+0x114)
/* So far handled by *_REQUIRES_INIT_CMD.  May need later. */
#define SL_ERR_MCTP_CMDSTATUS_WRONG_STATE                           (SL_ERR_MAIN_MCTP+0x115)
#define SL_ERR_MCTP_CMDSTATUS_CANNOT_RESUME                         (SL_ERR_MAIN_MCTP+0x116)
/* Neither of the below two are used right now.  Helps that the controller
 * reports the amount of data that was transmitted back and it is reflected
 * in the total response size. */
#define SL_ERR_MCTP_CMDSTATUS_UNDERRUN                              (SL_ERR_MAIN_MCTP+0x117)
#define SL_ERR_MCTP_CMDSTATUS_OVERRUN                               (SL_ERR_MAIN_MCTP+0x118)
#define SL_ERR_MCTP_UNEXPECTED_CMDSTATUS                            (SL_ERR_MAIN_MCTP+0x119)
#define SL_ERR_MCTP_WRONG_PKTSEQCOUNT                               (SL_ERR_MAIN_MCTP+0x11A)
#define SL_ERR_MCTP_MSG_SIZE_TOO_SMALL                              (SL_ERR_MAIN_MCTP+0x11B)
#define SL_ERR_MCTP_FAILURE                                         (SL_ERR_MAIN_MCTP+0x1FF)
#define SL_ERR_MCTP_CMDSTATUS_INFO_CHANGED                          (SL_ERR_MAIN_MCTP+0x200)
#define SL_ERR_MCTP_CMDSTATUS_UNKNOWN                               (SL_ERR_MAIN_MCTP+0x201)



//SLIR3 (BMC-I2C) error codes
#define SL_ERR_I2C_NOT_READY					        SL_ERR_MAIN_I2C	 + 0x01 //FW is not ready to communicate over I2C, retry later
#define SL_ERR_I2C_WRONG_STATE					        SL_ERR_MAIN_I2C	 + 0x02 //FW is not in a state to process the CMD sent to it over I2C
#define SL_ERR_I2C_CANNOT_RESUME					    SL_ERR_MAIN_I2C	 + 0x03 //FW cannot resume the CMD over I2C since it is not active
#define SL_ERR_I2C_SUSPENDED_STATE						SL_ERR_MAIN_I2C	 + 0x04 //All I2C functionalities are suspended.
#define SL_ERR_I2C_CRC_ERROR                            SL_ERR_MAIN_I2C	 + 0x05 //CRC is different from FW and calculated.
#define SL_ERR_I2C_INVALID_CMD                          SL_ERR_MAIN_I2C	 + 0x06 //Invalid command recieved from firmware
#define SL_ERR_I2C_INFO_CHANGED                         SL_ERR_MAIN_I2C	 + 0x07 //Information changed info received from firmware
#define SL_ERR_I2C_INVALID_PAYLOAD_ID                   SL_ERR_MAIN_I2C	 + 0x08 //Payload ID recieved from firmware which does not match with any of the Payload ID supported.
#define SL_ERR_I2C_PE_GENERAL							SL_ERR_MAIN_I2C	 + 0x09 //Packet Exception: General
#define SL_ERR_I2C_PE_OOO								SL_ERR_MAIN_I2C	 + 0x0A //Packet Exception: Out of Order Packet
#define SL_ERR_I2C_PE_PEC_FAIL							SL_ERR_MAIN_I2C	 + 0x0B //Packet Exception: PEC Failure
#define SL_ERR_I2C_PE_INVALID_PARAM						SL_ERR_MAIN_I2C	 + 0x0C //Packet Exception: Header with Invalid Parameter
#define SL_ERR_I2C_PE_BUSY_RETRY						SL_ERR_MAIN_I2C	 + 0x0D //Packet Exception: Busy Retry
#define SL_ERR_I2C_PE_AMT_INUSE							SL_ERR_MAIN_I2C	 + 0x0E //Packet Exception: Application Message Tag already in use
#define SL_ERR_I2C_PE_FW_IN_WRONG_STATE					SL_ERR_MAIN_I2C	 + 0x0F //Packet Exception: Firmware In Wrong State to process the packet
#define SL_ERR_I2C_ABORT_AND_RESEND_CMD					SL_ERR_MAIN_I2C	 + 0x10 //Resend/Retry the command
#define SL_ERR_I2C_I2C_IOCTL_FAILED						SL_ERR_MAIN_I2C	 + 0x11 //I2C ioctl failed
#define SL_ERR_I2C_CMD_STATUS_BUSY						SL_ERR_MAIN_I2C	 + 0x12 //Service is unable to honor the request at this time
#define SL_ERR_I2C_CMD_STATUS_NOT_READY					SL_ERR_MAIN_I2C	 + 0x13 //Service is not ready
#define SL_ERR_I2C_CMD_STATUS_WRONG_STATE				SL_ERR_MAIN_I2C	 + 0x14 //Service cannot process this command at this time
#define SL_ERR_I2C_CMD_STATUS_INVALID_CMD				SL_ERR_MAIN_I2C	 + 0x15 //Invalid control command
#define SL_ERR_I2C_CMD_STATUS_CANNOT_RESUME				SL_ERR_MAIN_I2C	 + 0x16 //Service cannot resume; currently not in use
#define SL_ERR_I2C_CMD_STATUS_INFO_CHANGED				SL_ERR_MAIN_I2C	 + 0x17 //Indicates a configuration or device change.This bit is a flag and may appear in any of the preceding statuses.
#define SL_ERR_I2C_CMD_STATUS_INVALID_PARAMETER			SL_ERR_MAIN_I2C	 + 0x18 //Invalid control command parameter
#define SL_ERR_I2C_INVALID_CMD_STATUS                   SL_ERR_MAIN_I2C	 + 0x19 //CmdStatus recieved from firmware which does not match with any of the CmdStatus supported.
#define SL_ERR_I2C_MCTP_PAYLOADID_SERVICE_ERR			SL_ERR_MAIN_I2C	 + 0x1A //The MCTP service could not be properly handled, retry 


#endif // __SLERRORS_H__

