/** @file pstorc.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore main header file
   Include this file to access the public data structures and function of the storageCore API

*/

#ifndef PSTORC_H
#define PSTORC_H

#if defined(__cplusplus)
extern "C" {
#endif
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/** Here to make the compiler happy. */
typedef void *PCDA;
#endif
/** Data object with information on mirror & parity groups of a logical drive
 * Used in the [Mirror/Parity Group API](@ref storc_properties_ld_raid_groups)
 * @ingroup storc_properties_ld_raid_groups
 */
typedef void *PRAID_GROUP_INFO;
#if defined(__cplusplus)
}
#endif

/*****************************//**
 * @defgroup storcore storageCore
 * @brief
 * @details
 ********************************/

/* Include other files */
#include "psysc_types.h"
#include "pstorc_version.h"
#include "pstorc_types.h"
#include "pstorc_utils.h"
#include "pstorc_cda.h"
#include "pstorc_features.h"
#include "pstorc_status.h"
#include "pstorc_properties.h"
#include "pstorc_configuration.h"
#include "pstorc_commands.h"
#include "pstorc_flash.h"
#include "pstorc_erase.h"
#include "pstorc_events.h"
#include "pstorc_encryption.h"

/*****************************//**
 * @addtogroup storcore storageCore
 * @{
 ********************************/

#if defined(__cplusplus)
extern "C" {
#endif

/** Initialize global objects of storageCore.
 * @attention Clients should invoke this before using any of the storageCore module API.
 * @deprecated Parameter `checksum` is not longer used.
 */
void InitializeStorageCore(const char * checksum);

/** Uninitialize global objects of StorageCore.
 * @attention Clients can no longer invoke any of the storageCore module API after this call.
 */
void UnInitializeStorageCore(void);

#if defined(__cplusplus)
}
#endif

/** @} */ /* storcore */

/** @mainpage
 * @tableofcontents
 * @vendor_copyright
 * @ref changelog
 *
 * @section storpage_overview Overview
 *
 * @ref storcore is an Application Programming Interface (API) used to interface with Microchip
 * Smart Array, SmartRAID, and SmartHBA controllers. The functional goals for storageCore are:
 *
 * * Provide a complete interface to inventory, configure, and monitor PMC Smart Array, Smart RAID and Smart HBA controllers.
 * * Become the business logic engine for current and new configuration utilities.
 * * Run in environments where a C++ compiler or runtime is not available such as BMC, UEFI.
 * * Provide customers a programming interface to integrate configuration and monitoring capabilities into their existing infrastructure.
 *
 * @section storpage_basic_arch Basic Architecture
 *
 * @ref storcore is composed of modules that implement distinct functionality:
 *
 * -# sysCore: This is a low-level hardware discovery and command handling API.  It works as an abstraction layer to hide the underlying communication mechanism to the hardware.
 * -# storageCore: This is the main inventory and configuration API.
 * -# diagnosticCore: An API that extends storageCore with diagnostic report generation tools.
 *
 * @subsection storpage_module_syscore sysCore
 * The @ref syscore API is a hardware abstraction layer that interfaces between the storageCore API and the
 * underlying driver/operating system. @ref syscore serves 3 purposes:
 *
 * -# Discover the controllers in the system.
 * -# Transport SCSI commands from the @ref storcore API to the controller.
 * -# Provide any platform-specific functionality or requirements.
 *
 * @subsubsection storpage_module_syscore_dev_desc Device Descriptor
 * A device descriptor is an object that represents a controller to the underlying operating system.
 * It is used by the sysCore functions to uniquely identify and to route commands to the array controller.
 *
 * @subsubsection storpage_module_syscore_discovery Adapter Discovery Functions
 * The adapter discovery functions provide the means to:
 *
 * * create a list of device descriptors
 * * iterate over the device descriptor list.
 *
 * @subsubsection storpage_module_syscore_commands Command Routing and Execution Functions
 * These functions are used by the storageCore API functions to send management commands to the controller.
 * The basic commands supported are BMIC (proprietary Array controller configuration) commands,
 * SCSI commands, and other pass-through commands for underlying storage devices.
 *
 * @subsection storpage_module_storagecore storageCore
 * @ref storcore is larger set of API functions for:
 * * [Reading device status](@ref storcpage_module_storagecore_status)
 * * [Getting device properties](@ref storcpage_module_storagecore_props)
 * * [Configuring your storage](@ref storcpage_module_storagecore_config)
 * * [Updating physical device firmware](@ref storcpage_module_storagecore_update)
 * * Erasing/Sanitizing physical drives ([API functions](@ref storc_erase))
 * * Listening for events ([API functions](@ref storc_events))
 *
 * @subsubsection storpage_module_storagecore_cda Controller Data Area (CDA)
 * This structure stores a copy of all the data buffers retrieved from the controller and some cached
 * information such as controller status, etc. Configuration commands are routed through the CDA.
 * A pointer to this structure is the first parameter of all storageCore functions.
 *
 * @subsubsection storcpage_module_storagecore_cda_refresh CDA Initialization and Refresh Functions
 * These functions are used to initialize, populate, and refresh the controller data area structure.
 * A Device Descriptor obtained through the sysCore API is required in order to initialize a CDA.
 *
 * @subsubsection storcpage_module_storagecore_cda_features Controller Features Functions
 * API functions; @ref storc_features
 * These functions provide information about the features supported by the controller.
 * These functions are designed to return simple true/false or numeric values to report on each requested feature.
 *
 * @subsubsection storcpage_module_storagecore_props Device Property Functions
 * API functions: @ref storc_properties
 *
 * These functions provide information about the devices currently attached or configured on the controller.
 * Enumeration functions can be used to list items, one by one.
 *
 * @subsubsection storcpage_module_storagecore_status Device Status Functions
 * API functions: @ref storc_status
 *
 * These functions are used to obtain the status of each device attached to the controller or from the controller itself.
 *
 * @subsubsection storcpage_module_storagecore_config Configuration Functions
 * API functions @ref storc_configuration (See @ref storc_config_examples)
   *
 * These functions are used to modify the controller configuration.  Operations such as logical drive
 * creation, array expansion, physical drive erase, are part of this category. Configuration functions
 * typically come in pairs: There is a Can* function to determine whether a specific action can be taken,
 * and a Do/Change/Set* function that actually performs the action.
 *
 * There are 2 types of configuration changes:
 *
 * 1) Changes that are applied immediately:
 * 2) Changes that can be "staged":
 *
 * In order to perform "staged" actions, a 3 step process is required:
 *
 * 1. Create an editable configuration
 * 2. Create/delete/modify arrays, logical drives, spare drives.
 * 3. Commit the configuration all at once.
 *
 * Some configuration operations are performed immediately (outside the editable configuration).
 * They are still often paired with a Can* function to determine whether the operation can be taken.
 *
 * @subsubsection storcpage_module_storagecore_update Firmware Update Functions
 * API functions: @ref storc_flash
 *
 * These functions are used to upgrade the firmware of the array controller, or other physical devices
 * attached to the controller. The functions assume that a valid firmware image has been read from a file
 * and stored in a memory buffer.
 *
 * @subsection storpage_module_diagcore diagnosticCore
 * @ref diagcore provides a way to setup user callbacks to generate diagnostic reports.
 *
 * @ref storcore and @ref syscore are required when using diagnosticCore.
 *
 * @section storpage_getting_started Getting Started
 *
 * @subsection storpage_getting_started_discovery Initialization & Discovery
 *
 * Include needed headers.
 * @code
 * #include "psysc.h"
 * #include "pstorc.h" // <- Only if you need storageCore and/or diagCore
 * #include "pdiagc.h" // <- Only if you need diagnosticCore
 * @endcode
 *
 * Initialize static data (only initialize the modules linked with your program)
 * @code
 * InitializeSysCore(SC_CHECKSUM);
 * InitializeStorageCore(SC_CHECKSUM); // <- Only if you're using storageCore and/or diagnosticCore
 * InitializeDiagCore(SC_CHECKSUM);    // <- Only if you're using diagnosticCore
 * @endcode
 *
 * Collect a list of device handlers using @ref syscore_device_handler from @ref syscore.
 * @code
 * DeviceDescriptor test_handles[10]; // Needs as many elements as there are storage controllers.
 * int num_handles = 0;
 * int handle_index = 0;
 *
 * DeviceDescriptorList device_list = SC_CreateControllerDescriptorList();
 * DeviceDescriptorListIterator list_iterator = SC_CreateDeviceListIterator(device_list);
 *
 * while (!SC_IsDeviceListIteratorEnd(list_iterator))
 * {
 *     test_handles[num_handles++] = SC_GetDeviceListItemAt(list_iterator);
 *     SC_MoveDeviceListIteratorNext(list_iterator);
 * }
 * SC_DestroyDeviceListIterator(list_iterator);
 * @endcode
 *
 * Use the list of device handles to initialize a list of storage controller data areas ([CDA](@ref storc_cda)).
 * The @ref storc_cda is needed to use the @ref storcore API.
 * @code
 * PCDA test_CDAs[10]; // Needs as many elements as there are storage controllers.
 * int num_good_handles = 0;
 * for (handle_index = 0; handle_index < num_handles; handle_index++)
 * {
 *     PCDA test_CDA = (PCDA)SA_malloc(SA_GetCDASize());
 *     if (SA_InitializeCDA(test_CDA, test_handles[handle_index]) == kInitializeSucceeded)
 *     {
 *         test_CDAs[num_good_handles] = test_CDA;
 *         num_good_handles++;
 *     }
 *     else
 *     {
 *         SA_free(test_CDA);
 *     }
 * }
 * @endcode
 *
 * @subsection storpage_getting_started_storc What's Next?
 *
 * To refresh/resync a CDA so that is reflects the latest 'snapshot' of a controller's status & configuration
 * use @ref SA_ReInitializeCDA. This is usually done automatically by several API functions that make
 * changes to a controller's configuration but can be performed at any time to update a CDA with changes
 * outside the API such as hotplug/removal of physical devices, changes to device status/sensors, etc...
 *
 * Given one of the CDA elements of the controller list you can perform the following using @ref storcore
 *   - From [Full List of Functions](@ref storcore)
 *     - @ref storc_properties
 *     - @ref storc_features
 *     - @ref storc_status
 *     - @ref storc_configuration (See @ref storc_config_examples)
 *     - @ref storc_flash
 *     - @ref storc_erase
 *     - @ref storc_events
 *
 * @subsection storpage_getting_started_cleanup Cleaning Up
 *
 * Don't forget to cleanup!
 * @code
 * // Free up all the CDAs.
 * for (handle_index = 0; handle_index < num_good_handles; handle_index++)
 * {
       SA_UnInitializeCDA(test_CDAs[handle_index]);
 *     SA_free(test_CDAs[handle_index]);
 * }
 * SC_DestroyDeviceDescriptorList(device_list);
 * // Clear static data in reverse order (modules should no longer be used!)
 * UnInitializeDiagCore();
 * UnInitializeStorageCore();
 * UnInitializeSysCore();
 * @endcode
 *
 * @section storpage_dependencies Dependencies
 *
 * ## Windows
 *
 * ### Compiling
 * - Microsoft Visual Studio 2015 v14.0\n
 *   Client apps that link with the windows libraries are encouraged to build their tools
 *   using the same toolset version.
 *
 * ### Linking
 * - setupapi.lib\n
 *   The Windows host sysCore is requires references into the  Windows SDK
 *   setupapi library. This library must be included with linking any application
 *   with the Windows host sysCore library.
 * - MultiThread Runtime\n
 *   The Windows host storageCore libraries are compiled with the MultiThreaded (/MT) and
 *   MultiThreadedDebug (/MTd) runtime library flags (see https://msdn.microsoft.com/en-us/library/aa278396(v=vs.60).aspx).
 *
 * ## Linux/FreeBSD/Solaris
 *
 * ### Linking
 * - pthread\n
 *   storageCore requires linking with the POSIX pthread library (-lpthread).
 *
 * @section storpage_customizing Customizing
 * Some of storageCore's behavior is customizable at compile time. Most of these configurations are
 * available in the provided (default) project_properties.h header file. The project_properties.h
 * comes set with recommended values and will work for most applications. The following properties
 * can be set in a client provided 'project_properties.h' at compile time or on the compile command-line
 * as preprocessor compiler conditions.
 *
 * Property                             | Default | Description
 * ------------------------------------ | ------- | -----------
 * MIN_CONTROLLER_SUPPORT               | 1       | Defines the minimum controller support that storageCore will allow. Controller firmware families that fall below this value will not be handled by storageCore.
 * ARRAY_TYPE_INTERFACE_MIXING          | 0       | If 1, allows storageCore to mix physical drive interface types (i.e. SAS/SATA) for an Array. This functionality is still limited by firmware support.
 * ARRAY_TYPE_MEDIA_MIXING              | 0       | If 1, allows storageCore to mix physical drive media types (i.e. HDD/SSD) for an Array. This functionality is still limited by firmware support.
 * ARRAY_TYPE_RECORDING_MIXING          | 0       | If 1, allows storageCore to mix SMR physical drive recording types (i.e. HA/HM/DM) for an Array. This functionality is still limited by firmware support.
 * ALLOW_NON_512_BOOT_PHYSICAL_DRIVES   | 1       | If 1, storaegCore will allow physical drives with a logical blocksize other than 512 (i.e. 4kn) to be set as legacy boot volumes.
 * ALLOW_NON_512_BOOT_LOGICAL_DRIVES    | 1       | If 1, storaegCore will allow logical drives using drives with a blocksize other than 512 (i.e. 4kn) to be set as legacy boot volumes.
 * ENABLE_SOFTWARE_RAID_SUPPORT         | 1       | If 1, will enable discovery/configuration of supported software RAID controllers.
 * INVALIDATE_PARTITION_TABLE_ON_DELETE | 1       | If 1, when deleting a logical drive, storageCore will invalidate any partition table on the logical drive so that re-creating the logical drive will not cause any operating systems to rediscover the original partitions. In general, this property should be set with INVALIDATE_PARTITION_TABLE_ON_CREATE.
 * INVALIDATE_PARTITION_TABLE_ON_CREATE | 0       | If 1, when creating a logical drive, storageCore will invalidate any partition table on the logical drive so that any operating systems will not rediscover any previous partitions. In general, this property should be set with INVALIDATE_PARTITION_TABLE_ON_DELETE.
 * ALLOW_RPI_ON_ALL_RAID                | 0       | If 1, users can attempt to configure new logical drives with RPI (Rapid Parity Initialization), if supported by firmware, regardless of the fault tolerance (RAID) level of the logical drive.
 * MAX_MNP_DELAY                        | 1440    | Set a software max limit on the MNP delay.
 * CACHE_PERCENT_STEP                   | 5       | Increment value when setting the read/write cache ratio of the controller. For example, if 5, the read/write cache ratios can be set to (only) 0/100, 5/95, 10/90, ..., 95/5, 100/0.
 * MINIMUM_LOGICAL_DRIVE_SIZE_IN_BLOCKS | 32768   | Minimum allowed size for new logical drives (in blocks).
 * ALLOW_4KN_RAID_CONFIGURATION         | 1       | If 1, allow configuring RAID logical drives using 4kn physical drives (if supported by firmware).
 * ALLOW_NVME_RAID_CONFIGURATION        | 1       | If 1, allow configuring RAID logical drives using NVMe physical drives (if supported by firmware).
 * ALLOW_EDITABLE_TRANSFORMATIONS       | 1       | If 1, allow transformations.
 * ALLOW_FAILED_HBA_DISCOVERY           | 1       | If 1, failed HBA drives that are detected will be enumerated with all physical drives.
 * @todo add listing of performance tuning/debugging properties.
 */

/** @page storc_examples_page Examples
 * @tableofcontents
 * @vendor_copyright
 *
 * @section storc_prop_examples Device Properties Examples
 * @subsection storc_prop_example_inventory Inventory Example
 * @code{.c}
 * // Create and initialize a Controller Data Area (CDA)
 * PCDA my_CDA = (PCDA)SA_malloc(SA_GetCDASize());
 * int initialize_result = SA_InitializeCDA(my_cda, controller_handle);
 * if (initialize_result == kInitializeSucceeded)
 * {
 *   // Loop over each logical drive connected to the controller
 *   logical_drive_id = SA_EnumerateControllerLogicalDrives(my_cda, kInvalidLDNummber);
 *   while (logical_drive_id != kInvalidLDNumber)
 *   {
 *     // print the RAID level of the logical drive
 *     int raid_info = SA_GetLogicalDriveRAIDInfo(my_cda, logical_drive_id);
 *     switch (RAID_LEVEL(raid_info))
 *     {
 *     case kRAID0:
 *       printf(" Logical drive is RAID 0\n");
 *       break;
 *     // case kRAID1: ...
 *     }
 *
 *    logical_drive_id = SA_EnumerateControllerLogicalDrives(my_cda, logical_drive_id);
 *   }
 * }
 * @endcode
 *
 * @section storc_config_examples Configuration Examples
 *
 * @subsection storc_config_example_action Change Controller Mode
 * @code{.c}
 * // Create and initialize a Controller Data Area (CDA)
 * PCDA my_CDA = (PCDA)SA_malloc(SA_GetCDASize());
 * int initialize_result = SA_InitializeCDA(my_cda, controller_handle);
 * if (initialize_result == kInitializeSucceeded)
 * {
 *   // Can the controller be changed to HBA Mode?
 *   SA_BYTE disposition = SA_CanChangeControllerMode(my_cda, kControllerModeHBA);
 *   if (disposition == kChangeControllerModeOperationAllowed)
 *   {
 *     SA_DoChangeControllerMode(my_cda, kControllerModeHBA);
 *   }
 *   else
 *   {
 *     switch(disposition)
 *     {
 *     case kChangeControllerModeUnsupportedMode:
 *       printf("HBA mode is not supported");
 *       break;
 *     case kChangeControllerModeLogicalDrivesExist:
 *       printf("Cannot change to HBA mode because logical drives exist");
 *       break;
 *     };
 *   }
 * }
 * @endcode
 *
 * @subsection storc_config_example_create Create New Single Drive/RAID0 Logical Drive
 * @code{.c}
 *
 * // Attempt to create a single RAID 0 logical drive using 1 unassigned physical drive
 * // Since we're using the editable configuration, if any error occurs (or SA_CommitEditableConfiguration
 * // is not called), no changes will be applied.
 * //
 * // This example could be modified to create a series of logical drives on existing or new
 * // physical drive arrays and then either committed/applied, or dropped at any time an no
 * // changes would be made to hardware. This is allows users to virtual configure their storage
 * // and only apply those changes knowing it is a valid configuration.
 * int CreateSingleLogicalDriveExample(PCDA cda)
 * {
 *   // Create a new editable configuration to edit (only one can exist at a time).
 *   if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 *   {
 *     // Decode reason editable configuration cannot be created
 *     SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *     if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *     {
 *       fprintf(stderr, "Error: An editable configuration already exists\n");
 *     }
 *     // else if (cannot_create_reason == ...
 *     return 1;
 *   }
 *
 *   int error = ConfigureSingleDriveRaid0(cda);
 *   // You could make more virtual changes here...
 *
 *   // Once we've made all our changes we can either:
 *   // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 *   // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 *   //
 *   // In either case, a new editable configuration can be created to make more changes.
 *   // For this example we're going to commit/apply our changes.
 *   if (!error)
 *   {
 *     SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *     if (can_commit == kCanCommitEditableConfigurationOK)
 *     {
 *       if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *       {
 *         fprintf(stderr, "Error: Failed to commit changes\n");
 *         error = 1;
 *       }
 *     }
 *     else
 *     {
 *       if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *       {
 *         fprintf(stderr, "Error: There are unlocked logical drives\n");
 *       }
 *       // else if (can_commit == ...
 *       error = 1;
 *     }
 *   }
 *
 *   // Ensure we always clean the editable configuration.
 *   SA_DestroyEditableConfiguration(cda);
 *   return error;
 * }
 *
 * // Create a new Array, add 1 new RAID 0 logical drive to an existing editable configuration.
 * int ConfigureSingleDriveRaid0(PCDA cda)
 * {
 *   SA_WORD new_array_number = kInvalidArrayNumber;
 *   SA_WORD new_ld_number = kInvalidLDNumber;
 *
 *   // Create a new Array
 *   // Or add the new logical drive to an existing array of `new_array_number`.
 *   // This example will create a new Array.
 *   new_array_number = SA_CreateEditableArray(cda);
 *   if (new_array_number == kInvalidArrayNumber)
 *   {
 *      // Decode reasons editable array could not be created
 *      SA_BYTE cannot_create_array_reason = SA_CanCreateEditableArray(cda);
 *      if (cannot_create_array_reason == kCanCreateEditableArrayMaxLDCountReached)
 *      {
 *        fprintf(stderr, "Error: Max number of logical drive reached\n");
 *      }
 *      // else if (cannot_create_array_reason == ...
 *
 *      return 1;
 *   }
 *
 *   // Add data drives '2' to the new Array.
 *   SA_WORD pd_number = 2;
 *   if (SA_AddEditableArrayDataDrives(cda, new_array_number, &pd_number, 1, kExpandTypeAddDrivesToParityGroups) != kAddEditableArrayDataDrivesOK)
 *   {
 *      // Decode reason the drive could not be added to the new Array
 *      SA_BYTE cannot_assign_data_drive_reason = SA_CanAddEditableArrayDataDrives(cda, new_array_number, &pd_number, 1, kExpandTypeAddDrivesToParityGroups);
 *      if (cannot_assign_data_drive_reason == kCanAddEditableArrayDataDrivesDriveIsNotUnassigned)
 *      {
 *        fprintf(stderr, "Error: drive '2' is already assigned to another Array\n");
 *      }
 *      // else if (cannot_assign_data_drive_reason == ...
 *      return 1;
 *   }
 *
 *   // Create a new logical drive.
 *   // Note that this can happen in any order with creating a new Array
 *   // since the new logical drive is not yet assigned to an Array.
 *   new_ld_number = SA_CreateEditableLogicalDrive(cda);
 *   if (new_ld_number == kInvalidLDNumber)
 *   {
 *      // Decode reason the logical drive could not be create
 *      SA_BYTE cannot_create_ld_reason = SA_CanCreateEditableLogicalDrive(cda);
 *      if (cannot_create_ld_reason == kCanCreateEditableLogicalDriveControllerNotOK)
 *      {
 *        fprintf(stderr, "Error: Controller status is not 'OK'\n");
 *      }
 *      // else if (cannot_create_ld_reason == ...
 *      return 1;
 *   }
 *
 *   // Assign the new logical drive to the new (or an existing) Array.
 *   // OR, if the logical drive is not what you wanted, you can use `SA_DeleteEditableLogicalDrive`
 *   // and start over with a new editable logical drive.
 *   if (SA_SetEditableLogicalDriveArray(cda, new_ld_number, new_array_number) != kSetEditableLogicalDriveArrayOK)
 *   {
 *      // Decode reason the logical drive could not be assigned to the new array
 *      SA_BYTE cannot_assign_ld_reason = SA_CanSetEditableLogicalDriveArray(cda, new_ld_number, new_array_number);
 *      if (cannot_assign_ld_reason == kCanSetEditableLogicalDriveArrayHasUnlockedLD)
 *      {
 *        // Note that in this situation, this reason would never happen since this is the first
 *        // logical drive that we have tried to add to the [new] array.
 *        fprintf(stderr, "Error: The Array has unlocked logical drives\n");
 *      }
 *      // else if (cannot_assign_ld_reason == ...
 *      return 1;
 *   }
 *
 *   // Now we can configure the logical drive with settings for RAID/Stripsize/Size/...
 *   // Again, to start-over, simply delete the editable logical drive and create a new one.
 *   SA_BYTE number_of_parity_groups = 0; // does not apply to raid 0
 *   SA_BOOL set_default_strip_and_size = kTrue; // Auto-set the stripsize and max out the size of the logical drive.
 *   if (SA_SetEditableLogicalDriveRAIDLevel(cda, new_ld_number, kRAID0, number_of_parity_groups, set_default_strip_and_size) != kSetEditableLogicalDriveRAIDLevelOK)
 *   {
 *      // Decode reason the logical drive fault tolerance could not be set
 *      SA_BYTE cannot_set_ld_raid_reason = SA_CanSetEditableLogicalDriveRAIDLevel(cda, new_ld_number, kRAID0, number_of_parity_groups);
 *      if (cannot_set_ld_raid_reason == kCanSetEditableLogicalDriveRAIDLevelInvalidRAIDLevel)
 *      {
 *        fprintf(stderr, "Error: This controller does not support the desired fault tolerance level\n");
 *      }
 *      // else if (cannot_set_ld_raid_reason == ...
 *      return 1;
 *   }
 *   // [Optional]
 *   // Set other logical drive properties (all have defaults) such as: StripSize, Accelerator, Size, etc...
 *
 *   // Now lock/finalize the new logical drive. At this point, we can create more logical drives
 *   // and add them to this Array or a new one.
 *   if (SA_LockEditableLogicalDrive(cda, new_ld_number) != kLockEditableLogicalDriveOK)
 *   {
 *     // Decode reason the new logical drive could be locked/finalized
 *     SA_BYTE cannot_lock_ld_reason = SA_CanLockEditableLogicalDrive(cda, new_ld_number);
 *     if (cannot_lock_ld_reason == kCanLockEditableLogicalDriveAlreadyLocked)
 *     {
 *       // Note that in this situation, this reason would never happen
 *       fprintf(stderr, "Error: The Logical Drive is already locked.\n");
 *     }
 *     // else if (cannot_lock_ld_reason == ...
 *     return 1;
 *   }
 *
 *   return 0;
 * }
 *
 * @endcode
 *
 * @subsection storc_config_example_delete Delete Existing Logical Drive
 * @see storc_configuration_edit_ld_delete
 * @code{.c}
 *
 * int error = 0;
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   // Decode reason editable configuration cannot be created
 *   SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_create_reason == ...
 *   error = 1;
 * }
 *
 * // Delete an existing logical drive '2'.
 * if (!error && SA_DeleteEditableLogicalDrive(cda, 2) != kDeleteEditableLogicalDriveOK)
 * {
 *   // Decode reason logical drive '2' could not be deleted
 *   SA_BYTE cannot_delete_ld_reason = SA_CanDeleteEditableLogicalDrive(cda, 2);
 *   if (cannot_delete_ld_reason == kCanDeleteEditableLogicalDriveNotLastLDInArray)
 *   {
 *     fprintf(stderr, "Error: Can only delete the last logical drive of an Array\n");
 *   }
 *   // else if (cannot_delete_ld_reason == ...
 *   error = 1;
 * }
 *
 * // Once we've made all our changes we can either:
 * // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 * // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 * //
 * // In either case, a new editable configuration can be created to make more changes.
 * // For this example we're going to commit/apply our changes.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 * // Always remember to cleanup.
 * SA_DestroyEditableConfiguration(cda);
 *
 * @endcode
 *
 * @subsection storc_config_example_delete_cached_ld Delete Logical Drive with Cache Logical Drive
 * @see storc_configuration_edit_ld_delete
 * @code{.c}
 * // Delete data logical drive '2' and it's caching logical drive (if it has one).
 * // For this case, we do not have to worry if the LU caching write policy is write-back
 * // since we're deleting the data logical drive.
 * SA_WORD ld_number_to_delete = 2;
 *
 * int error = 0;
 * SA_WORD cache_ld_number = kInvalidLDNumber;
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   // Decode reason editable configuration cannot be created
 *   SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_create_reason == ...
 *   error = 1;
 * }
 *
 * if (SA_GetEditableLogicalDriveType(cda, ld_number_to_delete) == kLogicalDriveTypeData)
 * {
 *   cache_ld_number = SA_GetEditableLogicalDriveLUCacheNumber(cda, ld_number_to_delete);
 * }
 *
 * // Delete an existing logical drive 'ld_number_to_delete'.
 * if (!error)
 * {
 *   SA_BYTE cannot_delete_ld_reason = SA_CanDeleteEditableLogicalDrive(cda, ld_number_to_delete);
 *   error = 1;
 *   if (cannot_delete_ld_reason == kCanDeleteEditableLogicalDriveNotLastLDInArray)
 *   {
 *     fprintf(stderr, "Error: Can only delete the last logical drive of an Array\n");
 *   }
 *   // else if (cannot_delete_ld_reason == ...
 *   else if (cannot_delete_ld_reason == kCanDeleteEditableLogicalDriveOK)
 *   {
 *     if (SA_DeleteEditableLogicalDrive(cda, ld_number_to_delete) == kDeleteEditableLogicalDriveOK)
 *     {
 *       error = 0;
 *     }
 *     else
 *     {
 *       fprintf(stderr, "Error: Failed to delete logical drive %u\n", ld_number_to_delete);
 *     }
 *   }
 *   else if (cannot_delete_ld_reason == kCanDeleteEditableLogicalDriveOK)
 *   {
 *     fprintf(stderr, "Error: Cannot delete logical drive %u (unknown reason)\n", ld_number_to_delete);
 *   }
 * }
 *
 * // Now we delete the LU caching logical drive (if it exists) so that we do not leave an LU cache orphan
 * if (!error && cache_ld_number != kInvalidLDNumber)
 * {
 *   if (SA_DeleteEditableLogicalDrive(cda, cache_ld_number) != kDeleteEditableLogicalDriveOK)
 *   {
 *     // Decode reason caching logical drive could not be deleted
 *     SA_BYTE cannot_delete_ld_reason = SA_CanDeleteEditableLogicalDrive(cda, cache_ld_number);
 *     if (cannot_delete_ld_reason == kCanDeleteEditableLogicalDriveNotLastLDInArray)
 *     {
 *       fprintf(stderr, "Error: Can only delete the last LU caching logical drive of a LU cache Array\n");
 *     }
 *     // else if (cannot_delete_ld_reason == ...
 *     error = 1;
 *   }
 * }
 *
 * // Once we've made all our changes we can either:
 * // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 * // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 * //
 * // In either case, a new editable configuration can be created to make more changes.
 * // For this example we're going to commit/apply our changes.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 * // Always remember to cleanup.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 *
 * @subsection storc_config_lu_cache LU Cache
 * @see storc_configuration_edit_lu_cache
 *
 * First create a data logical drive (such as the example: @ref storc_config_example_create).
 *
 * Next, create and link a new LU cache logical drive to the data logical drive. The steps
 * for creating an LU cache logical drive are the same as creating a data logical drive (the
 * same API functions are used). The only differences are:
 *
 * 1) Use @ref SA_CreateEditableLUCacheArray (instead of @ref SA_CreateEditableArray).
 * 2) Use @ref SA_SetEditableLogicalDriveLUCache to link the lu cache logical drive to a data logical drive.
 *
 * The following example can be performed either before committing the new data logical drive
 * or as another editable configuration.
 *
 * The following examples assumes that the editable configuration has not yet been committed.
 *
 * @subsubsection storc_config_example_lu_cache_create Create LU Cache
 * @code{.c}
 * // Attempt to create a RAID 1 Write-Back LU cache logical drive and use it to cache
 * // data logical drive 1.
 * // This examples assumes the editable configuration has been created and logical drive 1
 * // either existed before the editable configuration was created or was created using
 * // this editable configuration instance.
 * SA_WORD new_array_number = kInvalidArrayNumber;
 * SA_WORD new_ld_number = kInvalidLDNumber;
 *
 * // Assume we have already created the editable configuration and that the caller
 * // will clean up/destroy the editable configuration.
 *
 * // Create a new LU caching Array
 * // Or add the new lu cache logical drive to the existing LU cache array of `new_array_number`.
 *
 * // This example will create a new LU cache Array.
 * new_array_number = SA_CreateEditableLUCacheArray(cda);
 * SA_BYTE cannot_create_array_reason = SA_CanCreateEditableLUCacheArray(cda);
 * if (cannot_create_array_reason == kCanCreateEditableLUCacheArrayOK)
 * {
 *     new_array_number = SA_CreateEditableLUCacheArray(cda);
 *     if (new_array_number == kInvalidArrayNumber)
 *     {
 *         fprintf(stderr, "Error: Failed to create LU cache Array\n");
 *         return 1;
 *     }
 * }
 * else
 * {
 *     if (cannot_create_array_reason == kCanCreateEditableLUCacheArrayMaxLDCountReached)
 *     {
 *         // Decode reason the LU cache Array cannot be created.
 *         fprintf(stderr, "Error: Max number of logical drive reached\n");
 *     }
 *     // else if (cannot_create_array_reason == ...
 *     return 1;
 * }
 *
 * // Add SSD drives '2' & '3' to the new Array.
 * SA_WORD pd_numbers[2] = { 2, 3 };
 * SA_BYTE cannot_assign_data_drive_reason = SA_CanAddEditableArrayDataDrives(cda, new_array_number, pd_numbers, 2, kExpandTypeAddDrivesToParityGroups);
 * if (cannot_assign_data_drive_reason == kCanAddEditableArrayDataDrivesOK)
 * {
 *     if (SA_AddEditableArrayDataDrives(cda, new_array_number, pd_numbers, 2, kExpandTypeAddDrivesToParityGroups) != kAddEditableArrayDataDrivesOK)
 *     {
 *         fprintf(stderr, "Error: Failed to add drives to LU cache Array\n");
 *         return 1;
 *     }
 * }
 * else
 * {
 *     // Decode reason the drive could not be added to the new Array
 *     if (cannot_assign_data_drive_reason == kCanAddEditableArrayDataDrivesDriveIsNotUnassigned)
 *     {
 *         fprintf(stderr, "Error: drive '2' is already assigned to another Array\n");
 *     }
 *     // else if (cannot_assign_data_drive_reason == ...
 *     return 1;
 * }
 *
 * // Create a new LU cache logical drive.
 * // Note that this can happen in any order with creating a new Array
 * // since the new logical drive is not yet assigned to an Array.
 * SA_BYTE cannot_create_ld_reason = SA_CanCreateEditableLogicalDrive(cda);
 * if (cannot_create_ld_reason == kCanCreateEditableLogicalDriveOK)
 * {
 *     new_ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (new_ld_number == kInvalidLDNumber)
 *     {
 *         fprintf(stderr, "Error: Failed to create logical drive\n");
 *         return 1;
 *     }
 * }
 * else
 * {
 *     if (cannot_create_ld_reason == kCanCreateEditableLogicalDriveControllerNotOK)
 *     {
 *         // Decode reason the logical drive could not be create
 *         fprintf(stderr, "Error: Controller status is not 'OK'\n");
 *     }
 *     // else if (cannot_create_ld_reason == ...
 *     return 1;
 * }
 *
 * // Assign the new logical drive to the new LU cache Array.
 * SA_BYTE cannot_assign_ld_reason = SA_CanSetEditableLogicalDriveArray(cda, new_ld_number, new_array_number);
 * if (cannot_assign_ld_reason == kCanSetEditableLogicalDriveArrayOK)
 * {
 *     if (SA_SetEditableLogicalDriveArray(cda, new_ld_number, new_array_number) != kSetEditableLogicalDriveArrayOK)
 *     {
 *         fprintf(stderr, "Error: Failed to assign LU cache logical drive to Array\n");
 *         return 1;
 *     }
 * }
 * else
 * {
 *     // Decode reason the logical drive could not be assigned to the new array
 *     if (cannot_assign_ld_reason == kCanSetEditableLogicalDriveArrayHasUnlockedLD)
 *     {
 *         fprintf(stderr, "Error: The Array has unlocked logical drives\n");
 *     }
 *     // else if (cannot_assign_ld_reason == ...
 *     return 1;
 * }
 *
 * // Now we can configure the logical drive with settings for RAID/Size/...
 * SA_BYTE cannot_set_ld_raid_reason = SA_CanSetEditableLogicalDriveRAIDLevel(cda, new_ld_number, kRAID0, number_of_parity_groups);
 * if (cannot_set_ld_raid_reason == kCanSetEditableLogicalDriveRAIDLevelOK)
 * {
 *     SA_BOOL set_default_strip_and_size = kTrue; // Max out the size of the cache logical drive.
 *     if (SA_SetEditableLogicalDriveRAIDLevel(cda, new_ld_number, kRAID1, 0, set_default_strip_and_size) != kSetEditableLogicalDriveRAIDLevelOK)
 *     {
 *         fprintf(stderr, "Error: Failed to set RAID level of LU cache logical drive\n");
 *         return 1;
 *     }
 * }
 * else
 * {
 *     // Decode reason the logical drive fault tolerance could not be set
 *     if (cannot_set_ld_raid_reason == kCanSetEditableLogicalDriveRAIDLevelInvalidRAIDLevel)
 *     {
 *         fprintf(stderr, "Error: This controller does not support the desired fault tolerance level\n");
 *     }
 *     // else if (cannot_set_ld_raid_reason == ...
 *     return 1;
 * }
 *
 * // Now we link the LU cache logical drive to logical drive 1 and set the write policy to write-back
 * SA_BYTE cannot_set_ld_lu_cache = SA_CanSetEditableLogicalDriveLUCache(cda, 1, new_ld_number, kLUCacheWritePolicyWriteBack);
 * if (cannot_set_ld_lu_cache == kCanSetEditableLogicalDriveLUCacheOK)
 * {
 *     if (SA_SetEditableLogicalDriveLUCache(cda, 1, new_ld_number, kLUCacheWritePolicyWriteBack) != kSetEditableLogicalDriveLUCacheOK)
 *     {
 *         fprintf(stderr, "Error: Failed to set write-back lu cache logical drive to logical drive\n");
 *         return 1;
 *     }
 * }
 * else
 * {
 *     // Decode reason the logical drive fault tolerance could not be set
 *     if (cannot_set_ld_lu_cache == kCanSetEditableLogicalDriveLUCachePolicyUnsupported)
 *     {
 *         fprintf(stderr, "Error: This controller does not support the desired cache write policy\n");
 *     }
 *     // else if (cannot_set_ld_lu_cache == ...
 *     return 1;
 * }
 *
 * // Now lock/finalize the new lu cache logical drive. At this point, we can create more logical drives
 * // and add them to this Array.
 * SA_BYTE cannot_lock_ld_reason = SA_CanLockEditableLogicalDrive(cda, new_ld_number);
 * if (cannot_lock_ld_reason == kCanLockEditableLogicalDriveOK)
 * {
 *     if (SA_LockEditableLogicalDrive(cda, new_ld_number) != kLockEditableLogicalDriveOK)
 *     {
 *         fprintf(stderr, "Error: Failed to lock the logical drive.\n");
 *         return 1;
 *     }
 * }
 * else
 * {
 *     // Decode reason the new logical drive could be locked/finalized
 *     if (cannot_lock_ld_reason == kCanLockEditableLogicalDriveAlreadyLocked)
 *     {
 *         fprintf(stderr, "Error: The Logical Drive is already locked.\n");
 *     }
 *     // else if (cannot_lock_ld_reason == ...
 *     return 1;
 * }
 *
 * return 0;
 * @endcode
 *
 * @section storc_config_transform_examples Transformation Examples
 *
 * @see storc_configuration_virtual
 * of @ref storc_configuration
 *
 * @subsection storc_config_transform_example_extend Logical Drive Expansion
 * @see storc_configuration_edit_ld_set_size
 * @code{.c}
 *
 * // Given an existing configuration of logical drives the following example
 * // will extend logical drive #2 to its max possible size.
 * //     Array 1                                Array 1
 * //   ,----------,                           ,----------,
 * //  (            )                         (            )
 * //  |~----------~|                         |~----------~|
 * //  |    LD 1    |                         |    LD 1    |
 * //  |~----------~|                         |~----------~|
 * //  |    LD 2    | | Extend LD 2 to Max =  |            |
 * //  |~----------~| V                       |            |
 * //  |    LD 3    |                         |    LD 2    |
 * //  |~----------~|                         |            |
 * //  |            |                         |            |
 * //  | Free Space |                         |~----------~|
 * //  |            |                         |    LD 3    |
 * //   "----------"                           "----------"
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   // Decode reason editable configuration cannot be created
 *   SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_create_reason == ...
 *   return 1;
 * }
 *
 * int error = 0;
 *
 * // Can use SA_GetEditableLogicalDriveSize with kLogicalDriveSizeMinResizeableSizeBlocks and kLogicalDriveSizeMaxResizeableSizeBlocks
 * // to get min/max transformable size for a logical drive. Note that logical drives cannot become smaller than their created size,
 * // however, kLogicalDriveSizeMinResizeableSizeBlocks can be used to reduce or undo an ld extension.
 * SA_QWORD new_max_size_blocks = 0;
 * SA_BYTE can_get_extended_size = SA_GetEditableLogicalDriveSize(cda, 2, kLogicalDriveSizeMaxResizeableSizeBlocks, &new_max_size_blocks);
 * if (can_get_extended_size != kGetEditableLogicalDriveSizeOK)
 * {
 *    // Decode reason the max extendable size for a logical drive could not be determined.
 *    if (can_get_extended_size == kGetEditableLogicalDriveSizeNoEditableConfigCreated)
 *    {
 *      fprintf(stderr, "Error: the editable configuration was not created\n");
 *    }
 *    // else if (can_get_extended_size == ...
 *    error = 1;
 * }
 *
 * if (!error && SA_SetEditableLogicalDriveSize(cda, 2, kEditableLogicalDriveSizeBlocks, new_max_size_blocks) != kSetEditableLogicalDriveSizeOK)
 * {
 *   // Decode reason editable logical drive could not be re-sized
 *   SA_BYTE cannot_extend_ld = SA_CanSetEditableLogicalDriveSize(cda, 2, kEditableLogicalDriveSizeBlocks, new_max_size_blocks);
 *   if (cannot_extend_ld == kCanSetEditableLogicalDriveSizeControllerStatusNotOK)
 *   {
 *     fprintf(stderr, "Error: Controller status no 'OK'\n");
 *   }
 *   // else if (cannot_extend_ld == ...
 *    error = 1;
 * }
 *
 * // We can either resize another logical drive on this or another Array, or undo/resize the target
 * // logical drive to something between kLogicalDriveSizeMinResizeableSizeBlocks and kLogicalDriveSizeMaxResizeableSizeBlocks.
 *
 * // Once we've made all our changes we can either:
 * // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 * // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 * //
 * // In either case, a new editable configuration can be created to make more changes.
 * // For this example we're going to commit/apply our changes.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 * // Always remember to cleanup.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 * @subsection storc_config_transform_example_ld_move Logical Drive/Array Move
 * @see storc_configuration_edit_ld_set_array
 * @code{.c}
 *
 * // Given an existing configuration of logical drives the following example
 * // will move Logical drives 1 from Array 1 to Array 2.
 * //
 * //     Array 1         Array 2          Array 1        Array 2
 * //   ,--------,       ,--------,       ,--------,     ,--------,
 * //  (          )     (          )     (          )   (          )
 * //  |~--------~|     |~--------~|     |~--------~|   |~--------~|
 * //  |   LD 1   | \   |   LD 3   | ==> |   LD 2   |   |   LD 3   |
 * //  |~--------~|  \  |~--------~|     |~--------~|   |~--------~|
 * //  |   LD 2   |   > |          |     |          |   |   LD 1   |
 * //   "--------"       "--------"       "--------"     "--------"
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   // Decode reason editable configuration cannot be created
 *   SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_create_reason == ...
 *   return 1;
 * }
 *
 * int error = 0;
 *
 * if (SA_SetEditableLogicalDriveArray(cda, 1, 2) != kSetEditableLogicalDriveArrayOK)
 * {
 *   // Decode reason editable logical drive could not be moved
 *   SA_BYTE cannot_move_ld = SA_CanSetEditableLogicalDriveArray(cda, 1, 2);
 *   if (cannot_move_ld == kCanSetEditableLogicalDriveArrayMismatchDriveCount)
 *   {
 *     fprintf(stderr, "Error: Destination Array has a different number of data drives\n");
 *   }
 *   // else if (cannot_move_ld == ...
 *   error = 1;
 * }
 *
 * // We can either move more logical drives from Array 1 and/or move other logical drives to Array 2.
 * // Note that the destination Array can be a newly created editable array (using SA_CreateEditableArray).
 *
 * // Once we've made all our changes we can either:
 * // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 * // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 * //
 * // In either case, a new editable configuration can be created to make more changes.
 * // For this example we're going to commit/apply our changes.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 * // Always remember to cleanup.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 * @subsection storc_config_transform_example_array_swap Array Data Drive Swap
 * @see storc_configuration_edit_array_move
 * @code{.c}
 *
 * // Given an existing configuration of logical drives the following example
 * // will swap data drive 2 with unassigned drive 3 for all logical drives
 * // of Array 15.
 * //
 * //           Array 15
 * //   ,--------,    ,--------,        ,--------,
 * //  (   PD 1   )  (   PD 2   )      (   PD 3   )
 * //  |~--------~|  |~--------~| Swap |~--------~|
 * //  |   LD 1   |  |   LD 1   | <==> |          |
 * //  |~--------~|  |~--------~|      |  Unused  |
 * //  |   LD 2   |  |   LD 2   |      |          |
 * //   "--------"    "--------"        "--------"
 * //                      |
 * //           Array 15   V
 * //   ,--------,    ,--------,        ,--------,
 * //  (   PD 1   )  (   PD 3   )      (   PD 2   )
 * //  |~--------~|  |~--------~|      |~--------~|
 * //  |   LD 1   |  |   LD 1   |      | Unusable |
 * //  |~--------~|  |~--------~|      | Pending  |
 * //  |   LD 2   |  |   LD 2   |      | Transform|
 * //   "--------"    "--------"        "--------"
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   // Decode reason editable configuration cannot be created
 *   SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_create_reason == ...
 *   return 1;
 * }
 *
 * int error = 0;
 *
 * if (!error && SA_SwapEditableArrayDataDrive(cda, 15, 2, 3) != kSwapEditableArrayDataDriveOK)
 * {
 *   // Decode reason editable Array could not swap data drives.
 *   SA_BYTE cannot_swap_drive = SA_CanSwapEditableArrayDataDrive(cda, 15, 2, 3);
 *   if (cannot_swap_drive == kCanSwapEditableArrayDataDriveIsTooSmall)
 *   {
 *     fprintf(stderr, "Error: Requested drive is too small for the Array\n");
 *   }
 *   // else if (cannot_swap_drive == ...
 *   error = 1;
 * }
 *
 * // Now, we can swap more (or all) data drives of the Array
 * // provided PD 2 is not used for any other operations.
 *
 * // Once we've made all our changes we can either:
 * // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 * // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 * //
 * // In either case, a new editable configuration can be created to make more changes.
 * // For this example we're going to commit/apply our changes.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 * // Always remember to cleanup.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 * @subsection storc_config_transform_example_ld_migrate Logical Drive RAID/StripSize Migration
 * @see storc_configuration_edit_ld_set_raid
 * @see storc_configuration_edit_ld_set_stripsize
 * @code{.c}
 *
 * // Given an existing configuration of logical drives the following example
 * // will migrate the RAID and stripsize of logical drive 2.
 * //
 * //     Array 1                            Array 1
 * //   ,----------,                       ,----------,
 * //  (  PDs...    )                     (  PDs...    )
 * //  |~----------~|                     |~----------~|
 * //  |    LD 1    |                     |    LD 1    |
 * //  |~----------~|  Migrate To         |~----------~|
 * //  |    LD 2    |  RAID 60 w/         |    LD 2    |
 * //  |   RAID 50  |  3 parity groups    |   RAID 60  |
 * //  |   npg 2    |  and stripsize 64.  |   npg 3    |
 * //  |   ss 512   |  =================> |   ss 64    |
 * //  |~----------~|                     |~----------~|
 * //  |    LD 3    |                     |    LD 3    |
 * //   "----------"                       "----------"
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   // Decode reason editable configuration cannot be created
 *   SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_create_reason == ...
 *   return 1;
 * }
 *
 * int error = 0;
 *
 * // Migrate the RAID/npg (but we could have done the stripsize first if we wanted).
 * if (SA_SetEditableLogicalDriveRAIDLevel(cda, 2, kRAID60, 3, kFalse) != kSetEditableLogicalDriveRAIDLevelOK)
 * {
 *   // Decode reason we could not queue the editable logical drive RAID migration
 *   SA_BYTE cannot_migrate_raid = SA_CanSetEditableLogicalDriveRAIDLevel(cda, 2, kRAID60, 3);
 *   if (cannot_migrate_raid == kCanSetEditableLogicalDriveRAIDLevelFullStripeSizeOverflow)
 *   {
 *     fprintf(stderr, "Error: The controller does not have the memory capacity to perform the transformation\n");
 *   }
 *   // else if (cannot_migrate_raid == ...
 *   error = 1;
 * }
 *
 * // Now to migrate the stripsize
 * if (!error && SA_SetEditableLogicalDriveStripSize(cda, 2, 64) != kSetEditableLogicalDriveStripSizeOK)
 * {
 *   // Decode reason editable
 *   SA_BYTE cannot_migrate_ss = SA_CanSetEditableLogicalDriveStripSize(cda, 2, 64);
 *   if (cannot_migrate_ss == kCanSetEditableLogicalDriveStripSizeFullStripeSizeOverflow)
 *   {
 *     fprintf(stderr, "Error: The controller does not have the memory capacity to perform the transformation\n");
 *   }
 *   // else if (cannot_migrate_ss == ...
 *   error = 1;
 * }
 *
 * // We can perform RAID/ss migration on logical drives of **other** Arrays or re-migrate
 * // the RAID/stripsize of logical drive 2.
 *
 * // Once we've made all our changes we can either:
 * // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 * // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 * //
 * // In either case, a new editable configuration can be created to make more changes.
 * // For this example we're going to commit/apply our changes.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 * // Always remember to cleanup.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 * @subsection storc_config_transform_example_expand Array Data Drive Add (Array Expand)
 * @see storc_configuration_edit_array_add_data_drive
 *
 * @subsubsection storc_config_transform_example_expand_dpg Array Expand Size Of Parity Groups
 * @code{.c}
 * // Given an existing configuration of 1 RAID 50 logical drive, the following example
 * // will add data drives 2 & 4 to Array 1.
 * //
 * //                            Array 1
 * //         Parity Group 1                Parity Group 2
 * //       ,----------------,  RAID 5+0  ,----------------,
 * //      /                  \          /                  \
 * //   ,----,    ,----,    ,----,    ,----,    ,----,    ,----,
 * //  (      )  (      )  (      )  (      )  (      )  (      )
 * //  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|
 * //  | PD 1 |  | PD 3 |  | PD 5 |  | PD 6 |  | PD 7 |  | PD 8 |
 * //   "----"    "----"    "----"    "----"    "----"    "----"
 * //
 * //         Add 2 PDs (1 per parity group)
 * //
 * //             Parity Group 1                          Parity Group 2
 * //       ,--------------------------,  RAID 5+0  ,--------------------------,
 * //      /                            \          /                            \
 * //   ,----,    ,----,    ,----,    ,----,    ,----,    ,----,    ,----,    ,----,
 * //  (      )  (      )  (      )  (      )  (      )  (      )  (      )  (      )
 * //  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|
 * //  | PD 1 |  | PD 2 |  | PD 3 |  | PD 4 |  | PD 5 |  | PD 6 |  | PD 7 |  | PD 8 |
 * //   "----"    "----"    "----"    "----"    "----"    "----"    "----"    "----"
 * //
 * // Note that you cannot controller which drives will be assigned to which parity groups.
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   // Decode reason editable configuration cannot be created
 *   SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_create_reason == ...
 *   return 1;
 * }
 *
 * int error = 0;
 *
 * SA_WORD pd_numbers[] = { 2, 4 };
 * // If we wanted to add an entire parity group (not possible given this number of drives) we would simply give
 * // a drive list with enough pd numbers to equal the size of 1 parity group and instead use @ref kExpandTypeAddPartyGroups.
 * if (SA_AddEditableArrayDataDrives(cda, 1, pd_numbers, 2, kExpandTypeAddDrivesToParityGroups) != kAddEditableArrayDataDrivesOK)
 * {
 *    // Decode reason could not add data drives to editable Array.
 *    SA_BYTE cannot_expand_array = SA_CanAddEditableArrayDataDrives(cda, 1, pd_numbers, 2, kExpandTypeAddDrivesToParityGroups);
 *    if (cannot_expand_array == kCanAddEditableArrayDataDrivesNotSupported)
 *    {
 *       fprintf(stderr, "Error: Array Expand not supported\n");
 *    }
 *    // else if (cannot_expand_array == ...
 *    error = 1;
 *  }
 *
 * // Now, we can add more data drives to the Array or perform other compatible transformations.
 *
 * // Once we've made all our changes we can either:
 * // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 * // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 * //
 * // In either case, a new editable configuration can be created to make more changes.
 * // For this example we're going to commit/apply our changes.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 * // Always remember to cleanup.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 * @subsubsection storc_config_transform_example_expand_npg Array Expand By Adding Parity Groups
 * @code{.c}
 * // Given an existing configuration of 1 RAID 50 logical drive, the following example
 * // will add data drives 2, 4, & 9 to Array 1 by adding an additional parity group.
 * //
 * //                            Array 1
 * //         Parity Group 1                Parity Group 2
 * //       ,----------------,  RAID 5+0  ,----------------,
 * //      /                  \          /                  \
 * //   ,----,    ,----,    ,----,    ,----,    ,----,    ,----,
 * //  (      )  (      )  (      )  (      )  (      )  (      )
 * //  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|
 * //  | PD 1 |  | PD 3 |  | PD 5 |  | PD 6 |  | PD 7 |  | PD 8 |
 * //   "----"    "----"    "----"    "----"    "----"    "----"
 * //
 * //         Add 3 PDs (Add 1 parity group)
 * //
 * //                                      RAID 5+0 (npg 3)
 * //          Parity Group 1               Parity Group 2                Parity Group 3
 * //       ,----------------,            ,----------------,            ,----------------,
 * //      /                  \          /                  \          /                  \
 * //   ,----,    ,----,    ,----,    ,----,    ,----,    ,----,    ,----,    ,----,    ,----,
 * //  (      )  (      )  (      )  (      )  (      )  (      )  (      )  (      )  (      )
 * //  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|
 * //  | PD 1 |  | PD 2 |  | PD 3 |  | PD 4 |  | PD 5 |  | PD 6 |  | PD 7 |  | PD 8 |  | PD 9 |
 * //   "----"    "----"    "----"    "----"    "----"    "----"    "----"    "----"    "----"
 * //
 * // Note that you cannot controller which drives will be assigned to which parity groups.
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   // Decode reason editable configuration cannot be created
 *   SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_create_reason == ...
 *   return 1;
 * }
 *
 * int error = 0;
 *
 * SA_WORD pd_numbers[] = { 2, 4, 9 };
 * if (SA_AddEditableArrayDataDrives(cda, 1, pd_numbers, 3, kExpandTypeAddPartyGroups) != kAddEditableArrayDataDrivesOK)
 * {
 *    // Decode reason could not add data drives to editable Array.
 *    SA_BYTE cannot_expand_array = SA_CanAddEditableArrayDataDrives(cda, 1, pd_numbers, 3, kExpandTypeAddPartyGroups);
 *    if (cannot_expand_array == kCanAddEditableArrayDataDrivesNotSupported)
 *    {
 *       fprintf(stderr, "Error: Array Expand not supported\n");
 *    }
 *    // else if (cannot_expand_array == ...
 *    error = 1;
 *  }
 *
 * // Now, we can add more data drives to the Array or perform other compatible transformations.
 *
 * // Once we've made all our changes we can either:
 * // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 * // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 * //
 * // In either case, a new editable configuration can be created to make more changes.
 * // For this example we're going to commit/apply our changes.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 * // Always remember to cleanup.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 * @subsection storc_config_transform_example_shrink Array Data Drive Remove (Array Shrink)
 * @see storc_configuration_edit_array_remove_data_drive
 *
 * @subsubsection storc_config_transform_example_shrink_dpg Array Shrink Size Of Parity Groups
 * @code{.c}
 * // Given an existing configuration of 1 RAID 50 logical drive the following example
 * // will remove data drives 2 & 4 from Array 1.
 * //
 * //                                     Array 1
 * //             Parity Group 1                          Parity Group 2
 * //       ,--------------------------,  RAID 5+0  ,--------------------------,
 * //      /                            \          /                            \
 * //   ,----,    ,----,    ,----,    ,----,    ,----,    ,----,    ,----,    ,----,
 * //  (      )  (      )  (      )  (      )  (      )  (      )  (      )  (      )
 * //  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|
 * //  | PD 1 |  | PD 2 |  | PD 3 |  | PD 4 |  | PD 5 |  | PD 6 |  | PD 7 |  | PD 8 |
 * //   "----"    "----"    "----"    "----"    "----"    "----"    "----"    "----"
 * //
 * //         Remove 2 PDs (1 per parity group)
 * //
 * //         Parity Group 1                Parity Group 2
 * //       ,----------------,  RAID 5+0  ,----------------,
 * //      /                  \          /                  \
 * //   ,----,    ,----,    ,----,    ,----,    ,----,    ,----,
 * //  (      )  (      )  (      )  (      )  (      )  (      )
 * //  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|
 * //  | PD 1 |  | PD 3 |  | PD 5 |  | PD 6 |  | PD 7 |  | PD 8 |
 * //   "----"    "----"    "----"    "----"    "----"    "----"
 * //
 * // Note that even if we want to remove 1 drive from each parity group, we do not
 * // have to explicitly remove a physical drive currently assigned to each parity
 * // group.
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   // Decode reason editable configuration cannot be created
 *   SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_create_reason == ...
 *   return 1;
 * }
 *
 * int error = 0;
 *
 * SA_WORD pd_numbers[] = { 2, 4 }; // Note that the removed drives can be part of the same parity group (or not).
 * // If we wanted to remove an entire parity group (not possible in this configuration) we would simply give
 * // a drive list with enough pd numbers to equal the size of 1 parity group and instead use @ref kShrinkTypeRemovePartyGroups.
 * if (SA_RemoveEditableArrayDataDrives(cda, 1, pd_numbers, 2, kShrinkTypeRemoveDrivesFromParityGroups) != kRemoveEditableArrayDataDriveOK)
 * {
 *    // Decode reason editable Array could not have data drives removed.
 *    SA_BYTE cannot_shrink_array = SA_CanRemoveEditableArrayDataDrives(cda, 1, pd_numbers, 2, kShrinkTypeRemoveDrivesFromParityGroups);
 *    if (cannot_shrink_array == kCanRemoveEditableArrayDataDrivesNotSupported)
 *    {
 *       fprintf(stderr, "Error: Array Shrink not supported\n");
 *    }
 *    // else if (cannot_shrink_array == ...
 *    error = 1;
 * }
 *
 * // Now, we can remove more data drives of the Array or perform other compatible transformations.
 *
 * // Once we've made all our changes we can either:
 * // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 * // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 * //
 * // In either case, a new editable configuration can be created to make more changes.
 * // For this example we're going to commit/apply our changes.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 * // Always remember to cleanup.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 * @subsubsection storc_config_transform_example_shrink_npg Array Shrink Number Of Parity Groups
 * @code{.c}
 * // Given an existing configuration of 1 RAID 50 logical drive the following example
 * // will swap remove data drives 2, 4, & 9 from Array 1 by removing a parity group.
 * //
 * //                                      RAID 5+0 (npg 3)
 * //          Parity Group 1               Parity Group 2                Parity Group 3
 * //       ,----------------,            ,----------------,            ,----------------,
 * //      /                  \          /                  \          /                  \
 * //   ,----,    ,----,    ,----,    ,----,    ,----,    ,----,    ,----,    ,----,    ,----,
 * //  (      )  (      )  (      )  (      )  (      )  (      )  (      )  (      )  (      )
 * //  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|
 * //  | PD 1 |  | PD 2 |  | PD 3 |  | PD 4 |  | PD 5 |  | PD 6 |  | PD 7 |  | PD 8 |  | PD 9 |
 * //   "----"    "----"    "----"    "----"    "----"    "----"    "----"    "----"    "----"
 * //
 * //         Remove 3 PDs (Remove 1 parity group)
 * //
 * //         Parity Group 1                Parity Group 2
 * //       ,----------------,  RAID 5+0  ,----------------,
 * //      /                  \          /                  \
 * //   ,----,    ,----,    ,----,    ,----,    ,----,    ,----,
 * //  (      )  (      )  (      )  (      )  (      )  (      )
 * //  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|  |~----~|
 * //  | PD 1 |  | PD 3 |  | PD 5 |  | PD 6 |  | PD 7 |  | PD 8 |
 * //   "----"    "----"    "----"    "----"    "----"    "----"
 * //
 * //
 * // Note that even if we want to remove a parity group, we do not
 * // have to explicitly remove only the physical drives currently assigned
 * // to that parity group.
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   // Decode reason editable configuration cannot be created
 *   SA_BYTE cannot_create_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_create_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_create_reason == ...
 *   return 1;
 * }
 *
 * int error = 0;
 *
 * SA_WORD pd_numbers[] = { 2, 4, 9 }; // Note that the removed drives can be part of the same parity group (or not).
 * if (SA_RemoveEditableArrayDataDrives(cda, 1, pd_numbers, 2, kShrinkTypeRemoveParityGroups) != kRemoveEditableArrayDataDriveOK)
 * {
 *    // Decode reason editable Array could not have data drives removed.
 *    SA_BYTE cannot_shrink_array = SA_CanRemoveEditableArrayDataDrives(cda, 1, pd_numbers, 2, kShrinkTypeRemoveParityGroups);
 *    if (cannot_shrink_array == kCanRemoveEditableArrayDataDrivesNotSupported)
 *    {
 *       fprintf(stderr, "Error: Array Shrink not supported\n");
 *    }
 *    // else if (cannot_shrink_array == ...
 *    error = 1;
 * }
 *
 * // Now, we can remove more data drives of the Array or perform other compatible transformations.
 *
 * // Once we've made all our changes we can either:
 * // 1) Use SA_DestroyEditableConfiguration to wipe our new configuration and start over (no changes made).
 * // 2) Use SA_CommitEditableConfiguration to apply our new configuration.
 * //
 * // In either case, a new editable configuration can be created to make more changes.
 * // For this example we're going to commit/apply our changes.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 * // Always remember to cleanup.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 * @section storc_encryption_examples Encryption Examples
 * @see storc_configuration_edit_encryption
 * @subsection storc_encryption_setup_example Encryption Setup Example
 *
 * @code{.c}
 * // Say we currently have an empty configuration and we want to
 * // enable local key manager encryption.
 *
 * int error = 0;
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * // If we cannot create an editable configuration, print a message to the user
 * // with the reason.
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   SA_BYTE cannot_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_reason == ...
 *   error = 1;
 * }
 *
 * // Setup encryption by selecting a key manager (i.e. local/remote).
 * // If the local mode cannot be configured, print a message explaining the reason.
 * if (!error && !SA_ChangeEditableControllerEncryption(cda, kControllerEncryptionLocal))
 * {
 *   SA_BYTE cannot_reason = SA_CanChangeEditableControllerEncryption(cda, kControllerEncryptionLocal);
 *   if (cannot_reason == kCanChangeEditableControllerEncryptionOperationUnsupported)
 *   {
 *     fprintf(stderr, "Error: not supported\n");
 *   }
 *   // else if (cannot_reason == ...
 *   error = 1;
 * }
 *
 * // Setup the crypto officer login password (required).
 * // GetUserPassword() returns user input (the desired login password).
 * // The user input could be pre-validated using SA_CheckEncryptionPassword
 * // (for example, on each keystroke) to help the user provide a valid
 * // password.
 * const char * login_password = GetUserPassword();
 *
 * // If the crypto officer password is not valid or cannot be set for any
 * // reason, print a message to the user.
 * if (!error &&
 *       SA_SetEditableControllerEncryptionPassword(cda,
 *          kControllerEncryptionCryptoOfficer,
 *          login_password) != kSetEditableControllerEncryptionPasswordOK)
 * {
 *   SA_BYTE cannot_reason =
 *     SA_CanSetEditableControllerEncryptionPassword(cda,
 *       kControllerEncryptionCryptoOfficer,
 *       login_password);
 *   if (cannot_reason == kCanSetEditableControllerEncryptionPasswordInvalidPassword)
 *   {
 *     fprintf(stderr, "Error: invalid password\n");
 *   }
 *   // else if (cannot_reason == ...
 *   error = 1;
 * }
 *
 * // Setup the crypto user login password (optional) using the same steps above for
 * // kControllerEncryptionCryptoUser.
 *
 * // Setup the encryption master key (required).
 * // GetEncryptionKey() returns user input (the desired encryption key).
 * // The user input could be pre-validated using SA_CheckEncryptionMasterKey
 * // (for example, on each keystroke) to help the user provide a valid
 * // master key.
 * const char * master_key = GetEncryptionKey();
 *
 * // If the master key is not valid or cannot be set for any
 * // reason, print a message to the user.
 * if (!error &&
 *       SA_SetEditableControllerEncryptionKey(cda,
 *          master_key) != kSetEditableControllerEncryptionKeyOK)
 * {
 *   SA_BYTE cannot_reason = SA_CanSetEditableControllerEncryptionKey(cda, master_key);
 *   if (cannot_reason == kCanSetEditableControllerEncryptionKeyInvalidKey)
 *   {
 *     fprintf(stderr, "Error: invalid key\n");
 *   }
 *   // else if (cannot_reason == ...
 *   error = 1;
 * }
 *
 * // Setup other encryption features (option).
 * // Set other encryption such as firmware lock, allow future plaintext logical drives, etc...
 *
 * // Create new plaintext and/or cipher-text logical drives.
 * // Logical drives created can be created and set to be either plaintext (unencrypted)
 * // or cipher-text (encrypted). Additionally, existing plaintext logical drives
 * // can be queued to be encoded.
 *
 * // Commit the editable configuration to apply changes. Otherwise, we could
 * // simply destroy the editable configuration to drop all staged changes.
 * // If we cannot commit, print a message explaining the reason.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEditableConfiguration(cda);
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 *
 * // Ensure we always clean the editable configuration.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 * @subsection storc_encryption_modify_example Modify Encryption Example
 *
 * @code{.c}
 * // Say we currently have an existing configuration with encryption enabled
 * // and we want to modify the encryption configuration as the crypto
 * // officer user.
 *
 * int error = 0;
 *
 * // Create a new editable configuration to edit (only one can exist at a time).
 * // If we cannot create an editable configuration, print a message to the user
 * // with the reason.
 * if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 * {
 *   SA_BYTE cannot_reason = SA_CanCreateEditableConfiguration(cda);
 *   if (cannot_reason == kCanCreateEditableConfigEditableConfigExists)
 *   {
 *     fprintf(stderr, "Error: An editable configuration already exists\n");
 *   }
 *   // else if (cannot_reason == ...
 *   error = 1;
 * }
 *
 * // Now we can modify the encryption configuration by do any of the following:
 * // 1. Change key manager
 * // 2. Enable the crypto user
 * // 3. Change the crypto officer/user password
 * // 4. Change the master key
 * // etc.
 * // Many of these operations require the crypto officer/user login to commit.
 * // In this case, SA_CanCommitEditableConfiguration(cda) will return
 * // kCanCommitEditableConfigurationPasswordRequired. For this editable configuration,
 * // the user must commit using SA_CommitEncryptedEditableConfiguration().
 *
 * // Get the current login information to commit the editable configuration.
 * // Additionally, SA_VerifyEncryptionPasswordSafe(user, password, password_size) can be used to
 * // validate user login providing a "login" session.
 * SA_BYTE encryption_user = GetEncryptionUserLogin();
 * const char * encryption_password = GetEncryptionUserPassword();
 *
 * // Commit the editable configuration to apply changes. Otherwise, we could
 * // simply destroy the editable configuration to drop all staged changes.
 * // If we cannot commit, print a message explaining the reason. If the commit
 * // fails, return a message on the reason.
 * if (!error)
 * {
 *   SA_BYTE can_commit = SA_CanCommitEncryptedEditableConfigurationSafe(cda, encrypton_user, encryption_password, sizeof(encryption_password));
 *   if (can_commit == kCanCommitEditableConfigurationOK)
 *   {
 *     if (SA_CommitEncryptedEditableConfigurationSafe(cda, encrypton_user, encryption_password, sizeof(encryption_password))
 *           != kCommitEditableConfigurationOK))
 *     {
 *       fprintf(stderr, "Error: Failed to commit changes\n");
 *       error = 1;
 *     }
 *   }
 *   else
 *   {
 *     if (can_commit == kCanCommitEditableConfigurationUnlockedLogicalDrivesExist)
 *     {
 *       fprintf(stderr, "Error: There are unlocked logical drives\n");
 *     }
 *     // else if (can_commit == ...
 *     error = 1;
 *   }
 * }
 *
 * // Ensure we always clean the editable configuration.
 * SA_DestroyEditableConfiguration(cda);
 * @endcode
 *
 * @section storc_diagcore_examples Diagnostic Examples
 * @see diagcore.
 * @code
 * // This is a small snippet of code that demonstrates how the @ref diagcore API
 * // can be used to generate diagnostic reports.
 * // As described in the @ref diagcore module, generating a diagnostic report
 * // involves writing callbacks that write out (usually to a file) information
 * // provided by diagnosticCore and then registering those callbacks with the API.
 *
 * #include "psysc.h"
 * #include "pstorc.h"
 * #include "pdiagc.h"
 *
 * // So for example, if I implemented the following functions to dump, to the screen,
 * // all the devices attached/configured to a controller in an indented text tree:
 *
 * // Global text indentation level. This should be set back to 0 before generating
 * // each report.
 * static int indent = 0;
 *
 * static const char * Diag_PrintDeviceName(PCDA cda, const PDIAG_META_DEVICE device)
 * {
 *   char slot_buffer[9] = { 0 };
 *   SA_GetControllerSlotNumber(cda, slot_buffer, sizeof(slot_buffer));
 *   if (device->device_type == kDiagMetaDeviceTypeController)
 *   {
 *      printf("%*sController Slot %s", indent, slot_buffer);
 *   }
 *   else if (device->device_type == kDiagMetaDeviceTypeArray)
 *   {
 *      printf("%*sArray %u", indent, device->device_number);
 *   }
 *   else if (device->device_type == kDiagMetaDeviceTypeEnclosure)
 *   {
 *      printf("%*sEnclosure %u", indent, device->device_number);
 *   }
 *   else if (device->device_type == kDiagMetaDeviceTypeLogicalDrive)
 *   {
 *      printf("%*sLogical Drive %u", indent, device->device_number);
 *   }
 *   else if (device->device_type == kDiagMetaDeviceTypeRemoteVolume)
 *   {
 *      printf("%*sRemote Volume %u", indent, device->device_number);
 *   }
 *   else if (device->device_type == kDiagMetaDeviceTypePhysicalDrive)
 *   {
 *      printf("%*sPhysical Drive %u", indent, device->device_number);
 *   }
 *   else if (device->device_type == kDiagMetaDeviceTypeSEP)
 *   {
 *      printf("%*sSEP %u", indent, device->device_number);
 *   }
 *   else if (device->device_type == kDiagMetaDeviceTypeExpander)
 *   {
 *      printf("%*sExpander %u", indent, device->device_number);
 *   }
 *   return device_id;
 * }
 *
 * static void Diag_StartDevices(PCDA cda)
 * {
 *   indent += 2;
 * }
 * static void Diag_EndDevices(PCDA cda)
 * {
 *   indent -= 2;
 * }
 *
 * int main()
 * {
 *   InitializeSysCore(SC_CHECKSUM);
 *   InitializeStorageCore(SC_CHECKSUM);
 *   InitializeDiagCore(SC_CHECKSUM);
 *
 *   // Discovery controllers and foreach PCDA/cda...
 *
 *   // Create the diagCore report object
 *   PDIAG_REPORT pdr = DC_CreateDiagnosticReport(cda);
 *
 *   // Now we register our callbacks
 *   DC_SetMetaDeviceStructuresHandler(pdr, Diag_PrintDeviceName, NULL); // Setting an <end> device handler is usually for formats like XML.
 *   DC_SetMetaDeviceStructureHandler(pdr, Diag_StartDevices, Diag_EndDevices);
 *   // Since we dont want any other information, we dont need to register any more callbacks.
 *
 *   indent = 0;
 *   // Start report generation
 *   DC_GenerateDiagnosticReport(pdr);
 *   // The terminal should print out the device/configuration topology such as:
 *   // Controller Slot 3
 *   //   Array 0
 *   //   Array 1
 *   //   Logical Drive 0
 *   //   Logical Drive 1
 *   //   Enclosure 0
 *   //     Physical Drive 0
 *   //     Physical Drive 1
 *   //     Physical Drive 2
 *   //     SEP 378
 *   //     Expander 377
 *
 *   // Re-run report or, when done, cleanup diagnostic report object.
 *   DC_DestroyDiagnosticReport(pdr);
 *   return 0;
 * }
 * @endcode
 *
 * @section storc_command_examples Device Command Examples
 * @see storc_commands
 * @subsection storc_command_scsi_command_example Physical Device SCSI Inquiry Example
 * @see storc_commands_pd
 * @code
 * // In this example we will send a SCSI Inquiry command to all physical devices
 * // attached to a controller.
 *
 * SA_WORD pd_number = kInvalidPDNumber;
 * FOREACH_PHYSICAL_DEVICE(cda, pd_number) // Or use SA_EnumerateControllerPhysicalDevices(cda, pd_number)
 * {
 *   if (SA_CanSendPhysicalDeviceSCSICommand(cda, pd_number))
 *   {
 *     SCSIRequest command;
 *     SA_BYTE cdb[6] = { SCSI_INQUIRY, 0, 0, 0, 0, 0 };
 *     const SA_DWORD MIN_SCSI_INQ_SIZE = 36;
 *     SA_memset(command, 0, sizeof(SCSIRequest));
 *     command->transfer_type     = eDATA_TRANSFER_NONE;
 *     command->cdb               = cdb;
 *     command->cdb_size          = 6;
 *     command.transfer_type      = eDATA_TRANSFER_IN;
 *     command.io_buffer          = SA_malloc(MIN_SCSI_INQ_SIZE);
 *     command.io_buffer_size     = MIN_SCSI_INQ_SIZE;
 *     cdb[3] = (SA_BYTE)((MIN_SCSI_INQ_SIZE >> 8) & 0xFF);
 *     cdb[4] = (SA_BYTE)(MIN_SCSI_INQ_SIZE & 0xFF);
 *     if (SA_SendPhysicalDeviceSCSICommand(cda, pd_number, &command))
 *     {
 *       if (command.scsi_status == 2)
 *       {
 *         fprintf("Error: SCSI command failed with Check Condition\n");
 *       }
 *       else
 *       {
 *         // print/decode SCSI command data in command.io_buffer
 *       }
 *     }
 *     else
 *     {
 *        fprintf(stderr, "Communication error: failed to send command to device %u\n", pd_number);
 *     }
 *     SA_free(command.io_buffer);
 *   }
 *   else
 *   {
 *     fprintf(stderr, "Cannot send SCSI command to device %u\n", pd_number);
 *   }
 * }
 * @endcode
 *
 * @section storc_flash_examples Controller Firmware Update Examples
 * @see storc_flash_ctrl
 *
 * @subsection storc_flash_ofa_example Controller Online Firmware Activation Example
 * @see storc_flash_ctrl
 *
 * Simply put, the process of performing a controller Online Firmware Activation is
 * the act of performing a normal/deferred firmware update and then initiating a
 * device reset such that:
 *   - No data/configuration-metadata is lost
 *   - Performed without the need of a system-level reboot.
 *
 * Terms & Acronyms:
 * - OFA: Online Firmware Activation
 * - OFA Validate: The act of checking the new firmware image to determine if the
 *   new image can be activated immediately without a reboot.
 * - OFA Initiate: Starting the process of preparing the controller to safely reset
 *   in order to activate a newly flashed firmware image as well as actually performing
 *   the reset.
 * - OFA Abort: Stopping/Cancelling a scheduled OFA Initiate process that was started
 *   with a user given delay.
 *
 * In order to perform an Online Firmware Activation, the controller must both
 * support the feature and the required host driver must be loaded:
 * @code
 * SA_UINT32 ofa_support = SA_ControllerSupportsOnlineFirmwareActivation(cda);
 * if ((ofa_support & kControllerOnlineFirmwareActivationSupported) &&
 *       (ofa_support & kControllerOnlineFirmwareActivationEnabled))
 * {
 *    // ...
 * }
 * @endcode
 *
 * If the controller supports online firmware activation, the user must have a
 * new firmware image in order to determine if that image can be activated immediately.
 *
 * The minimum commands to perform an OFA update are:
 * @code
 * // First, perform a normal/deferred controller firmware update.
 * SA_FlashController(cda, image_address, image_size_in_bytes, NULL);
 * SA_FlashController(cda, image_address, image_size_in_bytes, NULL); // Although not required, it is recommended
 *                                                                    // to flash a controller twice in order to load
 *                                                                    // the new image to both the active and backup ROMs.
 *
 * // At this point, the new image is loaded into the active ROM and a reboot
 * // will cause the controller to start using the new image (deferred flash).
 * // However,
 * // to activate the new firmware image immediately without a system restart,
 * // we will 'initiate' the controller-managed/safe reset.
 * // The user can specify a delay (in minutes) to hold off on starting the reset
 * // process until the delay countdown has expired.
 * SA_InitiateControllerOnlineFlash(cda, delay_in_minutes);
 *
 * // If successful, at this point, the controller will prepare for a reset and
 * // may disappear from the system until the reset completes and the controller
 * // comes back online.
 * @endcode
 *
 * In the last example there was not error checking or pre-validation done.
 * Some additional steps could include:
 * @code
 * SA_BOOL OnlineFlashOrBust(PCDA cda, SA_BYTE* image_address, SA_UINT32 image_size)
 * {  // Attempts to perform an online firmware update or display a message to the
 *    // user explaining any errors while making as few changes as possible (such as
 *    // flashing the new image to the controller ROM).
 *
 *    // Before performing an firmware update, check if the new image reports that
 *    // both itself and the controller (given its current state & configuration)
 *    // are capable of performing an Online Firmware Activation.
 *    SA_UINT64 ofa_validate_status = SA_ValidateControllerOnlineFlash(cda, image_address, image_size);
 *    if (ofa_validate_status != kControllerOnlineFirmwareActivationValidateOk)
 *    {
 *       fprintf(stderr, "Sorry, new firmware cannot be activated immediately\n"
 *                       "Reasons: %s\n",
 *               DecodeOFAValidationFailureReasons(ofa_validate_status));
 *       // No firmware changes have been made at this point. Rebooting the system
 *       // will NOT cause the new firmware to be activated.
 *       return kFalse;
 *    }
 *
 *    // Here, the controller has reported that the new image can be activated
 *    // immediately without a reboot.
 *
 *    // Queue the firmware to the controller activate and backup ROM.
 *    if (!SA_FlashController(cda, image_address, image_size, NULL) != kFlashControllerSuccess))
 *    {
 *       fprintf(stderr, "Error: Failed to flash the new firmware image\n");
 *       return kFalse;
 *    }
 *    // Flash the backup ROM image.
 *    (void)SA_FlashController(cda, image_address, image_size, NULL);
 *
 *    // At this point we have committed to the new firmware image. We can either
 *    // reboot the system, or reset the controller.
 *
 *    // We have the option later to re-ask the controller if the new/pending
 *    // ROM image passes OFA validation. This is the same as the
 *    // SA_ValidateControllerOnlineFlash() step above but does not require the
 *    // new image and instead performs the check on the pending ROM image.
 *    ofa_validate_status = SA_ValidateControllerPendingImageForOnlineFlash(cda);
 *    if (ofa_validate_status != kControllerOnlineFirmwareActivationValidateOk)
 *    {
 *       fprintf(stderr, "Uh oh, the new pending firmware is now not capable of online activation."
 *                       "Has something changed (such as status or configuration)?\n"
 *                       "The image will still activate on the next system reboot.\n"
 *                       "Reasons: %s\n",
 *               DecodeOFAValidationFailureReasons(ofa_validate_status));
 *       // The firmware image is still queued for activation on the next system reboot.
 *       return kFalse;
 *    }
 *
 *    // So far so good, the controller has the new image in ROM and still reports
 *    // OFA is a go.
 *
 *    // We will now tell the controller to 'initiate' the update which will
 *    // eventually lead to a controller reset.
 *    // First, lets ask the user if they want to delay the OFA initiate.
 *    SA_UINT16 delay_in_minutes = (SA_UINT16)PromptUserInputNumber(
 *                                   "Controller firmware update ready.\n"
 *                                   "Enter delay (in minutes) before resetting (0-65536): ",
 *                                   0, 65536);
 *
 *    // The controller will still make another validation check so we still
 *    // may get another validation failure here.
 *    // Notice that the OFA validate and OFA initiate return the same possible errors.
 *    ofa_validate_status = SA_InitiateControllerPendingImageForOnlineFlash(cda, delay_in_minutes);
 *    if (ofa_validate_status != kControllerOnlineFirmwareActivationValidateOk)
 *    {
 *       fprintf(stderr, "Error: The new pending firmware is now not capable of online activation."
 *                       "Has something changed (such as status or configuration)?\n"
 *                       "The image will still activate on the next system reboot.\n"
 *                       "Reasons: %s\n",
 *               DecodeOFAValidationFailureReasons(ofa_validate_status));
 *       // The firmware image is still queued for activation on the next system reboot.
 *       return kFalse;
 *    }
 *
 *   // That's it. Any user given countdown has started and when it reaches 0, the controller
 *   // will start the OFA activation/reset process and once back online, will be
 *   // running the new firmware image. However it is possible that the activation will
 *   // fail if a change occurs between the initiate process being queued and the moment
 *   // just before the controller resets.
 *
 *   // If a delay was given, the pending OFA initiate can be aborted using
 *   // SA_AbortControllerOnlineFlash().
 * }
 * @endcode
 *
 * Using something like DisplayOnlineFirmwareActivationStatus() (example below)
 * will display the status of any pending, current, or previously-failed OFA attempt.
 *
 * @code
 * void DisplayOnlineFirmwareActivationStatus(PCDA cda);
 * {  // Display Online Firmware Activation status on any pending, current/running, or
 *    // or previously failed attempt.
 *    SA_CONTROLLER_OFA_STATUS ofa_status;
 *
 *    // Ensure the next call to SA_GetControllerOnlineFirmwareActivationStatus()
 *    // Returns up-to-date information.
 *    SA_UpdateControllerOnlineFirmwareActivationStatus(cda);
 *
 *    // Get OFA status
 *    if (SA_GetControllerOnlineFirmwareActivationStatus(cda, &ofa_status))
 *    {
 *       if (SA_HAS_CONTROLLER_OFA_STARTED(ofa_status.status))
 *       {
 *          printf("Online firmware activation in progress\n"
 *                 "  Status: %s\n"
 *                 "  Progress: %u%%\n",
 *                 DecodeOFAStatus(ofa_status.status),
 *                 ofa_status.progress_percent);
 *       }
 *       else if (SA_IS_CONTROLLER_OFA_SCHEDULED(ofa_status.status))
 *       {
 *          printf("Online firmware activation scheduled: %u/%u minutes remaining",
 *          ofa_status.delay_remaining_minutes,
 *          ofa_status.delay_requested_minutes);
 *       }
 *       else
 *       {
 *          // OFA is not running but the previous attempt may be reported as failed or aborted.
 *          printf("Online firmware activation not running: %s\n",
 *                 DecodeOFAStatus(ofa_status.status));
 *       }
 *    }
 * }
 * @endcode
 */

/** @page features Features
 * @tableofcontents
 * @vendor_copyright
 *
 * @section controller_settings Controller Settings
 * @subsection controller_settings_modules Modules
 * @subsubsection controller_settings_port_mode Controller/Port Mode
 * - @ref storc_features_ctrl_mode
 * - @ref storc_properties_ctrl_mode
 * - @ref storc_properties_ctrl_port_mode
 * - @ref storc_configuration_ctrl_mode
 * - @ref storc_configuration_port_mode
 * @subsubsection controller_settings_boot_volumes Controller BIOS Boot Volumes
 * - @ref storc_properties_ctrl_boot_volume
 * - @ref storc_configuration_edit_ctrl_boot_vol
 * - @ref storc_configuration_direct_os_bootable_drive_count
 * @subsubsection controller_settings_power_mode Controller Power Mode
 * - @ref storc_features_ctrl_power_mode
 * - @ref storc_features_ctrl_survival_mode
 * - @ref storc_properties_get_power_mode
 * - @ref storc_properties_ctrl_survival_mode
 * - @ref storc_configuration_ctrl_power_mode
 * - @ref storc_configuration_edit_survivalmode
 * @subsubsection controller_settings_sam Controller Spare Activation Mode
 * - @ref storc_features_ctrl_predictive_spare_rebuild
 * - @ref storc_properties_spare_activation_mode
 * - @ref storc_configuration_edit_ctrl_sam
 * @subsubsection controller_settings_drive_write_cache Controller Drive Write Cache
 * - @ref storc_features_ctrl_drive_write_cache
 * - @ref storc_properties_ctrl_drive_write_cache
 * - @ref storc_configuration_edit_ctrl_drive_write_cache
 * @subsubsection controller_settings_rebuild_priority Controller Rebuild Priority
 * - @ref storc_properties_ctrl_rebuild_priority
 * - @ref storc_configuration_edit_ctrl_rebuild_priority
 * @subsubsection controller_settings_expand_priority Controller Expand Priority
 * - @ref storc_properties_ctrl_expand_priority
 * - @ref storc_configuration_edit_ctrl_expand_priority
 * @subsubsection controller_settings_ssap Controller Surface Scan Analysis Priority
 * - @ref storc_properties_ctrl_surface_analysis_priority
 * - @ref storc_configuration_edit_ctrl_ssap
 * @subsubsection controller_settings_pssc Controller Parallel Surface Scan Count
 * - @ref storc_features_ctrl_parallel_surface_scan_support
 * - @ref storc_properties_ctrl_parallel_surface_scan_current_count
 * - @ref storc_configuration_edit_ctrl_pssc
 * @subsubsection controller_settings_dpo Controller Degraded Performance Optimization
 * - @ref storc_features_ctrl_dpo_support
 * - @ref storc_properties_ctrl_dpo
 * - @ref storc_configuration_edit_ctrl_dpo
 * @subsubsection controller_settings_irp Controller Inconsistency Repair Policy
 * - @ref storc_features_ctrl_irp_support
 * - @ref storc_properties_ctrl_irp
 * - @ref storc_configuration_edit_ctrl_irp
 * @subsubsection controller_settings_elevator_sort Controller Elevator Sort
 * - @ref storc_properties_ctrl_elevator_sort
 * - @ref storc_configuration_edit_ctrl_elevator_sort
 * @subsubsection controller_settings_queue_depth Controller Queue Depth
 * - @ref storc_features_ctrl_queue_depth_support
 * - @ref storc_properties_ctrl_queue_depth
 * - @ref storc_configuration_edit_queue_depth
 * @subsubsection controller_settings_fls Controller Flexible Latency Scheduler
 * - @ref storc_features_ctrl_fls_supported
 * - @ref storc_properties_ctrl_fls_setting
 * - @ref storc_configuration_edit_fls
 * @subsubsection controller_settings_nbwc Controller No Battery Write Cache
 * - @ref storc_features_ctrl_nbwc_supported
 * - @ref storc_properties_ctrl_nbwc_setting
 * - @ref storc_configuration_edit_nbwc
 * @subsubsection controller_settings_read_cache_percent Controller Read/Write Cache Ratio
 * - @ref storc_properties_ctrl_read_cache_percent
 * - @ref storc_configuration_edit_read_cache_percent
 * @subsubsection controller_settings_mnp_delay Controller M&P Delay
 * - @ref storc_properties_mnp_delay
 * - @ref storc_configuration_edit_mnp_delay
 * @subsubsection controller_settings_wcbt Controller Write Cache Bypass Threshold
 * - @ref storc_features_ctrl_cache_bypass
 * - @ref storc_properties_ctrl_write_cache_bypass_threshold
 * - @ref storc_configuration_edit_ctrl_write_cache_bypass_threshold
 * @subsubsection controller_settings_sanitize_lock Controller Sanitize Lock
 * - @ref storc_features_ctrl_support_sanitize_lock
 * - @ref storc_properties_ctrl_sanitize_lock
 * - @ref storc_configuration_edit_ctrl_sanitize_lock
 * @subsubsection controller_settings_notify_on_any_broadcast Controller Notify On Any Broadcast
 * - @ref storc_features_ctrl_support_notify_any_broadcast
 * - @ref storc_properties_ctrl_notify_on_any_broadcast
 * - @ref storc_configuration_edit_ctrl_notify_any_broadcast
 * @subsubsection controller_settings_time Controller Time
 * - @ref storc_properties_ctrl_time
 * - @ref storc_configuration_ctrl_time
 * @subsubsection controller_settings_oob Controller Out-of-Band Settings
 * - @ref storc_configuration_oob
 * @subsubsection controller_settings_max_phyrate Controller Max Allowed Phy Rate
 * - @ref storc_configuration_port_phyrate
 *
 * @section online_firmware_activation Controller Online Firmware Activation
 * @subsection online_firmware_activation_modules Modules
 * @subsubsection online_firmware_activation_support Controller Online Firmware Activation Support
 * - @ref storc_features_ctrl_support_online_firmware_activation
 * @subsubsection online_firmware_activation_status Controller Online Firmware Activation Status
 * - @ref storc_status_ctrl_online_firmware_activation
 * @subsubsection online_firmware_activation_flash Controller Online Firmware Activation Flash
 * - @ref storc_flash_ctrl
 * @subsection online_firmware_activation_examples Examples
 * - @ref storc_flash_ofa_example
 *
 * @section encryption Encryption
 * @subsection encryption_modules Modules
 * @subsubsection encryption_support Encryption Support
 * - @ref storc_features_ctrl_encryption_information
 * - @ref storc_features_ctrl_support_encryption_key_cache
 * - @ref storc_features_ctrl_support_encryption_persistent_key_cache
 * @subsubsection enccryption_status Encryption Status
 * - @ref storc_encryption_status_info
 * - @ref storc_encryption_key_cache
 * @subsubsection encryption_direct_config Encryption (Direct Operations)
 * - @ref storc_encryption_rekey_ctrl
 * - @ref storc_encryption_clear
 * - @ref storc_encryption_import_foreign_key
 * - @ref storc_encryption_verify_password
 * - @ref storc_encryption_user_capabilities
 * - @ref storc_encryption_rekey_volume
 * - @ref storc_encryption_recover_password
 * - @ref storc_encryption_restore_volatile_key_backup
 * - @ref storc_encryption_rescan_remote_keys
 * - @ref storc_encryption_crypto_erase
 * @subsubsection encryption_edit_config Encryption (Virtual Configuration)
 * - @ref storc_configuration_edit_encryption_set
 * - @ref storc_configuration_edit_encryption_key
 * - @ref storc_configuration_edit_encryption_password
 * - @ref storc_configuration_edit_encryption_allow_plaintext
 * - @ref storc_configuration_edit_encryption_toggle
 * - @ref storc_configuration_edit_encryption_recovery_question_answer
 * - @ref storc_configuration_edit_encryption_boot_password
 * - @ref storc_configuration_edit_encryption_boot_password_toggle
 * - @ref storc_configuration_edit_encryption_ld_set_encryption
 * - @ref storc_configuration_edit_encryption_ld_volatile_key
 * - @ref storc_configuration_edit_encryption_ctrl_fw_lock
 * - @ref storc_configuration_edit_encryption_key_cache
 *
 * @section ld_config Configuration
 * - @ref storc_configuration_ctrl_clear
 * @subsection direct_config Direct Configuration
 * - @ref storc_configuration_direct
 * @subsubsection direct_config_array Array
 * - @ref storc_configuration_array_split_mirror
 * - @ref storc_configuration_array_heal
 * - @ref storc_configuration_array_fill
 * - @ref storc_configuration_array_add_spare_drive
 * - @ref storc_configuration_array_remove_spare_drive
 * - @ref storc_configuration_array_change_spare_type
 * @subsubsection direct_config_pd Physical Drive
 * - @ref storc_configuration_pd_clear_ccm
 * @subsection editable_config Virtual Configuration
 * - @ref storc_configuration_virtual
 * @subsubsection editable_config_ctrl Controller
 * - @ref storc_configuration_edit_ctrl
 * @subsubsection editable_config_array Array
 * - @ref storc_configuration_edit_array
 * @subsubsection editable_config_ld Logical Drive
 * - @ref storc_configuration_edit_ld
 * @subsubsection editable_config_pd Physical Drive
 * - @ref storc_configuration_edit_pd
 *
 * @section license_key License Keys
 * @ref storc_configuration_lk
 * @subsection license_key_module Modules
 * - @ref storc_configuration_lk_install
 * - @ref storc_configuration_lk_remove
 *
 * @section lu_cache LU Cache
 * @subsection lu_cache_modules Modules
 * @subsubsection lu_cache_support LU Cache Support
 * - @ref storc_features_ctrl_lu_cache
 * @subsubsection lu_cache_status LU Cache Status
 * - @ref storc_status_ld_lu_cache_status
 * @subsubsection lu_cache_prop LU Cache Properties
 * - @ref storc_properties_lu_ld_cache_number
 * - @ref storc_properties_lu_cache_info
 * - @ref storc_properties_lu_cache_stats
 * @subsubsection lu_cache_edit LU Cache Configuration
 * - @ref storc_configuration_edit_lu_cache
 * - @ref storc_configuration_edit_ld
 *
 * @section erase Erase
 * @subsection erase_modules Modules
 * @subsubsection erase_support Erase Support
 * - @ref storc_features_ctrl_pd_erase
 * - @ref storc_features_pd_erase_patterns_supported
 * - @ref storc_features_ctrl_support_sanitize_lock
 * @subsubsection erase_settings Erase Settings
 * - @ref storc_properties_ctrl_sanitize_lock
 * - @ref storc_configuration_edit_ctrl_sanitize_lock
 * @subsubsection erase_pd Erase Physical Drive
 * - @ref storc_erase_pd
 * - @ref storc_stop_erase_pd
 * - @ref storc_enable_erased_pd
 *
 * @section uart UART/Events
 * @subsection uart_modules Modules
 * - @ref storc_events
 * - @ref storc_event_codes
 *
 * @section flash Flash/Firmware Update
 * @subsection flash_modules Modules
 * - @ref storc_flash_ctrl
 * - @ref storc_flash_sep
 * - @ref storc_flash_drive
 * - @ref storc_flash_drive_deferred
 *
 * @section commands Special Commands
 * @subsection ctrl_commands Controller Commands
 * - @ref storc_commands_ctrl_reset
 * - @ref storc_commands_ctrl_bkg_act
 * - @ref storc_commands_ctrl_recover_cache_module
 * - @ref storc_commands_ctrl_debug_logs
 * @subsection pdisk_commands Physical Drive/Device Commands
 * - @ref storc_commands_pd_scsi
 * - @ref storc_commands_pd_led
 * - @ref storc_commands_pd_requery
 * - @ref storc_commands_pd_disable
 * @subsection pdevice_commands Physical Device Commands
 * - @ref storc_commands_pdevice_smp
 * @subsection ld_commands Logical Drive Commands
 * - @ref storc_commands_ld_enable_failed_ld
 *
 */

#endif /* PSTORC_H */
