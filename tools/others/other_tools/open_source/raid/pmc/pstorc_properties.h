/** @file pstorc_properties.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore properties header file
   Defines functions for obtaining device properties

*/

#ifndef PSTORC_PROPERTIES_H
#define PSTORC_PROPERTIES_H

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/************************************//**
 * @defgroup storc_properties Properties
 * @brief
 * @details
 * @{
 ***************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*****************************************//**
 * @defgroup storc_properties_ctrl Controller
 * @brief Read controller properties.
 * @details
 * @see storc_configuration_edit_ctrl
 * @{
 ********************************************/

/** @defgroup storc_properties_ctrl_pcie Controller PCI Properties
 * @brief Get PCI information of the controller.
 * @{ */
/** Get the PCI Express Generation of the controller
 *
 * @param[in]  cda  Controller data area.
 * @return Controller PCI Express Generation (default 0).
 */
SA_BYTE SA_GetPCIeGen(PCDA cda);
/** Get the PCI Express Width of the controller
 *
 * @param[in]  cda  Controller data area.
 * @return Controller PCI Express Generation (default 0).
 */
SA_BYTE SA_GetPCIeWidth(PCDA cda);

/** Get the PCI Express Negotiated Speed (data rate) of the controller in MB/s
 *
 * @param[in]  cda  Controller data area.
 * @return Controller PCI Express Negotiated Speed (data rate) in MB/s (default 0).
 */
SA_DWORD SA_GetPCIeNegotiatedSpeed(PCDA cda);

/** Minimum buffer length (in bytes) required for @ref SA_GetControllerPCILocation. */
#define kMinGetControllerPCILocationBufferLength 17
/** Write the controller PCI location (as read from the controller itself) to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 * @warning The PCI domain is not supported in FreeBSD or ESXi environments.
 *
 * @pre `location_buffer` is at least @ref kMinGetControllerPCILocationBufferLength bytes
 * to contain the PCI location and terminating NULL.
 *
 * @param[in]  cda                 Controller data area.
 * @param[out] location_buffer     Buffer to write the PCI location.
 * @param[in]  buffer_size         Size (in bytes) of `location_buffer`.
 *
 * @return NULL terminating string of PCI location if known, same as `location_buffer`. Format: XXX:XXX:XXX.XXX (Domain:Device:Bus.Function)
 * @return "UNKNOWN" if PCI location is unknown.
 * @return Empty string if given buffer was not large enough to store entire PCI location.
 */
char *SA_GetControllerPCILocation(PCDA cda, char *location_buffer, size_t buffer_size);

/** @name PCI Values
 * @{ */
#define kPCIVendorDeviceID          1     /**< PCI Vendor ID (MSW) + PCI Device ID (LSW) */
#define kPCISubsystemVendorDeviceID 2     /**< PCI Subsystem Vendor ID (MSW) + PCI Subsystem Device ID (LSW) */
#define kPCIMaxLinkRate             3     /**< Max PCIe link rate supported */
#define kPCIMaxLinkWidth            4     /**< Max. PCIe link width supported */
#define kPCIeGeneration             5     /**< PCIe Generation number */
#define kPCIeWidth                  6     /**< Current PCIe link width */
/** @} */ /* PCI Values */

/** Get the requested PCI value
 *
 * @param[in]  cda                Controller data area.
 * @param[in]  pci_value          One of [PCI Values](@ref storc_properties_ctrl_pcie).
 * @return Requested PCI value
 */
SA_DWORD SA_GetControllerPCIValue(PCDA cda, SA_BYTE pci_value);
/** @} */ /* storc_properties_ctrl_pcie */

/** @defgroup storc_properties_ctrl_memory_size Controller Memory Size
 * @brief Get The controller's total memory size.
 * @{ */
/** Get the total controller memory size in MB.
 *
 * @param[in]  cda  Controller data area.
 * @return The total controller memory size in MB.
 */
SA_WORD SA_GetControllerMemorySizeMB(PCDA cda);
/** @} */ /* storc_properties_ctrl_memory_size */

/** @defgroup storc_properties_ctrl_flash_size Controller Flash Size
 * @brief Get the controller's flash size.
 * @{ */
/** Get controller flash size in MiB.
 *
 * @param[in] cda  Controller data area.
 * @return Controller flash size in MiB.
 */
SA_WORD SA_GetControllerFlashSizeMB(PCDA cda);
/** @} */ /* storc_properties_ctrl_flash_size */

/** @defgroup storc_properties_ctrl_mode Controller Mode
 * @brief Get the controller operating mode (RAID/HBA/Mixed/...).
 * @details
 * @see storc_features_ctrl_mode
 * @see storc_properties_ctrl_port_mode
 * @see storc_configuration_ctrl_mode
 * @{ */
/** Get all controller mode information.
 *
 * @param[in] cda  Controller data area.
 * @return Use masks at [Controller Mode Info Returns](@ref storc_properties_ctrl_mode)
 * @return Controller mode capabilities: Supports HBA mode, Mixed mode
 * @return Current controller mode: SmartArray, HBA, Mixed
 * @return Pending controller mode: SmartArray, HBA, Mixed
 *
 * __Examples__
 * @code
 * printf("Controller supports HBA mode %s\n",
 *    (SA_GetControllerModeInfo(cda) & kControllerModeSupportsMask) == kControllerModeSupportsHBAMode
 *       ? "yes" : "no");
 * @endcode
 */
SA_WORD SA_GetControllerModeInfo(PCDA cda);
   /** @name Controller Mode Info Returns
    * @attention These values are subject to change at any time.
    * @outputof SA_GetControllerModeInfo.
    * @{ */
   #define kControllerModeSupportsMask         0x01C0  /**< Mask output of @ref SA_GetControllerModeInfo to get supported controller mode(s). */
   #define kControllerModeSupportsHBAMode      0x0040  /**< Controller supports HBA mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModeSupportsMask. */
   #define kControllerModeSupportsSmartHBAMode 0x0080  /**< Controller supports SmartHBA mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModeSupportsMask. */
   #define kControllerModeSupportsMixedMode    0x0100  /**< Controller supports Mixed mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModeSupportsMask. */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
   #define kControllerModeTypeMask             0x0600  /**< @deprecated No longer maintained. */
   #define kControllerModeTypeSmartArray       0x0200  /**< @deprecated No longer maintained. */
   #define kControllerModeTypeSmartHBA         0x0400  /**< @deprecated No longer maintained. */
   #define kControllerModeTypeSmartUnknown     0x0600  /**< @deprecated No longer maintained. */
#endif
   #define kControllerModeCurrentMask          0x0007  /**< Mask output of @ref SA_GetControllerModeInfo to get current controller mode. */
   #define kControllerModeCurrentUnknown       0x0001  /**< Controller mode set to an unknown mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModeCurrentMask. */
   #define kControllerModeCurrentRAID          0x0002  /**< Controller mode set to RAID mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModeCurrentMask. */
   #define kControllerModeCurrentHBA           0x0003  /**< Controller mode set to HBA mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModeCurrentMask. */
   #define kControllerModeCurrentMixed         0x0004  /**< Controller mode set to Mixed mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModeCurrentMask. */

   #define kControllerModePendingMask          0x0038  /**< Mask output of @ref SA_GetControllerModeInfo to get controller pending mode. */
   #define kControllerModePendingUnknown       0x0008  /**< Controller pending an unknown mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModePendingMask. */
   #define kControllerModePendingRAID          0x0010  /**< Controller pending RAID mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModePendingMask. */
   #define kControllerModePendingHBA           0x0018  /**< Controller pending HBA mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModePendingMask. */
   #define kControllerModePendingMixed         0x0020  /**< Controller pending Mixed mode; @outputof SA_GetControllerModeInfo when masked with @ref kControllerModePendingMask. */
   /** @} */ /* Controller Mode Info Returns */

/** Return @ref kTrue/@ref kFalse if the Controller is (or is not) in RAID mode currently. */
SA_BOOL SA_ControllerCurrentModeIsRAID(PCDA cda);
/** Return @ref kTrue/@ref kFalse if the Controller is (or is not) in HBA mode currently. */
SA_BOOL SA_ControllerCurrentModeIsHBA(PCDA cda);
/** Return @ref kTrue/@ref kFalse if the Controller is (or is not) in Mixed mode currently. */
SA_BOOL SA_ControllerCurrentModeIsMixed(PCDA cda);
/** Return @ref kTrue/@ref kFalse if the Controller is (or is not) in an unknown mode currently. */
SA_BOOL SA_ControllerCurrentModeIsUnknown(PCDA cda);

/** Return @ref kTrue/@ref kFalse if the Controller is (or is not) pending RAID mode. */
SA_BOOL SA_ControllerPendingModeIsRAID(PCDA cda);
/** Return @ref kTrue/@ref kFalse if the Controller is (or is not) pending HBA mode. */
SA_BOOL SA_ControllerPendingModeIsHBA(PCDA cda);
/** Return @ref kTrue/@ref kFalse if the Controller is (or is not) pending Mixed mode. */
SA_BOOL SA_ControllerPendingModeIsMixed(PCDA cda);
/** Return @ref kTrue/@ref kFalse if the Controller is (or is not) pending an unknown mode. */
SA_BOOL SA_ControllerPendingModeIsUnknown(PCDA cda);
/** @} */ /* storc_properties_ctrl_mode */

/** @defgroup storc_properties_ctrl_product Controller Product
 * @brief Get the controller product ID/model name.
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetControllerProductID. */
#define kMinGetControllerProductIDBufferLength 17
/** Write the controller product ID to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `product_id_buffer` is at least @ref kMinGetControllerProductIDBufferLength bytes
 * to contain the product ID and terminating NULL.
 *
 * @param[in]  cda                Controller data area.
 * @param[out] product_id_buffer  Buffer to write product ID.
 * @param[in]  buffer_size        Size (in bytes) of `product_id_buffer`.
 * @return Product ID of the controller, same as `product_id_buffer`.
 * @return Empty string if given buffer was not large enough to store entire product ID.
 */
char *SA_GetControllerProductID(PCDA cda, char *product_id_buffer, size_t buffer_size);
/** @} */ /* storc_properties_ctrl_product */

/** @defgroup storc_properties_ctrl_family Controller Family
 * @brief Get the controller's family ID/name (controller model name prefix).
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetControllerFamilyID. */
#define kMinGetControllerFamilyIDBufferLength 33
/** Write the controller family ID to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `family_id_buffer` is at least @ref kMinGetControllerFamilyIDBufferLength bytes
 * to contain the family ID and terminating NULL.
 *
 * @param[in]  cda                Controller data area.
 * @param[out] family_id_buffer   Buffer to write controller family ID.
 * @param[in]  buffer_size        Size (in bytes) of `family_id_buffer`.
 * @return Family ID of the controller, same as `family_id_buffer`.
 * @return Empty string if given buffer was not large enough to store entire family ID.
 */
char *SA_GetControllerFamilyID(PCDA cda, char *family_id_buffer, size_t buffer_size);
/** @} */ /* storc_properties_ctrl_family */

/** @defgroup storc_properties_ctrl_vendor Controller Vendor
 * @brief Get the controller's vendor ID/name.
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetControllerVendorID. */
#define kMinGetControllerVendorIDBufferLength 9
/** Write the controller vendor ID to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `vendor_id_buffer` is at least @ref kMinGetControllerVendorIDBufferLength bytes
 * to contain the vendor ID and terminating NULL.
 *
 * @param[in]  cda               Controller data area.
 * @param[out] vendor_id_buffer  Buffer to write vendor ID.
 * @param[in]  buffer_size       Size (in bytes) of `vendor_id_buffer`.
 * @return Vendor ID of the controller, same as `vendor_id_buffer`.
 * @return Empty string if given buffer was not large enough to store entire vendor ID.
 */
char *SA_GetControllerVendorID(PCDA cda, char *vendor_id_buffer, size_t buffer_size);
/** @} */ /* storc_properties_ctrl_vendor */

/** @defgroup storc_properties_ctrl_slot Controller Slot
 * @brief Get the controller PCI slot number/name.
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetControllerSlotNumber. */
#define kMinGetControllerSlotNumberBufferLength 8
/** Write the controller slot number (as read from the controller itself) to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `slot_number_buffer` is at least @ref kMinGetControllerSlotNumberBufferLength bytes
 * to contain the slot number and terminating NULL.
 *
 * @param[in]  cda                 Controller data area.
 * @param[out] slot_number_buffer  Buffer to write the slot number.
 * @param[in]  buffer_size         Size (in bytes) of `slot_number_buffer`.
 *
 * @return NULL terminating string of slot if known, same as `slot_number_buffer`.
 * @return "UNKNOWN" if slot is unknown.
 * @return Empty string if given buffer was not large enough to store entire slot number.
 */
char *SA_GetControllerSlotNumber(PCDA cda, char *slot_number_buffer, size_t buffer_size);
/** @} */ /* storc_properties_ctrl_slot */

/** @defgroup storc_properties_ctrl_wwn Controller World Wide Name (WWN)
 * @brief Get the controller's world wide name (WWID).
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetControllerWWN. */
#define kMinGetControllerWWNBufferLength 17
/** Write the controller WWN (in hex digits) to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `wwn_buffer` is at least @ref kMinGetControllerWWNBufferLength bytes
 * to contain the WWN representation and terminating NULL.
 *
 * @param[in]  cda                 Controller data area.
 * @param[out] wwn_buffer          Buffer to write the WWN.
 * @param[in]  buffer_size         Size (in bytes) of `wwn_buffer`.
 *
 * @return NULL terminated string of WWN, same as `wwn_buffer`.
 * @return Empty string if given buffer was not large enough to store entire WWN.
 */
char *SA_GetControllerWWN(PCDA cda, char *wwn_buffer, size_t buffer_size);

/** Return the controller WWN as a 8 byte value.
 *
 * @param[in]  cda                 Controller data area.
 * @return 8 byte value for controller's WWN.
 */
SA_UINT64 SA_GetControllerRawWWN(PCDA cda);
/** @} */ /* storc_properties_ctrl_wwn */

/** @defgroup storc_properties_ctrl_adapter_type Controller Adapter Type
 * @brief Get the controller's type.
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetControllerAdapterType. */
#define kMinGetControllerAdapterTypeBufferLength 17
/** Get the controller adapter type.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `adapter_type_buffer` is at least @ref kMinGetControllerAdapterTypeBufferLength bytes
 * to contain the adapter type and terminating NULL.
 *
 * @param[in]  cda                  Controller data area.
 * @param[out] adapter_type_buffer  Buffer to store the product ID to determine adapter type.
 * @param[in]  buffer_size          Size (in bytes) of `adapter_type_buffer`.
 * @return Populates `adapter_type_buffer` with a NULL-terminated ASCII string of the Controller's adapter type.
 * @return See [Adapter Types Returns] (@ref storc_properties_ctrl_adapter_type).
 * @return Empty string if given buffer was not large enough to store entire adapter type.
 */
SA_WORD SA_GetControllerAdapterType(PCDA cda, char *adapter_type_buffer, size_t buffer_size);
   /** @name Adapter Types Returns
    * @outputof SA_GetControllerAdapterType.
    * @{ */
   #define kAdapterSmartArray        0 /**< Adapter type: SmartArray; @outputof SA_GetControllerAdapterType. */
   #define kAdapterSmartHBA          1 /**< Adapter type: SmartHBA; @outputof SA_GetControllerAdapterType. */
   #define kAdapterDynamicSmartArray 2 /**< Adapter type: Dynamic SmartArray; @outputof SA_GetControllerAdapterType. */
   #define kAdapterArrayController   3 /**< Adapter type: generic array controller; @outputof SA_GetControllerAdapterType. */
   /** @} */ /* Adapter Types Returns */
/** @} */ /* storc_properties_ctrl_adapter_type */

/** @defgroup storc_properties_ctrl_firmware_version Controller Firmware Version
 * @brief Get the controller's firmware version.
 * @{ */
/** Minimum buffer size (in bytes) required for input of @ref storc_properties_ctrl_firmware_version APIs. */
#define kMinGetControllerFirmwareVersionBufferLength 33
/** Write the controller firmware version to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `hardware_version_buffer` is at least @ref kMinGetControllerFirmwareVersionBufferLength bytes
 * to contain the firmware version and terminating NULL.
 *
 * @param[in]  cda                      Controller data area.
 * @param[out] firmware_version_buffer  Buffer to write the firmware version.
 * @param[in]  buffer_size              Size (in bytes) of `hardware_revision_buffer`.
 * @return NULL terminating string of Controller firmware version, same as `firmware_version_buffer`.
 * @return Empty string if given buffer was not large enough to store entire firmware version.
 */
char *SA_GetControllerFirmwareVersion(PCDA cda, char *firmware_version_buffer, size_t buffer_size);

/** Minimum buffer length (in bytes) required for @ref SA_GetControllerPendingFirmwareVersion. */
#define kMinGetControllerPendingFirmwareVersionBufferLength 33
/** Write the controller pending firmware version to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `hardware_version_buffer` is at least @ref kMinGetControllerPendingFirmwareVersionBufferLength bytes
 * to contain the pending firmware version and terminating NULL.
 *
 * @param[in]  cda                      Controller data area.
 * @param[out] firmware_version_buffer  Buffer to write the pending firmware version.
 * @param[in]  buffer_size              Size (in bytes) of `hardware_revision_buffer`.
 * @return NULL terminating string of Controller firmware version, same as `firmware_version_buffer`.
 * @return Empty string if given buffer was not large enough to store entire pending firmware version.
 */
char *SA_GetControllerPendingFirmwareVersion(PCDA cda, char *firmware_version_buffer, size_t buffer_size);

/** Get the controller firmware build number.
 *
 * @param[in]  cda                      Controller data area.
 * @return Controller firmware build number (default 0).
 */
SA_WORD SA_GetControllerFirmwareBuildNumber(PCDA cda);

/** Minimum buffer length (in bytes) required for @ref SA_GetControllerFirmwareCompileTimeStamp. */
#define kMinGetControllerFirmwareCompileTimeStampBufferLength 25
/** Write the controller firmware compile timestamp to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `timestamp_buffer` is at least @ref kMinGetControllerFirmwareCompileTimeStampBufferLength bytes
 * to contain the firmware compile timestamp and terminating NULL.
 *
 * @param[in]  cda               Controller data area.
 * @param[out] timestamp_buffer  Buffer to write the firmware compile timestamp.
 * @param[in]  buffer_size       Size (in bytes) of `hardware_revision_buffer`.
 * @return NULL terminating string of Controller firmware compile timestamp (i.e. "Mar 14 2017 14:04:29"), same as `timestamp_buffer`.
 * @return Empty string if given buffer was not large enough to store entire firmware compile timestamp.
 */
char *SA_GetControllerFirmwareCompileTimeStamp(PCDA cda, char *timestamp_buffer, size_t buffer_size);

/** @name Controller Firmware Comparison Stamp
 * @{ */
/** Return a comparison stamp integer for the controller's current running firmware.
 * This value is an unsigned integer that can be used to compare controller firmware
 * versions. A larger value indicates an upgrade, a lower value indicates a downgrade,
 * and a matching stamps indicate a re-write.
 * @return Comparison stamp of the current running controller firmware version.
 * @return 0 on error of if the controller does not support reporting a comparison stamp.
 */
SA_UINT32 SA_GetControllerFirmwareComparisonStamp(PCDA cda);

/** Return a comparison stamp integer for the controller's pending running firmware.
 * This value is an unsigned integer that can be used to compare controller firmware
 * versions. A larger value indicates an upgrade, a lower value indicates a downgrade,
 * and a matching stamps indicate a re-write.
 * @return Comparison stamp of the pending running controller firmware version.
 * @return 0 on error of if the controller does not support reporting a comparison stamp.
 */
SA_UINT32 SA_GetControllerPendingFirmwareComparisonStamp(PCDA cda);
/** @} */ /* Controller Firmware Comparison Stamp */
/** @} */ /* storc_properties_ctrl_firmware_version */

/** @defgroup storc_properties_ctrl_hardware_revision Controller Hardware Revision
 * @brief Get the controller's hardware revision.
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetControllerHardwareRevision. */
#define kMinGetControllerHardwareRevisionBufferLength 2
/** Write the controller hardware revision to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `hardware_revision_buffer` is at least @ref kMinGetControllerHardwareRevisionBufferLength bytes
 * to contain the hardware revision and terminating NULL.
 *
 * @param[in]  cda                       Controller data area.
 * @param[out] hardware_revision_buffer  Buffer to write the hardware revision.
 * @param[in]  buffer_size               Size (in bytes) of `hardware_revision_buffer`.
 * @return NULL terminating string of Controller hardware revision, same as `hardware_revision_buffer`.
 * @return Empty string if given buffer was not large enough to store entire hardware revision.
 */
char *SA_GetControllerHardwareRevision(PCDA cda, char *hardware_revision_buffer, size_t buffer_size);
/** @} */ /* storc_properties_ctrl_hardware_revision */

/** @defgroup storc_properties_ctrl_cache_size Controller Cache Size
 * @brief Get the controller's cache size.
 * @{ */
/** Get controller I/O read/write cache size available (in MiB).
 *
 * @param[in] cda  Controller data area.
 * @return Amount of cache memory (in MiB) available for read/write cache operations.
 */
SA_WORD SA_GetControllerTotalCacheSize(PCDA cda);

/** Get controller's battery backed write I/O cache size (in MiB).
 *
 * @param[in] cda  Controller data area.
 * @return Amount of battery backed cache memory (in MiB) for write cache operations.
 */
SA_WORD SA_GetControllerBatteryBackedCacheSize(PCDA cda);
/** @} */ /* storc_properties_ctrl_cache_size */

/** @defgroup storc_properties_ctrl_serial_number Controller Serial Number
 * @brief Get the controller's serial number.
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetControllerSerialNumber. */
#define kMinGetControllerSerialNumberBufferLength 33
/** Write the controller serial number to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `serial_number_buffer` is at least @ref kMinGetControllerSerialNumberBufferLength bytes
 * to contain the serial number and terminating NULL.
 *
 * @param[in]  cda                   Controller data area.
 * @param[out] serial_number_buffer  Buffer to write the controller serial number.
 * @param[in]  buffer_size           Size (in bytes) of `serial_number_buffer`.
 * @return NULL terminating string of Controller serial number, same as `serial_number_buffer`.
 * @return Empty string if given buffer was not large enough to store entire serial number.
 */
char *SA_GetControllerSerialNumber(PCDA cda, char *serial_number_buffer, size_t buffer_size);
/** @} */ /* storc_properties_ctrl_serial_number */

/** @defgroup storc_properties_get_power_mode Controller Power Mode
 * @brief Get current controller power mode settings.
 * @details
 * @see storc_features_ctrl_power_mode
 * @see storc_properties_ctrl_survival_mode
 * @see storc_configuration_ctrl_power_mode
 * @{ */
/** Get all controller power mode information.
 *
 * @param[in] cda  Controller data area.
 * @return The controller power mode info (see [Controller Power Mode Info Returns](@ref storc_properties_get_power_mode)).
 * @return Current controller power mode: Invalid, Min, Balanced, Max
 * @return Pending controller power mode: Invalid, Min, Balanced, Max
 * @return Default controller power mode: Invalid, Min, Balanced, Max
 */
SA_WORD SA_GetPowerModeInformation(PCDA cda);
   /** @name Controller Power Mode Info Returns
    * @outputof SA_GetPowerModeInformation
    * @{ */
   #define kPowerModeCurrentMask                0x0003 /**< Mask output of @ref SA_GetPowerModeInformation to get current power mode of the controller. */
   #define kPowerModeCurrentInvalid             0x0000 /**< Controller's __current__ power mode is __invalid__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeCurrentMask. */
   #define kPowerModeCurrentMin                 0x0001 /**< Controller's __current__ power mode is __min__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeCurrentMask. */
   #define kPowerModeCurrentBalanced            0x0002 /**< Controller's __current__ power mode is __balanced__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeCurrentMask. */
   #define kPowerModeCurrentMax                 0x0003 /**< Controller's __current__ power mode is __max__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeCurrentMask. */

   #define kPowerModePendingMask                0x000C /**< Mask output of @ref SA_GetPowerModeInformation to get the pending power mode of the controller. */
   #define kPowerModePendingInvalid             0x0000 /**< Controller's __pending__ power mode is __invalid__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModePendingMask. */
   #define kPowerModePendingMin                 0x0004 /**< Controller's __pending__ power mode is __min__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModePendingMask. */
   #define kPowerModePendingBalanced            0x0008 /**< Controller's __pending__ power mode is __balanced__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModePendingMask. */
   #define kPowerModePendingMax                 0x000C /**< Controller's __pending__ power mode is __max__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModePendingMask. */

   #define kPowerModeWarningMask                0x07F0 /**< Mask output of @ref SA_GetPowerModeInformation to get power mode warning for the controller. */
   #define kPowerModeWarningRebootRequired      0x0010 /**< Controller power mode warning: __Reboot Required__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeWarningMask. */
   #define kPowerModeWarningTemp                0x0020 /**< Controller power mode warning: __Temperature Warning__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeWarningMask. */
   #define kPowerModeWarningNewMode             0x0040 /**< Controller power mode warning: __New Mode__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeWarningMask. */
   #define kPowerModeWarningConfigChange        0x0080 /**< Controller power mode warning: __Configuration Change__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeWarningMask. */
   #define kPowerModeWarningRebootPower         0x0100 /**< Controller power mode warning: __Reboot Power__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeWarningMask. */
   #define kPowerModeWarningRebootPerformance   0x0200 /**< Controller power mode warning: __Reboot Performance__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeWarningMask. */
   #define kPowerModeWarningColdBootRequired    0x0400 /**< Controller power mode warning: __Reboot Performance__; @outputof SA_GetPowerModeInformation when masked with @ref kPowerModeWarningMask. */

   #define kSupportedPowerModesMask             0x3800 /**< Mask output of @ref SA_GetPowerModeInformation to get the controller's supported power modes. */
   #define kPowerModeNoneSupported              0x0000 /**< Controller supports no power mode(s); @outputof SA_GetPowerModeInformation when masked with @ref kSupportedPowerModesMask. */
   #define kPowerModeMinSupported               0x0800 /**< Controller supports __min__ power mode; @outputof SA_GetPowerModeInformation when masked with @ref kSupportedPowerModesMask. */
   #define kPowerModeBalancedSupported          0x1000 /**< Controller supports __balanced__ power mode; @outputof SA_GetPowerModeInformation when masked with @ref kSupportedPowerModesMask. */
   #define kPowerModeMaxSupported               0x2000 /**< Controller supports __max__ power mode; @outputof SA_GetPowerModeInformation when masked with @ref kSupportedPowerModesMask. */

   #define kDefaultPowerModeMask                0xC000 /**< Mask output of @ref SA_GetPowerModeInformation to get the controller's default power mode. */
   #define kPowerModeDefaultInvalid             0x0000 /**< Controller's default power mode is __unknown__; @outputof SA_GetPowerModeInformation when masked with @ref kDefaultPowerModeMask. */
   #define kPowerModeDefaultMin                 0x4000 /**< Controller's default power mode is __min__; @outputof SA_GetPowerModeInformation when masked with @ref kDefaultPowerModeMask. */
   #define kPowerModeDefaultBalanced            0x8000 /**< Controller's default power mode is __balanced__; @outputof SA_GetPowerModeInformation when masked with @ref kDefaultPowerModeMask. */
   #define kPowerModeDefaultMax                 0xC000 /**< Controller's default power mode is __max__; @outputof SA_GetPowerModeInformation when masked with @ref kDefaultPowerModeMask. */
   /** @} */

/** Return @ref kTrue/@ref kFalse if Controller is (or is not) using a _min_ power mode.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_CurrentPowerModeIsMin(PCDA cda);
/** Return @ref kTrue/@ref kFalse if Controller is (or is not) using a _balanced_ power mode.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_CurrentPowerModeIsBalanced(PCDA cda);
/** Return @ref kTrue/@ref kFalse if Controller is (or is not) using a _max_ power mode.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_CurrentPowerModeIsMax(PCDA cda);
/** Return @ref kTrue/@ref kFalse if Controller is (or is not) using a _invalid_ power mode.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_CurrentPowerModeIsInvalid(PCDA cda);

/** Return @ref kTrue/@ref kFalse if Controller's pending power mode is (or is not) _min_.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_PendingPowerModeIsMin(PCDA cda);
/** Return @ref kTrue/@ref kFalse if Controller's pending power mode is (or is not) _balanced_.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_PendingPowerModeIsBalanced(PCDA cda);
/** Return @ref kTrue/@ref kFalse if Controller's pending power mode is (or is not) _max_.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_PendingPowerModeIsMax(PCDA cda);
/** Return @ref kTrue/@ref kFalse if Controller's pending power mode is (or is not) _invalid_.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_PendingPowerModeIsInvalid(PCDA cda);

/** Return @ref kTrue/@ref kFalse if there is (or is not) a power mode [reboot required warning](@ref kPowerModeWarningNewMode).
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_PowerModeWarningRebootRequired(PCDA cda);
/** Return @ref kTrue/@ref kFalse if there is (or is not) a power mode [temperature warning](@ref kPowerModeWarningTemp).
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_PowerModeWarningTemp(PCDA cda);
/** Return @ref kTrue/@ref kFalse if there is (or is not) a power mode [new mode warning](@ref kPowerModeWarningNewMode).
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_PowerModeWarningNewMode(PCDA cda);
/** Return @ref kTrue/@ref kFalse if there is (or is not) a power mode [configuration change warning](@ref kPowerModeWarningConfigChange).
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_PowerModeWarningConfigChange(PCDA cda);
/** Return @ref kTrue/@ref kFalse if there is (or is not) a power mode [reboot power warning](@ref kPowerModeWarningRebootPower).
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_PowerModeWarningRebootPower(PCDA cda);
/** Return @ref kTrue/@ref kFalse if there is (or is not) a power mode [reboot performance warning](@ref kPowerModeWarningRebootPerformance).
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_PowerModeWarningRebootPerformance(PCDA cda);
/** @} */ /* storc_properties_get_power_mode */

/** @defgroup storc_properties_ctrl_survival_mode Controller Survival Power Mode
 * @brief Get current controller survival power mode settings.
 * @details
 * @see storc_features_ctrl_survival_mode
 * @see storc_properties_get_power_mode
 * @see storc_configuration_edit_survivalmode
 * @{ */
/** Get the survival mode status of the controller.
 *
 * @param[in] cda Controller data area.
 * @return Controller's current survival mode setting (see [Survival Power Mode Values](@ref storc_properties_ctrl_survival_mode)).
 */
SA_BYTE SA_GetControllerSurvivalPowerMode(PCDA cda);
   /** @name Survival Power Mode Values
    * @outputof SA_GetControllerSurvivalPowerMode.
    * @{ */
   #define kControllerSurvivalPowerModeDisabled 0 /**< Survival mode disabled; @outputof SA_GetControllerSurvivalPowerMode. */
   #define kControllerSurvivalPowerModeEnabled  1 /**< Survival mode enabled; @outputof SA_GetControllerSurvivalPowerMode. */
   #define kControllerSurvivalPowerModeInvalid  0xFF /**< Survival mode invalid; @outputof SA_GetControllerSurvivalPowerMode. */
    /** @} */
/** @} */ /* storc_properties_ctrl_survival_mode */


/****************************************************//**
 * @defgroup storc_properties_storage_volumes Storage Volumes
 * @brief Get storage volume information
 * @details
 * @{
 *******************************************************/

/** @name Disk Extent Info
 * @{ */
struct _DISK_EXTENT_INFO
{
   SA_BYTE partition_type;          /**< Partition type (see [Disk Extent Partition Type](@ref storc_properties_storage_volumes)) */
   SA_DWORD extent_number;          /**< Extent number (zero-based) */
   SA_QWORD extent_offset;          /**< Extent offset in bytes */
   SA_QWORD extent_size;            /**< Extent size in bytes */
   char *mount_points;              /**< Comma-separated list of mount points */
};
typedef struct _DISK_EXTENT_INFO DISK_EXTENT_INFO;
/** @} */

/** @name Disk Extent Partition Type
 * @outputof SA_GetLogicalDriveStorageVolumeInfo
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 * @{ */
#define kDiskExtentInfoPartitionTypeUnknown 0 /**< Disk extent has an Unknown partition type. */
#define kDiskExtentInfoPartitionTypeMBR     1 /**< Disk extent has a MBR partition type. */
#define kDiskExtentInfoPartitionTypeGPT     2 /**< Disk extent has a GPT partition type. */
/** @} */

/** Minimum length (in bytes) needed for @ref STORAGE_VOLUME_INFO.volume_model. */
#define kMinGetStorageVolumeModelBufferLength 41

/** Minimum length (in bytes) needed for @ref STORAGE_VOLUME_INFO.volume_serial_number. */
#define kMinGetStorageVolumeSerialNumberBufferLength 41

/** Minimum length (in bytes) needed for @ref STORAGE_VOLUME_INFO.volume_unique_id. */
#define kMinGetStorageVolumeUniqueIdBufferLength 41

/** @name Storage Volume Information
 * @outputof SA_GetLogicalDriveStorageVolumeInfo
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 * @{ */
struct _STORAGE_VOLUME_INFO
{
   SA_WORD  volume_information_flags;                                            /**< Volume information flags (see [Storage Volume Information Flags](@ref storc_properties_storage_volumes)). */
   SA_QWORD usable_size_in_bytes;                                                /**< Usable size in bytes. */
   SA_QWORD logical_block_size;                                                  /**< Logical block size. */
   SA_QWORD physical_block_size;                                                 /**< Physical block size. */
   SA_QWORD num_cylinders;                                                       /**< Number of cylinders. */
   SA_QWORD num_heads;                                                           /**< Number of heads. */
   SA_QWORD num_sectors;                                                         /**< Number of sectors. */
   SA_BYTE  volume_bus_id;                                                       /**< Volume bus identifier. */
   SA_BYTE  volume_target_id;                                                    /**< Volume target identifier. */
   SA_BYTE  volume_lun_id;                                                       /**< Volume LUN identifier. */
   SA_WORD  volume_bus_type;                                                     /**< Volume bus type (see [Storage Volume Bus Types](@ref storc_properties_storage_volumes)). */
   SA_WORD  volume_disk_id;                                                      /**< Volume disk identifier. */
   char     volume_model[kMinGetStorageVolumeModelBufferLength];                 /**< Volume model. */
   char     volume_serial_number[kMinGetStorageVolumeSerialNumberBufferLength];  /**< Volume serial number. */
   char     volume_unique_id[kMinGetStorageVolumeUniqueIdBufferLength];          /**< Volume unique identifier. */
   char     *drive_access_name;                                                  /**< Drive OS access name. */
   SA_WORD  disk_extent_count;                                                   /**< Disk extent count. */
   DISK_EXTENT_INFO *disk_extent_info;                                           /**< Disk extent info (see [Disk Extent Info](@ref storc_properties_storage_volumes)). */
};
typedef struct _STORAGE_VOLUME_INFO STORAGE_VOLUME_INFO;
/** @} */

/** @name Storage Volume Information List
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 * @{ */
struct _STORAGE_VOLUME_INFO_LIST
{
   SA_WORD storage_volume_info_list_count;
   STORAGE_VOLUME_INFO **storage_volume_info;
};
typedef struct _STORAGE_VOLUME_INFO_LIST STORAGE_VOLUME_INFO_LIST;
/** @} */
/** @name Storage Volume Information Flags
 * @outputof SA_GetLogicalDriveStorageVolumeInfo
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 * @{ */
#define kStorageVolumeInfoFlagUsableSizeValid          0x0001 /**< Value in @ref STORAGE_VOLUME_INFO.usable_size_in_bytes is valid. */
#define kStorageVolumeInfoFlagLogicalBlockSizeValid    0x0002 /**< Value in @ref STORAGE_VOLUME_INFO.logical_block_size is valid. */
#define kStorageVolumeInfoFlagPhysicalBlockSizeValid   0x0004 /**< Value in @ref STORAGE_VOLUME_INFO.physical_block_size is valid. */
#define kStorageVolumeInfoFlagNumCylindersValid        0x0008 /**< Value in @ref STORAGE_VOLUME_INFO.num_cylinders is valid. */
#define kStorageVolumeInfoFlagNumHeadsValid            0x0010 /**< Value in @ref STORAGE_VOLUME_INFO.num_heads is valid. */
#define kStorageVolumeInfoFlagNumSectorsValid          0x0020 /**< Value in @ref STORAGE_VOLUME_INFO.num_sectors is valid. */
#define kStorageVolumeInfoFlagVolumeBusIdValid         0x0040 /**< Value in @ref STORAGE_VOLUME_INFO.volume_bus_id is valid. */
#define kStorageVolumeInfoFlagVolumeTargetIdValid      0x0080 /**< Value in @ref STORAGE_VOLUME_INFO.volume_target_id is valid. */
#define kStorageVolumeInfoFlagVolumeLUNIdValid         0x0100 /**< Value in @ref STORAGE_VOLUME_INFO.volume_lun_id is valid. */
/** @} */

/** @name Storage Volume Bus Types
 * @outputof SA_GetLogicalDriveStorageVolumeInfo
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 * @{ */
#define kStorageVolumeBusTypeATA    1 /**< Storage volume bus type is ATA. */
#define kStorageVolumeBusTypeSCSI   2 /**< Storage volume bus type is SCSI. */
#define kStorageVolumeBusTypeSCSIPT 3 /**< Storage volume bus type is SCSI PT. */
#define kStorageVolumeBusTypeRAID   4 /**< Storage volume bus type is RAID. */
#define kStorageVolumeBusTypeNVME   5 /**< Storage volume bus type is NVME. */
#define kStorageVolumeBusTypeOther  6 /**< Storage volume bus type is Other. */
/** @} */

/** Get storage volume information for a logical drive.
 * @attention This API can be slower in some cases, use @ref SA_GetLogicalDriveStorageVolumeInfoFast
 * to avoid delays.
 *
 * @post Returned object should always be cleaned using @ref SA_DestroyStorageVolumeInfo.
 *
 * @param[in] cda  Controller data area.
 * @param[in] ld_number  Target logical drive number.
 * @param[out] sv_info  Address to write storage volume info.
 * @return On success return @ref kGetStorageVolumeInfoSuccess and populate storage volume info in `sv_info`.
 * @return On failure return [Get Storage Volume Info Returns](@ref storc_properties_storage_volumes) and set `sv_info` to NULL.
 * __Example__
 * @code{.c}
 * STORAGE_VOLUME_INFO *sv_info = 0;
 * SA_WORD status = SA_GetLogicalDriveStorageVolumeInfo(cda, ld_number, &sv_info);
 * if (status == kLogicalDriveStorageVolumeInfoSuccess)
 * {
 *     SA_WORD i = 0;
 *     SA_WORD disk_extent_count = SA_GetStorageVolumeDiskExtentCount(sv_info);
 *     printf("Storage volume usable size %llu bytes\n", sv_info->usable_size_in_bytes);
 *     printf("Found %u disk extents\n", disk_extent_count);
 *     while (i < disk_extent_count)
 *     {
 *         DISK_EXTENT_INFO *disk_extent_info = SA_GetStorageVolumeDiskExtentInfo(sv_info, i);
 *         if (disk_extent_info)
 *         {
 *            printf("Disk extent mount points %s\n", i, disk_extent_info->mount_points);
 *         }
 *         i++;
 *     }
 * }
 * SA_DestroyStorageVolumeInfo(sv_info);
 * @endcode
 */
SA_BYTE SA_GetLogicalDriveStorageVolumeInfo(PCDA cda, SA_WORD ld_number, STORAGE_VOLUME_INFO **sv_info);

/** @name Get Storage Volume Info Returns
 * @outputof SA_GetLogicalDriveStorageVolumeInfo
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 * @{ */
/** Successfully populated storage volume information.
 * @outputof SA_GetLogicalDriveStorageVolumeInfo.
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo.
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 */
#define kGetStorageVolumeInfoSuccess                  0
/** Invalid argument.
 * @outputof SA_GetLogicalDriveStorageVolumeInfo.
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo.
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 */
#define kGetStorageVolumeInfoInvalidArgument          1
/** Invalid logical drive number.
 * @outputof SA_GetLogicalDriveStorageVolumeInfo.
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 */
#define kGetStorageVolumeInfoInvalidLDNumber          2
/** No storage volumes found.
 * @outputof SA_GetLogicalDriveStorageVolumeInfo.
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo.
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 */
#define kGetStorageVolumeInfoNoStorageVolumeFound     3
/** Invalid storage volume information flags.
 * @outputof SA_GetLogicalDriveStorageVolumeInfo.
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo.
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 */
#define kGetStorageVolumeInfoInvalidVolumeInfoFlags   4
/** Could not allocate memory.
 * @outputof SA_GetLogicalDriveStorageVolumeInfo.
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo.
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 */
#define kGetStorageVolumeInfoAllocationFailure        5
/** Cannot get storage volume information because the controller has an active
 *  or pending online firmware activation operation.
 * @outputof SA_GetLogicalDriveStorageVolumeInfo.
 * @outputof SA_GetLogicalDriveStorageVolumeInfoFast
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 * @see storc_status_ctrl_online_firmware_activation.
 */
#define kGetStorageVolumeInfoControllerOFAActive      6
/** Invalid physical drive number.
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo.
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 */
#define kGetStorageVolumeInfoInvalidPDNumber          7
/** Cannot get storage volume information because the physical drive
 * has a status of @ref kPhysicalDriveStatusFailed.
 * @outputof SA_GetPhysicalDriveStorageVolumeInfo
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoList
 * @outputof SA_GetPhysicalDriveStorageVolumeInfoListFast
 * @see SA_GetPhysicalDriveStatusInfo.
 */
#define kGetStorageVolumeInfoPhysicalDriveIsFailed    8
/** @} */

/** Clean data returned by any of the following APIs:
 * - @ref SA_GetLogicalDriveStorageVolumeInfo
 * - @ref SA_GetLogicalDriveStorageVolumeInfoFast
 * - @ref SA_GetPhysicalDriveStorageVolumeInfo
 * @pre `sv_info` is a valid pointer see [Storage Volume Information](@ref storc_properties_storage_volumes).
 * @post Memory at `sv_info` will not be valid.
 *
 * @param[in] sv_info Storage volume information.
 */
void SA_DestroyStorageVolumeInfo(STORAGE_VOLUME_INFO* sv_info);

/** Clean data returned by any of the following APIs:
 * - @ref SA_GetPhysicalDriveStorageVolumeInfoList
 * - @ref SA_GetPhysicalDriveStorageVolumeInfoListFast
 * @pre `sv_info_list` is a valid pointer see [Storage Volume Information List](@ref storc_properties_storage_volumes).
 * @post Memory at `sv_info_list` will not be valid.
 *
 * @param[in] sv_info_list Storage volume information list.
 */
void SA_DestroyStorageVolumeInfoList(STORAGE_VOLUME_INFO_LIST* sv_info_list);

/** Get number of disk extent in a storage volume info.
 * @pre `storage_volume_info` is a valid pointer see [Storage Volume Information](@ref storc_properties_storage_volumes).
 *
 * @param[in] storage_volume_info Storage volume information.
 * @return Number of disk extent attached to the storage_volume_info.
 * @return 0 if no disk extent or storage_volume_info is NULL.
 */
SA_WORD SA_GetStorageVolumeDiskExtentCount(STORAGE_VOLUME_INFO* storage_volume_info);

/** Return information on storage volume disk extent.
 * @pre `disk_extent_index` is a valid disk extent index (@ref SA_GetStorageVolumeDiskExtentCount).
 * @pre `storage_volume_info` is a valid pointer see [Storage Volume Information](@ref storc_properties_storage_volumes).
 *
 * @param[in] storage_volume_info Storage volume information.
 * @param[in] disk_extent_index Index of the disk extent to retrieve.
 * @return A valid [Disk Extent Info](@ref storc_properties_storage_volumes).
 * @return NULL if an error occurred.
 */
DISK_EXTENT_INFO* SA_GetStorageVolumeDiskExtentInfo(STORAGE_VOLUME_INFO* storage_volume_info, SA_WORD disk_extent_index);

/** Get storage volume information for a physical drive.
 * @attention This API returns only a single storage volume. Use @ref SA_GetPhysicalDriveStorageVolumeInfoList
 *       to get a list of all storage volumes on a physical drive.
 * @post Returned object should always be cleaned using @ref SA_DestroyStorageVolumeInfo.
 *
 * @param[in] cda  Controller data area.
 * @param[in] pd_number  Target physical drive number.
 * @param[out] sv_info  Address to write storage volume info.
 * @return On success return @ref kGetStorageVolumeInfoSuccess and populate storage volume info in 'sv_info'.
 * @return On failure return [Get Storage Volume Info Returns](@ref storc_properties_storage_volumes) and set 'sv_info' to NULL.
 * __Example__
 * @code{.c}
 * STORAGE_VOLUME_INFO *sv_info = 0;
 * SA_WORD status = SA_GetPhysicalDriveStorageVolumeInfo(cda, pd_number, &sv_info);
 * if (status == kPhysicalDriveStorageVolumeInfoSuccess)
 * {
 *     SA_DWORD i = 0;
 *     SA_WORD disk_extent_count = SA_GetStorageVolumeDiskExtentCount(sv_info);
 *     printf("Storage volume usable size %llu bytes\n", sv_info->usable_size_in_bytes);
 *     printf("Found %u disk extents\n", disk_extent_count);
 *     while (i < disk_extent_count)
 *     {
 *         DISK_EXTENT_INFO *disk_extent_info = SA_GetStorageVolumeDiskExtentInfo(sv_info, i);
 *         if (disk_extent_info)
 *         {
 *            printf("Disk extent mount points %s\n", i, disk_extent_info->mount_points);
 *         }
 *         i++;
 *     }
 * }
 * SA_DestroyStorageVolumeInfo(sv_info);
 * @endcode
 */
SA_BYTE SA_GetPhysicalDriveStorageVolumeInfo(PCDA cda, SA_WORD pd_number, STORAGE_VOLUME_INFO **sv_info);

/** Get a list of all storage volume information for a physical drive.
 * @attention This API can be slower in some cases, use @ref SA_GetPhysicalDriveStorageVolumeInfoListFast
 * to avoid delays.
 *
 * @post Returned object should always be cleaned using @ref SA_DestroyStorageVolumeInfoList.
 *
 * @param[in] cda  Controller data area.
 * @param[in] pd_number  Target physical drive number.
 * @param[out] sv_info_list  Address to write storage volume info list.
 * @return On success return @ref kGetStorageVolumeInfoSuccess and populate storage volume info list in 'sv_info_list'.
 * @return On failure return [Get Storage Volume Info Returns](@ref storc_properties_storage_volumes) and set 'sv_info_list' to NULL.
 * __Example__
 * @code{.c}
 * STORAGE_VOLUME_INFO_LIST *sv_info_list = NULL;
 * STORAGE_VOLUME_INFO *sv_info = NULL;
 * SA_WORD list_cnt = 0;
 * SA_WORD status = SA_GetPhysicalDriveStorageVolumeInfoList(cda, pd_number, &sv_info_list);
 * if (status == kGetStorageVolumeInfoSuccess)
 * {
 *    for (list_cnt = 0; list_cnt < SA_GetStorageVolumeCount(sv_info_list); list_cnt++)
 *    {
 *       sv_info = SA_GetStorageVolumeInfo(sv_info_list, list_cnt);
 *       if (status == kPhysicalDriveStorageVolumeInfoSuccess)
 *       {
 *          SA_DWORD i = 0;
 *          SA_WORD disk_extent_count = SA_GetStorageVolumeDiskExtentCount(sv_info_list[i]);
 *          printf("Storage volume usable size %llu bytes\n", sv_info_list[i]->usable_size_in_bytes);
 *          printf("Found %u disk extents\n", disk_extent_count);
 *          while (i < disk_extent_count)
 *          {
 *             DISK_EXTENT_INFO *disk_extent_info = SA_GetStorageVolumeDiskExtentInfo(sv_info_list[i], i);
 *             if (disk_extent_info)
 *             {
 *                printf("Disk extent mount points %s\n", i, disk_extent_info->mount_points);
 *             }
 *             i++;
 *          }
 *       }
 *    }
 *    SA_DestroyStorageVolumeInfoList(sv_info_list);
 * }
 * @endcode
 */
SA_BYTE SA_GetPhysicalDriveStorageVolumeInfoList(PCDA cda, SA_WORD pd_number, STORAGE_VOLUME_INFO_LIST **sv_info_list);

/** Return total number of storage volumes for the specified sv_info_list.
 * @pre Use @ref SA_GetPhysicalDriveStorageVolumeInfoList or @ref SA_GetPhysicalDriveStorageVolumeInfoListFast before calling SA_GetStorageVolumeCount.
 * @param[in] sv_info_list            Pointer to storage volume info list.
 * @return Number of storage volumes in the storage volume info list.
 * @return This value can be used as the upper bound of any storage volume info list index
 */
SA_WORD SA_GetStorageVolumeCount(STORAGE_VOLUME_INFO_LIST *sv_info_list);

/** Return the storage volumes info from sv_info_list for the specified sv_info_index.
 * @pre Use @ref SA_GetPhysicalDriveStorageVolumeInfoList or @ref SA_GetPhysicalDriveStorageVolumeInfoListFast before calling SA_GetStorageVolumeInfo.
 * @param[in] sv_info_list            Pointer to storage volume info list.
 * @param[in] sv_info_index           Index of the storage volume info to access from the sv_info_list.
 *                                    The valid range is between 0 and the return of @ref SA_GetStorageVolumeCount
 * @return A pointer to the STORAGE_VOLUME_INFO struct for the specified index.
 * @return NULL if an error occurred.
 */
STORAGE_VOLUME_INFO* SA_GetStorageVolumeInfo(STORAGE_VOLUME_INFO_LIST *sv_info_list, SA_WORD sv_info_index);

/** @defgroup storc_properties_get_all_storage_volumes Get All Storage Volume Information
 * @brief Get all storage volume information on the controller.
 * __Example__
 * @code{.c}
 *
 * SA_BYTE status;
 * void *ctrl_sv_info = NULL;
 *
 * // Get the controller storage volume info buffer which will be used to get
 * // storage volume information for all physical and logical drives.
 * status = SA_GetControllerAllStorageVolumeInfoBuffer(cda, &ctrl_sv_info);
 *
 * if (status == kGetControllerAllStorageVolumeInfoBufferSuccess)
 * {
 *    SA_WORD pd_number = kInvalidPDNumber;
 *    SA_WORD ld_number = kInvalidLDNumber;
 *
 *    // Get the storage volume list for all physical drives.
 *    FOREACH_PHYSICAL_DRIVE(cda, pd_number)
 *    {
 *       STORAGE_VOLUME_INFO_LIST *sv_info_list;
 *
 *       status = SA_GetPhysicalDriveStorageVolumeInfoListFast(ctrl_sv_info, pd_number, &sv_info_list);
 *
 *       if (status == kGetStorageVolumeInfoSuccess)
 *       {
 *          SA_WORD sv_index;
 *          fprintf(stdout, "Storage Volume Count: %u\n", sv_info_list->storage_volume_info_list_count);
 *          for (sv_index = 0; sv_index < sv_info_list->storage_volume_info_list_count; sv_index++)
 *          {
 *             STORAGE_VOLUME_INFO *sv_info = sv_info_list->storage_volume_info[sv_index];
 *             SA_WORD extent_index;
 *             SA_WORD extent_count = SA_GetStorageVolumeDiskExtentCount(sv_info);
 *
 *             // Print members of STORAGE_VOLUME_INFO here.
 *
 *             for (extent_index = 0; extent_index < extent_count; extent_index++)
 *             {
 *                DISK_EXTENT_INFO *extent_info = SA_GetStorageVolumeDiskExtentInfo(sv_info, extent_index);
 *
 *                // Print members of DISK_EXTENT_INFO here.
 *             }
 *          }
 *       }
 *       else
 *       {
 *          fprintf(stderr, "Failed to get storage volume info for PD #%u, reason=%u\n",
 *             pd_number, status);
 *       }
 *
 *       // Destroy physical drive's storage volume info list.
 *       SA_DestroyStorageVolumeInfoList(sv_info_list);
 *    }
 *
 *    // Get the storage volume info for all logical drives.
 *    FOREACH_LOGICAL_DRIVE(cda, ld_number)
 *    {
 *       STORAGE_VOLUME_INFO *sv_info;
 *
 *       status = SA_GetLogicalDriveStorageVolumeInfoFast(ctrl_sv_info, ld_number, &sv_info);
 *
 *       if (status == kGetStorageVolumeInfoSuccess)
 *       {
 *          SA_WORD extent_index;
 *          SA_WORD extent_count = SA_GetStorageVolumeDiskExtentCount(sv_info);
 *
 *          // Print members of STORAGE_VOLUME_INFO here.
 *
 *          for (extent_index = 0; extent_index < extent_count; extent_index++)
 *          {
 *             DISK_EXTENT_INFO *extent_info = SA_GetStorageVolumeDiskExtentInfo(sv_info, extent_index);
 *
 *             // Print members of DISK_EXTENT_INFO here.
 *          }
 *       }
 *       else
 *       {
 *          fprintf(stderr, "Failed to get storage volume info for LD #%u, reason=%u\n",
 *             ld_number, status);
 *       }
 *
 *       // Destroy logical drive's storage volume info.
 *       SA_DestroyStorageVolumeInfo(sv_info);
 *    }
 * }
 * else
 * {
 *    fprintf(stderr, "Failed get controller's storage volume info buffer, reason=%u\n",
 *       status);
 * }
 *
 * // Destroy the controller storage volume info buffer.
 * SA_DestroyControllerAllStorageVolumeInfoBuffer(ctrl_sv_info);
 *
 * @endcode
 ** @{ */

/** Get controller's storage volume infor buffer.
 * See [Example](@ref storc_properties_get_all_storage_volumes) on how to use this API.
 * @post Once all storage volume information using @ref SA_GetPhysicalDriveStorageVolumeInfoListFast
 * and @ref SA_GetLogicalDriveStorageVolumeInfoFast has been retrieved, use
 * @ref SA_DestroyControllerAllStorageVolumeInfoBuffer to free up @p controller_sv_info.
 *
 * @param[in] cda  Controller data area.
 * @param[out] controller_sv_info Controller's storage volume info buffer pointer which will be used in other APIs.
 * @return See [Get All Storage Volume Info Buffer Returns] (@ref storc_properties_get_all_storage_volumes).
 */
SA_BYTE SA_GetControllerAllStorageVolumeInfoBuffer(PCDA cda, void **controller_sv_info);
   /** @name Get All Storage Volume Info Buffer Returns
    ** @{ */
   /** Successfully retrieved storage volume infor buffer.
    * @outputof SA_GetControllerAllStorageVolumeInfoBuffer.
    */
   #define kGetControllerAllStorageVolumeInfoBufferSuccess           0
   /** Failed because an invalid argument was specified.
    * @outputof SA_GetControllerAllStorageVolumeInfoBuffer.
    */
   #define kGetControllerAllStorageVolumeInfoBufferInvalidArgument   1
   /** Failed because of an out of memory error.
    * @outputof SA_GetControllerAllStorageVolumeInfoBuffer.
    */
   #define kGetControllerAllStorageVolumeInfoBufferOutOfMemory       2
   /** @} */ /* Get All Storage Volume Info Buffer Returns */

/** Get a list of all storage volume information for a physical drive (Fast).
 * See [Example](@ref storc_properties_get_all_storage_volumes) on how to use this API.
 * @post Returned object should always be cleaned using @ref SA_DestroyStorageVolumeInfoList.
 *
 * @param[in] controller_sv_info  Controller's storage volume info buffer pointer retrieved using @ref SA_GetControllerAllStorageVolumeInfoBuffer.
 * @param[in] pd_number  Target physical drive number.
 * @param[out] sv_info_list  Address to write storage volume info list.
 * @return On success return @ref kGetStorageVolumeInfoSuccess and populate storage volume info list in 'sv_info_list'.
 * @return On failure return [Get Storage Volume Info Returns](@ref storc_properties_storage_volumes) and set 'sv_info_list' to NULL.
 */
SA_BYTE SA_GetPhysicalDriveStorageVolumeInfoListFast(void *controller_sv_info, SA_WORD pd_number, STORAGE_VOLUME_INFO_LIST **sv_info_list);

/** Get storage volume information for a logical drive (Fast).
 * See [Example](@ref storc_properties_get_all_storage_volumes) on how to use this API.
 * @post Returned object should always be cleaned using @ref SA_DestroyStorageVolumeInfo.
 *
 * @param[in] controller_sv_info  Controller's storage volume info buffer pointer retrieved using @ref SA_GetControllerAllStorageVolumeInfoBuffer.
 * @param[in] ld_number  Target logical drive number.
 * @param[out] sv_info  Address to write storage volume info.
 * @return On success return @ref kGetStorageVolumeInfoSuccess and populate storage volume info in `sv_info`.
 * @return On failure return [Get Storage Volume Info Returns](@ref storc_properties_storage_volumes) and set `sv_info` to NULL.
 */
SA_BYTE SA_GetLogicalDriveStorageVolumeInfoFast(void *controller_sv_info, SA_WORD ld_number, STORAGE_VOLUME_INFO **sv_info);

/** Destroy the controller's storage volume info buffer.
 * See [Example](@ref storc_properties_get_all_storage_volumes) on how to use this API.
 * @post Memory at @p controller_sv_info will no longer be valid.
 *
 * @param[in] controller_sv_info Controller's storage volume info buffer pointer retrieved using @ref SA_GetControllerAllStorageVolumeInfoBuffer.
 */
void SA_DestroyControllerAllStorageVolumeInfoBuffer(void *controller_sv_info);
/** @} */ /* storc_properties_get_all_storage_volumes */

/** @} */ /* storc_properties_storage_volumes */

/** @defgroup storc_properties_ctrl_write_cache_bypass_threshold Controller Write Cache Bypass Threshold
 * @brief Get the controller's write cache bypass threshold.
 * @details
 * @see storc_features_ctrl_cache_bypass
 * @see storc_configuration_edit_ctrl_write_cache_bypass_threshold
 * @{ */
/** Get the write cache bypass threshold of the controller.
 *
 * @param[in] cda  Controller data area.
 * @return Controller write cache bypass threshold [1..65] (in cache lines).
 * @return 0, if an error occured.
 * @return 0, if invalid for current configuration.
 */
SA_DWORD SA_GetControllerWCBT(PCDA cda);
/** @} */ /* storc_properties_ctrl_write_cache_bypass_threshold */

/** @defgroup storc_properties_ctrl_expand_priority Controller Expand Priority
 * @brief Get the controller's expand priority.
 * @details
 * @see storc_configuration_edit_ctrl_expand_priority
 * @{ */
/** Get the expand priority of the controller.
 *
 * @param[in] cda  Controller data area.
 * @return Controller expand priority (see [Controller Expand Priorities](@ref storc_properties_ctrl_expand_priority)).
 */
SA_BYTE SA_GetControllerExpandPriority(PCDA cda);
   /** @name Controller Expand Priorities
    * @outputof SA_GetControllerExpandPriority.
    * @{ */
   #define kControllerExpandPriorityLow      0 /**< Expand Priority Low; @outputof SA_GetControllerExpandPriority. */
   #define kControllerExpandPriorityMedium   1 /**< Expand Priority Medium; @outputof SA_GetControllerExpandPriority. */
   #define kControllerExpandPriorityHigh     2 /**< Expand Priority High; @outputof SA_GetControllerExpandPriority. */
   #define kControllerExpandPriorityInvalid  0xFF /**< Expand Priority Invalid; @outputof SA_GetControllerExpandPriority. */
   /** @} */

/** Get the transformation rate percent of the controller.
   *
   * @param[in] cda  Controller data area.
   * @return Controller transformation rate percent
   */
SA_BYTE SA_GetControllerTransformationRatePercent(PCDA cda);
   /** @name Controller Transformatoin Rate Percent
    * @{ */
   #define kControllerTransformationRatePercentInvalid 0xFF
   /** @} */
/** @} */ /* storc_properties_ctrl_expand_priority */

/** @defgroup storc_properties_ctrl_rebuild_priority Controller Rebuild Priority
 * @brief Get the controller's rebuild priority.
 * @details
 * @see storc_configuration_edit_ctrl_rebuild_priority
 * @{ */
/** Get the rebuild priority of the controller.
 *
 * @param[in] cda  Controller data area.
 * @return Controller rebuild priority (see [Controller Rebuild Priorities](@ref storc_properties_ctrl_rebuild_priority)).
 */
SA_BYTE SA_GetControllerRebuildPriority(PCDA cda);
   /** @name Controller Rebuild Priorities
    * @outputof SA_GetControllerRebuildPriority.
    * @{ */
   #define kControllerRebuildPriorityLow               0 /**< Rebuild Priority Low; @outputof SA_GetControllerRebuildPriority. */
   #define kControllerRebuildPriorityMedium            1 /**< Rebuild Priority Medium; @outputof SA_GetControllerRebuildPriority. */
   #define kControllerRebuildPriorityMediumHigh        2 /**< Rebuild Priority Rapid Medium High; @outputof SA_GetControllerRebuildPriority. */
   #define kControllerRebuildPriorityHigh              3 /**< Rebuild Priority High; @outputof SA_GetControllerRebuildPriority. */
   #define kControllerRebuildPriorityInvalid           0xFF /**< Rebuild Priority Invalid; @outputof SA_GetControllerRebuildPriority. */
   /** @} */

/** Get the rebuild rate of the controller as a percentage.
 *
 * @param[in] cda  Controller data area.
 * @return Controller rebuild rate as a percentage.
 * @return @ref kControllerRebuildRatePercentageInvalid if this field is invalid.
 */
SA_BYTE SA_GetControllerRebuildRatePercent(PCDA cda);
   /** @name Controller Rebuild Rate Percentage
    * @{ */
   #define kControllerRebuildRatePercentageInvalid    0xFF /**< Rebuild Rate Percentage Invalid; @outputof SA_GetControllerRebuildRatePercent. */
   /** @} */
/** @} */ /* storc_properties_ctrl_rebuild_priority */

/** @defgroup storc_properties_ctrl_surface_analysis_priority Controller Surface Analysis Priority
 * @brief Get the controller's surface analysis priority setting.
 * @details
 * @see storc_features_ctrl_parallel_surface_scan_support
 * @see storc_properties_ctrl_parallel_surface_scan_current_count
 * @see storc_configuration_edit_ctrl_ssap
 * @{ */
/** Get the surface analysis priority of the controller.
 *
 * @param[in] cda  Controller data area.
 * @return Surface analysis delay: Total amount of controller idle time (in tenths-of-seconds)
 * that must elapse before background surface analysis activity begins on ANY volume.
 * @return Use [Helper Functions for Surface Analysis Priority] to decode the output.
 */
SA_WORD SA_GetControllerSSAPriority(PCDA cda);
   /** @name Helper Functions for Surface Analysis Priority
    * @{ */
   /** Decode output of @ref SA_GetControllerSSAPriority to determine if surface scan analysis priority is disabled. */
   #define IS_SURFACE_SCAN_PRIORITY_DISABLED(info_value) (info_value == 0)
   /** Decode output of @ref SA_GetControllerSSAPriority to determine if surface scan analysis priority is high. */
   #define IS_SURFACE_SCAN_PRIORITY_HIGH(info_value) (info_value == 1)
   /** Decode output of @ref SA_GetControllerSSAPriority to determine if surface scan analysis priority is idle. */
   #define IS_SURFACE_SCAN_PRIORITY_IDLE(info_value) (info_value >= 2 && info_value <= 300)
   /** Decode output of @ref SA_GetControllerSSAPriority to determine if surface scan analysis priority is invalid. */
   #define IS_SURFACE_SCAN_PRIORITY_INVALID(info_value) (info_value > 300)
   /** @} */

/** Get the controller's surface analysis priority as a percentage.
 *
 * @param[in] cda  Controller data area.
 * @return Controller surface analysis priority as a percentage.
 * @return @ref kControllerSSARatePercentInvalid if this field is invalid.
 */
SA_BYTE SA_GetControllerSSARatePercent(PCDA cda);
   /** @name Controller Surface Analysis Priority Percentage
    * @{ */
   #define kControllerSSARatePercentInvalid    0xFF /**< Surface Analysis Priority Percentage Invalid; @outputof SA_GetControllerSSARatePercent. */
   /** @} */
/** @} */ /* storc_properties_ctrl_surface_analysis_priority */

/** @defgroup storc_properties_ctrl_parallel_surface_scan_current_count Controller Parallel Surface Scan Current Count
 * @brief Get the controller's parallel surface scan count setting.
 * @details
 * @see storc_properties_ctrl_surface_analysis_priority
 * @see storc_configuration_edit_ctrl_pssc
 * @{ */
/** Get the number of disks the controller is scanning in parallel.
 *
 * @param[in] cda  Controller data area.
 * @return The number of disks the controller is scanning in parallel.
 * @return @ref kControllerPSSCountInvalid if this field is invalid.
 */
SA_BYTE SA_GetControllerPSSCount(PCDA cda);
   /** @name Controller Parallel Surface Scan Counts
    * @{ */
   #define kControllerPSSCountInvalid  0
   /** @} */
/** @} */ /* storc_properties_ctrl_parallel_surface_scan_current_count */

/** @defgroup storc_properties_ctrl_drive_write_cache Controller Drive Write Cache
 * @brief Get controller's drive write cache setting.
 * @details
 * @see storc_features_ctrl_drive_write_cache
 * @see storc_configuration_edit_ctrl_drive_write_cache
 * @{ */

/** @name Drive Write Cache Drive Types
 * @{
 */
#define kDriveWriteCacheDriveTypeSASHDD   0x00 /**< Drive write cache for any SAS HDD drives. */
#define kDriveWriteCacheDriveTypeSASSSD   0x01 /**< Drive write cache for any SAS SSD drives. */
#define kDriveWriteCacheDriveTypeSATAHDD  0x02 /**< Drive write cache for any SATA HDD drives. */
#define kDriveWriteCacheDriveTypeSATASSD  0x03 /**< Drive write cache for any SATA SSD drives. */
#define kDriveWriteCacheDriveTypeNVMEHDD  0x04 /**< Drive write cache for any NVME HDD drives. */
#define kDriveWriteCacheDriveTypeNVMESSD  0x05 /**< Drive write cache for any NVME SSD drives. */
/** @} */ /* Drive Write Cache Drive Types */

/** @name Drive Write Cache Usage Types
 * @{
 */
/** Drive write cache for configured drive.
 * Allowed only if the controller supports RAID or Mixed mode.
 */
#define kDriveWriteCacheUsageTypeConfigured    0x00
/** Drive write cache for unassigned drive attached to port in Mixed mode.
 * Allowed only if the controller supports Mixed mode.
 */
#define kDriveWriteCacheUsageTypeUnconfigured  0x01
/** Drive write cache for drive attached to port in HBA mode.
 * Allowed only if the controller supports HBA mode.
 */
#define kDriveWriteCacheUsageTypeHBA           0x02
/** @} */ /* Drive Write Cache Usage Types */

/** @name Drive Write Cache Policies
 * @{
 */
/** Parameter to have a default or no policy for physical drive write cache.
 *
 * For drives with usage type @ref kDriveWriteCacheUsageTypeConfigured, this
 * policy means that the controller firmware will enable and disable the drive
 * write cache on each physical drive to maximize efficiency and data protection.
 *
 * For drives with usage type @ref kDriveWriteCacheUsageTypeUnconfigured or
 * @ref kDriveWriteCacheUsageTypeHBA, this policy means that the controller firmware
 * will not modify the drive write cache of the physical drive configured via
 * other tools or utilities.
 *
 * @attention For usage types @ref kDriveWriteCacheUsageTypeUnconfigured and
 * @ref kDriveWriteCacheUsageTypeHBA, the @ref kControllerDriveWriteCacheDefault
 * and @ref kControllerDriveWriteCacheUnchanged policies are functionally
 * identical.
 *
 * @outputof SA_GetControllerDriveWriteCachePolicy.
 */
#define kControllerDriveWriteCacheDefault   0
/** Parameter to enable the physical drive write cache.
 *
 * For all usage types, this policy means that the controller will enable the
 * drive write cache on the selected physical drives.
 *
 * @outputof SA_GetControllerDriveWriteCachePolicy.
 */
#define kControllerDriveWriteCacheEnabled   1
/** Parameter to disable the physical drive write cache.
 *
 * For all usage types, this policy means that the controller will disable the
 * drive write cache on the selected physical drives.
 *
 * @outputof SA_GetControllerDriveWriteCachePolicy.
 */
#define kControllerDriveWriteCacheDisabled  2
/** Parameter to leave the physical drive write cache unchanged.
 *
 * For all usage types, this policy means that the controller will not modify
 * the drive write cache on the selected physical drives.
 *
 * @attention For usage types @ref kDriveWriteCacheUsageTypeUnconfigured and
 * @ref kDriveWriteCacheUsageTypeHBA, the @ref kControllerDriveWriteCacheDefault
 * and @ref kControllerDriveWriteCacheUnchanged policies are functionally
 * identical.
 *
 * @outputof SA_GetControllerDriveWriteCachePolicy.
 */
#define kControllerDriveWriteCacheUnchanged 3
/** No configuration: physical drive write cache setting is invalid/unsupported.
 * @outputof SA_GetControllerDriveWriteCachePolicy.
 */
#define kControllerDriveWriteCacheInvalid   0xFF
/** @} */ /* Drive Write Cache Policies */

/** Get the state of the write cache of physical drives.
 * @deprecated Use @ref SA_GetControllerDriveWriteCachePolicy instead.
 * @param[in] cda  Controller data area.
 * @return Controller drive write cache setting (see [Drive Write Cache Policies](@ref storc_properties_ctrl_drive_write_cache)).
 */
SA_BYTE SA_GetControllerDriveWriteCache(PCDA cda);

/** Get the controller's drive write cache policy for a given drive type/usage.
 * @param[in] cda  Controller data area.
 * @param[in] drive_type [Drive write cache drive type](@ref storc_properties_ctrl_drive_write_cache).
 * @param[in] usage_type [Drive write cache usage type](@ref storc_properties_ctrl_drive_write_cache).
 * @return Controller drive write cache setting for the given drive type/usage (see [Drive Write Cache Policies](@ref storc_properties_ctrl_drive_write_cache)).
 */
SA_BYTE SA_GetControllerDriveWriteCachePolicy(PCDA cda, SA_BYTE drive_type, SA_BYTE usage_type);
/** @} */ /* storc_properties_ctrl_drive_write_cache */

/** @defgroup storc_properties_ctrl_dpo Controller Degraded Performance Optimization
 * @brief Get the controller's degraded performance optimization setting.
 * @details
 * @see storc_features_ctrl_dpo_support
 * @see storc_configuration_edit_ctrl_dpo
 * @{ */
/** Get degraded performance optimization setting.
 *
 * @param[in] cda  Controller data area.
 * @return Degraded performance optimization setting (see [Controller Degraded Performance Optimization Settings](@ref storc_properties_ctrl_dpo)).
 */
SA_BYTE SA_GetControllerDPO(PCDA cda);
   /** @name Degraded Performance Optimization Settings
    * @outputof SA_GetControllerDPO.
    * @{ */
   #define kControllerDPOEnabled  0 /**< DPO is enabled.; @outputof SA_GetControllerDPO. */
   #define kControllerDPODisabled 1 /**< DPO is disabled.; @outputof SA_GetControllerDPO. */
   #define kControllerDPOInvalid  0xFF /**< No configuration: DPO setting is invalid.; @outputof SA_GetControllerDPO. */
   /** @} */
/** @} */ /* storc_properties_ctrl_dpo */

/** @defgroup storc_properties_ctrl_elevator_sort Controller Elevator Sort
 * @brief Get the controller's elevator sort toggle.
 * @details
 * @see storc_configuration_edit_ctrl_elevator_sort
 * @{ */
/** Get elevator sort setting.
 *
 * @param[in] cda  Controller data area.
 * @return Elevator sort setting (see [Controller Elevator Sort Settings](@ref storc_properties_ctrl_elevator_sort)).
 */
SA_BYTE SA_GetControllerElevatorSort(PCDA cda);
   /** @name Controller Elevator Sort Settings
    * @outputof SA_GetControllerElevatorSort.
    * @{ */
   #define kControllerElevatorSortEnabled  0 /**< Elevator sort is enabled.; @outputof SA_GetControllerElevatorSort. */
   #define kControllerElevatorSortDisabled 1 /**< Elevator sort is disabled.; @outputof SA_GetControllerElevatorSort. */
   #define kControllerElevatorSortInvalid  0xFF /**< No configuration: Elevator sort is invalid.; @outputof SA_GetControllerElevatorSort. */
   /** @} */
/** @} */ /* storc_properties_ctrl_elevator_sort */

/** @defgroup storc_properties_ctrl_irp Controller Inconsistency Repair Policy
 * @brief Get the controller's inconsistency repair policy.
 * @details
 * @see storc_features_ctrl_irp_support
 * @see storc_configuration_edit_ctrl_irp
 * @{ */
/** Get inconsistency repair policy setting.
 *
 * @param[in] cda  Controller data area.
 * @return Inconsistency repair policy setting (see [Controller Inconsistency Repair Policy Settings](@ref storc_properties_ctrl_irp)).
 */
SA_BYTE SA_GetControllerIRP(PCDA cda);
   /** @name Controller Inconsistency Repair Policy Settings
    * @outputof SA_GetControllerIRP.
    * @{ */
   #define kControllerIRPEnabled  0 /**< Inconsistency repair policy is enabled.; @outputof SA_GetControllerIRP. */
   #define kControllerIRPDisabled 1 /**< Inconsistency repair policy is disabled.; @outputof SA_GetControllerIRP. */
   #define kControllerIRPInvalid  0xFF /**< No configuration: Inconsistency repair policy is invalid.; @outputof SA_GetControllerIRP. */
   /** @} */
/** @} */ /* storc_properties_ctrl_irp */

/** @defgroup storc_properties_ctrl_inconsistency_event_notification_setting Controller Surface Analysis Inconsistency Event Notification Setting
 * @brief Get a controller's surface analysis inconsistency event notification setting.
 * @details
 * @see storc_features_inconsistency_event_notification_support
 * @see storc_configuration_edit_inconsistency_event_notification
 * @{ */
/** Get the controller inconsistency event notification setting.
 *
 * @param[in] cda Controller data area.
 * @return Inconsistency Event Notification setting value (see [Controller Inconsistency Event Notification Setting](@ref storc_properties_ctrl_inconsistency_event_notification_setting)).
 */
SA_BYTE SA_GetControllerInconsistencyEventNotification(PCDA cda);
   /** @name Controller Inconsistency Event Notification Setting
    * @outputof SA_GetControllerInconsistencyEventNotification.
    * @{ */
   #define kControllerInconsistencyEventNotificationDisabled         0
   #define kControllerInconsistencyEventNotificationEnabled          1
   #define kControllerInconsistencyEventNotificationNotSupported     0xFF
   /** @} */
/** @} */ /* storc_properties_ctrl_inconsistency_event_notification_setting */

/** @defgroup storc_properties_ctrl_queue_depth Controller Queue Depth
 * @brief Get a controller's queue depth setting.
 * @details
 * @see storc_features_ctrl_queue_depth_support
 * @see storc_configuration_edit_queue_depth
 * @{ */
/** Get the controller queue depth.
 *
 * @param[in] cda  Controller data area.
 * @return Queue depth value (see [Controller Queue Depths](@ref storc_properties_ctrl_queue_depth)).
 */
SA_BYTE SA_GetControllerQueueDepth(PCDA cda);
   /** @name Controller Queue Depths
    * @outputof SA_GetControllerQueueDepth.
    * @{ */
   #define kControllerQueueDepthAutomatic  0 /**< Automatic queue depth; @outputof SA_GetControllerQueueDepth. */
   #define kControllerQueueDepth2          1 /**< Queue depth of 2; @outputof SA_GetControllerQueueDepth. */
   #define kControllerQueueDepth4          2 /**< Queue depth of 4; @outputof SA_GetControllerQueueDepth. */
   #define kControllerQueueDepth8          3 /**< Queue depth of 8; @outputof SA_GetControllerQueueDepth. */
   #define kControllerQueueDepth16         4 /**< Queue depth of 16; @outputof SA_GetControllerQueueDepth. */
   #define kControllerQueueDepth32         5 /**< Queue depth of 32; @outputof SA_GetControllerQueueDepth. */
   #define kControllerQueueDepthInvalid    0xFF /** No configuration: queue depth is invalid; @outputof SA_GetControllerQueueDepth. */
   /** @} */
/** @} */ /* storc_properties_ctrl_queue_depth */

/** @defgroup storc_properties_ctrl_fls_setting Controller FLS Setting
 * @brief Get a controller's flexible latency scheduler setting.
 * @details
 * @see storc_features_ctrl_fls_supported
 * @see storc_configuration_edit_fls
 * @{ */
/** Get the controller flexible latency scheduler setting.
 *
 * @details
 * Flexible Latency Scheduler (FLS) is a controller option where the controller can re-prioritize I/O
 * requests to prevent some requests to HDDs from timing out. Under normal operation (when FLS is
 * disabled, or in controllers that don't support FLS), the controller will sort incoming requests in
 * order to minimize the amount of travel for the HDD's read heads (A.K.A. elevator sorting). This
 * strategy works well for workloads that access sequential data, or workloads that require multiple
 * requests from localized sectors in the drive. For highly random workloads, such as transaction
 * processing, some requests will end up  on the wrong side of the disk platter and, due to their high
 * latency, will be marked as timed out. When FLS is enabled, it will detect these high-latency requests
 * and apply a cutoff value, after which it will suspend elevator sorting and service the request right
 * away.
 * @param[in] cda    Controller data area.
 * @return FLS setting value (see [Controller FLS Settings](@ref storc_properties_ctrl_fls_setting)).
 */
SA_BYTE SA_GetControllerFLS(PCDA cda);
   /** @name Controller FLS Settings
    * @outputof SA_GetControllerFLS.
    * @{ */
   #define kControllerFLSDefault        0 /**< The controller's default latency cap; @outputof SA_GetControllerFLS. */
   #define kControllerFLSLow250         1 /**< Low latency cap of 250ms; @outputof SA_GetControllerFLS. */
   #define kControllerFLSMiddle100      2 /**< Middle latency cap of 100ms; @outputof SA_GetControllerFLS. */
   #define kControllerFLSMiddle50       3 /**< Middle latency cap of 50ms; @outputof SA_GetControllerFLS. */
   #define kControllerFLSAggressive30   4 /**< Aggressive latency cap of 30ms; @outputof SA_GetControllerFLS. */
   #define kControllerFLSAggressive10   5 /**< Aggressive latency cap of 10ms; @outputof SA_GetControllerFLS. */
   #define kControllerFLSInvalid        0xFF /**< Invalid/Unknown FLS setting; @outputof SA_GetControllerFLS. */
   /** @} */
/** @} */ /* storc_properties_ctrl_fls_setting */

/** @defgroup storc_properties_spare_activation_mode Controller Spare Activation Mode
 * @brief Get current spare activation mode setting of the controller.
 * @details
 * @see storc_features_ctrl_predictive_spare_rebuild
 * @see storc_configuration_edit_ctrl_sam
 * @{ */
/** Get all controller spare activation mode information.
 *
 * @param[in] cda  Controller data area.
 * @return Use masks at [Spare Activation Mode Info Returns](@ref storc_properties_spare_activation_mode)
 * @return Spare Activation mode capabilities: Supports Failure Spare Activation mode, Predictive Spare Activation mode
 * @return Current Spare Activation mode: Failure Spare Activation, Predictive Spare Activation
 *
 * __Examples__
 * @code
 * printf("Controller supports Failure Spare Activation mode %s\n",
 *    (SA_GetControllerSAM(cda) == kSpareActivationModeSupportsFailureMode
 *       ? "yes" : "no");
 * @endcode
 */
SA_BYTE SA_GetControllerSAM(PCDA cda);
   /** @name Spare Activation Modes
    * @attention These values are subject to change at any time.
    * @outputof SA_GetControllerSAM.
    * @{ */
   #define kControllerSAMTypeFailure       0  /**< Spare Activation mode is type Failure; @outputof SA_GetControllerSAM  */
   #define kControllerSAMTypePredictive    1  /**< Spare Activation mode is type Predictive; @outputof SA_GetControllerSAM  */
   #define kControllerSAMTypeInvalid       0xFF /**< No configuration: spare activation mode is invalid; @outputof SA_GetControllerSAM  */
   /** @} */
/** @} */ /* storc_properties_spare_activation_mode */

/** @defgroup storc_properties_ctrl_nbwc_setting Controller No Battery Write Cache Setting
 * @brief Get a controller's no battery write cache setting.
 * @details
 * @see storc_features_ctrl_nbwc_supported
 * @see storc_configuration_edit_nbwc
 * @{ */
/** Get the controller no battery write cache setting.
 *
 * @param[in] cda  Controller data area.
 * @return No Battery Write Cache setting value (see [Controller NBWC Settings](@ref storc_properties_ctrl_nbwc_setting)).
 */
SA_BYTE SA_GetControllerNBWC(PCDA cda);
   /** @name Controller NBWC Settings
    * @outputof SA_GetControllerNBWC.
    * @{ */
   #define kControllerNBWCEnabled   0
   #define kControllerNBWCDisabled  1
   #define kControllerNBWCInvalid   0xFF
   /** @} */
/** @} */ /* storc_properties_ctrl_nbwc_setting */

/** @defgroup storc_properties_ctrl_wait_for_cache_room Controller Wait For Cache Room Setting
 * @brief Get a controller's wait for cache room setting.
 * @details
 * @see storc_features_ctrl_wait_for_cache_room_supported
 * @see storc_configuration_edit_wait_for_cache_room
 * @{ */
/** Get the controller wait for cache room setting.
 *
 * @param[in] cda  Controller data area.
 * @return Wait For Cache Room setting value (see [Controller Wait For Cache Room Settings](@ref storc_properties_ctrl_wait_for_cache_room)).
 */
SA_BYTE SA_GetControllerWaitForCacheRoom(PCDA cda);
   /** @name Controller Wait For Cache Room Settings
    * @outputof SA_GetControllerWaitForCacheRoom.
    * @{ */
   #define kControllerWaitForCacheRoomDisabled      0
   #define kControllerWaitForCacheRoomEnabled       1
   #define kControllerWaitForCacheRoomInvalid       0xFF
   /** @} */
/** @} */ /* storc_properties_ctrl_wait_for_cache_room */

/** @defgroup storc_properties_ctrl_read_cache_percent Controller Read Cache Percent
 * @brief Get the controller's read/write cache ratio.
 * @details
 * @see storc_configuration_edit_read_cache_percent
 * @{ */
/** Get the controller read cache percent.
 *
 * @param[in] cda  Controller data area.
 * @return Read cache ratio percent [0-100].
 * @return @ref kControllerReadCachePercentInvalid if this field is invalid.
 */
SA_BYTE SA_GetControllerReadCachePercent(PCDA cda);
   /** @name Controller Read Cache Percent
    * @{ */
   #define kControllerReadCachePercentInvalid 0xFF
   /** @} */

/** Get controller's non-battery backed minimum and maximum read cache percent.
 * @param[in] cda Controller data area.
 * @return Use @ref GET_CONTROLLER_NON_BATTERY_BACKED_MIN_READ_CACHE_PERCENT to get controller's non-battery backed minimum read cache percent. (@ref storc_properties_ctrl_read_cache_percent)
 * @return Use @ref GET_CONTROLLER_NON_BATTERY_BACKED_MAX_READ_CACHE_PERCENT to get controller's non-battery backed maximum read cache percent. (@ref storc_properties_ctrl_read_cache_percent)
 */
SA_WORD SA_GetControllerNonBatteryBackedReadCachePercentLimits(PCDA cda);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kGetControllerNonBatteryBackedReadCachePercentMinLimitMask     0x00FF
#define kGetControllerNonBatteryBackedReadCachePercentMinLimitBitShift 0
#endif

/** Decode controller's non-battery backed minimum read cache percent.
 * @outputof SA_GetControllerNonBatteryBackedReadCachePercentLimits.
 */
#define GET_CONTROLLER_NON_BATTERY_BACKED_MIN_READ_CACHE_PERCENT(limit) (((limit) & kGetControllerNonBatteryBackedReadCachePercentMinLimitMask) >> kGetControllerNonBatteryBackedReadCachePercentMinLimitBitShift)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kGetControllerNonBatteryBackedReadCachePercentMaxLimitMask     0xFF00
#define kGetControllerNonBatteryBackedReadCachePercentMaxLimitBitShift 8
#endif

/** Decode controller's non-battery backed maximum read cache percent.
 * @outputof SA_GetControllerNonBatteryBackedReadCachePercentLimits.
 */
#define GET_CONTROLLER_NON_BATTERY_BACKED_MAX_READ_CACHE_PERCENT(limit) (((limit) & kGetControllerNonBatteryBackedReadCachePercentMaxLimitMask) >> kGetControllerNonBatteryBackedReadCachePercentMaxLimitBitShift)

/** Invalid non-battery backed minimum or maximum read cache percent for controller.
 * @outputof GET_CONTROLLER_NON_BATTERY_BACKED_MIN_READ_CACHE_PERCENT.
 * @outputof GET_CONTROLLER_NON_BATTERY_BACKED_MAX_READ_CACHE_PERCENT.
 */
#define kGetControllerNonBatteryBackedReadCachePercentLimitsInvalid  101
/** @} */ /* storc_properties_ctrl_read_cache_percent */

/** @defgroup storc_properties_mnp_delay Controller MNP Delay
 * @brief Get the controller's monitor & performance analysis delay setting.
 * @details
 * @see storc_configuration_edit_mnp_delay
 * @{ */
/** Get controller MNP delay in minutes.
 *
 * @param[in] cda  Controller data area.
 * @return The controller's Monitor and Performance Analysis Delay in minutes.
 * @return @ref kControllerMNPDelayInvalid if this field is invalid.
 *
 * __Examples__
 * @code
 * printf("Controller MNP delay %i\n",
 *         SA_GetControllerMNPDelay(cda));
 * @endcode
 */
SA_WORD SA_GetControllerMNPDelay(PCDA cda);
   /** @name Controller MNP Delay
    * @{ */
   #define kControllerMNPDelayInvalid 0xFFFF
   /** @} */
/** @} */ /* storc_properties_mnp_delay */

/** @defgroup storc_properties_ctrl_boot_volume Controller Boot Volume ID
 * @brief Get the controller's legacy BIOS boot logical/physical volume settings.
 * @details
 * @see storc_configuration_edit_ctrl_boot_vol
 * @{ */

/** @name Bootable Volume Precedence
 * @{ */
/** Not a bootable volume. */
#define kBootableVolumePrecedenceNone      0
/** Primary bootable volume. */
#define kBootableVolumePrecedencePrimary   1
/** Secondary bootable volume. */
#define kBootableVolumePrecedenceSecondary 2
/** Both primary and secondary bootable volumes. */
#define kBootableVolumePrecedenceAll       3
/** @} */

/** @name Bootable Volume Drive Type
 * @{ */
/** Bootable volume is type logical drive. */
#define kBootableVolumeDriveTypeLogical    0
/** Bootable volume is type physical drive. */
#define kBootableVolumeDriveTypePhysical   1
/** Bootable volume is type logical remote. */
#define kBootableVolumeDriveTypeRemote     2
/** @} */

/** Return current device number of controller's bootable volume.
 * @param[in] cda                  Controller data area.
 * @param[in] precedence           Specifies the primary or secondary boot volume ID to get (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @param[in] volume_type          Type to specify if target drive is a logical or physical drive (see [Bootable Volume Drive Type](@ref storc_properties_ctrl_boot_volume)).
 * @return Device number of requested boot volume or @ref kInvalidLDNumber/@ref kInvalidPDNumber/@ref kInvalidRemoteVolumeNumber.
 */
SA_WORD SA_GetControllerBootVolumeNumber(PCDA cda, SA_BYTE precedence, SA_BYTE volume_type);

/** Minimum buffer length (in bytes) required for @ref SA_GetControllerBootVolumeID. */
#define kMinGetControllerBootVolumeIDBufferLength 33
/** Get the primary boot volume of the controller.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `boot_volume_buffer` is at least @ref kMinGetControllerBootVolumeIDBufferLength bytes
 * to contain the boot volume ID and terminating NULL.
 *
 * @param[in]  cda                 Controller data area.
 * @param[in]  precedence          Specifies the primary or secondary boot volume ID to retrieve (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @param[out] boot_volume_buffer  Buffer to write the boot volume ID.
 * @param[in]  buffer_size         Size (in bytes) of `boot_volume_buffer`.
 * @return NULL terminating ASCII string of the specified boot volume ID, same as `boot_volume_buffer`.
 */
char *SA_GetControllerBootVolumeID(PCDA cda, SA_BYTE precedence, char *boot_volume_buffer, size_t buffer_size);
/** @} */ /* storc_properties_ctrl_boot_volume */

/** @defgroup storc_properties_ctrl_driver_version Controller Driver Version
 * @brief Get the controller's host driver version.
 * @{ */

/** Minimum buffer length (in bytes) required for @ref SA_GetControllerDriverVersion. */
#define kMinGetControllerDriverVersionBufferLength 33
/** Write the controller driver version to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `driver_version_buffer` is at least @ref kMinGetControllerDriverVersionBufferLength bytes
 * to contain the product ID and terminating NULL.
 *
 * @param[in]  cda                Controller data area.
 * @param[out] driver_version_buffer  Buffer to write the driver version.
 * @param[in]  buffer_size        Size (in bytes) of `driver_version_buffer`.
 * @return Driver version of the controller, same as `driver_version_buffer`.
 * @return Empty string if given buffer was not large enough to store entire driver version string, or if the controller does not publish this value.
 */
char *SA_GetControllerDriverVersion(PCDA cda, char *driver_version_buffer, size_t buffer_size);
/** @} */ /* storc_properties_ctrl_driver_version */

/** @defgroup storc_properties_ctrl_spinup_parameters Controller Spinup Parameters
 * @brief Get spinup parameters of the controller.
 * @{ */

/** @name Spinup Parameter Type
 * @{ */
#define kSimultaneousSpinupCount 1    /**< Max. number of drives that are spun up simultaneously. */
#define kDriveSpinupDelaySec     2    /**< Spinup delay in seconds between each spinup group. */
/** @} */

/** Get the value of the requested spinup parameter type.
 *
 * @param[in]  cda                Controller data area.
 * @param[in]  parameter_type     one of [Spinup Parameter Type](@ref storc_properties_ctrl_spinup_parameters).
 * @return Spinup parameter value.
 * @return 0 if the controller does not publish this value.
 */
SA_DWORD SA_GetControllerSpinupParameter(PCDA cda, SA_BYTE parameter_type);
/** @} */ /* storc_properties_ctrl_spinup_parameters */

/** @defgroup storc_properties_ctrl_sensor Controller Sensors
 * @brief Get information from the controller's sensors.
 * @{ */

/** @defgroup storc_properties_ctrl_sensor_temp Controller Temperature Sensors
 * @brief Get controller temperature information.
 * @{ */

/** @name Controller Temperature Sensor Info
 * @{ */
/** Controller Temperature Sensor Info.
 * Use by @ref SA_GetControllerTemperatureSensorsInfo.
 */
struct _CONTROLLER_TEMPERATURE_SENSOR_INFO
{
   SA_DWORD current_reading;    /**< Sensor reading. */
   SA_DWORD max_reading;        /**< Max recorded sensor reading. */
   SA_BYTE location;            /**< Sensor location (see [Controller Temperature Sensor Locations](@ref storc_properties_ctrl_sensor_temp)). */
   SA_DWORD critical_threshold; /**< Critical/Fatal temperature threshold. */
   SA_DWORD caution_threshold;  /**< Caution temperature threshold. */
   SA_DWORD warning_threshold;  /**< Warning temperature threshold. */
   SA_BYTE sensor_index;        /**< Controller internal sensor index. */
};
typedef struct _CONTROLLER_TEMPERATURE_SENSOR_INFO CONTROLLER_TEMPERATURE_SENSOR_INFO;
/** @} */

/** Return information on all controller temperature sensors.
 * @post If return is non-null, the return pointer must be cleaned using @ref SA_DestroyControllerTemperatureSensorsInfo.
 * @param[in]  cda               Controller handle.
 * @param[out] number_of_sensors Address to return the number of sensors in the return list.
 * @return An array of [Controller Temperature Sensor Info](@ref storc_properties_ctrl_sensor_temp) which must be destroyed by caller.
 * @return NULL if an error occurred.
 * __Example__
 * @code{.c}
 * CONTROLLER_TEMPERATURE_SENSOR_INFO *sensor_info = 0;
 * SA_DWORD sensor_count = 0;
 * sensor_info = SA_GetControllerTemperatureSensorsInfo(cda, &sensor_count);
 * if (sensor_info)
 * {
 *     SA_DWORD i = 0;
 *     printf("Found %u temp sensors\n", sensor_count);
 *     while (i < sensor_count)
 *     {
 *         printf("Sensor[%u] reading: %u\n", i, sensor_info[i].current_reading);
 *         i++;
 *     }
 *     SA_DestroyControllerTemperatureSensorsInfo(sensor_info);
 * }
 * @endcode
 */
CONTROLLER_TEMPERATURE_SENSOR_INFO* SA_GetControllerTemperatureSensorsInfo(PCDA cda, SA_DWORD *number_of_sensors);

/** Clean data returned by @ref SA_GetControllerTemperatureSensorsInfo.
 * @post Memory at `sensor_info` will not be valid.
 * @param[in] sensor_info Sensor info returned from @ref SA_GetControllerTemperatureSensorsInfo.
 */
void SA_DestroyControllerTemperatureSensorsInfo(CONTROLLER_TEMPERATURE_SENSOR_INFO* sensor_info);

/** Get number of detected temperature sensors on the controller.
 * @deprecated Use SA_GetControllerTemperatureSensorsInfo instead.
 * @param[in] cda  Controller data area.
 * @return Number of temperature sensors on controller.
 */
SA_DWORD SA_GetControllerTemperatureSensorCount(PCDA cda);

/** @name Controller Temperature Info Types
 * Input to @ref SA_GetControllerTemperatureSensorInfo.
 * @{ */
#define kGetControllerTemperatureSensorLocation    0 /**< Request location of temperature sensor; Input to @ref SA_GetControllerTemperatureSensorInfo. */
#define kGetControllerTemperatureSensorReading     1 /**< Request current reading of temperature sensor; Input to @ref SA_GetControllerTemperatureSensorInfo. */
#define kGetControllerTemperatureSensorMaxReading  2 /**< Request max reading of temperature sensor since power on; Input to @ref SA_GetControllerTemperatureSensorInfo. */
/** @} */

/** Get information of a temperature sensor on the controller.
 * @deprecated Use SA_GetControllerTemperatureSensorsInfo instead.
 * @param[in] cda             Controller data area.
 * @param[in] sensor_index    Temperature sensor index (0-based).
 * @param[in] temp_info_type  See [Controller Temperature Info Types](@ref storc_properties_ctrl_sensor_temp).
 * @return User requesed infomation on of `sensor_index`-th temperature sensor.
 */
SA_DWORD SA_GetControllerTemperatureSensorInfo(PCDA cda, SA_DWORD sensor_index, SA_BYTE temp_info_type);

/** Get location of a temperature sensor on the controller.
 * @deprecated Use SA_GetControllerTemperatureSensorsInfo instead.
 * @param[in] cda           Controller data area.
 * @param[in] sensor_index  Temperature sensor index (0-based).
 * @return Location identifier of `sensor_index`-th temperature sensor (See [Controller Temperature Sensor Locations](@ref storc_properties_ctrl_sensor_temp)).
 * @see SA_GetControllerTemperatureSensorInfo.
 * @see kGetControllerTemperatureSensorLocation.
 */
SA_DWORD SA_GetControllerTemperatureSensorLocation(PCDA cda, SA_DWORD sensor_index);
   /** @name Controller Temperature Sensor Locations
    * @outputof SA_GetControllerTemperatureSensorLocation
    * @{ */
   #define kControllerTemperatureSensorLocationInletAmbient 0 /**< Inlet Ambient; @outputof SA_GetControllerTemperatureSensorLocation */
   #define kControllerTemperatureSensorLocationChipASIC     1 /**< Chip ASIC; @outputof SA_GetControllerTemperatureSensorLocation */
   #define kControllerTemperatureSensorLocationTop          2 /**< Top; @outputof SA_GetControllerTemperatureSensorLocation */
   #define kControllerTemperatureSensorLocationBottom       3 /**< Bottom; @outputof SA_GetControllerTemperatureSensorLocation */
   #define kControllerTemperatureSensorLocationFront        4 /**< Front; @outputof SA_GetControllerTemperatureSensorLocation */
   #define kControllerTemperatureSensorLocationBack         5 /**< Back; @outputof SA_GetControllerTemperatureSensorLocation */
   #define kControllerTemperatureSensorLocationCache        6 /**< Cache; @outputof SA_GetControllerTemperatureSensorLocation */
   /** @} */

/** Get current reading from a temperature sensor on the controller.
 * @deprecated Use SA_GetControllerTemperatureSensorsInfo instead.
 * @param[in] cda           Controller data area.
 * @param[in] sensor_index  Temperature sensor index (0-based).
 * @return Temperature reading of `sensor_index`-th temperature sensor.
 * @see SA_GetControllerTemperatureSensorInfo.
 * @see kGetControllerTemperatureSensorReading.
 */
SA_DWORD SA_GetControllerTemperatureSensorReading(PCDA cda, SA_DWORD sensor_index);

/** Get max reading from a temperature sensor on the controller since the last power on.
 * @deprecated Use SA_GetControllerTemperatureSensorsInfo instead.
 * @param[in] cda           Controller data area.
 * @param[in] sensor_index  Temperature sensor index (0-based).
 * @return Max Temperature reading of `sensor_index`-th temperature sensor since power on.
 * @see SA_GetControllerTemperatureSensorInfo.
 * @see kGetControllerTemperatureSensorMaxReading.
 */
SA_DWORD SA_GetControllerTemperatureSensorMaxReading(PCDA cda, SA_DWORD sensor_index);

/** @} */ /* storc_properties_ctrl_sensor_temp */

/** @} */ /* storc_properties_ctrl_sensor */

/** @defgroup storc_properties_ctrl_time Controller Time
 * @brief Get the controller's time.
 * @details
 * @see storc_configuration_ctrl_time
 * @{ */

/** Get the current controller time.
 * @param[in] cda           Controller data area.
 * @return Controller time since UNIX epoch. -1 if this reading this value from the controller is not supported.
 */
SA_QWORD SA_GetControllerTime(PCDA cda);

/** @} */ /* storc_properties_ctrl_time */

/** @defgroup storc_properties_ctrl_backup_power_source_type Controller Backup Power Source Type
 * @brief Get information on the controller's backup power source.
 * @{ */

/** Get the backup power source type (battery/capacitor, etc.) of the controller.
 * @param[in] cda       Controller data area.
 * @return See [Controller Backup Power Source Type Returns](@ref storc_properties_ctrl_backup_power_source_type)
 */
SA_BYTE SA_GetControllerBackupPowerSourceType(PCDA cda);

/** @name Controller Backup Power Source Type Returns
* @outputof SA_GetControllerBackupPowerSourceType.
* @{ */
#define kBackupPowerSourceTypeNone         0     /**< No backup power source needed; @outputof SA_GetControllerBackupPowerSourceType. */
#define kBackupPowerSourceTypeCapacitor    1     /**< Super capacitor; @outputof SA_GetControllerBackupPowerSourceType. */
#define kBackupPowerSourceTypeBattery      2     /**< Battery; @outputof SA_GetControllerBackupPowerSourceType. */
#define kBackupPowerSourceTypeSmartBattery 3     /**< Smart Battery; @outputof SA_GetControllerBackupPowerSourceType. */
#define kBackupPowerSourceTypeUnknown      0xFF  /**< Unknown backup power source; @outputof SA_GetControllerBackupPowerSourceType. */
/** @} */

/** @} */ /* storc_properties_ctrl_backup_power_source_type */


/** @defgroup storc_properties_ctrl_backup_power_source_counts Controller Backup Power Source Counts
 * @brief Get a count of the controller's backup power sources.
 * @{ */

/** Get the backup power source counts (battery/capacitor, etc.) of the controller.
 * @param[in] cda       Controller data area.
 * @return DWORD value containing the number of sources, the total number of installed sources and total number of failed sources in one value.
 * @return @ref BACKUP_POWER_SOURCE_COUNT_RESULT to get the return code of the API
 * @return @ref TOTAL_BACKUP_POWER_SOURCE_COUNT to get the total number of backup power sources.
 * @return @ref INSTALLED_BACKUP_POWER_SOURCE_COUNT to get the total number of installed power sources.
 * @return @ref FAILED_BACKUP_POWER_SOURCE_COUNT to get the total number of failed power sources.
 * __Example__
 * @code
 * SA_DWORD power_source_counts = SA_GetControllerBackupPowerSourceCounts(cda);
 * if (BACKUP_POWER_SOURCE_COUNT_RESULT(power_source_counts) == kBackupPowerSourceCountsSuccessful)
 * {
 *    printf("# of Power Sources  : %u\n", TOTAL_BACKUP_POWER_SOURCE_COUNT(power_source_counts));
 *    printf("# Installed Sources : %u\n", INSTALLED_BACKUP_POWER_SOURCE_COUNT(power_source_counts));
 *    printf("# Failed Sources    : %u\n", FAILED_BACKUP_POWER_SOURCE_COUNT(power_source_counts));
 * }
 * else
 * {
 *    printf(" ERROR: Backup power source counts are not available.\n");
 * }
 * @endcode
 */
SA_DWORD SA_GetControllerBackupPowerSourceCounts(PCDA cda);

/** @name Controller Backup Power Source Counts Returns
 * @{ */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kInstalledBackupPowerSourcesBitshift 8
#define kFailedBackupPowerSourcesBitshift    16
#define kBackupPowerSourceResultBitshift     24
#define kBackupPowerSourceCountMask          0xFF
#define kInstalledBackupPowerSourcesMask     0xFF00
#define kFailedBackupPowerSourcesMask        0xFF0000
#define kBackupPowerSourceResultMask         0xFF000000
#endif

/** @name Backup Power Source Count Result
 * @outputof SA_GetControllerBackupPowerSourceCounts to return result of the API call
 * @{ */
#define kBackupPowerSourceCountsUnavailable 0 /**< Unavailable; @outputof SA_GetControllerBackupPowerSourceCounts. */
#define kBackupPowerSourceCountsSuccessful  1 /**< Successful;  @outputof SA_GetControllerBackupPowerSourceCounts. */

#define BACKUP_POWER_SOURCE_COUNT_RESULT(psc) ((SA_BYTE)(((SA_DWORD)psc & kBackupPowerSourceResultMask) >> kBackupPowerSourceResultBitshift))
/** @} */

/** @name Total Backup Power Source Count
 * @outputof SA_GetControllerBackupPowerSourceCounts to return total number of power sources on the controller.
 * @attention Valid only if @ref BACKUP_POWER_SOURCE_COUNT_RESULT returns kBackupPowerSourceCountsSuccessful.
 * @{ */
#define TOTAL_BACKUP_POWER_SOURCE_COUNT(psc) ((SA_BYTE)((SA_DWORD)psc & kBackupPowerSourceCountMask))
/** @} */

/** @name Installed Backup Power Source Count
 * @outputof SA_GetControllerBackupPowerSourceCounts to return the total number of installed power sources on the controller.
 * @attention Valid only if @ref BACKUP_POWER_SOURCE_COUNT_RESULT returns kBackupPowerSourceCountsSuccessful.
 * @{ */
#define INSTALLED_BACKUP_POWER_SOURCE_COUNT(psc) ((SA_BYTE)(((SA_DWORD)psc & kInstalledBackupPowerSourcesMask) >> kInstalledBackupPowerSourcesBitshift))
/** @} */

/** @name Failed Backup Power Source Count
 * @outputof SA_GetControllerBackupPowerSourceCounts to return the total number of failed power sources on the controller.
 * @attention Valid only if @ref BACKUP_POWER_SOURCE_COUNT_RESULT returns kBackupPowerSourceCountsSuccessful.
 * @{ */
#define FAILED_BACKUP_POWER_SOURCE_COUNT(psc) ((SA_BYTE)(((SA_DWORD)psc & kFailedBackupPowerSourcesMask) >> kFailedBackupPowerSourcesBitshift))
/** @} */

/** @} */ /* storc_properties_ctrl_backup_power_source_counts */

/** @defgroup storc_properties_ctrl_sanitize_lock Controller Sanitize Lock
 * @brief Get the controller's ATA freeze/antifreeze sanitize lock setting.
 * @details
 * @see storc_features_ctrl_support_sanitize_lock
 * @see storc_configuration_edit_ctrl_sanitize_lock
 * @{ */

/** Get the current and pending sanitize lock state of the controller.
 * @param[in] cda         Controller Data Area
 * @return See [Controller Sanitize Locks](@ref storc_properties_ctrl_sanitize_lock)
 */
SA_BYTE SA_GetControllerSanitizeLock(PCDA cda);

/** @name Controller Sanitize Locks
 * @outputof SA_GetControllerSanitizeLock.
 * @{ */
/** The controller sanitize lock state is Disabled.
 * @outputof SA_GetControllerSanitizeLock.
 */
#define kControllerSanitizeLockDisabled        0x00
/** The controller sanitize lock state is Freeze.
 *  Drives that support this feature will abort all sanitize commands.
 * @outputof SA_GetControllerSanitizeLock.
 */
#define kControllerSanitizeLockFreeze          0x01
/** The controller sanitize lock state is Anti-Freeze.
 *  Drives that support this feature will abort all sanitize freeze lock commands.
 * @outputof SA_GetControllerSanitizeLock.
 */
#define kControllerSanitizeLockAntiFreeze      0x02
/** The controller sanitize lock state is unknown.
 * @outputof SA_GetControllerSanitizeLock.
 */
#define kControllerSanitizeLockUnknown         0x03

/** Mask to use on output of @ref SA_GetControllerSanitizeLock to get the current sanitize lock state. */
#define kCurrentControllerSanitizeLockMask     0x0F
/** Bitshift to use with output of @ref SA_GetControllerSanitizeLock (after masking with @ref kCurrentControllerSanitizeLockMask) to get the current sanitize lock state. */
#define kCurrentControllerSanitizeLockBitShift 0
/** Mask to use on output of @ref SA_GetControllerSanitizeLock to get the pending sanitize lock state. */
#define kPendingControllerSanitizeLockMask     0xF0
/** Bitshift to use with output of @ref SA_GetControllerSanitizeLock (after masking with @ref kPendingControllerSanitizeLockMask) to get the pending sanitize lock state. */
#define kPendingControllerSanitizeLockBitShift 4

 /** Decode output of @ref SA_GetControllerSanitizeLock to get the current sanitize lock state of the controller. */
#define CURRENT_CTRL_SANITIZE_LOCK(info_value) ((info_value & kCurrentControllerSanitizeLockMask) >> kCurrentControllerSanitizeLockBitShift)
 /** Decode output of @ref SA_GetControllerSanitizeLock to get the pending sanitize lock state of the controller. */
#define PENDING_CTRL_SANITIZE_LOCK(info_value) ((info_value & kPendingControllerSanitizeLockMask) >> kPendingControllerSanitizeLockBitShift)
/** @} */ /* Controller Sanitize Locks */

/** @} */ /* storc_properties_ctrl_sanitize_lock */

/** @defgroup storc_properties_ctrl_notify_on_any_broadcast Controller Notify On Any Broadcast
 * @brief Get the controller's "Notify On Any Broadcast" setting.
 * @details
 * @see storc_features_ctrl_support_notify_any_broadcast
 * @see storc_configuration_edit_ctrl_notify_any_broadcast
 * @{ */
/** Get current controller setting for notify on any broadcast.
 * @param[in] cda  Controller data area.
 * @return See [Controller Notify On Any Broadcast Returns](@ref storc_properties_ctrl_notify_on_any_broadcast).
 * @see SA_ControllerSupportsNotifyOnAnyBroadcast.
 */
SA_BYTE SA_GetControllerNotifyOnAnyBroadcast(PCDA cda);
/** @name Controller Notify On Any Broadcast Returns
 * @{ */
/** Invalid/Unknown setting.
 * @outputof SA_GetControllerNotifyOnAnyBroadcast.
 * @outputof SA_GetEditableControllerNotifyOnAnyBroadcast.
 */
#define kControllerNotifyOnAnyBroadcastError         0
/** Feature not supported by the controller.
 * @outputof SA_GetControllerNotifyOnAnyBroadcast.
 * @outputof SA_GetEditableControllerNotifyOnAnyBroadcast.
 */
#define kControllerNotifyOnAnyBroadcastNotSupported  1
/** Notify on any broadcast enabled.
 * @outputof SA_GetControllerNotifyOnAnyBroadcast
 * @outputof SA_GetEditableControllerNotifyOnAnyBroadcast.
 * Input to @ref SA_CanChangeEditableControllerNotifyOnAnyBroadcast.
 * Input to @ref SA_ChangeEditableControllerNotifyOnAnyBroadcast.
 */
#define kControllerNotifyOnAnyBroadcastDisabled      2
/** Notify on any broadcast disabled.
 * @outputof SA_GetControllerNotifyOnAnyBroadcast
 * @outputof SA_GetEditableControllerNotifyOnAnyBroadcast.
 * Input to @ref SA_CanChangeEditableControllerNotifyOnAnyBroadcast.
 * Input to @ref SA_ChangeEditableControllerNotifyOnAnyBroadcast.
 */
#define kControllerNotifyOnAnyBroadcastEnabled       3
/** @} */
/** @} */ /* storc_properties_ctrl_notify_on_any_broadcast */

/** @defgroup storc_properties_ctrl_os_bootable_drives Controller Number of OS Bootable Drives
 * @brief Get the controller's bootable OS drive count.
 * @{ */
/** Return number of reported OS bootable drives configured on the controller.
 * @param[in] cda [Controller data area](@ref storc_cda).
 * @return Configured number of OS bootable drives.
 */
SA_BYTE SA_GetControllerOSBootableDriveCount(PCDA cda);
/** @} */ /* storc_properties_ctrl_os_bootable_drives */

/** @defgroup storc_properties_ctrl_manufacturing_data Controller Manufacturing Data
 * @brief Get the controller's manufacturing data.
 * @{ */
/**  Type used to get [controller manufacturing data](@ref storc_properties_ctrl_manufacturing_data). */
struct _SA_CONTROLLER_MANUFACTURING_DATA
{
   SA_BYTE WWID[8];            /**< WWID of the board. Value is in big endian. */
   char serial_number[16];     /**< Serial number of the board as a null-terminated ASCII string. */
   char spare_part_number[16]; /**< Optional spare part number as a null-terminated ASCII string. */
   SA_BYTE wellness_log[12];   /**< Optional space to store board specific information. */
   SA_BYTE revision;           /**< Board revision. */
   char part_number[16];       /**< Optional part number as a null-terminated ASCII string. */
};
typedef struct _SA_CONTROLLER_MANUFACTURING_DATA SA_CONTROLLER_MANUFACTURING_DATA;
/** Gather manufacturing data of a controller.
 *
 * @attention @p manufacturing_data bytes will be initialized to zero.
 *
 * @param[in]  cda                 Controller data area.
 * @param[out] manufacturing_data  Address of a @ref SA_CONTROLLER_MANUFACTURING_DATA to update.
 * @return @ref kTrue if @p manufacturing_data was successfully updated, @ref kFalse otherwise.
 */
SA_BOOL SA_GetControllerManufacturingData(PCDA cda, SA_CONTROLLER_MANUFACTURING_DATA *manufacturing_data);

/** Gather OEM manufacturing data and data type.
 *
 * @param[in]  cda        Controller data area.
 * @param[out] data_type  Indicates the data type of the manufacturing data.
 * @param[out] revision   Revision of the manufacturing data.
 * @return Pointer to the corrisponding manufacturing data structure based on the data_type.
 * @return NULL on failure.
 * @remark If the data_type is 0 and the revision is 0, use @ref SA_CONTROLLER_MANUFACTURING_DATA to parse the returned data.
 *
 * __Example__
 * @code
 * SA_BYTE data_type;
 * SA_BYTE revision;
 * void *manufacturing_buffer;
 * struct SA_CONTROLLER_MANUFACTURING_DATA manufacturing_data;
 *
 * if ((manufacturing_buffer = SA_GetControllerOEMManufacturingData(cda, &data_type, &revision)) != NULL)
 * {
 *    if (data_type == 0 && revision == 0)
 *    {
 *       manufacturing_data = (SA_CONTROLLER_MANUFACTURING_DATA *)manufacturing_buffer;
 *    }
 *    SA_free(manufacturing_buffer);
 * }
 * @endcode
 */
void* SA_GetControllerOEMManufacturingData(PCDA cda, SA_BYTE* data_type, SA_BYTE* revision);
/** @} */ /* storc_properties_ctrl_manufacturing_data */

/** @defgroup storc_properties_ctrl_oob_config Controller OOB Configuration Information
 * @details
 * Read the controller's out-of-band configuration information.
 *
 * __Example__
 * Read I2C address
 * @code
 * SA_WORD i2c_address = SA_GetControllerI2CAddress(cda);
 * if (i2c_address != kUnknownI2CAddress)
 * {
 *    printf("I2C Address: 0x%02X\n", i2c_address);
 * }
 * else
 * {
 *    printf("I2C Address unknown.");
 * }
 * @endcode
 *
 * __Example__
 * Read out-of-band configuration
 * @code
 * OOBConfiguration oob_config;
 * if (SA_GetControllerActiveOOBConfiguration(cda, oob_config) == kGetControllerOOBConfigurationSuccess)
 * {
 *    if (oob_config.interface_type == kInterfaceTypePBSI)
 *    {
 *       printf("PBSI Interface:\n");
 *       printf("  Clock Speed: %s\n",
 *            oob_config.pbsi.clock_speed == kClockSpeedDisabled ? "Disabled"
 *          : oob_config.pbsi.clock_speed == kClockSpeed10KHZ    ? "10 KHZ"
 *          : oob_config.pbsi.clock_speed == kClockSpeed100KHZ   ? "100 KHZ"
 *          : oob_config.pbsi.clock_speed == kClockSpeed400KHZ   ? "400 KHZ"
 *          : oob_config.pbsi.clock_speed == kClockSpeed800KHZ   ? "800 KHZ"
 *          : "Unknown");
 *       printf("  Clock Stretching: %s\n", oob_config.pbsi.clock_stretching == 1 ? "Enabled" : "Disabled");
 *    }
 *    else if (oob_config.interface_type == kInterfaceTypeMCTP)
 *    {
 *       printf("MCTP Interface:\n");
 *       printf("  SMBUS Device Type: %s\n",
 *            oob_config.mctp.smbus_device_type == kMCTPSMBUSDeviceTypeDefault ? "Default (ARP)"
 *          : oob_config.mctp.smbus_device_type == kMCTPSMBUSDeviceTypeFixed   ? "Fixed"
 *          : oob_config.mctp.smbus_device_type == kMCTPSMBUSDeviceTypeARP     ? "ARP"
 *          : "Unknown");
 *       printf("  VDM Discovery Notify: %s\n", oob_config.mctp.vdm_discovery_notify == 1 ? "Enabled" : "Disabled");
 *       printf("  Use Static EIDs: %s\n", oob_config.mctp.use_static_eids_on_init == 1 ? "Enabled" : "Disabled");
 *       printf("  Enable SMBUS Physical Channel: %s\n", oob_config.mctp.enable_smbus_physical_channel == 0 ? "Enabled" : "Disabled");
 *    }
 *    else if (oob_config.interface_type == kInterfaceTypeDisabled)
 *    {
 *       printf("OOB interface is disabled.");
 *    }
 * }
 * else
 * {
 *    printf("Failed to get active out-of-band configuration.");
 * }
 * @endcode
 * @{ */

/** @name OOB Interface Types
 * Values for @ref OOBConfiguration.interface_type.
 * @{ */
#define kOOBInterfaceTypePBSI      0x01 /**< Out of band configuration interface type PBSI. */
#define kOOBInterfaceTypeMCTP      0x02 /**< Out of band configuration interface type MCTP. */
#define kOOBInterfaceTypeDisabled  0xFF /**< Out of band configuration interface type is disabled. */
#define kOOBInterfaceTypeReset     0x00 /**< Use along with setting ALL fields in [OOBConfiguration](@ref _OOBConfiguration) to 0 to reset
                                         * OOB configuration back to factory defaults. */
/** @} */ /* OOB Interface Types */

/** @name PBSI Clock Speeds
 * Values for [OOBConfiguration.i2c_configuration.pbsi.clock_speed](@ref storc_properties_ctrl_oob_config).
 * Applies only when [OOBConfiguration.interface_type](@ref storc_properties_ctrl_oob_config) is [kOOBInterfaceTypePBSI](@ref storc_properties_ctrl_oob_config).
 * @{ */
#define kPBSIClockSpeedDisabled    0x00 /**< Clock rate is disabled. */
#define kPBSIClockSpeed10KHZ       0x01 /**< 10KHz clock rate. */
#define kPBSIClockSpeed100KHZ      0x02 /**< 100KHz clock rate. */
#define kPBSIClockSpeed400KHZ      0x03 /**< 400KHz clock rate. */
#define kPBSIClockSpeed800KHZ      0x04 /**< 800KHz clock rate. */
/** @} */

/** @name MCTP SMBUS Device Types
 * Values for [OOBConfiguration.i2c_configuration.mctp.smbus_device_type](@ref storc_properties_ctrl_oob_config).
 * Applies only when [OOBConfiguration.interface_type](@ref storc_properties_ctrl_oob_config) is [kOOBInterfaceTypeMCTP](@ref storc_properties_ctrl_oob_config).
 * @{ */
#define kMCTPSMBUSDeviceTypeDefault  0x00 /**< SMBUS device type is default (ARP). */
#define kMCTPSMBUSDeviceTypeFixed    0x01 /**< SMBUS device type is fixed. */
#define kMCTPSMBUSDeviceTypeARP      0x02 /**< SMBUS device type is ARP. */
/** @} */

#if defined(_MSC_VER)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
/** Controller OOB configuration information.
 * @see storc_configuration_oob_config
 *
 * Data structure used to get the active/pending OOB configuration as well as
 * to configure the OOB settings of the controller.
 */
struct _OOBConfiguration
{
   SA_BYTE interface_type; /**< OOB interface type. See [OOB Interface Types](@ref storc_properties_ctrl_oob_config) */

   /** I2C configuration. */
   union
   {
      struct _pbsi
      {
         SA_BYTE clock_speed      : 3; /**< PBSI clock speed.
                                        * See [PBSI Clock Speeds](@ref storc_properties_ctrl_oob_config) */
         SA_BYTE clock_stretching : 1; /**< PBSI clock stretching.
                                        * Set to 1 to enable clock stretching. */
         SA_BYTE reserved         : 4;
      } STRUCT_POSTFIX(packed) pbsi;   /**< I2C Configuration for PBSI. */

      struct _mctp
      {
         SA_BYTE smbus_device_type             : 2; /**< MCTP SMBUS device type.
                                                     * See [MCTP SMBUS Device Types](@ref storc_properties_ctrl_oob_config) */
         SA_BYTE vdm_discovery_notify          : 1; /**< MCTP VDM discovery notify.
                                                     * Set to 1 to enable VDM discovery notify. */
         SA_BYTE use_static_eids_on_init       : 1; /**< MCTP use static eids on initialization.
                                                     * Set to 1 to enable static eids on initialization. */
         SA_BYTE enable_smbus_physical_channel : 1; /**< MCTP enable SMBUS physical channel.
                                                     * Set to 0 to enable the SMBUS physical channel. */
         SA_BYTE reserved                      : 3;
      } STRUCT_POSTFIX(packed) mctp;  /**< I2C Configuration for MCTP. */

      SA_BYTE raw;                    /**< Raw byte representing I2C Configuration. */
   } i2c_configuration;

   SA_BYTE slave_address; /**< I2C address for the BMC interface. */

   SA_BYTE reserved;
} STRUCT_POSTFIX(packed);

typedef struct _OOBConfiguration OOBConfiguration;
#if defined(_MSC_VER)
#pragma pack(pop)
#else
#pragma pack()
#endif

/** Get the I2C address of the controller.
 * @param[in] cda       Controller data area.
 * @return The I2C address of the controller, @ref kUnknownI2CAddress on error.
 */
SA_WORD SA_GetControllerI2CAddress(PCDA cda);
/** @name Controller I2C Address returns
 * @outputof SA_GetControllerI2CAddress.
 * @{ */
#define kUnknownI2CAddress 0xFFFF /**< Unknown I2C address. */
/** @} */

/** Returns Controller OOB Active Configuration.
 * @param[in] cda              Controller data area.
 * @param[out] oob_config      Address of [OOBConfiguration](@ref storc_properties_ctrl_oob_config) to store OOB configuration.
 * @return [Controller OOBConfiguration returns](@ref storc_properties_ctrl_oob_config)
 */
SA_BYTE SA_GetControllerActiveOOBConfiguration(PCDA cda, OOBConfiguration *oob_config);
/** @name Controller OOBConfiguration returns
 * @outputof SA_GetControllerActiveOOBConfiguration.
 * @outputof SA_GetControllerPendingOOBConfiguration.
 * @{ */
#define kGetControllerOOBConfigurationSuccess                      0x01   /**< Get the OOB configuration successfully. */
#define kGetControllerOOBConfigurationInvalidInput                 0x02   /**< Invalid input. */
#define kGetControllerOOBConfigurationNotSupported                 0x03   /**< OOB is not supported. */
#define kGetControllerOOBConfigurationFailed                       0x04   /**< Get OOB configuration failed. */
 /** @} */

/** Returns Controller OOB Pending Configuration.
 * @param[in] cda              Controller data area.
 * @param[out] oob_config      Address of [OOBConfiguration](@ref storc_properties_ctrl_oob_config) to store OOB configuration.
 * @return [Controller OOBConfiguration returns](@ref storc_properties_ctrl_oob_config)
 */
SA_BYTE SA_GetControllerPendingOOBConfiguration(PCDA cda, OOBConfiguration *oob_config);

/** @} */ /* storc_properties_ctrl_oob_config */

/** @defgroup storc_properties_ctrl_expander_scan_duration Controller Expander Scan Duration
 * @brief Get the minimum and maximum allowable expander scan duration
 * @{ */
/** Return the maximum expander scan duration the controller allows.
 *
 * @param[in] cda [Controller data area](@ref storc_cda).
 * @return Maximum allowable expander scan duration in seconds.
 * @return [Controller Expander Scan Duration returns](@ref storc_properties_ctrl_expander_scan_duration) on error.
 */
SA_DWORD SA_GetControllerExpanderMaximumAllowableScanDuration(PCDA cda);
/** @name Controller Maximum Allowable Expander Scan Duration returns
 * @outputof SA_GetControllerExpanderMaximumAllowableScanDuration.
 * @outputof SA_GetControllerExpanderMinimumScanDuration.
 * @{ */
#define kGetControllerExpanderScanDurationFailed       0xFFFFFFFF /**< Getting the expander scan duration failed. */
#define kGetControllerExpanderScanDurationNotSupported 0xFFFFFFFE /**< The controller does not support getting the expander scan duration. */
/** @} */

/** Return the current minimum expander scan duration of the controller.
 *
 * @param[in] cda [Controller data area](@ref storc_cda).
 * @return Current minimum expander scan duration in seconds.
 * @return [Controller Expander Scan Duration returns](@ref storc_properties_ctrl_expander_scan_duration) on error.
 */
SA_DWORD SA_GetControllerExpanderMinimumScanDuration(PCDA cda);

/** @} */ /* storc_properties_ctrl_expander_scan_duration */

/** @defgroup storc_properties_ctrl_pcie_ordering Controller PCIe Ordering Information
 * @brief Get the PCIe ordering mode of the controller.
 * @{ */
/** Return the controller current and pending PCIe ordering information.
 *
 * @param[in] cda  Controller data area
 * @return See [Controller PCIe Ordering Modes](@ref storc_properties_ctrl_pcie_ordering)
 */
SA_BYTE SA_GetControllerPCIeOrdering(PCDA cda);

/** @name Controller PCIe Ordering Modes
 * @outputof SA_GetControllerPCIeOrdering.
 * @{ */
/** Reading the PCIe ordering is not supported on this controller.
 * @outputof SA_GetControllerPCIeOrdering.
 */
#define kControllerPCIeOrderingNotSupported 0
/** The controller PCIe ordering is unknown or invalid.
 * @outputof SA_GetControllerPCIeOrdering.
 */
#define kControllerPCIeOrderingUnknown      1
/** The controller PCIe ordering is set to strict.
 * @outputof SA_GetControllerPCIeOrdering.
 */
#define kControllerPCIeOrderingStrict       2
/** The controller PCIe ordering is set to relaxed.
 * @outputof SA_GetControllerPCIeOrdering.
 */
#define kControllerPCIeOrderingRelaxed      3

/** Mask to use on output of @ref SA_GetControllerPCIeOrdering to get the current PCIe ordering. */
#define kCurrentControllerPCIeOrderingMask     0x0F
/** Bitshift to use with output of @ref SA_GetControllerPCIeOrdering (after masking with @ref kCurrentControllerPCIeOrderingMask) to get the current PCIe ordering. */
#define kCurrentControllerPCIeOrderingBitshift 0
/** Mask to use on output of @ref SA_GetControllerPCIeOrdering to get the pending PCIe ordering. */
#define kPendingControllerPCIeOrderingMask     0xF0
/** Bitshift to use with output of @ref SA_GetControllerPCIeOrdering (after masking with @ref kCurrentControllerPCIeOrderingMask) to get the current PCIe ordering. */
#define kPendingControllerPCIeOrderingBitshift 4

/** Decode output of @ref SA_GetControllerPCIeOrdering to get the current PCIe ordering mode of the controller. */
#define CURRENT_CTRL_PCIE_ORDERING(info_value) ((info_value & kCurrentControllerPCIeOrderingMask) >> kCurrentControllerPCIeOrderingBitshift)
/** Decode output of @ref SA_GetControllerPCIeOrdering to get the pending PCIe ordering mode of the controller. */
#define PENDING_CTRL_PCIE_ORDERING(info_value) ((info_value & kPendingControllerPCIeOrderingMask) >> kPendingControllerPCIeOrderingBitshift)
/** @} */ /* Controller PCIe Ordering Modes */

/** @} */ /* storc_properties_ctrl_pcie_ordering */

/** @} */ /* storc_properties_ctrl */

/**************************************************//**
 * @defgroup storc_properties_ctrl_port Connector/Port
 * @brief
 * @details
 * @{
 *****************************************************/

#define kInvalidConnectorIndex  0xFFFF /**< Invalid connector index. */

/** @name Connector Phy Rates
 * Value for @ref CONTROLLER_CONNECTOR_INFO::connector_max_allowed_phy_rate.
 * Output of @ref SA_ControllerSupportsPhyRates / @ref DEFAULT_PHY_RATE.
 * Input to @ref SA_CanSetConnectorMaxPhyRate / @ref SA_SetConnectorMaxPhyRate.
 * @{
 */
#define kPhyRateUnknown    0x00  /**< Unknown */
#define kPhyRate1G         0x08  /**< Phy Rate is 1Gb/s */
#define kPhyRate3G         0x09  /**< Phy Rate is 3Gb/s */
#define kPhyRate6G         0x0A  /**< Phy Rate is 6Gb/s */
#define kPhyRate12G        0x0B  /**< Phy Rate is 12Gb/s */
#define kPhyRate24G        0x0C  /**< Phy Rate is 24Gb/s */
 /** @} */

/** @defgroup storc_properties_ctrl_port_mode Controller Port Mode
 * @brief Get the operating mode (RAID/HBA/Mixed/...) settings for the controller's port/connectors.
 * @details
 * @see storc_properties_ctrl_mode
 * @see storc_configuration_port_mode
 * @{ */

/** @name Connector Modes
 * @attention These values are subject to change at any time.
 * @{ */
#define kConnectorModeRAID    0 /**< Connector/port mode RAID. */
#define kConnectorModeHBA     1 /**< Connector/port mode HBA. */
#define kConnectorModeMixed   2 /**< Connector/port mode Mixed. */
#define kConnectorModeUnknown 3 /**< Connector/port mode unknown. */
/** @} */

/** Used to gather connector information in @ref SA_GetControllerConnectorInfo. */
struct _CONTROLLER_CONNECTOR_INFO
{
   SA_BOOL is_external_connector;   /**< Connector is external (@ref kTrue) or internal (@ref kFalse). */
   SA_BOOL is_boot_connector;       /**< Connector is a dedicated boot connector (@ref kTrue) or a normal connector (@ref kFalse). */
   SA_QWORD phy_map;                /**< A bitmap representing the phys that are linked to this connector. */
   SA_BYTE SASAddress[8];           /**< Connector's SAS address. */
   SA_BYTE connector_number;        /**< Connector index/number used for identification. */
   char connector_name[9];          /**< Short (or long if supported) connector name including terminating NULL. */
   SA_BYTE current_connector_mode;  /**< One of [Connector Modes](@ref storc_properties_ctrl_port_mode). */
   SA_BYTE pending_connector_mode;  /**< One of [Connector Modes](@ref storc_properties_ctrl_port_mode). */
   SA_BOOL connector_supports_RAID_mode;  /**< Connector supports RAID mode (@ref kTrue) or not (@ref kFalse). */
   SA_BOOL connector_supports_HBA_mode;   /**< Connector supports HBA mode (@ref kTrue) or not (@ref kFalse). */
   SA_BOOL connector_supports_mixed_mode; /**< Connector supports Mixed mode (@ref kTrue) or not (@ref kFalse). */
   SA_BYTE connector_max_allowed_phy_rate; /**< Connector's max allowed phy rate; see [Connector Phy Rates](@ref storc_properties_ctrl_port_mode)*/
   SA_QWORD active_phy_map;         /**< A bitmap representing the phys in this connector that are linked to a physical device. */
   SA_BOOL is_active_phy_map_valid; /**< The active_phy_map field is supported by the controller (@ref kTrue) or not (@ref kFalse). */
};
typedef struct _CONTROLLER_CONNECTOR_INFO CONTROLLER_CONNECTOR_INFO;
typedef struct _CONTROLLER_CONNECTOR_INFO *PCONTROLLER_CONNECTOR_INFO;

/** Returns Controller Port mode info
 * @pre `connector_index` references an existing connector.
 * @param[in] cda              Controller data area.
 * @param[in] connector_index  Index of target connector on controller.
 * @param[out] connector_info  Address of buffer to store connector data.
 * @return @ref kTrue if connector mode info was successfully written to `connector_info`, @ref kFalse otherwise.
 */
SA_BOOL SA_GetControllerConnectorInfo(PCDA cda, SA_WORD connector_index, PCONTROLLER_CONNECTOR_INFO connector_info);

/** Return @ref kTrue/@ref kFalse if the connector is (or is not) in RAID mode currently. */
SA_BOOL SA_ConnectorCurrentModeIsRAID(PCDA cda, SA_WORD connector_index);
/** Return @ref kTrue/@ref kFalse if the connector is (or is not) in HBA mode currently. */
SA_BOOL SA_ConnectorCurrentModeIsHBA(PCDA cda, SA_WORD connector_index);
/** Return @ref kTrue/@ref kFalse if the connector is (or is not) in Mixed mode currently. */
SA_BOOL SA_ConnectorCurrentModeIsMixed(PCDA cda, SA_WORD connector_index);
/** Return @ref kTrue/@ref kFalse if the connector's mode is (or is not) currently unknown. */
SA_BOOL SA_ConnectorCurrentModeIsUnknown(PCDA cda, SA_WORD connector_index);

/** Return @ref kTrue/@ref kFalse if the connector is (or is not) in __pending__ RAID mode. */
SA_BOOL SA_ConnectorPendingModeIsRAID(PCDA cda, SA_WORD connector_index);
/** Return @ref kTrue/@ref kFalse if the connector is (or is not) in __pending__ HBA mode. */
SA_BOOL SA_ConnectorPendingModeIsHBA(PCDA cda, SA_WORD connector_index);
/** Return @ref kTrue/@ref kFalse if the connector is (or is not) in __pending__ Mixed mode. */
SA_BOOL SA_ConnectorPendingModeIsMixed(PCDA cda, SA_WORD connector_index);
/** Return @ref kTrue/@ref kFalse if the connector is (or is not) __pending__ an unknown mode. */
SA_BOOL SA_ConnectorPendingModeIsUnknown(PCDA cda, SA_WORD connector_index);

/** Return @ref kTrue/@ref kFalse if connector is a dedicated boot connector. */
SA_BOOL SA_ConnectorIsBootConnector(PCDA cda, SA_WORD connector_index);
/** @} */ /* storc_properties_ctrl_port_mode */

/** @defgroup storc_properties_ctrl_port_count Port Count
 * @{ */
/** Get internal/external connector counts of the Controller.
 *
 * @param[in] cda  Controller data area.
 * @return Bitmap with connector counts (decode with [Controller Mode Info Returns](@ref storc_properties_ctrl_port_count)).
 *
 * __Examples__
 * @code
 * printf("Internal Connectors: %u\n",
 *     SA_GetControllerConnectorCounts(cda) & kControllerConnectorCountInternalMask >> kControllerConnectorCountInternalBitShift);
 *
 * printf("External Connectors: %u\n",
 *     SA_GetControllerConnectorCounts(cda) & kControllerConnectorCountExternalMask >> kControllerConnectorCountExternalBitShift);
 * @endcode
 */
SA_WORD SA_GetControllerConnectorCounts(PCDA cda);
   /** @name Controller Mode Info Returns
    * Decodes output of @ref SA_GetControllerConnectorCounts.
    * @{ */
   /** Mask to use on output of @ref SA_GetControllerConnectorCounts to get internal connector count. */
   #define kControllerConnectorCountInternalMask      0x00FF
   /** Bitshift to use with output of @ref SA_GetControllerConnectorCounts (after masking with @ref kControllerConnectorCountInternalMask) to get internal connector count. */
   #define kControllerConnectorCountInternalBitShift  0
   /** Mask to use on output of @ref SA_GetControllerConnectorCounts to get external connector count. */
   #define kControllerConnectorCountExternalMask      0xFF00
   /** Bitshift to use with output of @ref SA_GetControllerConnectorCounts (after masking with @ref kControllerConnectorCountExternalMask) to get external connector count. */
   #define kControllerConnectorCountExternalBitShift  8
   /** @} */

/** @return Total number of controller connectors/ports. */
SA_WORD SA_GetControllerTotalConnectorCount(PCDA cda);

/** @return Number of internal controller connectors/ports. */
SA_WORD SA_GetControllerInternalConnectorCount(PCDA cda);

/** @return Number of external controller connectors/ports. */
SA_WORD SA_GetControllerExternalConnectorCount(PCDA cda);

/** Iterate over all controller connectors/ports and execute the block with each onnector index.
 * @attention ANSI C Safe.
 * @param[in] cda              Controller data area.
 * @param[in] connector_index  Variable to store connector indecies.
 *
 * __Example__
 * @code
 * SA_WORD connector_index;
 * FOREACH_CONNECTOR(cda, connector_index)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_CONNECTOR(cda, connector_index) \
   for (connector_index = 0; \
        connector_index < SA_GetControllerTotalConnectorCount(cda); \
        connector_index++)

/** @} */ /* storc_properties_ctrl_port_count*/

/** @defgroup storc_properties_ctrl_port_discovery_configuration Controller Port Discovery Configuration
 * @brief Get discovery configuration for the controller's port/connectors.
 * @{ */

/** @name Connector Discovery Protocol
 * @attention These values are subject to change at any time.
 * Value for @ref CONNECTOR_DISCOVERY_CONFIGURATION::discovery_protocol.
 * Output of @ref SA_GetConnectorDefaultDiscoveryConfiguration / @ref SA_GetConnectorPendingDiscoveryConfiguration / @ref SA_GetConnectorActiveDiscoveryConfiguration.
 * Input to @ref SA_CanChangeConnectorDiscoveryProtocol / @ref SA_DoChangeConnectorDiscoveryProtocol.
 * @{ */
#define kConnectorDiscoveryProtocolAutoDetect          0 /**< Connector will auto detect backplanes. */
#define kConnectorDiscoveryProtocolUBM                 1 /**< Connector will detect UBM backplanes. */
#define kConnectorDiscoveryProtocolSGPIO               2 /**< Connector will detect SGPIO backplanes. */
#define kConnectorDiscoveryProtocolVPP                 6 /**< Connector will detect VPP backplanes. */
#define kConnectorDiscoveryProtocolDirectAttachedCable 7 /**< Connector will detect direct-attached cable. */
#define kConnectorDiscoveryProtocolUnknown          0xFF /**< Connector has an unknown discovery protocol. */
/** @} */ /* Connector Discovery Protocol */

/** @name Connector Direct-Attached Cable Target Count
 * Value for @ref CONNECTOR_DISCOVERY_CONFIGURATION::target_count.
 * @{ */
#define kConnectorDirectAttachTargetCountNotApplicable      0 /**< Target count is not applicable because the port discovery protocol is
                                                                   not @ref kConnectorDiscoveryProtocolDirectAttachedCable or the query is for
                                                                   @ref SA_GetConnectorDefaultDiscoveryConfiguration. */
#define kConnectorDirectAttachTargetCountUnknown         0xFF /**< Target count is invalid. */
/** @} */

/** Used to gather per connector discovery configuration information. */
struct _CONNECTOR_DISCOVERY_CONFIGURATION
{
   SA_BYTE discovery_protocol;   /**< Connector's discovery protocol. See [Connector Discovery Protocol](@ref storc_properties_ctrl_port_discovery_configuration). */
   SA_BYTE target_count;         /**< Connector's direct-attached cable target count. See [Connector Direct-Attached Cable Target Count](@ref storc_properties_ctrl_port_discovery_configuration).*/
};
typedef struct _CONNECTOR_DISCOVERY_CONFIGURATION CONNECTOR_DISCOVERY_CONFIGURATION;
typedef struct _CONNECTOR_DISCOVERY_CONFIGURATION *PCONNECTOR_DISCOVERY_CONFIGURATION;

/** @name Get Connector Discovery Configuration Returns
 * @outputof SA_GetConnectorDefaultDiscoveryConfiguration.
 * @outputof SA_GetConnectorPendingDiscoveryConfiguration.
 * @outputof SA_GetConnectorActiveDiscoveryConfiguration.
 * @{ */
/** Success in getting connector's discovery configuration information.
 * @outputof SA_GetConnectorDefaultDiscoveryConfiguration.
 * @outputof SA_GetConnectorPendingDiscoveryConfiguration.
 * @outputof SA_GetConnectorActiveDiscoveryConfiguration.
 */
#define kGetConnectorDiscoveryConfigurationSuccess                 0
/** Invalid argument.
 * @outputof SA_GetConnectorDefaultDiscoveryConfiguration.
 * @outputof SA_GetConnectorPendingDiscoveryConfiguration.
 * @outputof SA_GetConnectorActiveDiscoveryConfiguration.
 */
#define kGetConnectorDiscoveryConfigurationInvalidArgument         1
/** The command is not supported.
 * @outputof SA_GetConnectorDefaultDiscoveryConfiguration.
 * @outputof SA_GetConnectorPendingDiscoveryConfiguration.
 * @outputof SA_GetConnectorActiveDiscoveryConfiguration.
 */
#define kGetConnectorDiscoveryConfigurationNotSupported            2
/** Invalid connector index.
 * @outputof SA_GetConnectorDefaultDiscoveryConfiguration.
 * @outputof SA_GetConnectorPendingDiscoveryConfiguration.
 * @outputof SA_GetConnectorActiveDiscoveryConfiguration.
 */
#define kGetConnectorDiscoveryConfigurationInvalidConnectorIndex   3
/** Failed to get connector's discovery configuration information.
 * @outputof SA_GetConnectorDefaultDiscoveryConfiguration.
 * @outputof SA_GetConnectorPendingDiscoveryConfiguration.
 * @outputof SA_GetConnectorActiveDiscoveryConfiguration.
 */
#define kGetConnectorDiscoveryConfigurationFailed                  4
/** @} */ /* Get Connector Discovery Configuration Returns */

/** Returns connector's default discovery configuration information.
 * @pre `connector_index` references an existing connector. Make sure connector index is in the range of
 *       [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @pre @ref kControllerGetDefaultConnectorDiscoveryConfigurationSupported is set by @ref SA_GetControllerConnectorDiscoveryConfigurationSupport.
 * @param[in] cda                 Controller data area.
 * @param[in] connector_index     Index of target connector on controller.
 * @param[out] connector_discovery_config  Address of buffer to store connector's default discovery configuration info.
 * @return See [Get Connector Discovery Configuration Returns] (@ref storc_properties_ctrl_port_discovery_configuration)
 *
 * __Example__
 * @code
 * SA_WORD connector_index;
 * FOREACH_CONNECTOR(cda, connector_index)
 * {
 *    CONNECTOR_DISCOVERY_CONFIGURATION conn_discovery_config;
 *    SA_BYTE result = SA_GetConnectorDefaultDiscoveryConfiguration(cda, connector_index, &conn_discovery_config);
 *    if (result == kGetConnectorDiscoveryConfigurationSuccess)
 *    {
 *       // Print CONNECTOR_DISCOVERY_CONFIGURATION structure fields.
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_GetConnectorDefaultDiscoveryConfiguration(PCDA cda, SA_WORD connector_index, CONNECTOR_DISCOVERY_CONFIGURATION* connector_discovery_config);

/** Returns connector's pending discovery configuration information.
 * @pre `connector_index` references an existing connector. Make sure connector index is in the range of
 *       [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @pre @ref kControllerGetPendingConnectorDiscoveryConfigurationSupported is set by @ref SA_GetControllerConnectorDiscoveryConfigurationSupport.
 * @param[in] cda                      Controller data area.
 * @param[in] connector_index          Index of target connector on controller.
 * @param[out] connector_discovery_config  Address of buffer to store connector's pending discovery configuration info.
 * @return See [Get Connector Discovery Configuration Returns] (@ref storc_properties_ctrl_port_discovery_configuration)
 *
 * __Example__
 * @code
 * SA_WORD connector_index;
 * FOREACH_CONNECTOR(cda, connector_index)
 * {
 *    CONNECTOR_DISCOVERY_CONFIGURATION conn_discovery_config;
 *    SA_BYTE result = SA_GetConnectorPendingDiscoveryConfiguration(cda, connector_index, &conn_discovery_config);
 *    if (result == kGetConnectorDiscoveryConfigurationSuccess)
 *    {
 *       // Print CONNECTOR_DISCOVERY_CONFIGURATION structure fields.
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_GetConnectorPendingDiscoveryConfiguration(PCDA cda, SA_WORD connector_index, CONNECTOR_DISCOVERY_CONFIGURATION* connector_discovery_config);

/** Returns connector's active discovery configuration information.
 * @pre `connector_index` references an existing connector. Make sure connector index is in the range of
 *       [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @pre @ref kControllerGetActiveConnectorDiscoveryConfigurationSupported is set by @ref SA_GetControllerConnectorDiscoveryConfigurationSupport.
 * @param[in] cda                 Controller data area.
 * @param[in] connector_index     Index of target connector on controller.
 * @param[out] connector_discovery_config  Address of buffer to store connector's active discovery configuration info.
 * @return See [Get Connector Discovery Configuration Returns] (@ref storc_properties_ctrl_port_discovery_configuration)
 *
 * __Example__
 * @code
 * SA_WORD connector_index;
 * FOREACH_CONNECTOR(cda, connector_index)
 * {
 *    CONNECTOR_DISCOVERY_CONFIGURATION conn_discovery_config;
 *    SA_BYTE result = SA_GetConnectorActiveDiscoveryConfiguration(cda, connector_index, &conn_discovery_config);
 *    if (result == kGetConnectorDiscoveryConfigurationSuccess)
 *    {
 *       // Print CONNECTOR_DISCOVERY_CONFIGURATION structure fields.
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_GetConnectorActiveDiscoveryConfiguration(PCDA cda, SA_WORD connector_index, CONNECTOR_DISCOVERY_CONFIGURATION* connector_discovery_config);

/** Returns connector's supported discovery protocol information.
 * @pre `connector_index` references an existing connector. Make sure connector index is in the range of
 *       [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @param[in] cda                 Controller data area.
 * @param[in] connector_index     Index of target connector on controller.
 * @return Connector discovery protocol support info (see [Get Connector Discovery Protocol Support Returns] (@ref storc_properties_ctrl_port_discovery_configuration)).
 *
 * __Example__
 * @code
 * SA_WORD connector_index;
 * FOREACH_CONNECTOR(cda, connector_index)
 * {
 *    SA_BYTE result = SA_GetConnectorDiscoveryProtocolSupport(PCDA cda, SA_WORD connector_index);
 *    printf("Connector Discovery Protocol Support:\n", indent, " ");
 *    printf("   Auto Detect      : %s\n", (result & kConnectorSupportsDiscoveryProtocolAutoDetect) ? "Supported" : "Not Supported");
 *    printf("   UBM Backplanes   : %s\n", (result & kConnectorSupportsDiscoveryProtocolUBM) ? "Supported" : "Not Supported");
 *    printf("   SGPIO Backplanes : %s\n", (result & kConnectorSupportsDiscoveryProtocolSGPIO) ? "Supported" : "Not Supported");
 * }
 * @endcode
 */
SA_BYTE SA_GetConnectorDiscoveryProtocolSupport(PCDA cda, SA_WORD connector_index);
   /** @name Get Connector Discovery Protocol Support Returns
    * @outputof SA_GetConnectorDiscoveryProtocolSupport.
    * @{ */
   /** Connector supports auto detecting backplane.
    * @outputof SA_GetConnectorDiscoveryProtocolSupport.
    */
   #define kConnectorSupportsDiscoveryProtocolAutoDetect          0x01
   /** Connector supports detecting UBM backplanes.
    * @outputof SA_GetConnectorDiscoveryProtocolSupport.
    */
   #define kConnectorSupportsDiscoveryProtocolUBM                 0x02
   /** Connector supports detecting SGPIO backplanes.
    * @outputof SA_GetConnectorDiscoveryProtocolSupport.
    */
   #define kConnectorSupportsDiscoveryProtocolSGPIO               0x04
   /** Connector supports detecting VPP backplanes.
    * @outputof SA_GetConnectorDiscoveryProtocolSupport.
    */
   #define kConnectorSupportsDiscoveryProtocolVPP                 0x40
   /** Connector supports direct-attached cable backplanes.
    * @outputof SA_GetConnectorDiscoveryProtocolSupport.
    */
   #define kConnectorSupportsDiscoveryProtocolDirectAttachedCable 0x80
   /** @} */ /* Get Connector Discovery Protocol Support Returns */
/** @} */ /* storc_properties_ctrl_port_discovery_configuration*/

/** @defgroup storc_properties_ctrl_port_aggregated_speed Controller Port Aggregated Speed
 * @brief Get aggregated speed for controller's port/connector.
 * @{ */
/** Returns connector's aggregate speed in Gbps.
 * @pre `connector_index` references an existing connector. Make sure connector index is in the range of
 *       [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @param[in] cda                 Controller data area.
 * @param[in] connector_index     Index of target connector on controller.
 * @param[in] speed_type          Type of speed to aggregate see [Connector Aggregate Speed Type](@ref storc_properties_ctrl_port_aggregated_speed).
 * @return @ref CONNECTOR_LINK_RATE_SIGNIFICAND to get the integer portion of the aggregate speed.
 *         @ref CONNECTOR_LINK_RATE_EXPONENT to get the exponent of the aggregate speed.
 *         In case of error returns ZERO.
 *
 * __Example__
 * @code
 * SA_WORD connector_index;
 * FOREACH_CONNECTOR(cda, connector_index)
 * {
 *    SA_DWORD log_connector_speed = SA_GetConnectorAggregateSpeedGbps(cda, connector_index, kConnectorAggregateSpeedNegotiatedLogical);
 *    SA_DWORD phy_connector_speed = SA_GetConnectorAggregateSpeedGbps(cda, connector_index, kConnectorAggregateSpeedNegotiatedPhysical);
 *    SA_DWORD max_connector_speed = SA_GetConnectorAggregateSpeedGbps(cda, connector_index, kConnectorAggregateSpeedMax);
 *    printf("Negotiated Logical Connector speed: %uE-%u Gbps\n", CONNECTOR_LINK_RATE_SIGNIFICAND(log_connector_speed), CONNECTOR_LINK_RATE_EXPONENT(log_connector_speed));
 *    printf("Negotiated Physical Connector speed: %uE-%u Gbps\n", CONNECTOR_LINK_RATE_SIGNIFICAND(pjy_connector_speed), CONNECTOR_LINK_RATE_EXPONENT(phy_connector_speed));
 *    printf("Max Connector speed: %uE-%u Gbps\n", CONNECTOR_LINK_RATE_SIGNIFICAND(max_connector_speed), CONNECTOR_LINK_RATE_EXPONENT(max_connector_speed));
 * }
 * @endcode
 */
SA_DWORD SA_GetConnectorAggregateSpeedGbps(PCDA cda, SA_WORD connector_index, SA_BYTE speed_type);
/** CONNECTOR_LINK_RATE_SIGNIFICAND
 * Contains the integer portion of the aggregate speed used when calculating the aggregate speed
 * @outputof SA_GetConnectorAggregateSpeedGbps.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kGetConnectorAggregateSpeedGbpsSignificandMask  0xFFFF0000
#define kGetConnectorAggregateSpeedGbpsSignificandBitshift  16
#endif
#define CONNECTOR_LINK_RATE_SIGNIFICAND(info_value) (((info_value) & kGetConnectorAggregateSpeedGbpsSignificandMask) >> kGetConnectorAggregateSpeedGbpsSignificandBitshift)
/** CONNECTOR_LINK_RATE_EXPONENT
 * Contains the negative exponent used when calculating the aggregate speed
 * @outputof SA_GetConnectorAggregateSpeedGbps.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kGetConnectorAggregateSpeedGbpsExponentMask  0x0000FFFF
#define kGetConnectorAggregateSpeedGbpsExponentBitshift  0
#endif
#define CONNECTOR_LINK_RATE_EXPONENT(info_value) (((info_value) & kGetConnectorAggregateSpeedGbpsExponentMask) >> kGetConnectorAggregateSpeedGbpsExponentBitshift)
/** Connector Aggregate Speed Type
 */
#define kConnectorAggregateSpeedNegotiatedLogical           0 /**< Aggregate negotiated logical connector speed. */
#define kConnectorAggregateSpeedNegotiatedPhysical          1 /**< Aggregate negotiated physial connector speed. */
#define kConnectorAggregateSpeedMax                         2 /**< Aggregate max connector speed. */
/** @} */ /* Connector Aggregate Speed Type */
/** @} */ /* storc_properties_ctrl_port_aggregated_speed*/

/** @defgroup storc_properties_ctrl_port_phy_count Controller Port Phy Count
 * @brief Get the number of phys on a controller's port/connector.
 * @{ */
/** Returns connector's phy count.
 * @pre `connector_index` references an existing connector. Make sure connector index is in the range of
 *       [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @param[in] cda                 Controller data area.
 * @param[in] connector_index     Index of target connector on controller.
 * @return Connector total phy count
 */
SA_BYTE SA_GetConnectorPhyCount(PCDA cda, SA_WORD connector_index);
/** @} */ /* storc_properties_ctrl_port_phy_count*/

/** @defgroup storc_properties_ctrl_port_DIRECT_ATTACHED_CABLE_TARGET_count_support Controller Port Direct-Attached Cable Target Count Support
 * @brief Get the number of direct-attached cable targets supported for controller's port/connector.
 * @{ */
/** Returns connector's direct-attached cable target count support.
 * @pre `connector_index` references an existing connector. Make sure connector index is in the range of
 *       [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @param[in] cda                 Controller data area.
 * @param[in] connector_index     Index of target connector on controller.
 * @return Decode with @ref CONNECTOR_SUPPORTS_ONE_DIRECT_ATTACHED_CABLE_TARGET to get the boolean return of whether the connector supports one direct-attached cable target.
 * @return Decode with @ref CONNECTOR_SUPPORTS_TWO_DIRECT_ATTACHED_CABLE_TARGETS to get the boolean return of whether the connector supports two direct-attached cable targets.
 * @return Decode with @ref CONNECTOR_SUPPORTS_FOUR_DIRECT_ATTACHED_CABLE_TARGETS to get the boolean return of whether the connector supports four direct-attached cable targets.
 * @return Decode with @ref CONNECTOR_SUPPORTS_EIGHT_DIRECT_ATTACHED_CABLE_TARGETS to get the boolean return of whether the connector supports eight direct-attached cable targets.
 *
 * __Example__
 * @code
 * SA_WORD connector_index;
 * FOREACH_CONNECTOR(cda, connector_index)
 * {
 *    SA_BYTE count_support = SA_GetConnectorDirectAttachTargetCountSupport(cda, connector_index);
 *    if (CONNECTOR_SUPPORTS_ONE_DIRECT_ATTACHED_CABLE_TARGET(count_support))
 *    {
 *       printf("The connector supports one direct-attached cable target.\n");
 *    }
 *    if (CONNECTOR_SUPPORTS_ONE_DIRECT_ATTACHED_CABLE_TARGET(count_support))
 *    {
 *       printf("The connector supports two direct-attached cable targets.\n");
 *    }
 *    if (CONNECTOR_SUPPORTS_FOUR_DIRECT_ATTACHED_CABLE_TARGETS(count_support))
 *    {
 *       printf("The connector supports four direct-attached cable targets.\n");
 *    }
 *    if (CONNECTOR_SUPPORTS_EIGHT_DIRECT_ATTACHED_CABLE_TARGETS(count_support))
 *    {
 *       printf("The connector supports eight direct-attached cable targets.\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_GetConnectorDirectAttachTargetCountSupport(PCDA cda, SA_WORD connector_index);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kConnectorSupportsTargetCount1           0x01 /**< Connector supports a direct-atached cable target count of 1. */
#define kConnectorSupportsTargetCount2           0x02 /**< Connector supports a direct-atached cable target count of 2. */
#define kConnectorSupportsTargetCount4           0x04 /**< Connector supports a direct-atached cable target count of 4. */
#define kConnectorSupportsTargetCount8           0x08 /**< Connector supports a direct-atached cable target count of 8. */
#endif
/** Decode output of @ref SA_GetConnectorDirectAttachTargetCountSupport to determine if the connector supports one direct-attached cable target. */
#define CONNECTOR_SUPPORTS_ONE_DIRECT_ATTACHED_CABLE_TARGET(info_value) ((SA_BOOL)(((info_value) & kConnectorSupportsTargetCount1) ? kTrue : kFalse))
/** Decode output of @ref SA_GetConnectorDirectAttachTargetCountSupport to determine if the connector supports two direct-attached cable targets. */
#define CONNECTOR_SUPPORTS_TWO_DIRECT_ATTACHED_CABLE_TARGETS(info_value) ((SA_BOOL)(((info_value) & kConnectorSupportsTargetCount2) ? kTrue : kFalse))
/** Decode output of @ref SA_GetConnectorDirectAttachTargetCountSupport to determine if the connector supports four direct-attached cable targets. */
#define CONNECTOR_SUPPORTS_FOUR_DIRECT_ATTACHED_CABLE_TARGETS(info_value) ((SA_BOOL)(((info_value) & kConnectorSupportsTargetCount4) ? kTrue : kFalse))
/** Decode output of @ref SA_GetConnectorDirectAttachTargetCountSupport to determine if the connector supports eight direct-attached cable targets. */
#define CONNECTOR_SUPPORTS_EIGHT_DIRECT_ATTACHED_CABLE_TARGETS(info_value) ((SA_BOOL)(((info_value) & kConnectorSupportsTargetCount8) ? kTrue : kFalse))
/** @} */ /* storc_properties_ctrl_port_DIRECT_ATTACHED_CABLE_TARGET_count_support */

/** @} */ /* storc_properties_ctrl_port */

/******************************************//**
 * @defgroup storc_properties_ld Logical Drive
 * @brief
 * @details
 * @{
 *********************************************/

/** @defgroup storc_properties_ld_bootable_enum Enumerate Bootable Logical Drives
 * @{
 */
/** Start enumeration of controller bootable logical drives.
 * Start with @ref kInvalidLDNumber as the first number.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Target logical drive number.
 * @return The next valid logical drive number, or kInvalidLDNumber if no more drives exist.
 */
SA_WORD SA_EnumerateControllerBootableLogicalDrives(PCDA cda, SA_WORD ld_number);

/** Iterate over all controller bootable logical drives and execute the block with each valid ld_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] ld_number     Variable to store valid logical drive numbers.
 *
 * __Example__
 * @code
 * SA_WORD ld_number;
 * FOREACH_BOOTABLE_LOGICAL_DRIVE(cda, ld_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_BOOTABLE_LOGICAL_DRIVE(cda, ld_number) \
   for (ld_number = kInvalidLDNumber; (ld_number = SA_EnumerateControllerBootableLogicalDrives(cda, ld_number)) != kInvalidLDNumber;)
/** @} */ /* storc_properties_ld_bootable_enum */

/** @defgroup storc_properties_ld_enum Enumerate Logical Drives
 * @{
 */
/** Start enumeration of controller logical drives.
 * Start with @ref kInvalidLDNumber as the first number.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Target logical drive number.
 * @return The next valid logical drive number, or kInvalidLDNumber if no more drives exist.
 */
SA_WORD SA_EnumerateControllerLogicalDrives(PCDA cda, SA_WORD ld_number);
   /** @name Logical Drive Enumerate Returns
    * @{ */
   #define kInvalidLDNumber 0xFFFF /**< Invalid logical drive number. */
   /** @} */

/** Iterate over all controller logical drives and execute the block with each valid ld_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] ld_number     Variable to store valid logical drive numbers.
 *
 * __Example__
 * @code
 * SA_WORD ld_number;
 * FOREACH_LOGICAL_DRIVE(cda, ld_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_LOGICAL_DRIVE(cda, ld_number) \
   for (ld_number = kInvalidLDNumber; (ld_number = SA_EnumerateControllerLogicalDrives(cda, ld_number)) != kInvalidLDNumber;)
/** @} */ /* storc_properties_ld_enum */

/** @defgroup storc_properties_ld_array_ld_number_from_index Array Logical Drive Number From Index
 * @{ */
 /** @return Logical drive number (LUN) for the `ld_index`-th logical drive in the `array_index`-th Array.
  * @pre `array_index` is 0-based index to an existing Array.
  * @pre `ld_index` is 0-based index to an existing logical drive of the Array.
  * @param[in] cda          Controller data area.
  * @param[in] array_index  Index to an Array on the Controller.
  * @param[in] ld_index     Index to a logical drive of the Array.
  * @return Logical drive number or @ref kInvalidLDNumber if the logical drive was not found.
  */
SA_WORD SA_GetArrayLogicalDriveNumberFromIndex(PCDA cda, SA_WORD array_index, SA_WORD ld_index);
/** @} */ /* storc_properties_ld_array_ld_number_from_index */

/** @defgroup storc_properties_ld_array_number_from_ld_number Array Number From Logical Drive Number
 * @{ */
/** Returns the Array number of a Logical Drive.
 * @pre `ld_number` is a valid logical drive number.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Target logical drive number.
 * @return Valid Array number or @ref kInvalidArrayNumber.
 */
SA_WORD SA_GetLogicalDriveArray(PCDA cda, SA_WORD ld_number);
/** @} */ /* storc_properties_ld_array_number_from_ld_number */

/** @defgroup storc_properties_ld_split_sibling Logical Drive Split Mirror Sibling
 * @{ */
/** Get the logical drive number of a logical drive's split mirror primary or backup pair.
 * @param[in] cda        @ref storpage_module_storagecore_cda
 * @param[in] ld_number  Split mirror primary or backup logical drive number.
 * @return If `ld_number` is a split mirror primary, return the logical drive number of the split mirror backup.
 * @return If `ld_number` is a split mirror backup, return the logical drive number of the split mirror primary.
 * @return If `ld_number` is a split mirror orphan (or not a split mirror logical drive), return @ref kInvalidLDNumber.
 */
SA_WORD SA_GetLogicalDriveSplitMirrorNumber(PCDA cda, SA_WORD ld_number);
/** @} */ /* storc_properties_ld_split_sibling */

/** @defgroup storc_properties_lu_ld LU Cache Properties
 * @brief Get properties/statistics of a data/cache logical drive pair.
 * @details
 * @see storc_features_ctrl_lu_cache
 * @see storc_status_ld_lu_cache_status
 * @see storc_configuration_edit_lu_cache
 * @{ */

/** @defgroup storc_properties_lu_ld_cache_number LU Cache Number
 * @brief Get the "other" logical drive of a data/cache logical drive pair.
 * @{ */
/** Returns the logical drive number of either the caching or LU cached logical drive.
 * @pre `ld_number` is a valid logical drive number of either a LU cached or caching logical drive.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Current logical drive which is either a LU cached or caching logical drive.
 * @return If `ld_number` is a data logical drive with a LU cached logical drive, returns the logical drive number of the caching logical drive.
 * @return If `ld_number` is a caching logical drive, returns the logical drive number of the cached (data) logical drive.
 * @return If `ld_number` is not a valid LU cached or caching logical drive, returns @ref kInvalidLDNumber.
 */
SA_WORD SA_GetLogicalDriveLUCacheNumber(PCDA cda, SA_WORD ld_number);
/** @} */ /* storc_properties_lu_ld_cache_number */

/** @defgroup storc_properties_lu_cache_info LU Cache Info
 * @brief Get caching information of a data/cache logical drive pair.
 * @details
 * @see storc_properties_lu_cache_stats
 * @{ */
/** Returns information of a LU cache volume.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive which is either a LU cached or caching logical drive.
 * @return Information on an LU cache's algorithm (decode with @ref LU_CACHE_ALGORITHM at [LU Cache Info](@ref storc_properties_lu_cache_info)).
 * @return Information on an LU cache's actual write policy (decode with @ref LU_CACHE_ACTUAL_WRITE_POLICY at [LU Cache Info](@ref storc_properties_lu_cache_info)).
 * @return Information on an LU cache's requested write policy (decode with @ref LU_CACHE_REQUESTED_WRITE_POLICY at [LU Cache Info](@ref storc_properties_lu_cache_info)).
 * @return Information on an LU cache's cache line size (decode with @ref LU_CACHE_LINE_SIZE at [LU Cache Info](@ref storc_properties_lu_cache_info)).
 */
SA_WORD SA_GetLogicalDriveLUCacheInfo(PCDA cda, SA_WORD ld_number);
   /** LU cache algorithm hasing LRU; Output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info). */
   #define kLUCacheAlgorithmHashingLRU              0x0001
   /** LU cache algorithm hasing LRU2; Output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info). */
   #define kLUCacheAlgorithmHashingLRU2             0x0002
   /** LU cache algorithm regional; Output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info). */
   #define kLUCacheAlgorithmRegional                0x0003
   /** LU cache algorithm simple LRU; Output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info). */
   #define kLUCacheAlgorithmSimpleLRU               0x0004

   /** Used by @ref LU_CACHE_ALGORITHM to decode @ref SA_GetLogicalDriveLUCacheInfo. */
   #define kLUCacheAlgorithmMask                    0x0007
   /** Used by @ref LU_CACHE_ALGORITHM to decode @ref SA_GetLogicalDriveLUCacheInfo. */
   #define kLUCacheAlgorithmBitshift                0
   /** Decode LU cache algorithm from output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info). */
   #define LU_CACHE_ALGORITHM(info) ((info & kLUCacheAlgorithmMask) >> kLUCacheAlgorithmBitshift)

   /** LU cache write policy read-only; Output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info). */
   #define kLUCacheWritePolicyReadOnly              0x0001
   /** LU cache write policy write-through; Output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info). */
   #define kLUCacheWritePolicyWriteThru             0x0002
   /** LU cache write policy write-back; Output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info). */
   #define kLUCacheWritePolicyWriteBack             0x0003

   /** Used by @ref LU_CACHE_ACTUAL_WRITE_POLICY to decode @ref SA_GetLogicalDriveLUCacheInfo. */
   #define kLUCacheActualWritePolicyMask            0x0018
   /** Used by @ref LU_CACHE_ACTUAL_WRITE_POLICY to decode @ref SA_GetLogicalDriveLUCacheInfo. */
   #define kLUCacheActualWritePolicyBitshift        3
   /** Decode LU cache write policy from output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info). */
   #define LU_CACHE_ACTUAL_WRITE_POLICY(info) ((info & kLUCacheActualWritePolicyMask) >> kLUCacheActualWritePolicyBitshift)

   /** Used by @ref LU_CACHE_REQUESTED_WRITE_POLICY to decode @ref SA_GetLogicalDriveLUCacheInfo. */
   #define kLUCacheRequestedWritePolicyMask         0x0060
   /** Used by @ref LU_CACHE_REQUESTED_WRITE_POLICY to decode @ref SA_GetLogicalDriveLUCacheInfo. */
   #define kLUCacheRequestedWritePolicyBitshift     5
   /** Decode LU cache write policy from output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info). */
   #define LU_CACHE_REQUESTED_WRITE_POLICY(info) ((info & kLUCacheRequestedWritePolicyMask) >> kLUCacheRequestedWritePolicyBitshift)

   /** Used by @ref LU_CACHE_LINE_SIZE to decode @ref SA_GetLogicalDriveLUCacheInfo. */
   #define kLUCacheCacheLineSizeMask                0x0700
   /** Used by @ref LU_CACHE_LINE_SIZE to decode @ref SA_GetLogicalDriveLUCacheInfo. */
   #define kLUCacheCacheLineSizeBitshift            8
   /** Decode LU cache cache line size from output of [SA_GetLogicalDriveLUCacheInfo](@ref storc_properties_lu_cache_info).
    * Values for lu cache line sizes at [LU Cache Line Sizes](@ref storc_features_ctrl_lu_cache).
    */
   #define LU_CACHE_LINE_SIZE(info) ((info & kLUCacheCacheLineSizeMask) >> kLUCacheCacheLineSizeBitshift)
/** @} */ /* storc_properties_lu_cache_info */

/** @defgroup storc_properties_lu_cache_stats LU Cache Statistics
 * @brief Get statistic information of data/cache logical drive pair.
 * @details
 * @see storc_properties_lu_cache_info
 * @{ */
/** LU cache statistics populated by @ref SA_GetLogicalDriveLUCacheStats. */
struct _SA_LU_CACHE_STATISTICS
{
   /** Number of hits in the drive cache */
   SA_QWORD read_cache_hits;
   /** Number of times a cache request missed cache
    * but prompted a load of data from the primary storage to the drive cache. */
   SA_QWORD read_cache_misses_do_load;
   /** Number of times a cache request was not serviced right away because the data was loading. */
   SA_QWORD read_cache_misses_loading;
   /** Number of times a cache request was serviced by primary storage and not the drive cache,
    *  excluding those that triggered loads or where a load was already taking place. */
   SA_QWORD read_cache_misses_skip;
   /** Number of read cache misses due to primary LUN being written. */
   SA_QWORD read_cache_misses_locked;
   /** Total number of read cache misses for all reasons. */
   SA_QWORD read_cache_misses_total;

   /** Number of hits in the drive cache. */
   SA_QWORD write_cache_hits;
   /** Number of times a cache request missed cache
    *  but prompted a load of data from the primary storage to the drive cache. */
   SA_QWORD write_cache_misses_do_load;
   /** Number of times a cache request was not serviced right away because the data was loading. */
   SA_QWORD write_cache_misses_loading;
   /** Number of times a cache request was serviced by primary storage and not the drive cache,
    *  excluding those that triggered loads or where a load was already taking place. */
   SA_QWORD write_cache_misses_skip;
   /** Number of write cache misses due to primary LUN being written. */
   SA_QWORD write_cache_misses_locked;
   /** Total number of write cache misses for all reasons. */
   SA_QWORD write_cache_misses_total;
};
typedef struct _SA_LU_CACHE_STATISTICS SA_LU_CACHE_STATISTICS;
typedef struct _SA_LU_CACHE_STATISTICS *PSA_LU_CACHE_STATISTICS;

/** Return current cache statistics (number of cache hits/misses) for a given LU cache logical drive.
 * @pre Targeted on a valid LU cache or cached data logical drive.
 * @post If returns @ref kFalse, the contents of `lu_stats` are undefined.
 *
 * @param[in] cda       Controller data area.
 * @param[in] ld_number Target LU cache logical drive or LU cached data logical drive number.
 * @param[out] lu_stats Address to instance of @ref _SA_LU_CACHE_STATISTICS to populate stats (must not be NULL).
 * @return @ref kTrue iff `ld_number` is a valid LU cache/cached logical drive and `lu_stats` was writeable, @ref kFalse otherwise.
 */
SA_BOOL SA_GetLogicalDriveLUCacheStats(PCDA cda, SA_WORD ld_number, PSA_LU_CACHE_STATISTICS lu_stats);
/** @} */ /* storc_properties_lu_cache_stats */

/** @} */ /* storc_properties_lu_ld */

/** @defgroup storc_properties_ld_count Logical Drive Counts
 * @{ */
/** Get number of logical drives of the controller.
 * @param[in] cda  Controller data area.
 * @return Logical drive counts (decode with [Logical Drive Counts Returns](@ref storc_properties_ld_count))
 * @return [Number of data logical drives](@ref NUM_DATA_LOGICAL_DRIVES)
 * @return [Number of cache logical drives](@ref NUM_CACHE_LOGICAL_DRIVES)
 * @return [Total number of logical drives](@ref NUM_LOGICAL_DRIVES)
 */
SA_DWORD SA_GetControllerLogicalDriveCounts(PCDA cda);
   /** @name Logical Drive Counts Returns
    * @outputof SA_GetControllerLogicalDriveCounts.
    * @{ */
   /** Mask to use on output of @ref SA_GetControllerLogicalDriveCounts to get the number of data logical drives of the controller. */
   #define kDataLogicalDriveCountMask      0x00000FFF
   /** Bitshift to use with output of @ref SA_GetControllerLogicalDriveCounts (after masking with @ref kDataLogicalDriveCountMask) to get the number of data logical drives of the controller. */
   #define kDataLogicalDriveCountBitShift  0
   /** Decode output of @ref SA_GetControllerLogicalDriveCounts to get number of data logical drives. */
   #define NUM_DATA_LOGICAL_DRIVES(info_value) ((info_value & kDataLogicalDriveCountMask ) >> kDataLogicalDriveCountBitShift)

   /** Mask to use on output of @ref SA_GetControllerLogicalDriveCounts to get the number of cache logical drives of the controller. */
   #define kCacheLogicalDriveCountMask     0x00FFF000
   /** Bitshift to use with output of @ref SA_GetControllerLogicalDriveCounts (after masking with @ref kDataLogicalDriveCountMask) to get the number of cache logical drives of the controller. */
   #define kCacheLogicalDriveCountBitShift 12
   /** Decode output of @ref SA_GetControllerLogicalDriveCounts to get number of cache logical drives. */
   #define NUM_CACHE_LOGICAL_DRIVES(info_value) ((info_value & kCacheLogicalDriveCountMask ) >> kCacheLogicalDriveCountBitShift)

   /** Decode output of @ref SA_GetControllerLogicalDriveCounts to get total number of logical drives. */
   #define NUM_LOGICAL_DRIVES(info_value) NUM_DATA_LOGICAL_DRIVES(info_value)+NUM_CACHE_LOGICAL_DRIVES(info_value)
   /** @} */
/** @} */

/** @defgroup storc_properties_ld_parity_group_count Logical Drive Parity Group Count
 * @see storc_properties_ld_raid_info
 * @see SA_GetLogicalDriveRAIDInfo
 * @see NUM_PARITY_GROUPS
 */

/** @defgroup storc_properties_ld_raid_info Logical Drive RAID
 * @ingroup storc_properties_ld_parity_group_count
 * @{ */
/** Get RAID level, parity group count, and strip size for selected logical drive.
 * RAID levels are defined under @ref SA_ControllerSupportsRAIDLevel
 * @pre `ld_number` is a valid logical drive number.
 * @see SA_ControllerSupportsRAIDLevel
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Target logical drive number.
 * @return [Get Logical Drive RAID Returns](@ref storc_properties_ld_raid_info).
 * @return Decode with @ref RAID_LEVEL to get logical drive RAID level.
 * @return Decode with @ref NUM_PARITY_GROUPS to get logical drive parity groups.
 */
SA_WORD SA_GetLogicalDriveRAIDInfo(PCDA cda, SA_WORD ld_number);
   /** @name Get Logical Drive RAID Returns
    * @outputof SA_GetLogicalDriveRAIDInfo.
    * @{ */
   #define kRAIDLevelMask 0xFF  /**< Mask to use on output of @ref SA_GetLogicalDriveRAIDInfo to get logical drive RAID level. */
   #define kRAIDLevelBitShift 0 /**< Bitshift to use on output of @ref SA_GetLogicalDriveRAIDInfo to get logical drive RAID level. */
   #define kInvalidRAID  0xFF   /**< Invalid RAID level. */
   /** Use to decode output of @ref SA_GetLogicalDriveRAIDInfo to get the RAID level of the logical drive. */
   #define RAID_LEVEL(info_value) (info_value & kRAIDLevelMask)

   #define kParityGroupCountMask 0xFF00 /**< Mask to use on output of @ref SA_GetLogicalDriveRAIDInfo to get parity groups. */
   #define kParityGroupCountBitShift 8  /**< Bitshift to use on output of @ref SA_GetLogicalDriveRAIDInfo to get parity groups. */
   /** Use to decode output of @ref SA_GetLogicalDriveRAIDInfo to get the parity groups of the logical drive. */
   #define NUM_PARITY_GROUPS(info_value) ((info_value & kParityGroupCountMask) >> kParityGroupCountBitShift )
   /** @} */
/** @} */

/** @defgroup storc_properties_ld_basic_info Logical Drive Basic Info
 * @{ */
/** Get basic information of the logical drive.
 * @pre `ld_number` is a valid logical drive number.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Target logical drive number.
 * @return [Get Logical Drive Basic Info Returns](@ref storc_properties_ld_basic_info).
 * @return Decode with @ref LOGICAL_DRIVE_TYPE to get logical drive type.
 * @return Decode with @ref LOGICAL_DRIVE_ACCELERATOR to get logical drive accelerator.
 */
SA_DWORD SA_GetLogicalDriveBasicInfo(PCDA cda, SA_WORD ld_number);
   /** @name Get Logical Drive Basic Info Returns
    * @{ */
   #define kLogicalDriveTypeMask                    0x0F /**< Mask to use on output of @ref SA_GetLogicalDriveBasicInfo to get logical drive type. */
   #define kLogicalDriveTypeData                    0x01 /**< Logical drive type: data; @outputof SA_GetLogicalDriveBasicInfo after masking @ref kLogicalDriveTypeMask. */
   #define kLogicalDriveTypeCache                   0x02 /**< Logical drive type: cache; @outputof SA_GetLogicalDriveBasicInfo after masking @ref kLogicalDriveTypeMask. */
   #define kLogicalDriveTypeSplitMirrorSetPrimary   0x03 /**< Logical drive type: split mirror primary; @outputof SA_GetLogicalDriveBasicInfo after masking @ref kLogicalDriveTypeMask. */
   #define kLogicalDriveTypeSplitMirrorSetBackup    0x04 /**< Logical drive type: split mirror backup; @outputof SA_GetLogicalDriveBasicInfo after masking @ref kLogicalDriveTypeMask. */
   #define kLogicalDriveTypeSplitMirrorSetBackupOrphan 0x05 /**< Logical drive type: split mirror orphan; @outputof SA_GetLogicalDriveBasicInfo after masking @ref kLogicalDriveTypeMask. */
   /** Decode output of @ref SA_GetLogicalDriveBasicInfo to get logical drive type. */
   #define LOGICAL_DRIVE_TYPE(info_value) (info_value & kLogicalDriveTypeMask)

   #define kLogicalDriveAcceleratorMask             0xF0 /**< Mask to use on output of @ref SA_GetLogicalDriveBasicInfo along with @ref kLogicalDriveAcceleratorBitShift to get logical drive accelerator. */
   #define kLogicalDriveAcceleratorBitShift         4    /**< Bitshift to use with mask @ref kLogicalDriveAcceleratorMask to get logical drive accelerator from @ref SA_GetLogicalDriveBasicInfo. */
   #define kLogicalDriveAcceleratorUnknown          0x00 /**< Logical drive accelerator unknown or could not be determined; @outputof SA_GetLogicalDriveBasicInfo after masking @ref kLogicalDriveAcceleratorMask. */
   #define kLogicalDriveAcceleratorNone             0x01 /**< Logical drive accelerator type: none; @outputof SA_GetLogicalDriveBasicInfo after masking @ref kLogicalDriveAcceleratorMask. */
   #define kLogicalDriveAcceleratorControllerCache  0x02 /**< Logical drive accelerator type: controller cache; @outputof SA_GetLogicalDriveBasicInfo after masking @ref kLogicalDriveAcceleratorMask. */
   #define kLogicalDriveAcceleratorIOBypass         0x03 /**< Logical drive accelerator type: I/O Bypass; @outputof SA_GetLogicalDriveBasicInfo after masking @ref kLogicalDriveAcceleratorMask. */
   #define kLogicalDriveAcceleratorLUCache          0x04 /**< Logical drive accelerator type: LU Cache; @outputof SA_GetLogicalDriveBasicInfo after masking @ref kLogicalDriveAcceleratorMask. */
   /** Decode output of @ref SA_GetLogicalDriveBasicInfo to get logical drive accelerator. */
   #define LOGICAL_DRIVE_ACCELERATOR(info_value) ((info_value & kLogicalDriveAcceleratorMask) >> kLogicalDriveAcceleratorBitShift)
   /** @} */
/** @} */

/** @defgroup storc_properties_ld_size Logical Drive Size
 * @{ */
/** @name Logical Drive Size Types
 * Inputs to @ref SA_GetLogicalDriveSize.
 * @{ */
#define kLogicalDriveSizeBlocks           0x00   /**< Total size of the logical drive in blocks. */
#define kLogicalDriveSizeBytes            0x01   /**< Total size of the logical drive in bytes. */
#define kLogicalDriveStripSizeBlocks      0x02   /**< Strip size (on each drive) in blocks. */
#define kLogicalDriveStripSizeBytes       0x03   /**< Strip size (on each drive) in bytes. */
#define kLogicalDriveFullStripeSizeBlocks 0x04   /**< Full stripe size (on all drives) in blocks. */
#define kLogicalDriveFullStripeSizeBytes  0x05   /**< Full stripe size (on all drives) in bytes. */
#define kLogicalDriveBlockSizeBytes       0x10   /**< Logical drive block size in bytes. */
/** @} */
/** Get logical drive size.
 * @pre `ld_number` is a valid logical drive number.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Valid logical drive number.
 * @param[in] size_type  Specify what type of size data to return (see [Logical Drive Size Types](@ref storc_properties_ld_size)).
 * @return Size of logical drive (modified by param [size_type](@ref storc_properties_ld_size)).
 */
SA_QWORD SA_GetLogicalDriveSize(PCDA cda, SA_WORD ld_number, SA_BYTE size_type);
/** @} */

/** @defgroup storc_properties_ld_parity_init_type Logical Drive Parity Initialization Type
 * @{ */
/** Get the logical drive parity initialization type.
 *
 * @pre `ld_number` is a valid logical drive number.
 * @pre The target logical drive is currently undergoing parity initialization.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @return [Logical Drive Parity Initialization Types](@ref storc_properties_ld_parity_init_type).
 */
SA_WORD SA_GetLogicalDriveParityInitializationType(PCDA cda, SA_WORD ld_number);
   /** @name Logical Drive Parity Initialization Types
    * @outputof SA_GetLogicalDriveParityInitializationType.
    * @{ */
   /** Rapid parity initialization.
    * The logical drive will not be available to the host OS until parity initialization completes.
    * @outputof SA_GetLogicalDriveParityInitializationType.
    */
   #define kLogicalDriveParityInitializationRapid     0
   /** Default parity initialization.
    * The logical drive will immediately be available to host OS. Parity initialization will occur in the background.
    * @outputof SA_GetLogicalDriveParityInitializationType.
    */
   #define kLogicalDriveParityInitializationDefault   1
   /** @} */
/** @} */ /* storc_properties_ld_parity_init_type */

/** @defgroup storc_properties_ld_op_progress Logical Drive Operation Progress
 * @{ */
/** Defines progress/percent when the operation is not in progress or queued. */
#define kInvalidOperationProgressPercent  101
/** Defines progress/percent when the operation failed. */
#define kFailedOperationProgressPercent   102
/** @name Logical Drive Parity Initialization Progress
 * @{ */
/** Get the logical drive parity initialization progress.
 *
 * @internal
 * This function returns @ref kInvalidOperationProgressPercent for invalid (RAID 0/1/etc. drives)
 * This function retunrs @ref kFailedOperationProgressPercent for failed parity initialization state
 * @endinternal
 *
 * @pre `ld_number` is a valid logical drive number.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @return Parity initialization progress or error code >100.
 * @return @ref kInvalidOperationProgressPercent if volume does not use parity.
 * @return @ref kFailedOperationProgressPercent if parity initialization failed.
 */
SA_BYTE SA_GetLogicalDriveParityInitializationProgress(PCDA cda, SA_WORD ld_number);
/** Get the live logical drive rapid parity initialization progress.
 * @deprecated Use SA_GetLogicalDriveParityInitializationProgressLive instead.
 * Similar to @ref SA_GetLogicalDriveParityInitializationProgress except that
 * 1. The information only applies to logical drives under going *rapid* parity
 * initialization and
 * 2. the information is generated using up-to-date information from the device
 * (instead of cached information that updates only with
 * @ref SA_ReInitializeCDA).
 * @warning Calling this API repeatedly and often can impact controller performance
 * since each invocation resends necessary commands to the device.
 */
SA_BYTE SA_GetLogicalDriveRPIProgressLive(PCDA cda, SA_WORD ld_number);
/** Get the live logical drive parity initialization progress.
 *
 * Similar to @ref SA_GetLogicalDriveParityInitializationProgress except the
 * information is generated using up-to-date data from the device
 * (instead of cached information that updates only with
 * @ref SA_ReInitializeCDA).
 * @warning Calling this API repeatedly and often can impact controller performance
 * since each invocation resends necessary commands to the device.
 */
SA_BYTE SA_GetLogicalDriveParityInitializationProgressLive(PCDA cda, SA_WORD ld_number);
/** @} */ /* Logical Drive Parity Initialization Progress */

/** @name Logical Drive Rebuild Progress
 * @{ */
/** Get the logical drive rebuilding progress.
 *
 * @pre `ld_number` is a valid logical drive number that is rebuilding or queued for rebuild.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @return Rebuilding percent of logical drive (0 - 100).
 * @return @ref kInvalidOperationProgressPercent if volume is not rebuilding and is not is queued for rebuilding.
 */
SA_BYTE SA_GetLogicalDriveRebuildProgress(PCDA cda, SA_WORD ld_number);
/** Get the live logical drive rebuilding progress.
 *
 * Same as @ref SA_GetLogicalDriveRebuildProgress except that the information
 * is generated using up-to-date information from the device (instead of cached
 * information that updates only with @ref SA_ReInitializeCDA).
 * @warning Calling this API repeatedly and often can impact controller performance
 * since each invocation resends necessary commands to the device.
 */
SA_BYTE SA_GetLogicalDriveRebuildProgressLive(PCDA cda, SA_WORD ld_number);
/** @} */ /* Logical Drive Rebuild Progress */

/** @name Logical Drive Transformation Progress
 * @{ */
/** Get the logical drive transformation progress.
 *
 * @pre `ld_number` is a valid logical drive number that is transforming or queued for transformation.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @return Transforming percent of logical drive (0 - 100).
 * @return @ref kInvalidOperationProgressPercent if volume is not transforming and is not queued for transformation.
 */
SA_BYTE SA_GetLogicalDriveTransformationProgress(PCDA cda, SA_WORD ld_number);
/** Get the live logical drive transformation progress.
 *
 * Same as @ref SA_GetLogicalDriveTransformationProgress except that the information
 * is generated using up-to-date information from the device (instead of cached
 * information that updates only with @ref SA_ReInitializeCDA).
 * @warning Calling this API repeatedly and often can impact controller performance
 * since each invocation resends necessary commands to the device.
 */
SA_BYTE SA_GetLogicalDriveTransformationProgressLive(PCDA cda, SA_WORD ld_number);
/** @} */ /* Logical Drive Transformation Progress */

/** @name Logical Drive Rekey Progress
 * @{ */
/** Get the logical drive encoding/rekeying progress.
 *
 * @pre `ld_number` is a valid logical drive number that is rekeying or queued for rekey.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @return Rekeying percent of logical drive (0 - 100).
 * @return @ref kInvalidOperationProgressPercent if volume is not rekeying and is not queued for rekeying.
 */
SA_BYTE SA_GetLogicalDriveRekeyProgress(PCDA cda, SA_WORD ld_number);
/** Get the live logical drive encoding/rekeying progress.
 *
 * Same as @ref SA_GetLogicalDriveRekeyProgress except that the information
 * is generated using up-to-date information from the device (instead of cached
 * information that updates only with @ref SA_ReInitializeCDA).
 * @warning Calling this API repeatedly and often can impact controller performance
 * since each invocation resends necessary commands to the device.
 */
SA_BYTE SA_GetLogicalDriveRekeyProgressLive(PCDA cda, SA_WORD ld_number);
/** @} */ /* Logical Drive Rekey Progress */
/** @} */ /* storc_properties_ld_op_progress */

/** @defgroup storc_properties_ld_unique_id Logical Drive Unique Volume ID
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetLogicalDriveUniqueVolumeID. */
#define kMinGetLogicalDriveUniqueVolumeIDBufferLength 33
/** Get Unique Volume Identifier for the logical drive.
 *
 * From VPD Page 83
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `ld_number` is a valid logical drive number.
 * @pre `volume_id_buffer` is at least @ref kMinGetLogicalDriveUniqueVolumeIDBufferLength bytes
 * to contain the unique volume ID and terminating NULL.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  ld_number         Logical drive number.
 * @param[out] volume_id_buffer  Buffer to write the unique volume ID.
 * @param[in]  buffer_size       Size (in bytes) of `volume_id_buffer`.
 * @return NULL terminating ASCII string of the logical drive unique volume ID, same as `volume_id_buffer`.
 * @return Empty string if given buffer was not large enough to store entire unique volume ID.
 */
char *SA_GetLogicalDriveUniqueVolumeID(PCDA cda, SA_WORD ld_number, char *volume_id_buffer, size_t buffer_size);
/** @} */ /* storc_properties_ld_unique_id */

/** @defgroup storc_properties_ld_label Logical Drive Volume Label
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetLogicalDriveVolumeLabel. */
#define kMinGetLogicalDriveVolumeLabelBufferLength 65
/** Get the volume label for the logical drive.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `ld_number` is a valid logical drive number.
 * @pre `volume_label_buffer` is at least @ref kMinGetLogicalDriveVolumeLabelBufferLength bytes
 * to contain the volume label and terminating NULL.
 *
 * @param[in]  cda                  Controller data area.
 * @param[in]  ld_number            Logical drive number.
 * @param[out] volume_label_buffer  Buffer to write the volume label.
 * @param[in]  buffer_size          Size (in bytes) of `volume_label_buffer`.
 * @return NULL terminating ASCII string of the logical drive volume label, same as `volume_label_buffer`.
 * @return Empty string if given buffer was not large enough to store entire volume label.
 */
char *SA_GetLogicalDriveVolumeLabel(PCDA cda, SA_WORD ld_number, char *volume_label_buffer, size_t buffer_size);
/** @} */ /* storc_properties_ld_label */

/** @defgroup storc_properties_ld_boot_priority Logical Drive Boot Priority
 * @{ */
/** Get the boot priority of the logical drive.
 *
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @return [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume).
 */
SA_BYTE SA_GetLogicalDriveBootPriority(PCDA cda, SA_WORD ld_number);
/** @} */ /* storc_properties_ld_boot_priority */

/** @defgroup storc_properties_ld_geometry Logical Drive Geometry
 * @{ */
/** Get the geometry information of the logical drive
 *
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @return [Logical Drive Geometry](@ref storc_properties_ld_geometry).
 * @return Decode with @ref LOGICAL_DRIVE_CYLINDERS to get logical drive cylinders.
 * @return Decode with @ref LOGICAL_DRIVE_HEADS to get logical drive heads.
 * @return Decode with @ref LOGICAL_DRIVE_SECTORS to get logical drive sectors.
 */
SA_DWORD SA_GetLogicalDriveGeometry(PCDA cda, SA_WORD ld_number);
   /** @name Logical Drive Geometry
    * @outputof SA_GetLogicalDriveGeometry
    * @{ */
   #define kGeometryCylindersMask      0xFFFF0000 /**< Mask to use on output of @ref SA_GetLogicalDriveGeometry to get logical drive cylinders. */
   #define kGeometryCylindersBitshift  16         /**< Bitshift to use on output of @ref SA_GetLogicalDriveGeometry to get logical drive cylinders. */
   /** Use to decode output of @ref SA_GetLogicalDriveGeometry to get the cylinders of the logical drive. */
   #define LOGICAL_DRIVE_CYLINDERS(info_value) ((info_value & kGeometryCylindersMask) >> kGeometryCylindersBitshift)

   #define kGeometryHeadsMask          0x0000FF00 /**< Mask to use on output of @ref SA_GetLogicalDriveGeometry to get logical drive heads. */
   #define kGeometryHeadsBitshift      8          /**< Bitshift to use on output of @ref SA_GetLogicalDriveGeometry to get logical drive heads. */
   /** Use to decode output of @ref SA_GetLogicalDriveGeometry to get the heads of the logical drive. */
   #define LOGICAL_DRIVE_HEADS(info_value) ((info_value & kGeometryHeadsMask) >> kGeometryHeadsBitshift)

   #define kGeometrySectorsMask        0x000000FF /**< Mask to use on output of @ref SA_GetLogicalDriveGeometry to get logical drive sectors. */
   #define kGeometrySectorsBitshift    0          /**< Bitshift to use on output of @ref SA_GetLogicalDriveGeometry to get logical drive sectors. */
   /** Use to decode output of @ref SA_GetLogicalDriveGeometry to get the sectors of the logical drive. */
   #define LOGICAL_DRIVE_SECTORS(info_value) ((info_value & kGeometrySectorsMask) >> kGeometrySectorsBitshift)
   /** @} */
/** @} */ /* storc_properties_ld_geometry */

/** @defgroup storc_properties_ld_raid_groups Logical Drive Mirror/Parity Groups
 * @details
 * In order to get mirror/parity group information of a logical drive,
 * the client must retreive a @ref PRAID_GROUP_INFO object for a logical drive and then
 * use the following API to parse data from that object.
 * @code
 * PRAID_GROUP_INFO raid_group_info = 0;
 * SA_WORD raid_group_counts = SA_CreateLogicalDriveRaidGroupEnumerator(cda, ld_number, &raid_group_info);
 * if (raid_group_info == NULL)
 * {
 *    fprintf(stderr, "Failed to generate parity/mirror group info for ld %u\n", ld_number);
 *    return 1;
 * }
 *
 * SA_DWORD num_mirror_groups = MIRROR_GROUP_COUNT(raid_group_counts);
 * SA_DWORD m_group = 0;
 * for (m_group = 0; m_group < num_mirror_groups; ++m_group)
 * {
 *     SA_WORD pd_number = kInvalidPDNumber;
 *     printf("Mirror Group %u:\n", m_group);
 *     while ((pd_number = SA_EnumerateMirrorGroupPhysicalDrives(raid_group_info, m_group, pd_number)) != kInvalidPDNumber)
 *     {
 *         printf("  Physical Drive %u\n", pd_number);
 *     }
 * }
 *
 * SA_DWORD num_parity_groups = PARITY_GROUP_COUNT(raid_group_counts);
 * SA_DWORD m_group = 0;
 * for (m_group = 0; m_group < num_parity_groups; ++m_group)
 * {
 *     SA_WORD pd_number = kInvalidPDNumber;
 *     printf("Parity Group %u:\n", m_group);
 *     while ((pd_number = SA_EnumerateParityGroupPhysicalDrives(raid_group_info, m_group, pd_number)) != kInvalidPDNumber)
 *     {
 *         printf("  Physical Drive %u\n", pd_number);
 *     }
 * }
 *
 * SA_DestroyLogicalDriveRaidGroupEnumerator(raid_group_info);
 * return 0;
 *
 * // Example output for RAID 1+0 with 6 drives:
 * // Mirror Group 0:
 * //   Physical Drive 0
 * //   Physical Drive 1
 * //   Physical Drive 2
 * // Mirror Group 1:
 * //   Physical Drive 3
 * //   Physical Drive 4
 * //   Physical Drive 5
 * @endcode
 * @{ */

/** @name Generate Logical Drive Mirror/Parity Group Info
 * @{ */
/** Allocate and return a @ref PRAID_GROUP_INFO object containing information about a logical drives mirror or parity groups.
 * @post Returned object is parsed using other functions in @ref storc_properties_ld_raid_groups.
 * @post Returned object should be cleaned using @ref SA_DestroyLogicalDriveRaidGroupEnumerator.
 *
 * @param[in]  cda              Controller handle
 * @param[in]  ld_number        Target logcial drive number of a logical drive that has mirror or parity groups.
 * @param[out] raid_group_info  Address to write raid group info.
 * @return On success of a valid logical drive, populates `raid_group_info` with data on a logical drive's mirror/parity groups.
 * @return Returns a map describing the number of mirror or parity groups. Decode using @ref MIRROR_GROUP_COUNT and @ref PARITY_GROUP_COUNT.
 * @return On any failure, returns 0 and sets `raid_group_info` to NULL.
 */
SA_WORD SA_CreateLogicalDriveRaidGroupEnumerator(PCDA cda, SA_WORD ld_number, PRAID_GROUP_INFO* raid_group_info);
/** Use to decode output of @ref SA_CreateLogicalDriveRaidGroupEnumerator using @ref MIRROR_GROUP_COUNT. */
#define kCreateLogicalDriveRaidGroupMirrorGroupCountShift  0
/** Decode output of @ref SA_CreateLogicalDriveRaidGroupEnumerator to get number of mirror groups. */
#define MIRROR_GROUP_COUNT(info) (((info) >> kCreateLogicalDriveRaidGroupMirrorGroupCountShift) & 0xFF)

/** Use to decode output of @ref SA_CreateLogicalDriveRaidGroupEnumerator using @ref PARITY_GROUP_COUNT. */
#define kCreateLogicalDriveRaidGroupParityGroupCountShift  8
/** Decode output of @ref SA_CreateLogicalDriveRaidGroupEnumerator to get number of parity groups. */
#define PARITY_GROUP_COUNT(info) (((info) >> kCreateLogicalDriveRaidGroupParityGroupCountShift) & 0xFF)
/** @} */ /* Generate Logical Drive Mirror/Parity Group Info */

/** @name Destroy Logical Drive Mirror/Parity Group Info
 * @{ */
/** Clear data created using @ref SA_CreateLogicalDriveRaidGroupEnumerator. */
void SA_DestroyLogicalDriveRaidGroupEnumerator(PRAID_GROUP_INFO raid_group_info);
/** @} */

/** @name Get Logical Drive Mirror Group Information
 * @{ */
/** Enumerate all physical drives of a given logical drive's mirror group.
 * @param[in] raid_group_info     Raid group info previously allocated using @ref SA_CreateLogicalDriveRaidGroupEnumerator for a valid logical drive with mirror groups.
 * @param[in] mirror_group_index  Value [0,@ref MIRROR_GROUP_COUNT) for target mirror group.
 * @param[in] pd_number           Physical drive number (or @ref kInvalidPDNumber), the return value is the 'next' physical drive in the mirror group.
 * @return The next physical drive in the taret mirror group.
 * @return @ref kInvalidPDNumber if called using the last physical drive in the mirror group.
 */
SA_WORD SA_EnumerateMirrorGroupPhysicalDrives(const PRAID_GROUP_INFO raid_group_info, SA_BYTE mirror_group_index, SA_WORD pd_number);
/** @} */ /* Get Logical Drive Mirror Group Information */

/** @name Get Logical Drive Parity Group Information
 * @{ */
/** Enumerate all physical drives of a given logical drive's parity group.
 * @param[in] raid_group_info     Raid group info previously allocated using @ref SA_CreateLogicalDriveRaidGroupEnumerator for a valid logical drive with parity groups.
 * @param[in] parity_group_index  Value [0,@ref PARITY_GROUP_COUNT) for target parity group.
 * @param[in] pd_number           Physical drive number (or @ref kInvalidPDNumber), the return value is the 'next' physical drive in the parity group.
 * @return The next physical drive in the taret parity group.
 * @return @ref kInvalidPDNumber if called using the last physical drive in the parity group.
 */
SA_WORD SA_EnumerateParityGroupPhysicalDrives(const PRAID_GROUP_INFO raid_group_info, SA_BYTE parity_group_index, SA_WORD pd_number);
/** @} */ /* Get Logical Drive Parity Group Information */

/** @} */ /* storc_properties_ld_raid_groups */

/** @} */ /* storc_properties_ld */

/****************************************************//**
 * @defgroup storc_properties_remote_volume Remote Volume
 * @brief
 * @details
 * @{
 *******************************************************/

/** @name Logical Drive Enumerate Returns
 * @{ */
#define kInvalidRemoteVolumeNumber 0xFFFF /**< Invalid remote volume number. */
/** @} */

/** @defgroup storc_properties_remote_volume_count Remote Volume Count
 * @{
 */
/** Return total number of controller remote volumes.
 * @param[in] cda            Controller data area.
 * @return Number of remote volumes detected by the controller.
 * @return This value can be used as the upper bound of any remote volume index
 * used by remote volume APIs.
 */
SA_WORD SA_GetControllerRemoteVolumeCount(PCDA cda);
/** @} */ /* storc_properties_remote_volume_count */

/** @defgroup storc_properties_remote_volume_id Remote Volume Unique ID
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetRemoteVolumeUniqueID. */
#define kMinGetRemoteVolumeUniqueIDBufferLength 33
/** Get Unique Volume Identifier for the remote volume.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `volume_number` is a valid remote volume number.
 * @pre `volume_id_buffer` is at least @ref kMinGetRemoteVolumeUniqueIDBufferLength bytes
 * to contain the unique volume ID and terminating NULL.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  volume_number     Remote volume number.
 * @param[out] volume_id_buffer  Buffer to write the unique volume ID.
 * @param[in]  buffer_size       Size (in bytes) of `volume_id_buffer`.
 * @return NULL terminating ASCII string of the remote volume unique volume ID, same as `volume_id_buffer`.
 * @return Empty string if given buffer was not large enough to store entire unique volume ID.
 */
char *SA_GetRemoteVolumeUniqueID(PCDA cda, SA_WORD volume_number, char *volume_id_buffer, size_t buffer_size);
/** @} */ /* storc_properties_remote_volume_id */

/** @} */ /* storc_properties_remote_volume */

/***********************************************//**
 * @defgroup storc_properties_sep SEPs
 * @brief
 * @details
 * @{
 **************************************************/

/** @name Invalid SEP Number
 * @{ */
/** Invalid SEP number used when enumerating SEPs. */
#define kInvalidSEPNumber 0xFFFF
/** @} */ /* Invalid SEP Number */

/** @defgroup storc_properties_sep_enum Enumerate SEPs
 * @{
 */
/** Start enumeration of controller SEPs.
 * Start with kInvalidSEPNumber as the first number.
 * @param[in] cda         Controller data area.
 * @param[in] sep_number  Current SEP number.
 * @return The next valid SEP number, or @ref kInvalidSEPNumber if no more SEPs exist.
 */
SA_WORD SA_EnumerateControllerSEPs(PCDA cda, SA_WORD sep_number);

/** Iterate over all SEPs and execute the block with each valid sep_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] sep_number    Variable to store valid SEP numbers.
 *
 * __Example__
 * @code
 * SA_WORD sep_number;
 * FOREACH_SEP(cda, sep_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_SEP(cda, sep_number) \
   for (sep_number = kInvalidSEPNumber; (sep_number = SA_EnumerateControllerSEPs(cda, sep_number)) != kInvalidSEPNumber;)
/** @} */ /* storc_properties_sep_enum */

/** @defgroup storc_properties_sep_wwid SEP WWID
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetSEPWWID when not converting to a string. */
#define kMinGetSEPWWIDBufferLength 20
/** Minimum buffer length (in bytes) required for @ref SA_GetSEPWWID when converting to a string. */
#define kMinGetSEPWWIDStringLength 41
/** Get the WWID of the SEP.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre When converting to string, `wwid_buffer` is at least @ref kMinGetSEPWWIDStringLength bytes
 * to contain the SEP WWID and terminating NULL.
 * @pre When not converting to string, `wwid_buffer` is at least @ref kMinGetSEPWWIDBufferLength bytes.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  sep_number  SEP number.
 * @param[out] wwid_buffer Buffer to write WWID.
 * @param[in]  buffer_size Size (in bytes) of `wwid_buffer`.
 * @param[in]  convert_to_string kTrue => Convert value to string and store in wwid_buffer, kFalse => Copy raw binary value to wwid_buffer.
 * @return WWID of the SEP (same as parameter `wwid_buffer`).
 */
char *SA_GetSEPWWID(PCDA cda, SA_WORD sep_number, void *wwid_buffer, size_t buffer_size, SA_BOOL convert_to_string);
/** @} */ /* storc_properties_sep_wwid */

/** @defgroup storc_properties_sep_location SEP Location
 * @see storc_properties_pd_location_info.
 * @{ */
/** Get the location information of the SEP.
 *
 * The location is returned as Port:Box, or as separate components, based on the type argument.
 *
 * @pre @p location_buffer is at least @ref kMinGetPhysicalDeviceLocationCompleteBufferLength,
 * @ref kMinGetPhysicalDeviceLocationPortBufferLength, @ref kMinGetPhysicalDeviceLocationBoxBufferLength, or
 * @ref kMinGetPhysicalDeviceLocationBayBufferLength bytes depending on @p type. to contain the return
 * the location string (including a terminating NULL).
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  sep_number        SEP number.
 * @param[in]  type              Type of the location information required.
 * @param[out] location_buffer   Buffer to write location string.
 * @param[in]  buffer_size       Size (in bytes) of `location_buffer`.
 * @return Location of the SEP.
 */
char *SA_GetSEPLocation(PCDA cda, SA_WORD sep_number, SA_BYTE type, char *location_buffer, size_t buffer_size);
   /** @name SEP Location Formats
    * Input to @ref SA_GetSEPLocation.
    * @{ */
   #define kSEPLocationComplete       0x01  /**< Return a complete string as "CN0:1"; Input to @ref SA_GetSEPLocation. */
   #define kSEPLocationPort           0x02  /**< Return port only as "CN0"; Input to @ref SA_GetSEPLocation. */
   #define kSEPLocationBox            0x03  /**< Return box only as "1"; Input to @ref SA_GetSEPLocation. */
   /** @} */
/** @} */ /* storc_properties_sep_location */

/** @defgroup storc_properties_sep_model SEP Model/Product Name
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetSEPModel. */
#define kMinGetSEPModelBufferLength 49
/** Write the SEP model to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `model_buffer` is at least @ref kMinGetSEPModelBufferLength bytes
 * to contain the SEP model and terminating NULL.
 *
 * @param[in]  cda           Controller data area.
 * @param[in]  sep_number    SEP number.
 * @param[out] model_buffer  Buffer to write vendor ID.
 * @param[in]  buffer_size   Size (in bytes) of `vendor_id_buffer`.
 * @return SEP model (same as `model_buffer`).
 * @return Empty string if given buffer was not large enough to store entire SEP model.
 */
char *SA_GetSEPModel(PCDA cda, SA_WORD sep_number, char *model_buffer, size_t buffer_size);
/** @} */ /* storc_properties_sep_model */

/** @defgroup storc_properties_sep_firmware_version SEP Firmware Version
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetSEPFirmwareVersion. */
#define kMinGetSEPFirmwareVersionBufferLength 9
/** Write the SEP firmware version to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `firmware_version_buffer` is at least @ref kMinGetSEPFirmwareVersionBufferLength bytes
 * to contain the SEP firmware version and terminating NULL.
 *
 * @param[in]  cda                      Controller data area.
 * @param[in]  sep_number               SEP number.
 * @param[out] firmware_version_buffer  Buffer to write vendor ID.
 * @param[in]  buffer_size              Size (in bytes) of `firmware_version_buffer`.
 * @return Firmware version of the SEP.
 * @return Empty string if given buffer was not large enough to store entire SEP firmware version.
 */
char *SA_GetSEPFirmwareVersion(PCDA cda, SA_WORD sep_number, char *firmware_version_buffer, size_t buffer_size);
/** @} */ /* storc_properties_sep_firmware_version */

/** @defgroup storc_properties_sep_phy_info SEP Phy Info
 * @see storc_properties_pd_phy_info
 * @{
 * @}
 */

/** @} */ /* storc_properties_sep */

/***********************************************//**
 * @defgroup storc_properties_expander Expanders
 * @brief
 * @details
 * @{
 **************************************************/

/** @name Invalid Expander Number
 * @{ */
/** Invalid expander number used when enumerating expanders. */
#define kInvalidExpanderNumber 0xFFFF
/** @} */ /* Invalid Expander Number */

/** @defgroup storc_properties_expander_enum Enumerate Expanders
 * @{
 */
/** Start enumeration of controller expanders.
 * Start with kInvalidExpanderNumber as the first number.
 * @param[in] cda              Controller data area.
 * @param[in] expander_number  Current expander number.
 * @return The next valid expander number, or @ref kInvalidExpanderNumber if no more expanders exist.
 */
SA_WORD SA_EnumerateControllerExpanders(PCDA cda, SA_WORD expander_number);

/** Iterate over all expanders and execute the block with each valid expander_number.
 * @attention ANSI C Safe.
 * @param[in] cda              Controller data area.
 * @param[in] expander_number  Variable to store valid expander numbers.
 *
 * __Example__
 * @code
 * SA_WORD expander_number;
 * FOREACH_EXPANDER(cda, expander_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_EXPANDER(cda, expander_number) \
   for (expander_number = kInvalidExpanderNumber; (expander_number = SA_EnumerateControllerExpanders(cda, expander_number)) != kInvalidExpanderNumber;)
/** @} */ /* storc_properties_expander_enum */

/** @defgroup storc_properties_expander_wwid Expander WWID
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetExpanderWWID when not converting to a string. */
#define kMinGetExpanderWWIDBufferLength 20
/** Minimum buffer length (in bytes) required for @ref SA_GetExpanderWWID when converting to a string. */
#define kMinGetExpanderWWIDStringLength 41
/** Get the WWID of the expander.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre When converting to string, `wwid_buffer` is at least @ref kMinGetExpanderWWIDStringLength bytes
 * to contain the expander WWID and terminating NULL.
 * @pre When not converting to string, `wwid_buffer` is at least @ref kMinGetExpanderWWIDBufferLength bytes.
 *
 * @param[in]  cda              Controller data area.
 * @param[in]  expander_number  Expander number.
 * @param[out] wwid_buffer      Buffer to write WWID.
 * @param[in]  buffer_size      Size (in bytes) of `wwid_buffer`.
 * @param[in]  convert_to_string kTrue => Convert value to string and store in wwid_buffer, kFalse => Copy raw binary value to wwid_buffer.
 * @return WWID of the expander (same as parameter `wwid_buffer`).
 */
char *SA_GetExpanderWWID(PCDA cda, SA_WORD expander_number, void *wwid_buffer, size_t buffer_size, SA_BOOL convert_to_string);
/** @} */ /* storc_properties_expander_wwid */

/** @defgroup storc_properties_expander_location Expander Location
 * @see storc_properties_pd_location_info.
 * @{ */
/** Get the location information of the expander.
 *
 * The location is returned as Port:Box, or as separate components, based on the type argument.
 *
 * @pre @p location_buffer is at least @ref kMinGetPhysicalDeviceLocationCompleteBufferLength,
 * @ref kMinGetPhysicalDeviceLocationPortBufferLength, @ref kMinGetPhysicalDeviceLocationBoxBufferLength, or
 * @ref kMinGetPhysicalDeviceLocationBayBufferLength bytes depending on @p type. to contain the return
 * the location string (including a terminating NULL).
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  expander_number   Expander number.
 * @param[in]  type              Type of the location information required.
 * @param[out] location_buffer   Buffer to write location string.
 * @param[in]  buffer_size       Size (in bytes) of `location_buffer`.
 * @return Location of the expander.
 */
char *SA_GetExpanderLocation(PCDA cda, SA_WORD expander_number, SA_BYTE type, char *location_buffer, size_t buffer_size);
   /** @name Expander Location Formats
    * Input to @ref SA_GetExpanderLocation.
    * @{ */
   #define kExpanderLocationComplete       0x01  /**< Return a complete string as "CN0:1"; Input to @ref SA_GetExpanderLocation. */
   #define kExpanderLocationPort           0x02  /**< Return port only as "CN0"; Input to @ref SA_GetExpanderLocation. */
   #define kExpanderLocationBox            0x03  /**< Return box only as "1"; Input to @ref SA_GetExpanderLocation. */
   /** @} */
/** @} */ /* storc_properties_expander_location */

/** @defgroup storc_properties_expander_model Expander Model/Product Name
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetExpanderModel. */
#define kMinGetExpanderModelBufferLength 49
/** Write the expander model to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `model_buffer` is at least @ref kMinGetExpanderModelBufferLength bytes
 * to contain the expander model and terminating NULL.
 *
 * @param[in]  cda              Controller data area.
 * @param[in]  expander_number  Expander number.
 * @param[out] model_buffer     Buffer to write vendor ID.
 * @param[in]  buffer_size      Size (in bytes) of `vendor_id_buffer`.
 * @return Expander model (same as `model_buffer`).
 * @return Empty string if given buffer was not large enough to store entire expander model.
 */
char *SA_GetExpanderModel(PCDA cda, SA_WORD expander_number, char *model_buffer, size_t buffer_size);
/** @} */ /* storc_properties_expander_model */

/** @defgroup storc_properties_expander_firmware_version Expander Firmware Version
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetExpanderFirmwareVersion. */
#define kMinGetExpanderFirmwareVersionBufferLength 9
/** Write the expander firmware version to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `firmware_version_buffer` is at least @ref kMinGetExpanderFirmwareVersionBufferLength bytes
 * to contain the expander firmware version and terminating NULL.
 *
 * @param[in]  cda                      Controller data area.
 * @param[in]  expander_number          Expander number.
 * @param[out] firmware_version_buffer  Buffer to write vendor ID.
 * @param[in]  buffer_size              Size (in bytes) of `firmware_version_buffer`.
 * @return Firmware version of the expander.
 * @return Empty string if given buffer was not large enough to store entire expander firmware version.
 */
char *SA_GetExpanderFirmwareVersion(PCDA cda, SA_WORD expander_number, char *firmware_version_buffer, size_t buffer_size);
/** @} */ /* storc_properties_expander_firmware_version */

/** @defgroup storc_properties_expander_phy_info Expander Phy Info
 * @see storc_properties_pd_phy_info
 * @{
 * @}
 */

/** @} */ /* storc_properties_expander */

/***********************************************//**
 * @defgroup storc_properties_se Storage Enclosure
 * @brief
 * @details
 * @{
 **************************************************/

/** @name Storage Enclosure Enumerate Returns
 * @{ */
#define kInvalidSENumber 0xFFFF /**< Invalid storage enclosure number */
/** @} */

/** @defgroup storc_properties_se_enum Enumerate Storage Enclosures
 * @{
 */
/** Start enumeration of controller storage enclosures.
 * Start with kInvalidSENumber as the first number.
 * @param[in] cda        Controller data area.
 * @param[in] se_number  Current storage enclosure number.
 * @return The next valid storage enclosure number, or @ref kInvalidSENumber if no more enclosures exist.
 */
SA_WORD SA_EnumerateControllerStorageEnclosures(PCDA cda, SA_WORD se_number);

/** Iterate over all storage enclosures and execute the block with each valid se_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] se_number     Variable to store valid storage enclosure numbers.
 *
 * __Example__
 * @code
 * SA_WORD se_number;
 * FOREACH_STORAGE_ENCLOSURE(cda, se_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_STORAGE_ENCLOSURE(cda, se_number) \
   for (se_number = kInvalidSENumber; (se_number = SA_EnumerateControllerStorageEnclosures(cda, se_number)) != kInvalidSENumber;)
/** @} */ /* storc_properties_se_enum */

/** @defgroup storc_properties_se_pd_enum Enumerate Storage Enclosure Physical Drives
 * @{
 */
/** Enumerates physical drives in the storage enclosure.
 *
 * Start with @ref kInvalidPDNumber as the first item.
 *
 * @param[in] cda        Controller data area.
 * @param[in] se_number  Storage enclosure number.
 * @param[in] pd_number  Physical drive number.
 * @return Given a drive number, the next valid physical drive number is returned.
 * @return @ref kInvalidPDNumber if no valid drives are connected.
 */
SA_WORD SA_EnumerateStorageEnclosurePhysicalDrives(PCDA cda, SA_WORD se_number, SA_WORD pd_number);

/** Iterate over all physical drives of a storage enclosure and execute the block with each valid pd_number.
 * @attention ANSI C Safe.
 * @param[in] cda        Controller data area.
 * @param[in] se_number  Storage enclosure number.
 * @param[in] pd_number  Variable to store valid physical drive numbers.
 *
 * __Example__
 * @code
 * SA_WORD pd_number;
 * FOREACH_SE_PHYSICAL_DRIVE(cda, 1, pd_number)
 * {
 *    // do something for each physical drive in storage enclosure 1...
 * }
 * @endcode
 */
#define FOREACH_SE_PHYSICAL_DRIVE(cda, se_number, pd_number) \
   for (pd_number = kInvalidPDNumber; (pd_number = SA_EnumerateStorageEnclosurePhysicalDrives(cda, se_number, pd_number)) != kInvalidPDNumber;)
/** @} */ /* storc_properties_se_pd_enum */

/** @defgroup storc_properties_se_sep_enum Enumerate Storage Enclosure SEPs
 * @{
 */
/** Enumerate over all SEPs of the specified storage enclosure.
 * @pre `se_number` is a valid storage enclosure number.
 * @param[in] cda         Controller data area.
 * @param[in] se_number   Valid storage enclosure number.
 * @param[in] sep_number  Variable to store valid SEP numbers (or @ref kInvalidSEPNumber to get the first SEP of the enclosure).
 * Start with @ref kInvalidSEPNumber as the first item.
 * @return Number of next SEP of the enclosure, or @ref kInvalidSEPNumber if at the last SEP.
 */
SA_WORD SA_EnumerateStorageEnclosureSEPs(PCDA cda, SA_WORD se_number, SA_WORD sep_number);

/** Iterate over all SEPs of a storage enclosure and execute the block with each valid sep_number.
 * @attention ANSI C Safe.
 * @param[in] cda         Controller data area.
 * @param[in] se_number   Valid storage enclosure number.
 * @param[in] sep_number  Variable to store valid SEP numbers.
 *
 * __Example__
 * @code
 * SA_WORD sep_number;
 * FOREACH_SE_SEP(cda, se_number, sep_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_SE_SEP(cda, se_number, sep_number) \
   for (sep_number = kInvalidSEPNumber; (sep_number = SA_EnumerateStorageEnclosureSEPs(cda, se_number, sep_number)) != kInvalidSEPNumber;)
/** @} */ /* storc_properties_se_sep_enum */

/** @defgroup storc_properties_se_expander_enum Enumerate Storage Enclosure Expanders
 * @{
 */
/** Enumerate over all Expanders of the specified storage enclosure.
 * @pre `se_number` is a valid storage enclosure number.
 * @param[in] cda         Controller data area.
 * @param[in] se_number   Valid storage enclosure number.
 * @param[in] expander_number  Variable to store valid Expander numbers (or @ref kInvalidExpanderNumber to get the first Expander of the enclosure).
 * Start with @ref kInvalidExpanderNumber as the first item.
 * @return Number of next Expander of the enclosure, or @ref kInvalidExpanderNumber if at the last Expander.
 */
SA_WORD SA_EnumerateStorageEnclosureExpanders(PCDA cda, SA_WORD se_number, SA_WORD expander_number);

/** Iterate over all Expanders of a storage enclosure and execute the block with each valid expander_number.
 * @attention ANSI C Safe.
 * @param[in] cda         Controller data area.
 * @param[in] se_number   Valid storage enclosure number.
 * @param[in] expander_number  Variable to store valid Expander numbers.
 *
 * __Example__
 * @code
 * SA_WORD expander_number;
 * FOREACH_SE_EXPANDER(cda, se_number, expander_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_SE_EXPANDER(cda, se_number, expander_number) \
   for (expander_number = kInvalidExpanderNumber; (expander_number = SA_EnumerateStorageEnclosureExpanders(cda, se_number, expander_number)) != kInvalidExpanderNumber;)
/** @} */ /* storc_properties_se_expander_enum */

/** @defgroup storc_properties_se_dev_counts Storage Enclosure Device Counts
 * @{ */
/** @name Storage Enclosure Device Types
 * @{ */
#define kSEDeviceTypeBay            1
#define kSEDeviceTypePhysicalDrive  2
#define kSEDeviceTypeSEP            3
#define kSEDeviceTypeExpander       4
/** @} */ /* Storage Enclosure Device Types */
/** Return the number of detected Bays/Drives/SEPs/Expanders/... of the storage enclosure.
 * @pre `se_number` is a valid storage enclosure number.
 * @param[in] cda         Controller data area.
 * @param[in] se_number   Valid storage enclosure number.
 * @param[in] dev_type    Determine what device type count to return ([Storage Enclosure Device Types](@ref storc_properties_se_dev_counts)).
 * @return Number of `dev_type` devices of the storage enclosure.
 */
SA_WORD SA_GetStorageEnclosureDeviceCount(PCDA cda, SA_WORD se_number, SA_BYTE dev_type);
/** @} */ /* storc_properties_se_dev_counts */

/** @defgroup storc_properties_se_bay_pd Storage Enclosure Bay Drive Number
 * @{
 */
/** Get the PD number of a storage enclosure bay.
 * @param[in] cda        Controller data area.
 * @param[in] se_number  Selected storage enclosure number.
 * @param[in] bay_number Selected storage enclosure bay number.
 * @return The physical drive number of the device present in the requested bay number. @ref kInvalidPDNumber if no device present.
 */
SA_WORD SA_GetStorageEnclosurePDNumber(PCDA cda, SA_WORD se_number, SA_WORD bay_number);
/** @} */ /* storc_properties_se_bay_pd */

/** @defgroup storc_properties_se_vendor Storage Enclosure Vendor
 * @{
 */
/** Minimum buffer length (in bytes) required for @ref SA_GetStorageEnclosureVendorID. */
#define kMinGetStorageEnclosureVendorIDBufferLength 9
/** Write the storage enclosure vendor ID to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `vendor_id_buffer` is at least @ref kMinGetStorageEnclosureVendorIDBufferLength bytes
 * to contain the vendor ID and terminating NULL.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  se_number         Storage enclosure number.
 * @param[out] vendor_id_buffer  Buffer to write vendor ID.
 * @param[in]  buffer_size       Size (in bytes) of `vendor_id_buffer`.
 * @return Vendor ID of the storage enclosure, same as `vendor_id_buffer`.
 * @return Empty string if given buffer was not large enough to store entire vendor ID.
 */
char *SA_GetStorageEnclosureVendorID(PCDA cda, SA_WORD se_number, char *vendor_id_buffer, size_t buffer_size);
/** @} */ /* storc_properties_se_vendor */

/** @defgroup storc_properties_se_product Storage Enclosure Product
 * @{
 */
/** Minimum buffer length (in bytes) required for @ref SA_GetStorageEnclosureProductID. */
#define kMinGetStorageEnclosureProductIDBufferLength 17
/** Write the storage enclosure product ID to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `product_id_buffer` is at least @ref kMinGetStorageEnclosureProductIDBufferLength bytes
 * to contain the product ID and terminating NULL.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  se_number         Storage enclosure number.
 * @param[out] product_id_buffer  Buffer to write product ID.
 * @param[in]  buffer_size       Size (in bytes) of `product_id_buffer`.
 * @return Product ID of the storage enclosure, same as `product_id_buffer`.
 * @return Empty string if given buffer was not large enough to store entire product ID.
 */
char *SA_GetStorageEnclosureProductID(PCDA cda, SA_WORD se_number, char *product_id_buffer, size_t buffer_size);
/** @} */ /* storc_properties_se_product */

/** @defgroup storc_properties_se_serial_number Storage Enclosure Serial Number
 * @{
 */
/** Minimum buffer length (in bytes) required for @ref SA_GetStorageEnclosureSerialNumber. */
#define kMinGetStorageEnclosureSerialNumberBufferLength 41
/** Write the storage enclosure serial number to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `serial_num_id_buffer` is at least @ref kMinGetStorageEnclosureSerialNumberBufferLength bytes
 * to contain the serial number and terminating NULL.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  se_number         Storage enclosure number.
 * @param[out] serial_num_id_buffer  Buffer to write serial number.
 * @param[in]  buffer_size       Size (in bytes) of `serial_num_id_buffer`.
 * @return Serial Number of the storage enclosure, same as `serial_num_id_buffer`.
 * @return Empty string if given buffer was not large enough to store entire serial number.
 */
char *SA_GetStorageEnclosureSerialNumber(PCDA cda, SA_WORD se_number, char *serial_num_id_buffer, size_t buffer_size);
/** @} */ /* storc_properties_se_serial_number */

/** @defgroup storc_properties_se_path_info Storage Enclosure Path Info
 * @{
 */
/** Get the number of paths for the storage enclosure.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  se_number         Storage enclosure number.
 * @return Number of storage enclosure paths.
 */
SA_BYTE SA_GetStorageEnclosurePathCount(PCDA cda, SA_WORD se_number);

/** Get the requested storage enclosure path name.
 *
 * The path name is returned as Port:Box:Bay.
 *
 * @pre `path_name_buffer` is large enough to contain the storage enclosure path and terminating NULL.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  se_number         Storage enclosure number.
 * @param[in]  path_index        0-based index of path. Valid values are from 0 to @ref SA_GetStorageEnclosurePathCount - 1.
 * @param[in]  type              Type of the location information required (one of [Storage Enclosure Location Formats](@ref storc_properties_se_path_info)).
 * @param[out] path_name_buffer  Buffer to write the path name.
 * @param[in]  buffer_size       Size (in bytes) of `path_name_buffer`.
 * @return Storage enclosure path name for the requested path.
 */
char *SA_GetStorageEnclosurePathName(PCDA cda, SA_WORD se_number, SA_BYTE path_index, SA_BYTE type, char *path_name_buffer, size_t buffer_size);
   /** @name Storage Enclosure Location Formats
    * Input to @ref SA_GetStorageEnclosurePathName.
    * @{ */
   #define kStorageEnclosurePathNameComplete       0x01  /**< Return a complete string as "CN0:1:7"; Input to @ref SA_GetStorageEnclosurePathName. */
   #define kStorageEnclosurePathNamePort           0x02  /**< Return port only as "CN0"; Input to @ref SA_GetStorageEnclosurePathName. */
   #define kStorageEnclosurePathNameBox            0x03  /**< Return box only as "1"; Input to @ref SA_GetStorageEnclosurePathName. */
   /** @} */

/** Get the requested storage enclosure path information.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  se_number         Storage enclosure number.
 * @return Storage enclosure path flags (active, failed).
 * @return Decode with @ref IS_SE_PATH_ACTIVE to see is a path is currently active.
 * @return Decode with @ref IS_SE_PATH_FAILED to see is a path is currently failed.
 */
SA_WORD SA_GetStorageEnclosurePathInfo(PCDA cda, SA_WORD se_number);

/** Use to decode output of @ref SA_GetStorageEnclosurePathInfo to test whether the selected path is active or not.
 */
#define IS_SE_PATH_ACTIVE(path_info, path_index)  ((path_info & ((SA_WORD)0x0001 << path_index)) != 0)
/** Use to decode output of @ref SA_GetStorageEnclosurePathInfo to test whether the selected path is failed or not.
 */
#define IS_SE_PATH_FAILED(path_info, path_index)  ((path_info & ((SA_WORD)0x0100 << path_index)) != 0)

/** @} */ /* storc_properties_se_path_info */

/** @defgroup storc_properties_se_backplane_interface_type Storage Enclosure Backplane Interface Type
 * @{ */
/** Get the backplane interface type of the storage enclosure.
 *
 * @param[in]  cda                      Controller data area.
 * @param[in]  se_number                Storage enclosure number.
 * @return See [Storage Enclosure Backplane Interface Type Returns](@ref storc_properties_se_backplane_interface_type).
 */
SA_BYTE SA_GetStorageEnclosureBackplaneInterfaceType(PCDA cda, SA_WORD se_number);
/** @name Storage Enclosure Backplane Interface Type Returns
 * @{ */
/** No management or LED controller.
 * @outputof SA_GetStorageEnclosureBackplaneInterfaceType.
 */
#define kStorageEnclosureBackplaneInterfaceTypeNone            0x00
/** Direct SGPIO.
 * @outputof SA_GetStorageEnclosureBackplaneInterfaceType.
 */
#define kStorageEnclosureBackplaneInterfaceTypeDirectSGPIO     0x01
/** SGPIO over SMP.
 * @outputof SA_GetStorageEnclosureBackplaneInterfaceType.
 */
#define kStorageEnclosureBackplaneInterfaceTypeSGPIOOverSMP    0x02
/** SSP.
 * @outputof SA_GetStorageEnclosureBackplaneInterfaceType.
 */
#define kStorageEnclosureBackplaneInterfaceTypeSSP             0x03
/** UBM.
 * @outputof SA_GetStorageEnclosureBackplaneInterfaceType.
 */
#define kStorageEnclosureBackplaneInterfaceTypeUBM             0x04
/** Vendor-specific.
 * @outputof SA_GetStorageEnclosureBackplaneInterfaceType.
 */
#define kStorageEnclosureBackplaneInterfaceTypeVendorSpecific  0x05
/** VPP.
 * @outputof SA_GetStorageEnclosureBackplaneInterfaceType.
 */
#define kStorageEnclosureBackplaneInterfaceTypeVPP             0x06
/** Unknown backplane type.
 * @outputof SA_GetStorageEnclosureBackplaneInterfaceType.
 */
#define kStorageEnclosureBackplaneInterfaceTypeUnknown         0xFF
/** @} */ /* Storage Enclosure Backplane Interface Type Returns */
/** @} */ /* storc_properties_se_backplane_interface_type */

/** @defgroup storc_properties_se_location Storage Enclosure Location
 * @{ */ /** Get the location info of the storage enclosure.
 *
 * @param[in]  cda                      Controller data area.
 * @param[in]  se_number                Storage enclosure number.
 * @return See [Storage Enclosure Location Returns](@ref storc_properties_se_location).
 */
SA_BYTE SA_GetStorageEnclosureLocation(PCDA cda, SA_WORD se_number);
/** @name Storage Enclosure Location Returns
 * @{ */
/** Invalid Storage Enclosure Number
 * @outputof SA_GetStorageEnclosureLocation
 */
#define kGetStorageEnclosureLocationInvalidSENumber 0xFF
 /** Storage Enclosure location Internal
 * @outputof SA_GetStorageEnclosureLocation
 */
#define kGetStorageEnclosureLocationInternal 0
/** Storage Enclosure location External
 * @outputof SA_GetStorageEnclosureLocation
 */
#define kGetStorageEnclosureLocationExternal 1
/** Storage Enclosure location Unknown
 * @outputof SA_GetStorageEnclosureLocation
 */
#define kGetStorageEnclosureLocationUnknown  2
/** @} */ /* Storage Enclosure Location Returns */
/** @} */ /* storc_properties_se_location */

/** @defgroup storc_properties_se_ubm_manager_device_code UBM Manager Device Code
 * @{ */

/** @name UBM Backplane Manager Device Codes
 * @{ */
#define kUBMBackplaneManagerDeviceCodeInvalid 0xFFFFFFFF
/** @} */

/** Get the Manager Device Code of a UBM backplane.
 *
 * @param[in]  cda                      Controller data area.
 * @param[in]  se_number                Storage enclosure number.
 * @return Manager Device Code of a UBM backplane.
 * @return @ref kUBMBackplaneManagerDeviceCodeInvalid if no UBM backplane is present.
 */
SA_DWORD SA_GetUBMBackplaneManagerDeviceCode(PCDA cda, SA_WORD se_number);
/** @} */ /* storc_properties_se_ubm_manager_device_code */

/** @defgroup storc_properties_se_ubm_pci_vendor_id UBM PCI Vendor ID
 * @{ */
/** Get the PCI Vendor ID of a UBM backplane.
 *
 * @param[in]  cda                      Controller data area.
 * @param[in]  se_number                Storage enclosure number.
 * @return PCI Vendor ID of a UBM backplane.
 */
SA_WORD SA_GetUBMBackplanePCIVendorID(PCDA cda, SA_WORD se_number);
/** @} */ /* storc_properties_se_ubm_pci_vendor_id */

/** @defgroup storc_properties_se_ubm_backplane_version UBM Backplane Version
 * @{ */

/** Minimum buffer length (in bytes) required for @ref SA_GetUBMBackplaneVersion. */
#define kMinGetUBMBackplaneVersionBufferLength 8

/** Get the UBM backplane version of the storage enclosure.
 *
 * @pre ubm_version_buffer is at least kMinGetUBMBackplaneVersionBufferLength.
 *
 * @param[in]  cda                 Controller data area.
 * @param[in]  se_number           Storage enclosure number.
 * @param[out] ubm_version_buffer  Buffer to write the UBM backplane version.
 * @param[in]  buffer_size         Size (in bytes) of `ubm_version_buffer`.
 * @return NULL terminated ASCII string of the UBM backplane version, same as `ubm_version_buffer`.
 * @return Empty string if given buffer is not large enough.
 */
char *SA_GetUBMBackplaneVersion(PCDA cda, SA_WORD se_number, char *ubm_version_buffer, size_t buffer_size);
/** @} */ /* storc_properties_se_ubm_backplane_version */

/** @defgroup storc_properties_se_ubm_product_string UBM Product String
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetUBMBackplaneProductString. */
#define kMinGetUBMBackplaneProductStringBufferLength 33
/** Get the product string of a UBM backplane.
 *
 * @pre `product_string_buffer` is at least @ref kMinGetUBMBackplaneProductStringBufferLength bytes
 * to contain the product string and terminating NULL.
 *
 * @param[in]  cda                      Controller data area.
 * @param[in]  se_number                Storage enclosure number.
 * @param[out] product_string_buffer    Buffer to write the UBM product string.
 * @param[in]  buffer_size              Size (in bytes) of `product_string_buffer`.
 * @return NULL terminated ASCII string of the UBM product string, same as `product_string_buffer`.
 * @return Empty string if given buffer is not large enough.
 */
char *SA_GetUBMBackplaneProductString(PCDA cda, SA_WORD se_number, char* product_string_buffer, size_t buffer_size);
/** @} */ /* storc_properties_se_ubm_product_string */

/** @defgroup storc_properties_se_ubm_part_number UBM Part Number
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetUBMBackplanePartNumber. */
#define kMinGetUBMBackplanePartNumberBufferLength 33
/** Get the part number of a UBM backplane.
 *
 * @pre `part_number_buffer` is at least @ref kMinGetUBMBackplanePartNumberBufferLength bytes
 * to contain the part number and terminating NULL.
 *
 * @param[in]  cda                      Controller data area.
 * @param[in]  se_number                Storage enclosure number.
 * @param[out] part_number_buffer       Buffer to write the UBM part number.
 * @param[in]  buffer_size              Size (in bytes) of `part_number_buffer`.
 * @return NULL terminated ASCII string of the UBM part number, same as `part_number_buffer`.
 * @return Empty string if given buffer is not large enough.
 */
char *SA_GetUBMBackplanePartNumber(PCDA cda, SA_WORD se_number, char *part_number_buffer, size_t buffer_size);
/** @} */ /* storc_properties_se_ubm_part_number */

/** @defgroup storc_properties_se_ubm_backplane_number UBM Backplane Number
 * @{ */ /** Get the UBM Backplane Number
 *
 * @param[in]  cda                      Controller data area.
 * @param[in]  se_number                Storage enclosure number.
 * @return See [Get UBM Backplace Number Returns](@ref storc_properties_se_ubm_backplane_number).
 */
SA_BYTE SA_GetUBMBackplaneNumber(PCDA cda, SA_WORD se_number);
/** @name Get UBM Backplane Number Returns
* @{ */
/** Invalid Storage Enclosure Number
 * @outputof SA_GetUBMBackplaneNumber
 */
#define kGetUBMBackplaneNumberInvalidSENumber 0xFF
/** Not a UBM Backplane
 * @outputof SA_GetUBMBackplaneNumber
 */
#define kGetUBMBackplaneNumberNotUBMBackplane 0xFE
/** @} */ /* Get UBM Backplane Number Returns */
/** @} */ /* storc_properties_se_ubm_backplane_number */

/** @defgroup storc_properties_se_ubm_backplane_version_number UBM Backplane Version Number
 * @{
 */
/**  Get the UBM Backplane Version Number as a DWORD
 *
 * @param[in]  cda                      Controller data area.
 * @param[in]  se_number                Storage enclosure number.
 * @return See [Get UBM Backplace Version Number Returns](@ref storc_properties_se_ubm_backplane_version_number).
 *
 * __Example__
 * @code
 *    SA_DWORD version = SA_GetUBMBackplaneVersionNumber(cda, 1);
 *    if (version != kGetUBMBackplaneVersionNumberInvalidSENumber &&
 *        version != kGetUBMBackplaneVersionNumberNotUBMBackplane)
 *    {
 *       printf("Backplane Version Number (hex) = 0x%08X\n", version);
 *       printf("Major Version is: %d\n", GET_UBM_BACKPLANE_MAJOR_VERSION(version));
 *       printf("Minor Version is: %d\n", GET_UBM_BACKPLANE_MINOR_VERSION(version));
 *    }
 * @endcode
 */
SA_DWORD SA_GetUBMBackplaneVersionNumber(PCDA cda, SA_WORD se_number);
/** @name Get UBM Backplane Version Number Returns
 * @{ */
/** Invalid Storage Enclosure Number
 * @outputof SA_GetUBMBackplaneVersionNumber
 */
#define kGetUBMBackplaneVersionNumberInvalidSENumber 0xFFFFFFFF
/** Not a UBM Backplane
 * @outputof SA_GetUBMBackplaneVersionNumber
 */
#define kGetUBMBackplaneVersionNumberNotUBMBackplane 0xFFFFFFFE
/** @} */ /* Get UBM Backplane Version Number Returns */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kUBMBackplaneVersionNumberMajorMask 0x0000FF00
#define kUBMBackplaneVersionNumberMajorBitshift 8
#endif
/** Decode output of @ref SA_GetUBMBackplaneVersionNumber to return major version.
 */
#define GET_UBM_BACKPLANE_MAJOR_VERSION(version) (((version) & kUBMBackplaneVersionNumberMajorMask) >> kUBMBackplaneVersionNumberMajorBitshift)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kUBMBackplaneVersionNumberMinorMask 0x000000FF
#define kUBMBackplaneVersionNumberMinorBitshift 0
#endif
/** Decode output of @ref SA_GetUBMBackplaneVersionNumber to return minor version.
 */
#define GET_UBM_BACKPLANE_MINOR_VERSION(version) (((version) & kUBMBackplaneVersionNumberMinorMask) >> kUBMBackplaneVersionNumberMinorBitshift)
/** @} */ /* storc_properties_se_ubm_backplane_version_number */

/** @} */ /* storc_properties_se */

/*******************************************//**
 * @defgroup storc_properties_pd Physical Drive
 * @brief
 * @details
 * @{
 **********************************************/

/** @defgroup storc_properties_pd_enum Enumerate Physical Drives
 * @{
 */

/** @name Physical Drive Enumerate Returns
 * @{ */
#define kInvalidPDNumber 0xFFFF /**< Invalid physical drive number */
/** @} */

/** Enumerates physical drives in the controller.
 *
 * Start with @ref kInvalidPDNumber as the first item.
 *
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Physical drive number.
 * @return Given a drive number, the next valid physical drive number is returned.
 * @return @ref kInvalidPDNumber if no valid drives are connected.
 */
SA_WORD SA_EnumerateControllerPhysicalDrives(PCDA cda, SA_WORD pd_number);

/** Iterate over all physical drives and execute the block with each valid pd_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] pd_number     Variable to store valid physical drive numbers.
 *
 * __Example__
 * @code
 * SA_WORD pd_number;
 * FOREACH_PHYSICAL_DRIVE(cda, pd_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_PHYSICAL_DRIVE(cda, pd_number) \
   for (pd_number = kInvalidPDNumber; (pd_number = SA_EnumerateControllerPhysicalDrives(cda, pd_number)) != kInvalidPDNumber;)
/** @} */ /* storc_properties_pd_enum */

/** @defgroup storc_properties_bootable_pd_enum Enumerate Bootable Physical Drives
 * @{
 */
/** Start enumeration of controller bootable physical drives.
 * Start with kInvalidPDNumber as the first number.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Target physical drive number.
 * @return The next valid physical drive number, or @ref kInvalidPDNumber if no more drives exist.
 */
SA_WORD SA_EnumerateControllerBootablePhysicalDrives(PCDA cda, SA_WORD pd_number);

/** Iterate over all bootable physical drives and execute the block with each valid pd_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] pd_number     Variable to store valid physical drive numbers.
 *
 * __Example__
 * @code
 * SA_WORD pd_number;
 * FOREACH_BOOTABLE_PHYSICAL_DRIVE(cda, pd_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_BOOTABLE_PHYSICAL_DRIVE(cda, pd_number) \
   for (pd_number = kInvalidPDNumber; (pd_number = SA_EnumerateControllerBootablePhysicalDrives(cda, pd_number)) != kInvalidPDNumber;)
/** @} */ /* storc_properties_bootable_pd_enum */

/** @defgroup storc_properties_pd_types Physical Drive Types
 * These serve as inputs to physical drive methods.
 * @{ */
#define kPhysicalDriveTypeUnassigned               0x01 /**< Drive type: Unassigned. */
#define kPhysicalDriveTypeData                     0x02 /**< Drive type: Data. */
#define kPhysicalDriveTypeCache                    0x04 /**< Drive type: LUCache. */
#define kPhysicalDriveTypeSpare                    0x08 /**< Drive type: Spare. */
#define kPhysicalDriveTypeHBA                      0x10 /**< Drive type: HBA. */

#define kPhysicalDriveTypeAny                      0xFF /**< Drive type: Any. */
/** @} */

/** @defgroup storc_properties_pd_count Controller Physical Drive/Device Counts
 * @{ */

/** Return total number of physical drive on the controller regardless of types.
 * @see SA_GetControllerPhysicalDeviceCount.
 * @param[in] cda  Controller data area.
 * @return The total number of physical drives attached to the controller.
 */
SA_WORD SA_GetControllerTotalPhysicalDriveCount(PCDA cda);

/** Get the total number of physical devices (drives+non-drives) attached to the controller.
 * @param[in] cda  Controller data area.
 * @return The total number of physical devices attached to the controller.
 */
SA_WORD SA_GetControllerPhysicalDeviceCount(PCDA cda);

/** Get the number of unassigned physical drives attached to the controller.
 *
 * This includes the nubmer of transient unassigned drives.
 *
 * @param[in] cda  Controller data area.
 * @return The number of unassigned physical drives attached to the controller.
 */
SA_WORD SA_GetControllerUnassignedPhysicalDriveCount(PCDA cda);

/** Get the number of data physical drives attached to the controller.
 *
 * This includes the nubmer of transient data drives.
 *
 * @param[in] cda  Controller data area.
 * @return The number of data physical drives attached to the controller.
 */
SA_WORD SA_GetControllerDataPhysicalDriveCount(PCDA cda);

/** Get the number of cache physical drives attached to the controller.
 *
 * @param[in] cda  Controller data area.
 * @return The number of cache physical drives attached to the controller.
 */
SA_WORD SA_GetControllerCachePhysicalDriveCount(PCDA cda);

/** Get the number of spare physical drives attached to the controller.
 *
 * @param[in] cda  Controller data area.
 * @return The number of spare physical drives attached to the controller.
 */
SA_WORD SA_GetControllerSparePhysicalDriveCount(PCDA cda);

/** Get the number of HBA physical drives attached to the controller.
 *
 * @param[in] cda  Controller data area.
 * @return The number of HBA physical drives attached to the controller.
 */
SA_WORD SA_GetControllerHBAPhysicalDriveCount(PCDA cda);

/** Get number of physical drives on the Controller.
 * @param[in] cda         Controller data area.
 * @param[in] drive_type  Map from @ref storc_properties_pd_types.
 * @return Number of physical drives (of target type) attached to the Controller.
 *
 * __Examples__
 * @code
 * printf("Number of Cache & Data Drives: %lu\n",
 *     SA_GetControllerPhysicalDriveCount(cda, kPhysicalDriveTypeData | kPhysicalDriveTypeCache));
 * printf("Total Number of Drives: %lu\n", SA_GetControllerPhysicalDriveCount(cda, kPhysicalDriveTypeAny));
 * @endcode
 */
SA_WORD SA_GetControllerPhysicalDriveCount(PCDA cda, SA_BYTE drive_type);
/** @} */

/** @defgroup storc_properties_pd_unique_id Physical Drive Unique Volume ID
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetPhysicalDriveUniqueVolumeID. */
#define kMinGetPhysicalDriveUniqueVolumeIDBufferLength 33
/** Get Unique Volume Identifier for the physical drive.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `pd_number` is a valid physical drive number.
 * @pre `volume_id_buffer` is at least @ref kMinGetPhysicalDriveUniqueVolumeIDBufferLength bytes
 * to contain the unique volume ID and terminating NULL.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  pd_number         Physical drive number.
 * @param[out] volume_id_buffer  Buffer to write the unique volume ID.
 * @param[in]  buffer_size       Size (in bytes) of `volume_id_buffer`.
 * @return NULL terminating ASCII string of the physical drive unique volume ID, same as `volume_id_buffer`.
 * @return Empty string if given buffer was not large enough to store entire unique volume ID.
 */
char *SA_GetPhysicalDriveUniqueVolumeID(PCDA cda, SA_WORD pd_number, char *volume_id_buffer, size_t buffer_size);
/** @} */ /* storc_properties_pd_unique_id */

/** @defgroup storc_properties_pd_boot_priority Physical Drive Boot Priority
 * @{ */
/** Get the boot priority of the physical drive.
 *
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Physical drive number.
 * @return [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume).
 */
SA_BYTE SA_GetPhysicalDriveBootPriority(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_properties_pd_boot_priority */

/** @defgroup storc_properties_pd_se_number Physical Drive Enclosure Number
 * @{ */
/** Get the storage enclosure number for the specified device
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Selected physical device number.
 * @return The storage enclosure number where the device resides. @ref kInvalidSENumber if the device does not reside in a storage enclosure.
 */
SA_WORD SA_GetPhysicalDeviceStorageEnclosureNumber(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_properties_pd_se_number */

/** @defgroup storc_properties_pd_basic_info Physical Drive Basic Info
 * @{ */
/** Get basic information on the physical drive.
 *
 * @pre `pd_number` is a valid physical drive number.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Target physical drive number.
 * @return Information about: Drive Type (SAS,SATA : HDD,SSD,Tape).
 * @return See [Physical Drive Basic Info Returns](@ref storc_properties_pd_basic_info).
 */
SA_DWORD SA_GetPhysicalDriveBasicInfo(PCDA cda, SA_WORD pd_number);
   /** @name Physical Drive Basic Info Returns
    * @outputof SA_GetPhysicalDriveBasicInfo.
    * @{ */
   #define kPhysicalDriveTypeSAS          0x00000001 /**< Physical drive type SAS; @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveTypeSATA         0x00000002 /**< Physical drive type SATA; @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveTypeNVMe         0x00000004 /**< Physical drive type NVMe; @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveTypeUnknown      0x00000008 /**< Physical drive type Unknown; @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveInterfaceMask    0x0000000F /**< Mask @ref SA_GetPhysicalDriveBasicInfo to get physical drive media. */
   #define kPhysicalDriveTypeSSD          0x00000010 /**< Physical drive type SSD; @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveTypeHDD          0x00000020 /**< Physical drive type HDD; @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveTypeTape         0x00000040 /**< Physical drive type Tape; @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveMediaMask        0x00000070 /**< Mask @ref SA_GetPhysicalDriveBasicInfo to get physical drive interface. */
   #define kPhysicalDriveTypeMask         (kPhysicalDriveInterfaceMask | kPhysicalDriveMediaMask) /**< Mask @ref SA_GetPhysicalDriveBasicInfo to get physical drive type. */
   #define kPhysicalDriveIsPresent        0x00000080 /**< Physical drive is present, i.e. has been discovered via the main drive interface (SAS, PCIe, etc.); @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveRecordingSMRHA   0x00000100 /**< Physical drive recording method SMR HA; @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveRecordingSMRHM   0x00000200 /**< Physical drive recording method SMR HM; @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveRecordingSMRDM   0x00000400 /**< Physical drive recording method SMR DM; @outputof SA_GetPhysicalDriveBasicInfo. */
   #define kPhysicalDriveRecordingMask    0x00000700 /**< Mask @ref SA_GetPhysicalDriveBasicInfo to get physical drive recording method. */
   #define kPhysicalDriveOnBootConnector  0x00000800 /**< Physical drive is attached to boot connector */
   #define kPhysicalDriveIsInstalled      0x00001000 /**< Physical drive is installed, i.e. has been discovered via either the main drive interface or via a side-band interface; @outputof SA_GetPhysicalDriveBasicInfo. */
   /** Decode output of @ref SA_GetPhysicalDriveBasicInfo to get drive type. */
   #define PHYSICAL_DRIVE_TYPE(info_value) (SA_BYTE)(info_value & kPhysicalDriveTypeMask)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is drive type SAS? */
   #define IS_SAS_PHYSICAL_DRIVE(info_value) ((info_value & kPhysicalDriveTypeSAS) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is drive type SATA? */
   #define IS_SATA_PHYSICAL_DRIVE(info_value) ((info_value & kPhysicalDriveTypeSATA) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is drive type NVMe? */
   #define IS_NVME_PHYSICAL_DRIVE(info_value) ((info_value & kPhysicalDriveTypeNVMe) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is drive type Unknown? */
   #define IS_UNKNOWN_PHYSICAL_DRIVE(info_value) ((info_value & kPhysicalDriveTypeUnknown) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is drive type SSD? */
   #define IS_SSD_PHYSICAL_DRIVE(info_value) ((info_value & kPhysicalDriveTypeSSD) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is drive type HDD? */
   #define IS_HDD_PHYSICAL_DRIVE(info_value) ((info_value & kPhysicalDriveTypeHDD) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is recording method SMR HA? */
   #define IS_SMR_HA_PHYSICAL_DRIVE(info_value) ((info_value & kPhysicalDriveRecordingSMRHA) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is recording method SMR HM? */
   #define IS_SMR_HM_PHYSICAL_DRIVE(info_value) ((info_value & kPhysicalDriveRecordingSMRHM) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is recording method SMR DM? */
   #define IS_SMR_DM_PHYSICAL_DRIVE(info_value) ((info_value & kPhysicalDriveRecordingSMRDM) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is recording method any SMR? */
   #define IS_SMR_PHYSICAL_DRIVE(info_value) (IS_SMR_HA_PHYSICAL_DRIVE(info_value) || \
                                              IS_SMR_HM_PHYSICAL_DRIVE(info_value) || \
                                              IS_SMR_DM_PHYSICAL_DRIVE(info_value))
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is drive present? */
   #define IS_PHYSICAL_DRIVE_PRESENT(info_value) ((info_value & kPhysicalDriveIsPresent) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is drive installed? */
   #define IS_PHYSICAL_DRIVE_INSTALLED(info_value) ((info_value & kPhysicalDriveIsInstalled) ? kTrue : kFalse)
   /** From output of @ref SA_GetPhysicalDriveBasicInfo, is drive attached to boot connector? */
   #define IS_PHYSICAL_DRIVE_ON_BOOT_PORT(info_value) (((info_value) & kPhysicalDriveOnBootConnector) ? kTrue : kFalse)
   /** @} */
/** @} */

/** @defgroup storc_properties_pd_usage Physical Drive Usage
 * @{ */
/** Obtain the usage information for the physical drive.
 * @pre `pd_number` is a valid physical drive number.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Target physical drive number.
 * @return [Physical Drive Usage Returns](@ref storc_properties_pd_usage).
 */
SA_BYTE SA_GetPhysicalDriveUsageInfo(PCDA cda, SA_WORD pd_number);
   /** @name Physical Drive Usage Returns
    * @outputof SA_GetPhysicalDriveUsageInfo.
    * @{ */
   #define kPhysicalDriveUsageUnassigned              0x01   /**< Unassigned but elegible for assignment; @outputof SA_GetPhysicalDriveUsageInfo. */
   #define kPhysicalDriveUsageData                    0x02   /**< Data drive; @outputof SA_GetPhysicalDriveUsageInfo. */
   #define kPhysicalDriveUsageAutoReplaceStandbySpare 0x03   /**< Auto-replace (roaming) Spare; @outputof SA_GetPhysicalDriveUsageInfo. */
   #define kPhysicalDriveUsageShareableStandbySpare   0x04   /**< Shareable spare (standby); @outputof SA_GetPhysicalDriveUsageInfo. */
   #define kPhysicalDriveUsageShareableActiveSpare    0x05   /**< Shareable spare (active); @outputof SA_GetPhysicalDriveUsageInfo. */
   #define kPhysicalDriveUsageTransientData           0x06   /**< Transient data drive; @outputof SA_GetPhysicalDriveUsageInfo. */
   #define kPhysicalDriveUsageLUCacheData             0x07   /**< LU cache data drive; @outputof SA_GetPhysicalDriveUsageInfo. */
   #define kPhysicalDriveUsageHBA                     0x08   /**< HBA drive; @outputof SA_GetPhysicalDriveUsageInfo. */
   #define kPhysicalDriveUsageTransientUnassigned     0x09   /**< Transient unassigned (will be in HBA mode on reboot); @outputof SA_GetPhysicalDriveUsageInfo. */
   #define kPhysicalDriveUsageUnconfigurable          0x0A   /**< Unsupported for RAID drive; @outputof SA_GetPhysicalDriveUsageInfo. */
   /** @} */
/** @} */

/** @defgroup storc_properties_pd_size Physical Drive Size
 * @{ */
/** @name Physical Drive Size Types
 *
 * Input to @ref SA_GetPhysicalDriveSize.
 * @{ */
#define kPhysicalDriveSizeTotalBlocks        0x01 /**< Get size of physical drive in blocks; Input to @ref SA_GetPhysicalDriveSize. */
#define kPhysicalDriveSizeTotalBytes         0x02 /**< Get size of physical drive in bytes; Input to @ref SA_GetPhysicalDriveSize. */
#define kPhysicalDriveSizeUsableBlocks       0x03 /**< Get usable size of physical drive in blocks; Input to @ref SA_GetPhysicalDriveSize. */
#define kPhysicalDriveSizeUsableBytes        0x04 /**< Get usable size of physical drive in bytes; Input to @ref SA_GetPhysicalDriveSize. */
#define kPhysicalDriveBlockSizeBytes         0x05 /**< Get size of a physical drive's blocks (in bytes); Input to @ref SA_GetPhysicalDriveSize. */
#define kPhysicalDriveNativeBlockSizeBytes   0x06 /**< Get physical drive's native block size (in bytes); Input to @ref SA_GetPhysicalDriveSize. */
#define kPhysicalDriveReservedSizeBlocks     0x07 /**< Get reserved size of physical drive in blocks; Input to @ref SA_GetPhysicalDriveSize. */
#define kPhysicalDriveReservedSizeBytes      0x08 /**< Get reserved size of physical drive in bytes; Input to @ref SA_GetPhysicalDriveSize. */
/** @} */

/** Get physical device size.
 * @pre `pd_number` is a valid physical drive number.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Target physical drive number.
 * @param[in] size_type  What type of drive size to return (see [Physical Drive Size Types](@ref storc_properties_pd_size)).
 * @return Size of physical drive depending on param [size_type](@ref storc_properties_pd_size).
 */
SA_QWORD SA_GetPhysicalDriveSize(PCDA cda, SA_WORD pd_number, SA_BYTE size_type);
/** @} */

/** @defgroup storc_properties_pd_flags Physical Drive Flags
 * @{ */

/** @name Physical Drive Flags
 * @{ */
#define  PDF_StampedForMNP                            0  /**< Stamped for Monitoring and Performance Statistics */
#define  PDF_PresentAndOperational                    1  /**< Drive is present and operational */
#define  PDF_IsNonDiskDevice                          2  /**< Drive is not a disk device */
#define  PDF_SMARTSupported                           3  /**< Drive supports S.M.A.R.T. */
#define  PDF_SMARTEnabled                             4  /**< Drive S.M.A.R.T. functionality is enabled */
#define  PDF_SMARTErrorsInMNPFactory                  5  /**< S.M.A.R.T. errors are recorded in the factory Monitoring and Performance data for this drive */
#define  PDF_SMARTErrorsInMNPSincePowerOn             6  /**< S.M.A.R.T. errors are recorded in the Since PowerOn Monitoring and Performance data for this drive */
#define  PDF_AttachedToExternalConnector              7  /**< Drive is attached to an external connector */
#define  PDF_IsDataDrive                              8  /**< Drive is configured as part of a logical drive */
#define  PDF_IsSpareDrive                             9  /**< Drive is configured as a spare drive */
#define  PDF_DriveWriteCacheEnabledOnSpinup           10 /**< Write caching was enabled when the drive was spun up */
#define  PDF_DoesNotSupportMultiInitiatorOperation    11 /**< Drive does not support multiple initiators */
#define  PDF_DriveWriteCacheEnabled                   12 /**< Write cache is enabled */
#define  PDF_DriveWriteCacheIsChangeable              13 /**< Write cache can be enabled/disabled on this drive */
#define  PDF_IsSMARTOverheated                        14 /**< Drive is reporting on S.M.A.R.T. page that is it overheated */
#define  PDF_IsVirtualRAMDisk                         15 /**< Drive is a RAM drive */
#define  PDF_NCQSupported                             16 /**< This is a SATA drive that supports NCQ (Native Command Queuing */
#define  PDF_NCQEnabled                               17 /**< SATA NCQ is currently enabled on this drive */
#define  PDF_DriveNotSupportedForConfiguration        18 /**< This drive is not supported for configuration */
#define  PDF_MappedToZonedDriveBay                    19 /**< Drive is mapped tp a zoned drive bay */
#define  PDF_DriveHasOverheated                       20 /**< Drive has overheated at some point in its operating life */
#define  PDF_HasKnownBadFWVersion                     21 /**< Drive has a known-bad firmware version */
#define  PDF_DriveIsOffline                           22 /**< Drive is currently offline */
#define  PDF_DriveIsOfflineDueToEraseOperation        23 /**< Drive is offline due to an erase operation */
#define  PDF_IsSSD                                    24 /**< Drive is a solid-state device (SSD) */
#define  PDF_HasUnsupportedMetadata                   25 /**< Drive contains metadata that is unsupported on this controller */
#define  PDF_IsLowestNumberedDeviceWithKnownBadFW     26 /**< This is the first (lowest numbered) drive of this model that has a known-bad firmware version */
#define  PDF_IsBehindSASSwitchEnclosure               27 /**< Drive is or is located behind a SAS switch enclosure */
#define  PDF_IsSMARTWornOut                           28 /**< S.M.A.R.T. predictive failure was enabled dues to wearout */
#define  PDF_SSDWearInformationSupported              29 /**< Drive supports SSD Wear Gauge information */
#define  PDF_UnsupportedDueToMissingLicenseKey        30 /**< Drive is unsupported due to a missing license key */
#define  PDF_RPICapable                               31 /**< Drive is RPI (Rapid Parity Initialization) capable */
#define  PDF_ImmediateModeFWUpdateSupported           32 /**< Drive supports immediate mode disk drive firmware updates  */
#define  PDF_SupportsOverwrite                        33 /**< Drive supports Overwrite Sanitize operations */
#define  PDF_SupportsBlockErase                       34 /**< Drive supports Block Erase Sanitize operations */
#define  PDF_SupportsCryptoErase                      35 /**< Drive supports Crypto Erase Sanitize operations */
#define  PDF_SupportsUnrestrictedSanitizeOperations   36 /**< Drive supports unrestricted Sanitize operations */
#define  PDF_VendorVerifiedSupport                    37 /**< Drive vendor has verified the supported Sanitize methods */
#define  PDF_DriveSanitizingAfterPowerLoss            38 /**< Drive was sanitizing after a power loss */
#define  PDF_NoSanitizeOperationsSupported            39 /**< Drive does not support any controller-supported Sanitize operations */
#define  PDF_HasBeenPowerCycledForErrorRecovery       40 /**< Drive has been power-cycled for error recovery at least 3 times in the last 24 hour moitoring period. */
#define  PDF_DriveIsEncrypted                         41 /**< The drive is part of an encrypted logical drive. */
#define  PDF_SpareRebuildsDisabled                    42 /**< Indicates that volumes on this drive should not be rebuilt to a spare. */
#define  PDF_WriteBufferModeDSupported                43 /**< The drive supports write buffer mode D for microcode download as defined in SPC-4r36. */
#define  PDF_WriteBufferModeESupported                44 /**< The drive supports write buffer mode E for microcode download as defined in SPC-4r36. */
#define  PDF_WriteBufferMode7Supported                45 /**< The drive supports write buffer mode 7 for microcode download as defined in Inquiry VPD Page D0. */
#define  PDF_WriteBufferMode5Supported                46 /**< The drive supports write buffer mode 7 for microcode download as defined in Inquiry VPD Page D0. */
#define  PDF_DriveMaskedFromOSWithRIS                 47 /**< The drive is masked from the OS due to the presense of SmartArray RAID metadata (RIS). */
#define  PDF_IsHBADrive                               48 /**< The drive is in HBA mode. */
#define  PDF_SelectedAsPrimaryBootVolume              49 /**< Set if the drive is in HBA mode and selected as the primary boot volume. */
#define  PDF_SelectedAsSecondaryBootVolume            50 /**< Set if the drive is in HBA mode and selected as the secondary boot volume. */
#define  PDF_HostAwareSMRDrive                        51 /**< Set to '1' if the Physical Drive is a Host Aware SMR drive. */
#define  PDF_HostManagedSMRDrive                      52 /**< Set to '1' if the Physical Drive is a Host Managed SMR drive. */
#define  PDF_DeviceManagedSMRDrive                    53 /**< Set to '1' if the Physical Drive is a Device Managed SMR drive. */
#define  PDF_DeviceExposedToHost                      54 /**< Set to '1' if the physical drive is exposed/visible to the host. */
#define  PDF_DeviceOnBootConnector                    55 /**< Set to '1' if the physical drive is attached to a dedicated boot connector. */

#define  PDF_LastValidFlag                            56
/** @} */

/** Obtain all the physical drive flags for the given pd number.
  * @param[in] cda        Controller data area.
  * @param[in] pd_number  Target physical drive number.
  * @post Return buffer must be cleared using @ref SA_DestroyPhysicalDriveFlagsInfo.
  * @return Physical drive flags object that can be used with @ref SA_IsPhysicalDriveFlagSet.
  */
void *SA_GetPhysicalDriveFlagsInfo(PCDA cda, SA_WORD pd_number);

/** Obtain all the physical drive flags for the given pd number.
  * @param[in] pd_flags_info Physical drive flags object created using @ref SA_GetPhysicalDriveFlagsInfo.
  * @param[in] drive_flag    One of the individual [Physical Drive Flags](@ref storc_properties_pd_flags).
  * @pre Buffer was populated using @ref SA_GetPhysicalDriveFlagsInfo.
  * @return TRUE iff the requested `drive_flag` flag is set in `pd_flags_info`.
  */
SA_BOOL SA_IsPhysicalDriveFlagSet(void *pd_flags_info, SA_BYTE drive_flag);

/** Destroy the drive flags object created using @ref SA_GetPhysicalDriveFlagsInfo.
 * @param[in] pd_flags_info Physical drive flags object created using @ref SA_GetPhysicalDriveFlagsInfo.
 */
void SA_DestroyPhysicalDriveFlagsInfo(void *pd_flags_info);

/** @} */ /* storc_properties_pd_flags */

/** @defgroup storc_properties_pd_port_index Physical Drive Connector/Port Index
 * @{ */
/** Get the connector index of physical drive's active path port.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Target physical drive number.
 * @return Connector index of drive's active path or @ref kInvalidConnectorIndex.
 */
SA_WORD SA_GetConnectorIndexOfPhysicalDrive(PCDA cda, SA_WORD pd_number);
/** Alias for @ref SA_GetConnectorIndexOfPhysicalDrive */
SA_WORD SA_GetPhysicalDriveConnectorIndex(PCDA cda, SA_WORD pd_number);
/** Enumerate all ports associated with a physical drive.
 * @see storc_properties_pd_port_index
 * @pre `pd_number` is a valid physical drive number.
 * @param[in] cda              Controller data area.
 * @param[in] pd_number        Target physical drive number.
 * @param[in] connector_index  Current connector index (or @ref kInvalidConnectorIndex).
 * @return If `connector_index` is @ref kInvalidConnectorIndex, first connector index
 * with a path to the drive. Otherwise, next connector index with a path to the drive.
 * __Example__
 * @code
 * SA_WORD connector_index = kInvalidConnectorIndex;
 * while ((connector_index = SA_EnumeratePhysicalDriveConnectors(cda, pd_number, connector_index)) != kInvalidConnectorIndex)
 * {
 *    printf("PD %u is attached by connector %u\n", pd_number, connector_index);
 * }
 * @endcode
 */
SA_WORD SA_EnumeratePhysicalDriveConnectors(PCDA cda, SA_WORD pd_number, SA_WORD connector_index);
/** @} */

/** @defgroup storc_properties_pd_erase_status_info Physical Drive Erase Status Information
* @{ */

/** Refresh physical drive erase status/progress reported by @ref SA_GetPhysicalDriveEraseInfo.
 * @post Calling @ref SA_GetPhysicalDriveEraseInfo will return up-to-date erase status information on a physical drive.
 * @param cda  Controller data area.
 */
void SA_RefreshDriveEraseInfo(PCDA cda);

/** Get erase information for the physical drive.
 * @pre Use @ref SA_RefreshDriveEraseInfo periodically to get the latest erase status information.
 * @param[in] cda       Controller data area.
 * @param[in] pd_number Physical drive number
 * @return Physical drive erase information
 */
SA_DWORD SA_GetPhysicalDriveEraseInfo(PCDA cda, SA_WORD pd_number);
   /** @name Physical Drive Erase Statuses
    * @{ */
   /** Physical drive erase status not erasing.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusNotErasing       0x00000000
   /** Physical drive erase status erase queued.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusEraseQueued      0x00000001
   /** Physical drive erase status erase in progress.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusEraseInProgress  0x00000002
   /** Physical drive erase status erase completed.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusEraseCompleted   0x00000003
   /** Physical drive erase status erase failed.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusEraseFailed      0x00000004
   /** Physical drive erase status erase aborted.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusEraseAborted     0x00000005
   /** Use to mask output of @ref SA_GetPhysicalDriveEraseInfo for the physical drive erase status.
    * @see PHYSICAL_DRIVE_ERASE_STATUS.
    */
   #define kPhysicalDriveEraseStatusMask             0x0000000F
   /** Use to decode the physical drive's erase status output of @ref SA_GetPhysicalDriveEraseInfo
    *
    * __Example__
    * @code{.c}
    * SA_DWORD erase_status = SA_GetPhysicalDriveEraseInfo(cda, pd_number);
    * printf("Physical Drive %u Erase Statuses\n", pd_number);
    * if (PHYSICAL_DRIVE_ERASE_STATUS(pd_status) & kPhysicalDriveEraseStatusEraseInProgress)
    *     printf(" - Erase in Progress\n");
    * if (PHYSICAL_DRIVE_ERASE_STATUS(pd_status) & kPhysicalDriveEraseStatusEraseQueued)
    *     printf(" - Erase Queued\n");
    * // ...
    * @endcode
    */
   #define PHYSICAL_DRIVE_ERASE_STATUS(info_value) (info_value & kPhysicalDriveEraseStatusMask)

   /** Physical drive erase method: 1 pass of zeros.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusMethod1Pass      0x00000001
   /** Physical drive erase method: 1 random pass and 1 zero pass.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusMethod2Pass      0x00000002
   /** Physical drive erase method: 2 random passes and 1 zero pass.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusMethod3Pass      0x00000003
   /** Physical drive erase method: Sanitize Cryptographic Scramble (crypto scramble) changes the internal encryption keys that are used for user data, making the data unretrievable.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusMethodCrypto     0x00000004
   /** Physical drive erase method: Sanitize Block Erase sets the blocks on the drive to a vendor specific value, removing all user data.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusMethodBlock      0x00000005
   /** Physical drive erase method: Sanitize Overwrite fills the user data area of the drive with a pattern.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusMethodOverwrite  0x00000006
   /** Physical drive erase method: A Sanitize Erase Method is being performed on the physical drive.
    * @outputof SA_GetPhysicalDriveEraseInfo.
    */
   #define kPhysicalDriveEraseStatusMethodSanitize   0x00000007
   /** Physical drive erase method: Low Level Format (deprecated).
    * @deprecated No longer supported.
    */
   #define kPhysicalDriveEraseStatusMethodLow        0x00000008
   /** Use to decode physical drive's erase status method output of @ref SA_GetPhysicalDriveEraseInfo.
    * @see kPhysicalDriveEraseStatusProgressMask.
    */
   #define kPhysicalDriveEraseStatusMethodBitShift   8
   /** Use to decode physical drive's erase status method output of @ref SA_GetPhysicalDriveEraseInfo. */
   #define kPhysicalDriveEraseStatusMethodMask       0x00000F00
   #define PHYSICAL_DRIVE_ERASE_METHOD(info_value) ((info_value & kPhysicalDriveEraseStatusMethodMask) >> kPhysicalDriveEraseStatusMethodBitShift)

   /** Use to decode physical drive's erase progress output of @ref SA_GetPhysicalDriveEraseInfo.
    * @see kPhysicalDriveEraseStatusProgressMask.
    */
   #define kPhysicalDriveEraseStatusProgressBitShift 16
   /** Use to decode physical drive's erase progress output of @ref SA_GetPhysicalDriveEraseInfo. */
   #define kPhysicalDriveEraseStatusProgressMask     0xFFFF0000
   #define PHYSICAL_DRIVE_ERASE_PROGRESS(info_value) ((info_value & kPhysicalDriveEraseStatusProgressMask) >> kPhysicalDriveEraseStatusProgressBitShift)
   /** @} */

/** Get the live physical drive erasing progress.
 *
 * This is an alternative of using @ref SA_RefreshDriveEraseInfo and @ref SA_GetPhysicalDriveEraseInfo
 * that does not require or change the state of any cached data in the CDA.
 *
 * @warning Calling this API repeatedly and often can impact controller performance
 * since each invocation resends necessary commands to the device.
 *
 * @param[in] cda @ref storc_cda
 * @param[in] pd_number Valid pd_number of a physical drive undergoing erase.
 * @return The current erasing progress percent [0,100] of the physical drive.
 * @return 101 of the drive is currently not erasing or queued for erase.
 */
SA_BYTE SA_GetPhysicalDriveEraseProgressLive(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_properties_pd_erase_status_info */

/** @defgroup storc_properties_pd_model Physical Drive Model
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetPhysicalDriveModel. */
#define kMinGetPhysicalDriveModelBufferLength 49
/** Write the physical drive model to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `drive_model_buffer` is at least @ref kMinGetPhysicalDriveModelBufferLength bytes
 * to contain the drive model and terminating NULL.
 *
 * @param[in]  cda                  Controller data area.
 * @param[in]  pd_number            Physical drive number.
 * @param[out] drive_model_buffer   Buffer to write vendor ID.
 * @param[in]  buffer_size          Size (in bytes) of `vendor_id_buffer`.
 * @return Physical drive model.
 * @return Empty string if given buffer was not large enough to store entire drive model.
 */
char *SA_GetPhysicalDriveModel(PCDA cda, SA_WORD pd_number, char *drive_model_buffer, size_t buffer_size);
/** @} */ /* storc_properties_pd_model */

/** @defgroup storc_properties_pd_vendor Physical Drive Vendor
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetPhysicalDriveVendorID. */
#define kMinGetPhysicalDriveVendorIDBufferLength 9
/** Write the physical drive vendor name to the buffer.
 *
 * @attention @p buffer_size bytes will be initialized to zero.
 *
 * @pre @p vendor_buffer is at least @ref kMinGetPhysicalDriveVendorIDBufferLength bytes
 * to contain the drive vendor name and terminating NULL.
 *
 * @param[in]  cda            Controller data area.
 * @param[in]  pd_number      Physical drive number.
 * @param[out] vendor_buffer  Buffer to write vendor ID.
 * @param[in]  buffer_size    Size (in bytes) of @p vendor_buffer.
 * @return Physical drive vendor name.
 * @return Empty string if given buffer was not large enough to store entire drive vendor name.
 */
char *SA_GetPhysicalDriveVendorID(PCDA cda, SA_WORD pd_number, char *vendor_buffer, size_t buffer_size);
/** @} */ /* storc_properties_pd_vendor */

/** @defgroup storc_properties_pd_product Physical Drive Product Name
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetPhysicalDriveProductID. */
#define kMinGetPhysicalDriveProductIDBufferLength 41
/** Write the physical drive product name to the buffer.
 *
 * @attention @p buffer_size bytes will be initialized to zero.
 *
 * @pre @p product_buffer is at least @ref kMinGetPhysicalDriveProductIDBufferLength bytes
 * to contain the drive product name and terminating NULL.
 *
 * @param[in]  cda             Controller data area.
 * @param[in]  pd_number       Physical drive number.
 * @param[out] product_buffer  Buffer to write product name.
 * @param[in]  buffer_size     Size (in bytes) of @p product_buffer.
 * @return Physical drive product name.
 * @return Empty string if given buffer was not large enough to store entire drive product name.
 */
char *SA_GetPhysicalDriveProductID(PCDA cda, SA_WORD pd_number, char *product_buffer, size_t buffer_size);
/** @} */ /* storc_properties_pd_product */

/** @defgroup storc_properties_pd_serial_number Physical Drive Serial Number
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetPhysicalDriveSerialNumber. */
#define kMinGetPhysicalDriveSerialNumberBufferLength 41
/** Write the physical drive serial number to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `serial_number_buffer` is at least @ref kMinGetPhysicalDriveSerialNumberBufferLength bytes
 * to contain the drive model and terminating NULL.
 *
 * @param[in]  cda                  Controller data area.
 * @param[in]  pd_number            Physical drive number.
 * @param[out] serial_number_buffer Buffer to write vendor ID.
 * @param[in]  buffer_size          Size (in bytes) of `serial_number_buffer`.
 * @return Serial number of the controller.
 * @return Empty string if given buffer was not large enough to store entire drive serial number.
 */
char *SA_GetPhysicalDriveSerialNumber(PCDA cda, SA_WORD pd_number, char *serial_number_buffer, size_t buffer_size);
/** @} */ /* storc_properties_pd_serial_number */

/** @defgroup storc_properties_pd_firmware_version Physical Drive Firmware Version
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetPhysicalDriveFirmwareVersion. */
#define kMinGetPhysicalDriveFirmwareVersionBufferLength 9
/** Write the physical drive firmware version to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `firmware_version_buffer` is at least @ref kMinGetPhysicalDriveFirmwareVersionBufferLength bytes
 * to contain the drive firmware version and terminating NULL.
 *
 * @param[in]  cda                     Controller data area.
 * @param[in]  pd_number               Physical drive number.
 * @param[out] firmware_version_buffer Buffer to write vendor ID.
 * @param[in]  buffer_size             Size (in bytes) of `firmware_version_buffer`.
 * @return Firmware version of the physical drive.
 * @return Empty string if given buffer was not large enough to store entire drive firmware version.
 */
char *SA_GetPhysicalDriveFirmwareVersion(PCDA cda, SA_WORD pd_number, char *firmware_version_buffer, size_t buffer_size);
/** @} */ /* storc_properties_pd_firmware_version */

/** @defgroup storc_properties_pd_min_firmware_version Physical Drive Minimum Firmware Version
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetPhysicalDriveMinFirmwareVersion. */
#define kMinGetPhysicalDriveMinFirmwareVersionBufferLength 9
/** Write the physical drive minimum firmware version to the buffer.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre `min_firmware_version_buffer` is at least @ref kMinGetPhysicalDriveMinFirmwareVersionBufferLength bytes
 * to contain the drive minimum firmware version and terminating NULL.
 *
 * @param[in]  cda                           Controller data area.
 * @param[in]  pd_number                     Physical drive number.
 * @param[out] min_firmware_version_buffer   Buffer to write vendor ID.
 * @param[in]  buffer_size                   Size (in bytes) of `min_firmware_version_buffer`.
 * @return Serial number of the controller.
 * @return Empty string if given buffer was not large enough to store entire drive minimum firmware version.
 */
char *SA_GetPhysicalDriveMinFirmwareVersion(PCDA cda, SA_WORD pd_number, char *min_firmware_version_buffer, size_t buffer_size);
/** @} */ /* storc_properties_pd_min_firmware_version */

/** @defgroup storc_properties_pd_current_temperature Physical Drive Current Temperature
 * @{ */
/** Get the current temperature of the physical drive.
 * This function returns the cached current temperature since the last @ref SA_ReInitializeCDA.
 * If you need the non-cached temperature, use function @ref SA_GetPhysicalDriveTemperatureInfo().
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @return Current temperature of the physical drive (unsigned).
 * @return 0x00 or 0xFF if unknown.
 */
SA_BYTE SA_GetPhysicalDriveCurrentTemperature(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_properties_pd_current_temperature */

/** @defgroup storc_properties_pd_max_temperature Physical Drive Maximum Temperature
 * @{ */
/** Get the maximum temperature of the physical drive.
 * This function returns the cached maximum temperature since the last @ref SA_ReInitializeCDA.
 * If you need the non-cached temperature, use function @ref SA_GetPhysicalDriveTemperatureInfo().
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @return Maximum temperature of the physical drive (unsigned).
 * @return 0x00 if unknown.
 */
SA_BYTE SA_GetPhysicalDriveMaximumTemperature(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_properties_pd_max_temperature */

/** @defgroup storc_properties_pd_temperature_info Physical Drive Current and Maximum Temperature
 * @{ */
/** Get the current and maximum temperature of the physical drive live.
 *
 * It pulls the current temperature without using the cached data from the last @ref SA_ReInitializeCDA.
 *
 * @pre `pd_number` must be a valid pd number.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @return Current and Maximum temperature of the physical drive (unsigned).
 * @return 0x00 or 0xFF if unknown.
 * @return Use @ref PD_CURRENT_TEMPERATURE and @ref PD_CURRENT_TEMPERATURE to decode output.
 */
SA_WORD SA_GetPhysicalDriveTemperatureInfo(PCDA cda, SA_WORD pd_number);
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kPhysicalDriveCurrentTemperatureMask    0x00FF
#define kPhysicalDriveCurrentTemperatureOffset  0
#define kPhysicalDriveMaximumTemperatureMask    0xFF00
#define kPhysicalDriveMaximumTemperatureOffset  8
#endif
/** Use to get the current temperature of the physical drive from [SA_GetPhysicalDriveTemperatureInfo](@ref storc_properties_pd_temperature_info). */
#define PD_CURRENT_TEMPERATURE(temp_info) ((SA_BYTE)(((temp_info) & kPhysicalDriveCurrentTemperatureMask) >> kPhysicalDriveCurrentTemperatureOffset))
/** Use to get the maximum temperature of the physical drive from [SA_GetPhysicalDriveTemperatureInfo](@ref storc_properties_pd_temperature_info). */
#define PD_MAXIMUM_TEMPERATURE(temp_info) ((SA_BYTE)(((temp_info) & kPhysicalDriveMaximumTemperatureMask) >> kPhysicalDriveMaximumTemperatureOffset))
/** @} */ /* storc_properties_pd_temperature_info */

/** @defgroup storc_properties_pd_temperature_threshold Physical Drive Temperature Threshold
 * @{ */
/** Get the temperature threshold of the physical drive.
 *
 * This function returns the temperature threshold of the physical drive at which it would consider to be overheated.
 *
 * @pre `pd_number` must be a valid pd number.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @return Temperature threshold for the physical drive (unsigned).
 * @return 0xFF if unknown.
 */
SA_BYTE SA_GetPhysicalDriveTemperatureThreshold(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_properties_pd_temperature_threshold */

/** @defgroup storc_properties_pd_rotational_speed Physical Drive Rotational Speed
 * @{ */
/** Get the rotational speed of the physical drive.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @return Rotational speed of the physical drive (unsigned).
 * @return 0x00 if not applicable.
 */
SA_DWORD SA_GetPhysicalDriveRotationalSpeed(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_properties_pd_rotational_speed */

/** @defgroup storc_properties_pd_phy_info Physical Device PHY Info
 * Phy information fro drives, SEPs, expanders, etc...
 * @{ */

#define kInvalidPhyNumber  0xFFFF
#define kAnyPhyNumber      0xFFFE

/** Get the PHY count of the physical device.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  dev_number  Physical device number.
 * @return PHY count of the physical device (unsigned).
 */
SA_WORD SA_GetPhysicalDevicePHYCount(PCDA cda, SA_WORD dev_number);

/** Get the controller phy numbers for the selected device.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  dev_number  Physical device number.
 * @return The first and last controller phy numbers for the device.
 */
SA_DWORD SA_GetPhysicalDeviceControllerPhyNumbers(PCDA cda, SA_WORD dev_number);

/** Use to obtain the first phy number from the number returned by @ref SA_GetPhysicalDeviceControllerPhyNumbers. */
#define FIRST_CONTROLLER_PHY_NUMBER(phy_numbers) ((SA_WORD)(phy_numbers & 0xFFFF))
/** Use to obtain the last phy number from the number returned by @ref SA_GetPhysicalDeviceControllerPhyNumbers. */
#define LAST_CONTROLLER_PHY_NUMBER(phy_numbers) ((SA_WORD)((phy_numbers >> 16) & 0xFFFF))

/** Get the negotiated logical/physical and max transfer speed of the physical device phy.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  dev_number  Physical device number.
 * @param[in]  phy_number  Phy number (or kAnyPhyNumber). When using kAnyPhyNumber, get the information for the first connected phy found.
 * @return Use [PHY_NEGOTIATED_PHYSICAL_LINK_RATE](@ref storc_properties_pd_phy_info) to get the physical link rate.
 * @return Use [PHY_NEGOTIATED_LOGICAL_LINK_RATE](@ref storc_properties_pd_phy_info) to get the logical link rate.
 * @return Use [PHY_MAX_LINK_RATE](@ref storc_properties_pd_phy_info) to get the phy's max link rate.
 * @return Use [PHY_INFO_FORMAT](@ref storc_properties_pd_phy_info) to get the phy's info format.
 *  Will return @ref kPhyInfoFormatSAS for SAS link rates and @ref kPhyInfoFormatNVMe for NVMe link rates.
 * @return Logical/Physical/Max link rates of the phy are:
 * @return 0x00: Unknown (or disconnected)
 * @return 0x01: PHY Disabled.
 * @return 0x02: Negotiation Failed.
 * @return 0x03: SATA spin-up hold.
 * @return 0x08: 1.5 Gbps if @ref kPhyInfoFormatSAS. PCIe Gen 1.0 if @ref kPhyInfoFormatNVMe.
 * @return 0x09: 3.0 Gbps if @ref kPhyInfoFormatSAS. PCIe Gen 2.0 if @ref kPhyInfoFormatNVMe.
 * @return 0x0A: 6.0 Gbps if @ref kPhyInfoFormatSAS. PCIe Gen 3.0 if @ref kPhyInfoFormatNVMe.
 * @return 0x0B: 12.0 Gbps if @ref kPhyInfoFormatSAS. PCIe Gen 4.0 if @ref kPhyInfoFormatNVMe.
 * @return 0x0C: 22.5 Gbps if @ref kPhyInfoFormatSAS. PCIe Gen 5.0 if @ref kPhyInfoFormatNVMe.
 * __Example__
 * @code{.c}
 * SA_DWORD phy_info = SA_GetPhysicalDevicePhyLinkRate(cda, dev_number, phy_number);
 * if (PHY_INFO_FORMAT(phy_info) == kPhyInfoFormatSAS)
 * {
 *    switch(PHY_NEGOTIATED_PHYSICAL_LINK_RATE(phy_info))
 *    {
 *       case 0x08:
 *          printf('Negotiated Physical Link Rate is 1.5 Gbps');
 *          break;
 *       case 0x09:
 *          printf('Negotiated Physical Link Rate is 3.0 Gbps');
 *          break;
 *       case 0x0A:
 *          printf('Negotiated Physical Link Rate is 6.0 Gbps');
 *          break;
 *       case 0x0B:
 *          printf('Negotiated Physical Link Rate is 12.0 Gbps');
 *          break;
 *       case 0x0C:
 *          printf('Negotiated Physical Link Rate is 22.5 Gbps');
 *          break;
 *       default:
 *          printf('Negotiated Physical Link Rate is unknown');
 *          break;
 *    }
 * }
 * else if (PHY_INFO_FORMAT(phy_info) == kPhyInfoFormatNVMe)
 * {
 *    switch (PHY_NEGOTIATED_PHYSICAL_LINK_RATE(phy_info))
 *    {
 *       case 0x08:
 *          printf('Negotiated Physical Link Rate is PCIe Gen 1.0');
 *          break;
 *       case 0x09:
 *          printf('Negotiated Physical Link Rate is PCIe Gen 2.0');
 *          break;
 *      case 0x0A:
 *          printf('Negotiated Physical Link Rate is PCIe Gen 3.0');
 *          break;
 *       case 0x0B:
 *          printf('Negotiated Physical Link Rate is PCIe Gen 4.0');
 *          break;
 *       case 0x0C:
 *          printf('Negotiated Physical Link Rate is PCIe Gen 5.0');
 *          break;
 *       default:
 *          printf('Negotiated Physical Link Rate is unknown');
 *          break;
 *    }
 * }
 * @endcode
 */
SA_DWORD SA_GetPhysicalDevicePhyLinkRate(PCDA cda, SA_WORD dev_number, SA_WORD phy_number);
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kPhyNegotiatedPhysicalLinkRateMask   0x000000FF
#define kPhyNegotiatedPhysicalLinkRateOffset 0
#define kPhyNegotiatedLogicalLinkRateMask    0x0000FF00
#define kPhyNegotiatedLogicalLinkRateOffset  8
#define kPhyMaxLinkRateMask                  0x00FF0000
#define kPhyMaxLinkRateOffset                16
#define kPhyInfoFormatMask                   0xF0000000
#define kPhyInfoFormatOffset                 28
#endif
/** Phy link rate is SAS format.
 * Output of [PHY_INFO_FORMAT](@ref storc_properties_pd_phy_info).
 */
#define kPhyInfoFormatSAS                    1
 /** Phy link rate is NVMe format.
  * Output of [PHY_INFO_FORMAT](@ref storc_properties_pd_phy_info).
  */
#define kPhyInfoFormatNVMe                   2

/** Use to get the negotiated physical link rate from [SA_GetPhysicalDevicePhyLinkRate](@ref storc_properties_pd_phy_info). */
#define PHY_NEGOTIATED_PHYSICAL_LINK_RATE(info) ((SA_BYTE)(((info) & kPhyNegotiatedPhysicalLinkRateMask) >> kPhyNegotiatedPhysicalLinkRateOffset))
/** Use to get the negotiated logical link rate from [SA_GetPhysicalDevicePhyLinkRate](@ref storc_properties_pd_phy_info). */
#define PHY_NEGOTIATED_LOGICAL_LINK_RATE(info) ((SA_BYTE)(((info) & kPhyNegotiatedLogicalLinkRateMask) >> kPhyNegotiatedLogicalLinkRateOffset))
/** Use to get the max link rate from [SA_GetPhysicalDevicePhyLinkRate](@ref storc_properties_pd_phy_info). */
#define PHY_MAX_LINK_RATE(info) ((SA_BYTE)(((info) & kPhyMaxLinkRateMask) >> kPhyMaxLinkRateOffset))
/** Use to get the phy info format from [SA_GetPhysicalDevicePhyLinkRate](@ref storc_properties_pd_phy_info). */
#define PHY_INFO_FORMAT(info) ((SA_BYTE)(((info) & kPhyInfoFormatMask) >> kPhyInfoFormatOffset))

/** Get the controller connector index for the requested phy.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  dev_number  Physical device number.
 * @param[in]  phy_number  Phy number.
 * @return Controller connector index for this phy @ref kInvalidConnectorIndex.
 */
SA_WORD SA_GetPhysicalDevicePhyConnectorIndex(PCDA cda, SA_WORD dev_number, SA_WORD phy_number);

/** @name Phy Map Types
* @{ */
#define kActivePathMap  1     /**< 1 set at the position of the active phy */
#define kPathPresentMap 2     /**< 1 set at the position of all present phy connections */
#define kPathFailureMap 3     /**< 1 set at the position of all failed phy connections */
/** @} */

/** Get the requested phy map for the selected physical device.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  dev_number  Physical device number.
 * @param[in]  map_type    The type of map to retrieve. See [Phy Map Types](@ref storc_properties_pd_phy_info).
 * @return The requested phy map for the selected physical device.
 */
SA_BYTE SA_GetPhysicalDevicePhyMap(PCDA cda, SA_WORD dev_number, SA_BYTE map_type);
/** @} */ /* storc_properties_pd_phy_info */

/** @defgroup storc_properties_pd_power_on_hours Physical Drive Power On Hours
 * @{ */
/** Get the power on hours of the physical drive.
 *
 * This value is only valid for SSD drives.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @return Power on hours of an SSD drive, 0 if wear gauge is not supported.
 */
SA_WORD SA_GetPhysicalDrivePowerOnHours(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_properties_pd_power_on_hours */

/** @defgroup storc_properties_pd_utilization Physical Drive Utilization
 * @{ */
/** Get the SSD utilization of the physical drive.
 *
 * This value is only valid for SSD drives.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @return SSD utilization of the physical drive (percentage), 0xFFFF if wear gauge is not supported.
 */
SA_WORD SA_GetPhysicalDriveUtilization(PCDA cda, SA_WORD pd_number);

/** Use to decode output of @ref SA_GetPhysicalDriveUtilization and @ref SA_GetPhysicalDrivePowerOnHours to estimate the number of remaining days of the SSD.
 * @pre `poh` must be greater than zero.
 * @pre `util` must be greater than zero.
 * @param[in] poh  Power on hours from @ref SA_GetPhysicalDrivePowerOnHours.
 * @param[in] util Utilization (Wear) from @ref SA_GetPhysicalDriveUtilization.
 * @return Estimated usable days remaining for the target SSD.
 * @return 0.0 if remaining days could not be determined.
 */
#define SSD_REMAINING_DAYS(poh, util) \
   (poh > 0 && util > 0 && util <= 100 \
      ? ((100.0-(double)(util)) / ((((double)(util))/((double)(poh)))*24.0)) \
      : 0.0)
/** Use to decode output of @ref SA_GetPhysicalDriveUtilization and @ref SA_GetPhysicalDrivePowerOnHours to estimate the number of remaining hours of the SSD.
 * Unlike @ref SSD_REMAINING_DAYS, this macro can be used when float/double is not available since the calculation is done entirely with 'unsigned long'.
 * @pre `poh` must be greater than zero.
 * @pre `util` must be greater than zero.
 * @param[in] poh  Power on hours from @ref SA_GetPhysicalDrivePowerOnHours.
 * @param[in] util Utilization (Wear) from @ref SA_GetPhysicalDriveUtilization.
 * @return Estimated usable hours (as 'unsigned long') remaining for the target SSD.
 * @return 0 if remaining hours could not be determined.
 */
#define SSD_REMAINING_HOURS(poh, util) \
   (poh > 0 && util > 0 && util <= 100 \
      ? (util/poh > 0 \
         ? ((100-((unsigned long)(util))) / (((unsigned long)(util))/((unsigned long)(poh)))) \
         : ((100-((unsigned long)(util))) * (((unsigned long)(poh))/((unsigned long)(util))))) \
      : 0)
/** @} */ /* storc_properties_pd_utilization */

/** @defgroup storc_properties_pd_carrier_authentication Physical Drive Carrier Authentication Status
 * @{ */
/** Get the carrier authentication status of the physical drive.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @return Carrier authentication of the physical drive.
 */
SA_BYTE SA_GetPhysicalDriveCarrierAuthentication(PCDA cda, SA_WORD pd_number);
  /** @name Carrier Authentication Statuses
   * @{ */
   #define kPhysicalDriveCarrierFailed          0
   #define kPhysicalDriveCarrierOK              1
   #define kPhysicalDriveCarrierNoCommunication 2
   #define kPhysicalDriveCarrierNotApplicable   3
  /** @} */
/** @} */ /* storc_properties_pd_carrier_authentication */

/** @defgroup storc_properties_pd_carrier_application_version Physical Drive Carrier Application Version
 * @{ */
/** Get the carrier application version of the physical drive.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @return Carrier application version of the physical drive.
 */
SA_BYTE SA_GetPhysicalDriveCarrierApplicationVersion(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_properties_pd_carrier_application_version */

/** @defgroup storc_properties_pd_wwid Physical Drive WWID
 * @{ */
/** Minimum buffer length (in bytes) required for @ref SA_GetPhysicalDriveWWID when not converting to a string. */
#define kMinGetPhysicalDriveWWIDBufferLength 20
/** Minimum buffer length (in bytes) required for @ref SA_GetPhysicalDriveWWID when converting to a string. */
#define kMinGetPhysicalDriveWWIDStringLength 41
/** Get the WWID of the physical drive.
 *
 * @attention `buffer_size` bytes will be initialized to zero.
 *
 * @pre When converting to string, `wwid_buffer` is at least @ref kMinGetPhysicalDriveWWIDStringLength bytes
 * to contain the physical drive WWID and terminating NULL.
 * @pre When not converting to string, `wwid_buffer` is at least @ref kMinGetPhysicalDriveWWIDBufferLength bytes.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @param[out] wwid_buffer Buffer to write WWID.
 * @param[in]  buffer_size Size (in bytes) of `wwid_buffer`.
 * @param[in]  convert_to_string kTrue => Convert value to string and store in wwid_buffer, kFalse => Copy raw binary value to wwid_buffer.
 * @return WWID of the physical drive (same as parameter `wwid_buffer`).
 */
char *SA_GetPhysicalDriveWWID(PCDA cda, SA_WORD pd_number, void *wwid_buffer, size_t buffer_size, SA_BOOL convert_to_string);
/** @} */ /* storc_properties_pd_wwid */

/** @defgroup storc_properties_pd_location_info Physical Drive Location Information
 * @see storc_properties_sep_location
 * @see storc_properties_expander_location
 * @{ */


/** Minimum buffer length (in bytes) required for getting @ref kPhysicalDriveLocationComplete from @ref SA_GetPhysicalDriveLocation. */
#define kMinGetPhysicalDeviceLocationCompleteBufferLength 25
/** Minimum buffer length (in bytes) required for getting @ref kPhysicalDriveLocationPort from @ref SA_GetPhysicalDriveLocation. */
#define kMinGetPhysicalDeviceLocationPortBufferLength     9
/** Minimum buffer length (in bytes) required for getting @ref kPhysicalDriveLocationBox from @ref SA_GetPhysicalDriveLocation. */
#define kMinGetPhysicalDeviceLocationBoxBufferLength      8
/** Minimum buffer length (in bytes) required for getting @ref kPhysicalDriveLocationBay from @ref SA_GetPhysicalDriveLocation. */
#define kMinGetPhysicalDeviceLocationBayBufferLength      8

/** Get the location information of the physical drive.
 *
 * The location is returned as Port:Box:Bay, or as separate components, based on the type argument.
 *
 * @pre @p location_buffer is at least @ref kMinGetPhysicalDeviceLocationCompleteBufferLength,
 * @ref kMinGetPhysicalDeviceLocationPortBufferLength, @ref kMinGetPhysicalDeviceLocationBoxBufferLength, or
 * @ref kMinGetPhysicalDeviceLocationBayBufferLength bytes depending on @p type. to contain the return
 * the location string (including a terminating NULL).
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  pd_number         Physical drive number.
 * @param[in]  type              Type of the location information required (one of [Physical Drive Location Formats](@ref storc_properties_pd_location_info)).
 * @param[out] location_buffer   Buffer to write location string.
 * @param[in]  buffer_size       Size (in bytes) of `location_buffer`.
 * @return Location of the physical drive.
 */
char *SA_GetPhysicalDriveLocation(PCDA cda, SA_WORD pd_number, SA_BYTE type, char *location_buffer, size_t buffer_size);
   /** @name Physical Drive Location Formats
    * Input to @ref SA_GetPhysicalDriveLocation.
    * @{ */
   #define kPhysicalDriveLocationComplete       0x01  /**< Return a complete string as "CN0:1:7"; Input to @ref SA_GetPhysicalDriveLocation. */
   #define kPhysicalDriveLocationPort           0x02  /**< Return port only as "CN0"; Input to @ref SA_GetPhysicalDriveLocation. */
   #define kPhysicalDriveLocationBox            0x03  /**< Return box only as "1"; Input to @ref SA_GetPhysicalDriveLocation. */
   #define kPhysicalDriveLocationBay            0x04  /**< Return bay only as "7"; Input to @ref SA_GetPhysicalDriveLocation. */
   /** @} */

/** Get the number of paths for the physical device.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  pd_number         Physical device number.
 * @return Number of physical device paths.
 */
SA_BYTE SA_GetPhysicalDevicePathCount(PCDA cda, SA_WORD pd_number);

/** Get the requested physical device path name.
 *
 * The path name is returned as Port:Box:Bay.
 *
 * @pre @p path_name_buffer is at least @ref kMinGetPhysicalDeviceLocationCompleteBufferLength,
 * @ref kMinGetPhysicalDeviceLocationPortBufferLength, @ref kMinGetPhysicalDeviceLocationBoxBufferLength, or
 * @ref kMinGetPhysicalDeviceLocationBayBufferLength bytes depending on @p type. to contain the path name
 * string (including a terminating NULL).
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  pd_number         Physical device number.
 * @param[in]  path_index        0-based index of path. Valid values are from 0 to @ref SA_GetPhysicalDevicePathCount - 1.
 * @param[in]  type              Type of the location information required (one of [Physical Device Path Formats](@ref storc_properties_pd_location_info)).
 * @param[out] path_name_buffer  Buffer to write the path name.
 * @param[in]  buffer_size       Size (in bytes) of `path_name_buffer`.
 * @return Physical device path name for the requested path.
 */
char *SA_GetPhysicalDevicePathName(PCDA cda, SA_WORD pd_number, SA_BYTE path_index, SA_BYTE type, char *path_name_buffer, size_t buffer_size);
   /** @name Physical Device Path Formats
    * Input to @ref SA_GetPhysicalDevicePathName.
    * @{ */
   #define kPhysicalDevicePathNameComplete       0x01  /**< Return a complete string as "CN0:1:7"; Input to @ref SA_GetPhysicalDevicePathName. */
   #define kPhysicalDevicePathNamePort           0x02  /**< Return port only as "CN0"; Input to @ref SA_GetPhysicalDevicePathName. */
   #define kPhysicalDevicePathNameBox            0x03  /**< Return box only as "1"; Input to @ref SA_GetPhysicalDevicePathName. */
   #define kPhysicalDevicePathNameBay            0x04  /**< Return bay only as "7" (may not apply to all device type); Input to @ref SA_GetPhysicalDevicePathName. */
   /** @} */

/** Get the requested physical device path information.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  pd_number         Physical device number.
 * @return Physical device path flags (active, failed).
 * @return Decode with @ref IS_PATH_ACTIVE to see is a path is currently active.
 * @return Decode with @ref IS_PATH_FAILED to see is a path is currently failed.
 */
SA_WORD SA_GetPhysicalDevicePathInfo(PCDA cda, SA_WORD pd_number);

/** Use to decode output of @ref SA_GetPhysicalDevicePathInfo to test whether the selected path is active or not.
 */
#define IS_PATH_ACTIVE(path_info, path_index)  ((path_info & ((SA_WORD)0x0001 << path_index)) != 0)
/** Use to decode output of @ref SA_GetPhysicalDevicePathInfo to test whether the selected path is failed or not.
 */
#define IS_PATH_FAILED(path_info, path_index)  ((path_info & ((SA_WORD)0x0100 << path_index)) != 0)

/** @name Physical Device Connector Locations
 * @outputof SA_GetPhysicalDeviceConnectorLocation
 * @{ */
/** Unknown device connector location */
#define kPhysicalDeviceConnectorLocationUnknown   0
/** External device connector location */
#define kPhysicalDeviceConnectorLocationExternal  1
/** Internal device connector location */
#define kPhysicalDeviceConnectorLocationInternal  2
/** @} */

/** Determine if a physical device is multipath or not.
 * A device is considered as multipath only if there are multiple paths that are not failed.
 *
 * @param[in]  cda               Controller data area.
 * @param[in]  pd_number         Physical device number.
 * @return @ref kTrue if the device is multipath, @ref kFalse if the device is not multipath.
 */
SA_BOOL SA_PhysicalDeviceIsMultiPath(PCDA cda, SA_WORD pd_number);

/** Determine if a physical device is attached to an internal or external port/connector.
 * @param[in]  cda               Controller data area.
 * @param[in]  pd_number         Physical device number.
 * @return One of [Physical Device Connector Locations](@ref storc_properties_pd_location_info).
 */
SA_BYTE SA_GetPhysicalDeviceConnectorLocation(PCDA cda, SA_WORD pd_number);

/** @} */ /* storc_properties_pd_location_info */

/** @defgroup storc_properties_pd_mnp_count Physical Drive M&P Count
 * @{ */
/** Get the M&P Count of the physical drive.
 * May need to change the return type to SA_DWORD depending on the count size returned. So far, only SA_WORD.
 * @param[in]  cda        Controller data area.
 * @param[in]  pd_number  Physical drive number.
 * @param[in]  type       .
 * @return The M&P statistic count of the physical drive (see [Physical Drive M&P Count Returns](@ref storc_properties_pd_mnp_count)).
 */
SA_WORD SA_GetPhysicalDriveMNPStatistic(PCDA cda, SA_WORD pd_number, SA_BYTE type);
   /** @name Physical Drive M&P Count Returns
    * @{ */
   #define kPdMNPMediaFailures                     0x01
   /** @} */
/** @} */ /* storc_properties_pd_mnp_count */

/** @defgroup storc_properties_pd_array_number Physical Drive's Array Number
 * @{ */
/** Get the array number of the physical drive.
 *
 * If this physical drive is not used by any arrays, it returns @ref kInvalidArrayNumber.
 * If this physical drive is used as shared spare by multiple arrays, it returns the array number of the first array.
 *
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Physical drive number.
 * @return array number of the physical drive.
 */
SA_WORD SA_GetPhysicalDriveArray(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_properties_pd_array_number */

/** @defgroup storc_properties_pd_sanitize_lock Physical Drive Freeze/Anti-Freeze Sanitize Lock
 * @{ */
/** Get the current sanitize lock state of the physical drive.
 * @param[in] cda         Controller Data Area
 * @param[in]  pd_number  Physical drive number.
 * @return Current physical drive lock state using @ref PHYSICAL_DRIVE_SANITIZE_LOCK.
 * @return Sanitize lock errors for drive using @ref PHYSICAL_DRIVE_SANITIZE_LOCK_ERRORS.
 */
SA_BYTE SA_GetPhysicalDriveSanitizeLock(PCDA cda, SA_WORD pd_number);
/** @name Physical Drive Sanitize Locks
 * @outputof SA_GetPhysicalDriveSanitizeLock.
 * @{ */
/** The physical drive sanitize lock state is disabled or not supported.
 * @outputof SA_GetPhysicalDriveSanitizeLock.
 * @outputof PHYSICAL_DRIVE_SANITIZE_LOCK.
 */
#define kPhysicalDriveSanitizeLockDisabled        0x00
/** The physical drive sanitize lock state is Freeze.
 *  Drives that support this feature will abort all sanitize commands.
 * @outputof SA_GetPhysicalDriveSanitizeLock.
 * @outputof PHYSICAL_DRIVE_SANITIZE_LOCK.
 */
#define kPhysicalDriveSanitizeLockFreeze          0x01
/** The physical drive sanitize lock state is Anti-Freeze.
 *  Drives that support this feature will abort all sanitize freeze lock commands.
 * @outputof SA_GetPhysicalDriveSanitizeLock.
 * @outputof PHYSICAL_DRIVE_SANITIZE_LOCK.
 */
#define kPhysicalDriveSanitizeLockAntiFreeze      0x02
/** The physical drive sanitize lock state is unknown.
 * @outputof SA_GetPhysicalDriveSanitizeLock.
 * @outputof PHYSICAL_DRIVE_SANITIZE_LOCK.
 */
#define kPhysicalDriveSanitizeLockUnknown         0x03

/** Mask output of @ref SA_GetPhysicalDriveSanitizeLock to get current sanitize lock policy of a physical drive. */
#define kPhysicalDriveSanitizeLockStateMask 0x00000003
/** Bitshift output of @ref SA_GetPhysicalDriveSanitizeLock to get current sanitize lock policy of a physical drive. */
#define kPhysicalDriveSanitizeLockStateBitShift 0
/** Decode output of @ref SA_GetPhysicalDriveSanitizeLock to get current sanitize lock policy of a physical drive. */
#define PHYSICAL_DRIVE_SANITIZE_LOCK(info_value) ((info_value & kPhysicalDriveSanitizeLockStateMask) >> kPhysicalDriveSanitizeLockStateBitShift)

/** Not errors reporting setting the sanitize lock policy of the drive.
 * @outputof SA_GetPhysicalDriveSanitizeLock.
 * @outputof PHYSICAL_DRIVE_SANITIZE_LOCK_ERRORS.
 */
#define kPhysicalDriveSanitizeLockNoErrors          0x00
/** Sanitize Freeze was attempted on drive, but an error was encountered.
 * @outputof SA_GetPhysicalDriveSanitizeLock.
 * @outputof PHYSICAL_DRIVE_SANITIZE_LOCK_ERRORS.
 */
#define kPhysicalDriveSanitizeLockFreezeFailed      0x01
/** Sanitize Anti-Freeze was attempted on drive, but an error was encountered.
 * @outputof SA_GetPhysicalDriveSanitizeLock.
 * @outputof PHYSICAL_DRIVE_SANITIZE_LOCK_ERRORS.
 */
#define kPhysicalDriveSanitizeLockAntiFreezeFailed  0x02

/** Mask output of @ref SA_GetPhysicalDriveSanitizeLock to get errors on the sanitize lock policy of a physical drive. */
#define kPhysicalDriveSanitizeLockErrorsMask 0x0000000C
/** Bitshift output of @ref SA_GetPhysicalDriveSanitizeLock to get errors on the sanitize lock policy of a physical drive. */
#define kPhysicalDriveSanitizeLockErrorsBitShift 2
/** Decode output of @ref SA_GetPhysicalDriveSanitizeLock to get errors on the sanitize lock policy of a physical drive. */
#define PHYSICAL_DRIVE_SANITIZE_LOCK_ERRORS(info_value) ((info_value & kPhysicalDriveSanitizeLockErrorsMask) >> kPhysicalDriveSanitizeLockErrorsBitShift)
/** @} */ /* Physical Drive Sanitize Locks */

/** @} */ /* storc_properties_pd_sanitize_lock */

/** @} */ /* storc_properties_pd */

/*******************************************//**
 * @defgroup storc_properties_physical_devices Physical Devices
 * @{
 **********************************************/

/** @defgroup storc_properties_physical_devices_enum Enumerate Physical Devices
 * @{ */
/** Enumerate physical devices in the controller.
 *
 * This API enumerates other physical devices not found by using
 * @ref SA_EnumerateControllerPhysicalDrives. Drives found by this API
 * are usually ones not supported or managed by the controller and may
 * or may not be directly exposed to a host operation system.
 *
 * Start with @ref kInvalidDeviceNumber as the first item.
 *
 * @param[in] cda            Controller data area.
 * @param[in] device_number  Physical device number.
 * @return Given a device number, the next valid physical device number is returned.
 * @return @ref kInvalidDeviceNumber if no valid physical device are connected.
 */
SA_WORD SA_EnumerateControllerPhysicalDevices(PCDA cda, SA_WORD device_number);
   /** @name Physical Devices Enumerate Returns
    * @outputof SA_EnumerateControllerPhysicalDevices.
    * @{ */
   #define kInvalidDeviceNumber 0xFFFF /**< Invalid physical device number; @outputof SA_EnumerateControllerPhysicalDevices. */
   /** @} */

/** Iterate over all controller physical devices and execute the block with each valid dev_number.
 *
 * This API enumerates other physical devices not found by using
 * @ref SA_EnumerateControllerPhysicalDrives. Drives found by this API
 * are usually ones not supported or managed by the controller and may
 * or may not be directly exposed to a host operation system.
 *
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] dev_number    Variable to store valid device numbers.
 * @see SA_EnumerateControllerPhysicalDevices
 * __Example__
 * @code
 * void DoSomethingWithEachControllerPhysicalDevice(PCDA cda)
 * {
 *    SA_WORD dev_number;
 *    FOREACH_PHYSICAL_DEVICE(cda, dev_number)
 *    {
 *       // do something...
 *    }
 * }
 * @endcode
 */
#define FOREACH_PHYSICAL_DEVICE(cda, dev_number) \
   for (dev_number = kInvalidDeviceNumber; (dev_number = SA_EnumerateControllerPhysicalDevices(cda, dev_number)) != kInvalidDeviceNumber;)

/** Enumerate all physical drives and devices in the controller.
 *
 * This API enumerates all physical devices that appear using
 * @ref SA_EnumerateControllerPhysicalDevices and
 * @ref SA_EnumerateControllerPhysicalDrives.
 *
 * Start with @ref kInvalidDeviceNumber as the first item.
 *
 * @param[in] cda            Controller data area.
 * @param[in] device_number  Physical device number.
 * @return Given a device number, the next valid physical drive or device number is returned.
 * @return @ref kInvalidDeviceNumber if no valid physical drives or devices are connected.
 */
SA_WORD SA_EnumerateControllerAllPhysicalDevices(PCDA cda, SA_WORD device_number);

/** Iterate over all controller physical drives and devices and execute the block with each valid dev_number.
 *
 * This API enumerates all physical devices that appear using
 * @ref SA_EnumerateControllerPhysicalDevices and
 * @ref SA_EnumerateControllerPhysicalDrives.
 *
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] dev_number    Variable to store valid device numbers.
 * @see SA_EnumerateControllerAllPhysicalDevices
 *
 * __Example__
 * @code
 * void DoSomethingWithEachControllerPhysicalDeviceOrDrive(PCDA cda)
 * {
 *    SA_WORD dev_number;
 *    FOREACH_ALL_PHYSICAL_DEVICE(cda, dev_number)
 *    {
 *       // do something...
 *    }
 * }
 * @endcode
 */
#define FOREACH_ALL_PHYSICAL_DEVICE(cda, dev_number) \
   for (dev_number = kInvalidDeviceNumber; (dev_number = SA_EnumerateControllerAllPhysicalDevices(cda, dev_number)) != kInvalidDeviceNumber;)

/** @} */ /* storc_properties_physical_devices_enum */

/** Return device type for given physical device number.
 *
 * @param[in] cda            Controller data area.
 * @param[in] device_number  Physical device number.
 * @return See [Physical Devices Type Returns](@ref storc_properties_physical_devices).
 */
SA_BYTE SA_PhysicalDeviceType(PCDA cda, SA_WORD device_number);
   /** @name Physical Devices Type Returns
    * @outputof SA_PhysicalDeviceType.
    * @{ */
   #define kPhysicalDeviceTypeDrive            0x00  /**< Physical drive type; @outputof SA_PhysicalDeviceType. */
   #define kPhysicalDeviceTypeExpander         0x01  /**< Expander device type; @outputof SA_PhysicalDeviceType. */
   #define kPhysicalDeviceTypeSEP              0x02  /**< SEP device type; @outputof SA_PhysicalDeviceType. */
   #define kPhysicalDeviceTypeController       0x03  /**< Controller device type; @outputof SA_PhysicalDeviceType. */
   #define kPhysicalDeviceTypeSMP              0x04  /**< SMP device type; @outputof SA_PhysicalDeviceType. */
   #define kPhysicalDeviceTypePhyNotConnected  0x05  /**< Error: phy not connected; @outputof SA_PhysicalDeviceType. */
   #define kPhysicalDeviceTypeUnknown          0xFF  /**< Unknown physical device type; @outputof SA_PhysicalDeviceType. */
   /** @} */ /* Physical Devices Type Returns */
/** @} */ /* storc_properties_physical_devices */

/*************************************//**
 * @defgroup storc_properties_array Array
 * @brief
 * @details
 * @{
 ****************************************/

/** @defgroup storc_properties_array_enum Enumerate Arrays
 * @{
 */
/** Enumerates arrays in the controller.
 * Start with @ref kInvalidArrayNumber as the first item.
 * @param[in] cda           Controller data area.
 * @param[in] array_type    One of @ref storc_properties_array_types.
 * @param[in] array_number  Current array number (or @ref kInvalidArrayNumber).
 * @return Given an array number, the next valid array number is returned.
 * @return @ref kInvalidArrayNumber after the last valid array.
 */
SA_WORD SA_EnumerateArrays(PCDA cda, SA_BYTE array_type, SA_WORD array_number);

/** Iterate over all arrays of a given type and execute the block with each valid array_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] array_type    One of @ref storc_properties_array_types.
 * @param[in] array_number  Variable to store valid array numbers.
 *
 * __Example__
 * @code
 * SA_WORD array_number;
 * FOREACH_ARRAY_OF_TYPE(cda, kArrayTypeData, array_number)
 * {
 *    // do something for each data Array...
 * }
 * @endcode
 */
#define FOREACH_ARRAY_OF_TYPE(cda, array_type, array_number) \
   for (array_number = kInvalidArrayNumber; (array_number = SA_EnumerateArrays(cda, array_type, array_number)) != kInvalidArrayNumber;)

/** Enumerates all arrays (regardless of type) configured on the controller.
 * Start with @ref kInvalidArrayNumber as the first item.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Current array number (or @ref kInvalidArrayNumber).
 * @return Given an array number, the next valid array number is returned.
 * @return @ref kInvalidArrayNumber after the last valid array.
 */
SA_WORD SA_EnumerateAllArrays(PCDA cda, SA_WORD array_number);
   /** @name Array Enumerate Returns
    * @{ */
   #define kInvalidArrayNumber 0xFFFF /**< Invalid array number */
   /** @} */

/** Iterate over all arrays and execute the block with each valid array_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Variable to store valid array numbers.
 *
 * __Example__
 * @code
 * SA_WORD array_number;
 * FOREACH_ARRAY(cda, array_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_ARRAY(cda, array_number) \
   for (array_number = kInvalidArrayNumber; (array_number = SA_EnumerateAllArrays(cda, array_number)) != kInvalidArrayNumber;)
/** @} */ /* storc_properties_array_enum */

/** @defgroup storc_properties_array_pd_enum Enumerate Array Physical Drives
 * @{ */
/** Enumerate over all drives assigned to the specified array.
 * @pre `array_number` is a valid Array number.
 * @param[in] cda Controller data area.
 * @param[in] array_number Valid Array number.
 * @param[in] pd_number Current physical drive number (or @ref kInvalidPDNumber to get the first physical drive).
 * Start with @ref kInvalidPDNumber as the first item.
 * @return Number of next physical drive of the Array, or @ref kInvalidPDNumber if at the last physical drive.
 */
SA_WORD SA_EnumerateArrayPhysicalDrives(PCDA cda, SA_WORD array_number, SA_WORD pd_number);

/** Iterate over all physical drives of an array and execute the block with each valid pd_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Valid Array number.
 * @param[in] pd_number     Variable to store valid physical drive numbers.
 *
 * __Example__
 * @code
 * SA_WORD pd_number;
 * FOREACH_ARRAY_PHYSICAL_DRIVE(cda, 0, pd_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_ARRAY_PHYSICAL_DRIVE(cda, array_number, pd_number) \
   for (pd_number = kInvalidPDNumber; (pd_number = SA_EnumerateArrayPhysicalDrives(cda, array_number, pd_number)) != kInvalidPDNumber;)

/** Enumerate over data drives assigned to the specified array.
 * @pre `array_number` is a valid Array number.
 * @param[in] cda Controller data area.
 * @param[in] array_number Valid Array number.
 * @param[in] pd_number Current data drive number (or @ref kInvalidPDNumber to get the first data drive).
 * Start with @ref kInvalidPDNumber as the first item.
 * @return Number of next data physical drive of the Array, or @ref kInvalidPDNumber if at the last data physical drive.
 */
SA_WORD SA_EnumerateArrayDataDrives(PCDA cda, SA_WORD array_number, SA_WORD pd_number);

/** Iterate over all data drives of an array and execute the block with each valid pd_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Valid Array number.
 * @param[in] pd_number     Variable to store valid physical drive numbers.
 *
 * __Example__
 * @code
 * SA_WORD pd_number;
 * FOREACH_ARRAY_DATA_DRIVE(cda, 0, pd_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_ARRAY_DATA_DRIVE(cda, array_number, pd_number) \
   for (pd_number = kInvalidPDNumber; (pd_number = SA_EnumerateArrayDataDrives(cda, array_number, pd_number)) != kInvalidPDNumber;)

/** Enumerate over spare drives assigned to the specified array.
 * @pre `array_number` is a valid Array number.
 * @param[in] cda Controller data area.
 * @param[in] array_number Valid Array number.
 * @param[in] pd_number Current spare drive number (or @ref kInvalidPDNumber to get the first spare drive).
 * Start with @ref kInvalidPDNumber as the first item.
 * @return Number of next spare physical drive of the Array, or @ref kInvalidPDNumber if at the last spare physical drive.
 */
SA_WORD SA_EnumerateArraySpareDrives(PCDA cda, SA_WORD array_number, SA_WORD pd_number);

/** Iterate over all spare drives of an array and execute the block with each valid pd_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Valid Array number.
 * @param[in] pd_number     Variable to store valid physical drive numbers.
 *
 * __Example__
 * @code
 * SA_WORD pd_number;
 * FOREACH_ARRAY_SPARE_DRIVE(cda, 0, pd_number)
 * {
 *    // do something...
 * }
 * @endcode
 */
#define FOREACH_ARRAY_SPARE_DRIVE(cda, array_number, pd_number) \
   for (pd_number = kInvalidPDNumber; (pd_number = SA_EnumerateArraySpareDrives(cda, array_number, pd_number)) != kInvalidPDNumber;)
/** @} */ /* storc_properties_array_pd_enum */

/** @defgroup storc_properties_array_ld_enum Enumerate Array Logical Drives
 * @{ */
/** Enumerate the logical drives created on the specified array.
 * @pre `array_number` is a valid Array number.
 * @param[in] cda Controller data area.
 * @param[in] array_number Valid Array number.
 * @param[in] ld_number Current logical drive number (or @ref kInvalidLDNumber to get the first logical drive).
 * Start with @ref kInvalidLDNumber as the first item.
 * @return Number of next logical drive of the Array, or @ref kInvalidLDNumber if at the last logical drive.
 */
SA_WORD SA_EnumerateArrayLogicalDrives(PCDA cda, SA_WORD array_number, SA_WORD ld_number);

/** Iterate over all logical drives of an array and execute the block with each valid ld_number.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Valid Array number.
 * @param[in] ld_number     Variable to store valid logical drive numbers.
 *
 * __Example__
 * @code
 * void DoSomethingWithEachArrayLogicalDrive(PCDA cda, SA_WORD array_number)
 * {
 *    SA_WORD ld_number;
 *    FOREACH_ARRAY_LOGICAL_DRIVE(cda, array_number, ld_number)
 *    {
 *       // do something...
 *    }
 * }
 * @endcode
 */
#define FOREACH_ARRAY_LOGICAL_DRIVE(cda, array_number, ld_number) \
   for (ld_number = kInvalidLDNumber; (ld_number = SA_EnumerateArrayLogicalDrives(cda, array_number, ld_number)) != kInvalidLDNumber;)
/** @} */ /* storc_properties_array_ld_enum */

/** @defgroup storc_properties_array_types Array Types
 * These serve as inputs to physical drive methods.
 * @{ */
#define kArrayTypeData                    0x01 /**< Array type: Data. */
#define kArrayTypeLUCache                 0x02 /**< Array type: LUCache. */
#define kArrayTypeSplitMirrorPrimary      0x10 /**< Array type: Split-mirror primary. */
#define kArrayTypeSplitMirrorBackup       0x20 /**< Array type: Split-mirror backup. */
#define kArrayTypeSplitMirrorBackupOrphan 0x40 /**< Array type: Split-mirror orphan. */

#define kArrayTypeAny                     0xFF /**< Array type: Any. */
/** @} */ /* storc_properties_array_types */

/** @defgroup storc_properties_array_count Array Count
 * @{ */
/** Get number of Arrays configured on the Controller.
 * @param[in] cda         Controller data area.
 * @param[in] array_type  Map of @ref storc_properties_array_types.
 * @return number of Arrays (of target type) configured on the Controller.
 *
 * __Examples__
 * @code
 * printf("Number of Cache & Data Arrays: %lu\n",
 *    SA_GetControllerArrayCount(cda, kArrayTypeData | kArrayTypeLUCache));
 * printf("Total Number of Arrays: %lu\n", SA_GetControllerArrayCount(cda, kArrayTypeAny));
 * @endcode
 */
SA_WORD SA_GetControllerArrayCount(PCDA cda, SA_BYTE array_type);

/** @return Total number of arrays on the controller (data + cache + offline). */
SA_WORD SA_GetControllerTotalArrayCount(PCDA cda);

/** @return Number of data arrays configured on the controller. */
SA_WORD SA_GetControllerDataArrayCount(PCDA cda);

/** @return Number of cache arrays (at most 1) configured on the controller. */
SA_WORD SA_GetControllerCacheArrayCount(PCDA cda);

/** @return Number of offline arrays (split mirror backup) configured on the controller. */
SA_WORD SA_GetControllerOfflineArrayCount(PCDA cda);
/** @} */ /* storc_properties_array_count */

/** @defgroup storc_properties_array_type Array Type
 * @{ */
/** Get the type of the selected array.
 * @pre `array_number` is a valid Array number.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Valid target Array number.
 * @return One of @ref storc_properties_array_types.
 */
SA_BYTE SA_GetArrayType(PCDA cda, SA_WORD array_number);
/** @} */

/** @defgroup storc_properties_array_ld_count Array Logical Drive Counts
 * @ingroup storc_properties_ld_count
 * @{ */
/** Number of logical drives in the `array_index`-th Array of the Controller.
 * @pre `array_index` Valid index to an existing Array.
 * @param[in] cda          Controller data area.
 * @param[in] array_index  0-based index to an existing Array.
 * @return Number of logical drives of the Array.
 */
SA_WORD SA_GetArrayLogicalDriveCount(PCDA cda, SA_WORD array_index);
/** @} */

/** @defgroup storc_properties_array_pd_count Array Physical Drive Counts
 * @{ */
/** Get number of physical drives on an Array.
 * @pre `array_number` is a valid Array number.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Valid target Array number.
 * @return [Get Array Physical Drive Count Returns](@ref storc_properties_array_pd_count).
 * @return Decode with @ref NUM_DATA_PHYSICAL_DRIVES to get number of data drives on the Array.
 * @return Decode with @ref NUM_SPARE_PHYSICAL_DRIVES to get number of spare drives on the Array.
 * @return Decode with @ref NUM_TRANSIENT_PHYSICAL_DRIVES to get number of transient drives on the Array.
 * @return Decode with @ref NUM_FAILED_PHYSICAL_DRIVES to get number of failed drives on the Array.
 * @return Decode with @ref NUM_PHYSICAL_DRIVES to get the total number of drives on the Array.
 */
SA_DWORD SA_GetArrayPhysicalDriveCounts(PCDA cda, SA_WORD array_number);
   /** @name Get Array Physical Drive Count Returns
    * @outputof SA_GetArrayPhysicalDriveCounts & @ref SA_GetEditableArrayPhysicalDriveCounts.
    * @{ */
   /** Mask output of @ref SA_GetArrayPhysicalDriveCounts to get number of data drives on the Array. */
   #define kDataPhysicalDriveCountMask     0x000000FF
   /** Bitshift output of @ref SA_GetArrayPhysicalDriveCounts to get number of data drives on the Array. */
   #define kDataPhysicalDriveCountBitShift 0
   /** Decode output of @ref SA_GetArrayPhysicalDriveCounts to get number of data drives on the Array. */
   #define NUM_DATA_PHYSICAL_DRIVES(info_value) ((info_value & kDataPhysicalDriveCountMask ) >> kDataPhysicalDriveCountBitShift)

   /** Mask output of @ref SA_GetArrayPhysicalDriveCounts to get number of spare drives on the Array. */
   #define kSparePhysicalDriveCountMask       0x0000FF00
   /** Bitshift output of @ref SA_GetArrayPhysicalDriveCounts to get number of spare drives on the Array. */
   #define kSparePhysicalDriveCountBitShift   8
   /** Decode output of @ref SA_GetArrayPhysicalDriveCounts to get number of spare drives on the Array. */
   #define NUM_SPARE_PHYSICAL_DRIVES(info_value) ((info_value & kSparePhysicalDriveCountMask ) >> kSparePhysicalDriveCountBitShift)

   /** Mask output of @ref SA_GetArrayPhysicalDriveCounts to get number of transient drives on the Array. */
   #define kTransientPhysicalDriveCountMask       0x00FF0000
   /** Bitshift output of @ref SA_GetArrayPhysicalDriveCounts to get number of transient drives on the Array. */
   #define kTransientPhysicalDriveCountBitShift   16
   /** Decode output of @ref SA_GetArrayPhysicalDriveCounts to get number of transient drives on the Array. */
   #define NUM_TRANSIENT_PHYSICAL_DRIVES(info_value) ((info_value & kTransientPhysicalDriveCountMask ) >> kTransientPhysicalDriveCountBitShift)

   /** Mask output of @ref SA_GetArrayPhysicalDriveCounts to get number of failed drives on the Array. */
   #define kFailedPhysicalDriveCountMask       0xFF000000
   /** Bitshift output of @ref SA_GetArrayPhysicalDriveCounts to get number of failed drives on the Array. */
   #define kFailedPhysicalDriveCountBitShift   24
   /** Decode output of @ref SA_GetArrayPhysicalDriveCounts to get number of failed drives on the Array. */
   #define NUM_FAILED_PHYSICAL_DRIVES(info_value) ((info_value & kFailedPhysicalDriveCountMask ) >> kFailedPhysicalDriveCountBitShift)

   /** Decode output of @ref SA_GetArrayPhysicalDriveCounts to get number the total number of drives on the Array. */
   #define NUM_PHYSICAL_DRIVES(info_value) NUM_DATA_PHYSICAL_DRIVES(info_value)+NUM_SPARE_PHYSICAL_DRIVES(info_value)+NUM_TRANSIENT_PHYSICAL_DRIVES(info_value)
   /** @} */
/** @} */

/** @defgroup storc_properties_array_size Array Size
 * @{ */
/** @name Get Array Size Types
 * Input to @ref SA_GetArraySize.
 * @{ */
#define kArrayTotalSizeBlocks    0x01 /**< Get size of Array in blocks; Input to @ref SA_GetArraySize. */
#define kArrayTotalSizeBytes     0x02 /**< Get size of Array in bytes; Input to @ref SA_GetArraySize. */
#define kArrayFreeSizeBlocks     0x03 /**< Get free space of Array in blocks; Input to @ref SA_GetArraySize. */
#define kArrayFreeSizeBytes      0x04 /**< Get free space of Array in bytes; Input to @ref SA_GetArraySize. */
#define kArrayBlockSizeBytes     0x05 /**< Get size (in bytes) of blocks of Array; Input to @ref SA_GetArraySize. */
/** @} */
/** Get the requested size for the Array.
 * @pre `array_number` is a valid Array number.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Target Array number.
 * @param[in] size_type     What type of Array size to return (see [Get Array Size Types](@ref storc_properties_array_size)).
 * @return Size of Array depending on param [size_type](@ref storc_properties_array_size).
 */
SA_QWORD SA_GetArraySize(PCDA cda, SA_WORD array_number, SA_BYTE size_type);
/** @} */

/** @defgroup storc_properties_array_media_interface Array Media and Interface Type
 * @{ */
/** Get the media and interface type of the array.
 *
 * @param[in] cda          Controller CDA pointer
 * @param[in] array_number Array number
 * @return Media and interface type of the array (see [Array Media and Interface Types](@ref storc_properties_array_media_interface)
 * @return Decode with @ref ARRAY_MEDIA_TYPE and @ref ARRAY_INTERFACE_TYPE.
 *
 * __Examples__
 * @code
 * SA_BYTE array_type = SA_GetArrayMediaInterfaceType(cda, 1);
 * printf("Array 1 Media: %s\n", ARRAY_MEDIA_TYPE(array_type) == kArrayMediaSSD ? "SSD"
 *                               : ARRAY_MEDIA_TYPE(array_type) == kArrayMediaHDD ? "HDD" : "Unknown");
 * printf("Array 1 Interface: %s\n", ARRAY_INTERFACE_TYPE(array_type) == kArrayInterfaceSAS ? "SAS"
 *                                   : ARRAY_INTERFACE_TYPE(array_type) == kArrayInterfaceSATA ? "HDD" : "SATA");
 * @endcode
 */
SA_BYTE SA_GetArrayMediaInterfaceType(PCDA cda, SA_WORD array_number);
   /** @name Array Media and Interface Types
    * @outputof SA_GetArrayMediaInterfaceType
    * @{ */
   #define kArrayMediaUnknown       0x0 /**< Unknown Array media type; @outputof SA_GetArrayMediaInterfaceType. decoded with @ref ARRAY_MEDIA_TYPE. */
   #define kArrayMediaSSD           0x1 /**< SSD Array media type; @outputof SA_GetArrayMediaInterfaceType. decoded with @ref ARRAY_MEDIA_TYPE. */
   #define kArrayMediaHDD           0x2 /**< HDD Array media type; @outputof SA_GetArrayMediaInterfaceType. decoded with @ref ARRAY_MEDIA_TYPE. */
   #define kArrayMediaMask          0x3 /**< Mask output of @ref SA_GetArrayMediaInterfaceType to get Array media type. @see ARRAY_MEDIA_TYPE. */
   /** Decode output of @ref SA_GetArrayMediaInterfaceType to get Array media type. */
   #define ARRAY_MEDIA_TYPE(info_value) (info_value & kArrayMediaMask)

   #define kArrayInterfaceUnknown   0x0 /**< Unknown Array interface type; @outputof SA_GetArrayMediaInterfaceType. decoded with @ref ARRAY_INTERFACE_TYPE. */
   #define kArrayInterfaceSAS       0x4 /**< SAS Array interface type; @outputof SA_GetArrayMediaInterfaceType. decoded with @ref ARRAY_INTERFACE_TYPE. */
   #define kArrayInterfaceSATA      0x8 /**< SATA Array interface type; @outputof SA_GetArrayMediaInterfaceType. decoded with @ref ARRAY_INTERFACE_TYPE. */
   #define kArrayInterfaceNVMe      0x10 /**< NVME Array interface type; @outputof SA_GetArrayMediaInterfaceType. decoded with @ref ARRAY_INTERFACE_TYPE. */
   #define kArrayInterfaceMask      0x1C /**< Mask output of @ref SA_GetArrayMediaInterfaceType to get Array interface type. @see ARRAY_INTERFACE_TYPE. */
   /** Decode output of @ref SA_GetArrayMediaInterfaceType to get Array interface type. */
   #define ARRAY_INTERFACE_TYPE(info_value) (info_value & kArrayInterfaceMask)
   /** @} */
/** @} */

/** @defgroup storc_properties_array_physical_drive_block_size Array Physical Drive Block Size
 * @{ */
/** Get the physical drive block size of the array.
 *
 * @param[in] cda          Controller CDA pointer
 * @param[in] array_number Array number
 * @return Physical drive block size
 */
SA_QWORD SA_GetArrayPhysicalDriveBlockSize(PCDA cda, SA_WORD array_number);
/** @} */

/** @defgroup storc_properties_array_spare_type Array Spare Type
 * @{ */
 /** @name Array Spare Types
 * @outputof SA_GetArraySpareType & SA_GetEditableArraySpareType
 * Input to @ref SA_CanAddEditableArraySpareDrive & @ref SA_AddEditableArraySpareDrive.
 * Input to @ref SA_CanAddArraySpareDrive & @ref SA_AddArraySpareDrive.
 * @{ **/
 /** Array's spare type: use the existing spare type fo the array.
 * Input to @ref SA_CanAddEditableArraySpareDrive & @ref SA_AddEditableArraySpareDrive.
 * Input to @ref SA_CanAddArraySpareDrive & @ref SA_AddArraySpareDrive.
 */
#define kSpareDriveTypeExisting     0x00
 /** Array's spare type: auto-replace, also called roaming spare.
 * @outputof SA_GetEditableArraySpareType & SA_GetArraySpareType.
 * Input to @ref SA_CanAddEditableArraySpareDrive & @ref SA_AddEditableArraySpareDrive.
 */
#define kSpareDriveTypeAutoReplace  0x01
 /** Array's spare type: dedicated, also called shareable spare.
 * @outputof SA_GetEditableArraySpareType & SA_GetArraySpareType.
 * Input to @ref SA_CanAddEditableArraySpareDrive & @ref SA_AddEditableArraySpareDrive.
 */
#define kSpareDriveTypeDedicated    0x02
 /** @} */

/** Get the spare rebuild mode of the array.
 *
 * @param[in] cda          Controller CDA pointer
 * @param[in] array_number Array number
 * @return Spare type (see [Array Spare Types](@ref storc_properties_array_spare_type)).
 */
SA_BYTE SA_GetArraySpareType(PCDA cda, SA_WORD array_number);
   /** @name Array Spare Types
    * @outputof SA_GetArraySpareType.
    * @outputof SA_GetEditableArraySpareType.
    * @{ */
   /** The requested editable Array does not exist.
   * @outputof SA_GetArraySpareType & SA_GetEditableArraySpareType.
   */
   #define kSpareDriveTypeDoesNotExist  0x00
   /** @} */
/** @} */ /* storc_properties_array_spare_type */

/** @defgroup storc_properties_array_acceleration_mode Array Acceleration Mode
 * @{ */
/** Get the acceleration mode of the array.
 *
 * @param[in] cda          Controller CDA pointer
 * @param[in] array_number Array number
 * @return Acceleration mode (see [Array Spare Rebuild Modes](@ref storc_properties_array_acceleration_mode))
 */
SA_BYTE SA_GetArrayAccelerationMode(PCDA cda, SA_WORD array_number);
   /** @name Array Spare Rebuild Modes
    * @outputof SA_GetArrayAccelerationMode.
    * @{ */
   #define kArrayAccelerationModeUnknown     0 /** Array acceleration: UNKNOWN; @outputof SA_GetArrayAccelerationMode. */
   #define kArrayAccelerationModeIOBypass    1 /** Array acceleration: I/O Bypass; @outputof SA_GetArrayAccelerationMode. */
   #define kArrayAccelerationModeIndependent 2 /** Array acceleration: Independent; @outputof SA_GetArrayAccelerationMode. */
   /** @} */
/** @} */

/** @defgroup storc_properties_array_split_sibling Array Split Mirror Sibling
 * @{ */
/** Get the Array number of an Array's split mirror primary or backup pair.
 * @param[in] cda           @ref storpage_module_storagecore_cda
 * @param[in] array_number  Split mirror primary or backup Array.
 * @return If `array_number` is a split mirror primary, return the array number of the split mirror backup.
 * @return If `array_number` is a split mirror backup, return the array number of the split mirror primary.
 * @return If `array_number` is a split mirror orphan (or not a split mirror array), return @ref kInvalidArrayNumber.
 */
SA_WORD SA_GetArraySplitMirrorNumber(PCDA cda, SA_WORD array_number);
/** @} */ /* storc_properties_array_split_sibling */

/** @} */ /* storc_properties_array */

/*************************************//**
 * @defgroup storc_properties_lk License Key
 * @brief
 * @details
 * @{
 ****************************************/

/** @defgroup storc_properties_lk_count License Key Count
 * @{ */
/** Get number of installed license keys
 *
 * @param[in] cda  Controller data area.
 * @return Number of installed license keys.
 */
SA_WORD SA_GetControllerLicenseKeyCount(PCDA cda);

/** Iterate over all controller license key and execute the block with each valid ld_index.
 * @attention ANSI C Safe.
 * @param[in] cda           Controller data area.
 * @param[in] lk_index     Variable to store valid license key index.
 *
 * __Example__
 * @code
 * void DoSomethingWithEachControllerLicenseKey(PCDA cda)
 * {
 *    SA_WORD lk_index;
 *    FOREACH_LICENSE_KEY(cda, lk_index)
 *    {
 *       // do something...
 *    }
 * }
 * @endcode
 */
#define FOREACH_LICENSE_KEY(cda, lk_index) \
   for (lk_index = 0;\
        lk_index < SA_GetControllerLicenseKeyCount(cda); \
        lk_index++)
/** @} */ /* storc_properties_lk_count */

/** @defgroup storc_properties_lk_info License Key Information
 * @{ */
/** Extract license key install month from @ref LicenseKeyRecord::install_date. */
#define LICENSE_KEY_INSTALL_MONTH(date) ((date & 0x0000FF00) >> 8)
/** Extract license key install day from @ref LicenseKeyRecord::install_date. */
#define LICENSE_KEY_INSTALL_DAY(date)    (date & 0x000000FF)
/** Extract license key install year from @ref LicenseKeyRecord::install_date. */
#define LICENSE_KEY_INSTALL_YEAR(date)  ((date & 0xFFFF0000) >> 16)

/** @name License Key Status
 * Values for @ref LicenseKeyRecord::key_status.
 * @{ */
#define kLicenseKeyStatusDuplicateFeature             0x0001 /**< Feature has multiple Key Records (bit set for @ref LicenseKeyRecord::key_status). */
#define kLicenseKeyStatusUnknownFeature               0x0002 /**< Unknown feature (bit set for @ref LicenseKeyRecord::key_status). */
#define kLicenseKeyStatusHardwareSerialNumberMismatch 0x0004 /**< Serial number does not match (bit set for @ref LicenseKeyRecord::key_status). */
#define kLicenseKeyStatusHardwareSeriesMismatch       0x0008 /**< Series number does not match (bit set for @ref LicenseKeyRecord::key_status). */
#define kLicenseKeyStatusKeyExpired                   0x0010 /**< Demo or Beta feature has expired the usage date (bit set for @ref LicenseKeyRecord::key_status). */
#define kLicenseKeyStatusExpireNotification           0x0020 /**< An expiration event has already been logged (bit set for @ref LicenseKeyRecord::key_status). */
/** @} */

/** @name License Key Types
 * Values for @ref LicenseKeyRecord::key_type.
 * All other license key types should be considered 'unsupported'.
 * @{ */
#define kLicenseKeyTypeFlexQuantity     0x01 /**< Value for @ref LicenseKeyRecord::key_type. */
#define kLicenseKeyTypeDemoDaysOnly     0x02 /**< Value for @ref LicenseKeyRecord::key_type. */
#define kLicenseKeyTypeDemoWithSeats    0x04 /**< Value for @ref LicenseKeyRecord::key_type. */
#define kLicenseKeyTypeFreeFlexQuantity 0x07 /**< Value for @ref LicenseKeyRecord::key_type. */
#define kLicenseKeyTypeSite             0x09 /**< Value for @ref LicenseKeyRecord::key_type. */
/** @} */

/** License key information populated by calling @ref SA_GetControllerLicenseKeyInfo.
 */
struct _LicenseKeyRecord
{
   /** Install date of license key in the form 0xYYYYMMDD
    * @see LICENSE_KEY_INSTALL_MONTH
    * @see LICENSE_KEY_INSTALL_DAY_
    * @see LICENSE_KEY_INSTALL_YEAR
    */
   SA_DWORD install_date;
   /** License key feature name. */
   char key_name[9];
   /** License key value. */
   char key[29];
   /** License key status.
    * See [License Key Status](@ref storc_properties_lk_info)
    */
   SA_WORD key_status;
   /** License key type.
    * See [License Key Type](@ref storc_properties_lk_info)
    */
   SA_BYTE key_type;
};
typedef struct _LicenseKeyRecord  LicenseKeyRecord;
typedef struct _LicenseKeyRecord* PLicenseKeyRecord;
/** Get license key information
 * @attention Some controllers do not officially support license keys (see @ref storc_features_ctrl_lk).
 * @attention If not supported, information returned from this function may not be valid.
 * @param[in] cda       Controller data area.
 * @param[in] lk_index  License key index which must be less that @ref SA_GetControllerLicenseKeyCount.
 * @param[in] record    User provided area to populate with license key data.
 * @return @ref kTrue/@ref kFalse if license key record successfully populated.
 */
SA_BOOL SA_GetControllerLicenseKeyInfo(PCDA cda, SA_WORD lk_index, PLicenseKeyRecord record);

/** @} */ /* storc_properties_lk_info */

/** @defgroup storc_properties_pd_extended_data Physical Drive Extended Data
 * @{ */
/** Extended drive information raw data filled by calling @ref SA_GetPhysicalDriveOEMExtendedData
 */
struct _SA_EXTENDED_DRIVE_DATA
{
   SA_BYTE raw_data[64];
};
typedef struct _SA_EXTENDED_DRIVE_DATA  SA_EXTENDED_DRIVE_DATA;

/** Gather OEM physical drive extended data and data type.
 * @pre The user has used @ref SA_PhysicalDriveSupportsOEMExtendedDriveData to ensure the drive supports reporting this data.
 *
 * @param[in]  cda        Controller data area.
 * @param[in]  pd_number  Target physical drive number.
 * @param[out] data_type  Indicates the OEM ID for the physical drive extended data.
 * @return Pointer to the extended drive data buffer.
 * @return NULL on failure.
 * @post User must use @ref SA_free on the returned pointer after calling this function.
 * @remark If the data_type is 1, use @ref SA_EXTENDED_DRIVE_DATA to parse the returned data.
 *
 * __Example__
 * @code
 * SA_BYTE data_type;
 * SA_WORD pd_number;
 * void *extended_drive_data_buffer;
 * struct SA_EXTENDED_DRIVE_DATA extended_drive_data;
 *
 * if ((extended_drive_data_buffer = SA_GetPhysicalDriveOEMExtendedData(cda, pd_number, &data_type)) != NULL)
 * {
 *    if (data_type == 1)
 *    {
 *       extended_drive_data = (SA_EXTENDED_DRIVE_DATA*)extended_drive_data_buffer;
 *    }
 *    SA_free(extended_drive_data_buffer);
 * }
 * @endcode
 */
void* SA_GetPhysicalDriveOEMExtendedData(PCDA cda, SA_WORD pd_number, SA_BYTE* data_type);

/** @} */ /* storc_properties_pd_extended_data */

/** @defgroup storc_properties_pd_mnp_statistics Physical Drive Monitor and Performance Statistics
 * @{ */
/** Physical drive monitor and performance data filled by calling @ref SA_GetPhysicalDriveMNPStatistics
 */
struct _MNP_STATISTICS
{
   SA_WORD  aborted_commands;             /**< Number of aborted commands that could not be retried successfully. A value of 0xFFFF should be considered invalid. */
   SA_WORD  bad_target_errors;            /**< Number of times the physical drive did something that did not conform to the SCSI-2 Bus Protocol. A value of 0xFFFF should be considered invalid. */
   SA_QWORD ecc_recovered_read_errors;    /**< Number of ECC corrected read errors. A value of 0xFFFFFFFFFFFFFFFF should be considered invalid. */
   SA_WORD  failed_read_recovers;         /**< Number of times a recover of another drive in the logical volume failed due to a hard read error from this physical drive. A value of 0xFFFF should be considered invalid. */
   SA_WORD  failed_write_recovers;        /**< Number of times a recover of this physical drive failed due to an error occurring on this drive during a write operation. A value of 0xFFFF should be considered invalid. */
   SA_WORD  format_errors;                /**< Number of times a format command failed. A value of 0xFFFF should be considered invalid. */
   SA_WORD  hardware_errors;              /**< Number of times a bad hardware status was returned. A value of 0xFFFF should be considered invalid. */
   SA_DWORD hard_read_errors;             /**< Number of unrecoverable read errors. A value of 0xFFFFFFFF should be considered invalid. */
   SA_DWORD hard_write_errors;            /**< Number of unrecoverable write errors. A value of 0xFFFFFFFF should be considered invalid. */
   SA_DWORD hot_plug_count;               /**< Number of hot-plugs from some box types. A value of 0xFFFFFFFF should be considered invalid. */
   SA_WORD  media_failures;               /**< Number of failures due to unrecoverable media errors. A value of 0xFFFF should be considered invalid. */
   SA_DWORD not_ready_errors;             /**< Number of failures due to the physical drive never becoming ready after the spin up command was issued. A value of 0xFFFFFFFF should be considered invalid. */
   SA_WORD  other_time_out_errors;        /**< Number of timeouts other than DRQ timeouts. A value of 0xFFFF should be considered invalid. */
   SA_DWORD predictive_failures;          /**< Number of predictive failure errors given. A value of 0xFFFFFFFF should be considered invalid. */
   SA_DWORD retry_recovered_read_errors;  /**< Number of retry recovered read errors. A value of 0xFFFFFFFF should be considered invalid. */
   SA_DWORD retry_recovered_write_errors; /**< Number of retry recovered write errors. A value of 0xFFFFFFFF should be considered invalid. */
   SA_DWORD bus_faults;                   /**< Number of bus faults. A value of 0xFFFFFFFF should be considered invalid. */
   SA_QWORD sectors_read;                 /**< Number of sectors read. A value of 0xFFFFFFFFFFFFFFFF should be considered invalid. */
   SA_QWORD sectors_written;              /**< Number of sectors written. A value of 0xFFFFFFFFFFFFFFFF should be considered invalid. */
   SA_QWORD seek_count;                   /**< Number of seeks. A value of 0xFFFFFFFFFFFFFFFF should be considered invalid. */
   SA_QWORD seek_errors;                  /**< Number of seek errors. A value of 0xFFFFFFFFFFFFFFFF should be considered invalid. */
   SA_WORD  drq_timeouts;                 /**< Number of DRQ timeouts. A value of 0xFFFF should be considered invalid. */
   SA_WORD  spin_retries;                 /**< Number of times drive had to retry spin up. A value of 0xFFFF should be considered invalid. */
   SA_DWORD reference_hours;              /**< The time interval in hours over which all the above statistics have been accumulated. A value of 0xFFFFFFFF should be considered invalid. */
};
typedef struct _MNP_STATISTICS MNP_STATISTICS;

/** Returns Physical Drive Monitor and Performance Statistics.
 * @param[in]  cda             Controller data area.
 * @param[in]  pd_number       Target physical drive number.
 * @param[out] mnp_statistics  Address of [MNP_STATISTICS](@ref storc_properties_pd_mnp_statistics) to store physical drive MNP statistics.
 * @return [Physical Drive MNP statistics returns](@ref storc_properties_pd_mnp_statistics)
 */
SA_BYTE SA_GetPhysicalDriveMNPStatistics(PCDA cda, SA_WORD pd_number, MNP_STATISTICS* mnp_statistics);
/** @name Physical Drive MNP Statistics returns
 * @outputof SA_GetPhysicalDriveMNPStatistics.
 * @{ */
#define kGetPhysicalDriveMNPStatisticsSuccess         1  /**< Get the MNP statistics successfully. */
#define kGetPhysicalDriveMNPStatisticsInvalidInput    2  /**< Invalid input. */
#define kGetPhysicalDriveMNPStatisticsUnsupportedPD   3  /**< The specified physical drive is not supported for reading MNP statistics. */
#define kGetPhysicalDriveMNPStatisticsNotOKState      4  /**< The specified physical drive is in a warning or failed state. */
#define kGetPhysicalDriveMNPStatisticsFailed          5  /**< Get MNP statistics failed. */
/** @} */

/** @} */ /* storc_properties_pd_mnp_statistics */

/** @} */ /* storc_properties_lk */

/** @defgroup storc_properties_ctrl_nvram_errors Controller NVRAM Errors
 * @{ */
/** Get NVRAM checksum errors.
 *
 * @param[in] cda        Controller data area.
 * @return See [Controller NVRAM Error Returns] (@ref storc_properties_ctrl_nvram_errors).
 */
SA_BYTE SA_GetControllerNVRAMErrors(PCDA cda);
   /** @name Controller NVRAM Error Returns
    * @outputof SA_GetControllerNVRAMErrors.
    * @{ */
   #define kControllerNVRAMErrorNone                           0x00 /** No NVRAM errors; @outputof SA_GetControllerNVRAMErrors */
   #define kControllerNVRAMErrorBoardIDNotRead                 0x01 /** Board ID could not be read. Unrecoverable error; @outputof SA_GetControllerNVRAMErrors */
   #define kControllerNVRAMErrorImageFailedButCanBeRestored    0x02 /** Bootstrap NVRAM image failed checksum test, but a backup image was found and successfully restored. A system restart is needed; @outputof SA_GetControllerNVRAMErrors */
   #define kControllerNVRAMErrorImageFailedButCannotBeRestored 0x03 /** Bootstrap NVRAM image failed checksum test and could not be restored. This error may or may not be recoverable. A firmware update might be able to correct the error; @outputof SA_GetControllerNVRAMErrors */
   #define kControllerNVRAMErrorUnsupported                    0xFE /** Controller does not support reporting NVRAM errors; @outputof SA_GetControllerNVRAMErrors */
   #define kControllerNVRAMErrorUnknown                        0xFF /** Internal/unknown error; @outputof SA_GetControllerNVRAMErrors */
   /** @} */
/** @} */ /* storc_properties_ctrl_nvram_errors */

#if defined(__cplusplus)
}
#endif

/************************************//**
 * @defgroup storc_enumerators Enumerators
 * @brief
 * @details
 * @{
 * @addtogroup storc_properties_ld_enum
 * @addtogroup storc_properties_pd_enum
 * @addtogroup storc_properties_array_enum
 * @addtogroup storc_properties_array_pd_enum
 * @addtogroup storc_properties_array_ld_enum
 * @addtogroup storc_properties_physical_devices_enum
 * @addtogroup storc_properties_ld_bootable_enum
 * @addtogroup storc_properties_bootable_pd_enum
 * @addtogroup storc_properties_sep_enum
 * @addtogroup storc_properties_expander_enum
 * @addtogroup storc_properties_se_enum
 * @addtogroup storc_properties_se_pd_enum
 * @addtogroup storc_properties_se_sep_enum
 * @addtogroup storc_properties_se_expander_enum
 * @}
 */

/** @} */ /* storc_properties */

/** @} */ /* storcore */

#endif /* PSTORC_PROPERTIES_H */
