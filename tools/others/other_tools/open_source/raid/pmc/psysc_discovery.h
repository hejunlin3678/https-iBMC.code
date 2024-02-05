/** @file psysc_discovery.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore discovery header file

*/

#ifndef PSYSC_DISCOVERY_H
#define PSYSC_DISCOVERY_H

/** @addtogroup syscore sysCore
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/** @defgroup syscore_device_discovery Device Discovery
 * @brief Create/Iterate devices on system.
 * @details
 * __Example__
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
 * Don't forget to cleanup!
 * @code
 * // Free up all the CDAs.
 * for (handle_index = 0; handle_index < num_good_handles; handle_index++)
 * {
 *     SA_free(test_CDAs[handle_index]);
 * }
 * SC_DestroyDeviceDescriptorList(device_list);
 * @endcode
 * @{
 */

/** Storage extent information structure. */
struct _StorageExtentInformation
{
   SA_BYTE partition_type;               /**< Partition type (Unknown, MBR, or GPT) */
   SA_BYTE partition_guid[16];           /**< Partition GUID (GPT only) */
   SA_BYTE partition_type_guid[16];      /**< Partition type GUID (GPT only) */
   SA_DWORD extent_number;               /**< Extent number (zero-based) */
   SA_QWORD extent_offset;               /**< Extent offset in bytes */
   SA_QWORD extent_size;                 /**< Extent size in bytes */
   char *mount_points;                   /**< Comma-separated list of mount points */
   size_t mount_points_size;             /**< Size of @ref StorageExtentInformation.mount_points in bytes including the terminating null character. */
};
typedef struct _StorageExtentInformation StorageExtentInformation;
typedef struct _StorageExtentInformation *PStorageExtentInformation;

#define kPartitionTypeUnknown 0
#define kPartitionTypeMBR     1
#define kPartitionTypeGPT     2

/** Create a list of controller handles.
 * @post The list should be cleaned after use with @ref SC_DestroyDeviceDescriptorList.
 * @return Allocated array of controller handles.
 */
DeviceDescriptorList SC_CreateControllerDescriptorList(void);

/** Create a list of HBA handles.
 * @post The list should be cleaned after use with @ref SC_DestroyDeviceDescriptorList.
 * @return Allocated array of host bus adapter (non-Smart controllers) handles.
 */
DeviceDescriptorList SC_CreateHostBusAdapterDescriptorList(void);

/** Clean a list of device handles.
 * @internal
 * Does nothing if input is NULL.
 * @endinternal
 * @pre The list has been previously created with @ref SC_CreateControllerDescriptorList or @ref SC_CreateHostBusAdapterDescriptorList.
 * @param[in] descriptor_list  List of device handlers.
 */
void SC_DestroyDeviceDescriptorList(DeviceDescriptorList descriptor_list);

/** Create an iterator to a device handle list.
 * @pre The list has been previously created with @ref SC_CreateControllerDescriptorList or @ref SC_CreateHostBusAdapterDescriptorList.
 * @post The iterator should be cleaned after use with @ref SC_DestroyDeviceListIterator.
 * @param[in] descriptor_list  List of device handlers.
 * @return Created device handler list iterator.
 */
DeviceDescriptorListIterator SC_CreateDeviceListIterator(DeviceDescriptorList descriptor_list);

/** Clean device handle list iterator.
 * @internal
 * Does nothing if input is NULL.
 * @endinternal
 * @pre The iterator has been previously created with @ref SC_CreateDeviceListIterator.
 * @param[in] li  Iterator to a list of device handles.
 */
void SC_DestroyDeviceListIterator(DeviceDescriptorListIterator li);

/** Return @ref kTrue/@ref kFalse if device iterator is(/is not) at end of device handle list.
 * @pre Device handle list has been initialized with @ref SC_CreateControllerDescriptorList.
 * @pre Device handle list iterator has been initialized with @ref SC_CreateDeviceListIterator.
 * @param[in] li Device descriptor iterator.
 */
SA_BOOL SC_IsDeviceListIteratorEnd(DeviceDescriptorListIterator li);

/** Move device handle list iterator to next device handle in list.
 * @pre Device handle list iterator has been initialized with @ref SC_CreateDeviceListIterator.
 * @param[in] li Device descriptor iterator.
 */
void SC_MoveDeviceListIteratorNext(DeviceDescriptorListIterator li);

/** Dereference device handle list iterator to get device handle.
 * @pre Device handle list iterator has been initialized with @ref SC_CreateDeviceListIterator.
 * @pre Device handle list iterator has been check that it is not at the end of the list with @ref SC_IsDeviceListIteratorEnd.
 * @param[in] li Device descriptor iterator pointing to valid descriptor.
 */
DeviceDescriptor SC_GetDeviceListItemAt(DeviceDescriptorListIterator li);

/** Obtain the length of the device descriptor in bytes.
 * @param[in] device_descriptor Valid device descriptor.
 */
size_t SC_GetDeviceDescriptorSize(DeviceDescriptor device_descriptor);

/** Obtain a copy of the device descriptor.
* @param[in] device_descriptor Valid device descriptor.
*/
DeviceDescriptor SC_CopyDeviceDescriptor(DeviceDescriptor device_descriptor);

/** Destroy the device descriptor.
* @param[in] device_descriptor Valid device descriptor.
*/
void SC_DestroyDeviceDescriptor(DeviceDescriptor device_descriptor);

/** Return a handle for a newly created logical drive
 * @param[in] controller_descriptor  Device descriptor/handle to controller.
 * @param[in] ld_number              Logical drive number.
 * @return A device handle that can be used to access the logical drive.
 */
DeviceDescriptor SC_CreateSingleLogicalDeviceDescriptor(DeviceDescriptor controller_descriptor, SA_WORD ld_number);

/** Type for a list of logical drive numbers. */
typedef SA_WORD *LDNumbersArray;
/** Return a list of logical drive handles for a controller.
 * @param[in] controller_descriptor  Device descriptor/handle to controller.
 * @param[in] ld_numbers             List of target logical drive numbers.
 * @param[in] num_ld_devices         Number of logical drive numbers in `ld_numbers`.
 * @return A list of device handles that correspond to each logical drive number in `ld_numbers`.
 */
DeviceDescriptorList SC_CreateLogicalDeviceDescriptorList(DeviceDescriptor controller_descriptor, LDNumbersArray ld_numbers, SA_WORD num_ld_devices);

/** Return a list of all logical drive handles for a controller.
 * @param[in] controller_descriptor  Device descriptor/handle to controller.
 * @return A list of device handles that correspond to each logical drive number in the controller.
 */
DeviceDescriptorList SC_CreateAllLogicalDeviceDescriptorList(DeviceDescriptor controller_descriptor);

/** Type for a list of physical drive numbers. */
typedef SA_WORD *PDNumbersArray;
/** Return a list of physical drive handles for a controller.
 * @param[in] controller_descriptor  Device descriptor/handle to controller.
 * @param[in] pd_numbers             List of target physical drive numbers.
 * @param[in] num_pd_devices         Number of physical drive numbers in `pd_numbers`.
 * @return A list of device handles that correspond to each physical drive number in `pd_numbers`.
 */
DeviceDescriptorList SC_CreatePhysicalDriveDescriptorList(DeviceDescriptor controller_descriptor, PDNumbersArray pd_numbers, SA_WORD num_pd_devices);

/** Type for a list of SEP numbers. */
typedef SA_WORD *SEPNumbersArray;
/** Return a list of SEP handles for a controller.
 * @param[in] controller_descriptor  Device descriptor/handle to controller.
 * @param[in] sep_numbers             List of target SEP numbers.
 * @param[in] num_sep_devices         Number of SEP numbers in `sep_numbers`.
 * @return A list of device handles that correspond to each SEP number in `sep_numbers`.
 */
DeviceDescriptorList SC_CreateSEPDescriptorList(DeviceDescriptor controller_descriptor, SEPNumbersArray sep_numbers, SA_WORD num_sep_devices);

/** Return a list of all physical device handles for a controller.
 * @param[in] controller_descriptor  Device descriptor/handle to controller.
 * @return A list of device handles that correspond to each physical device attached to the controller.
 */
DeviceDescriptorList SC_CreateAllPhysicalDeviceDescriptorList(DeviceDescriptor controller_descriptor);

/** Return a list of all storage volume handles presented by the controller.
 * @param[in] controller_descriptor  Device descriptor/handle to controller.
 * @return A list of device handles that correspond to storage volumes presented by the controller
 */
DeviceDescriptorList SC_CreateStorageVolumeDeviceDescriptorList(DeviceDescriptor controller_descriptor);

/** Return an array of StorageExtentInformation elements.
 * @param[in] volume_handle          Device descriptor/handle to a storage volume.
 * @param[out] storage_extent_array  A pointer to an array of StorageExtentInformation entries
 * @return The number of storage extents added to the array. Note: if no extents are present, no memory is allocated in storage_extent_array
 */
int SC_CreateStorageExtentArray(DeviceDescriptor volume_handle, PStorageExtentInformation **storage_extent_array);

/** Destroy the array created in SC_CreateStorageExtentArray.
 * @param[in] storage_extent_array  A pointer to an array of StorageExtentInformation entries
 */
void SC_DestroyStorageExtentArray(PStorageExtentInformation **storage_extent_array);

/** @} */ /* syscore_device_discovery */

#if defined(__cplusplus)
}    /* extern "C" { */
#endif

/** @} */

#endif /* PSYSC_DISCOVERY_H */
