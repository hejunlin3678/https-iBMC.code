/** @file pstorc_commands.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore commands header file
   Defines functions for sending commands to devices

*/

#ifndef PSTORC_COMMANDS_H
#define PSTORC_COMMANDS_H

#include "psysc_commands.h"

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/*********************************//**
 * @defgroup storc_commands Commands
 * @brief Special commands to devices.
 * @details
 * ### Examples
 * - @ref storc_command_examples
 * @{
 ************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/**************************************************//**
 * @defgroup storc_commands_ctrl Controller Commands
 * @brief Send special commands to a controller.
 * @details
 * @{
 *****************************************************/

/** Retrieve the @ref syscore @ref DeviceDescriptor for the controller.
 * @warning The caller shall not change or free the return descriptor.
 * @param[in] cda        Controller data area.
 *
 * @return The @ref syscore @ref DeviceDescriptor for the controller.
 */
DeviceDescriptor SA_GetControllerDeviceDescriptor(PCDA cda);

/** @defgroup storc_commands_ctrl_reset Reset Controller
 * @brief Reset a controller (not supported on all platforms).
 * @{ */
/** Reset controller.
 * @warning Understand the risk of resetting an active controller.
 * @param[in] cda         Controller handler.
 * @param[in] reset_type  Reset type (see [Controller Reset Types](@ref syscore_command_reset)).
 * @return @ref kTrue/@ref kFalse if command was successful/unsuccessful.
 */
SA_BOOL SA_ResetController(PCDA cda, SA_BYTE reset_type);
/** @} */ /* storc_commands_ctrl_reset */

/** @defgroup storc_commands_ctrl_bkg_act Background Activity
 * @brief Pause/Resume background activities of a controller.
 * @{ */
/** Pause background activity of controller.
 * @pre Controller accepts BMIC commands.
 * @post User should resume background activity using @ref SA_ControllerResumeBackgroundActivity
 * @param[in] cda                   Controller handler.
 * @param[in] pause_hotplug_events  Set to true to also disable detection of hot-plug/remove physical devices.
 * @return @ref kTrue/@ref kFalse if command was successful/unsuccessful.
 */
SA_BOOL SA_ControllerPauseBackgroundActivity(PCDA cda, SA_BOOL pause_hotplug_events);

/** Resume background activity of controller.
 * @pre Controller accepts BMIC commands.
 * @pre Usually called after a call to @ref SA_ControllerPauseBackgroundActivity
 * @param[in] cda                   Controller handler.
 * @return @ref kTrue/@ref kFalse if command was successful/unsuccessful.
 */
SA_BOOL SA_ControllerResumeBackgroundActivity(PCDA cda);
/** @} */ /* storc_commands_ctrl_bkg_act */

/** @defgroup storc_commands_ctrl_recover_cache_module Recover Cache Module
 * @brief Recover/reactivate a controller's cache module after a failed cache backup.
 * @{ */
/** @name Can Recover Cache Module
 * @{ */
/** Can the controller recover the cache module?
 *
 * @post If allowed, user may call @ref SA_RecoverCacheModule.
 * @param[in] cda  Controller handler.
 * @return See returns for [Can Recover Cache Module](@ref storc_commands_ctrl_recover_cache_module).
 *
 * __Example__
 * @code
 * if (SA_CanRecoverCacheModule(cda) == kCanRecoverCacheModuleAllowed)
 * {
 *    if (SA_RecoverCacheModule(cda) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to recover cache module\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRecoverCacheModule(PCDA cda);
   /** Allowed.
    * @outputof SA_CanRecoverCacheModule.
    * @allows SA_RecoverCacheModule.
    */
   #define kCanRecoverCacheModuleAllowed                 1
   /** Recovering cache module is not allowed by the firmware.
    * @outputof SA_CanRecoverCacheModule.
    * @disallows SA_RecoverCacheModule.
    */
   #define kCanRecoverCacheModuleNotAllowedByFirmware    2
   /** Recovering cache module is not allowed by the firmware while an online activation operation is active or pending.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanRecoverCacheModule.
    * @disallows SA_RecoverCacheModule.
    */
   #define kCanRecoverCacheModuleControllerOFAActive     3

/** @} */ /* Can Recover Cache Module */

/** @name Recover Cache Module
 * @{
 */
/** Recover Cache Module.
 *
 * @pre The user has used @ref SA_CanRecoverCacheModule to ensure the operation can be performed.
 * @param[in] cda  Controller handler.
 * @return @ref kTrue/@ref kFalse if command was successful/unsuccessful.
 *
 * __Example__
 * @code
 * if (SA_CanRecoverCacheModule(cda) == kCanRecoverCacheModuleAllowed)
 * {
 *    if (SA_RecoverCacheModule(cda) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to recover cache module\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_RecoverCacheModule(PCDA cda);
/** @} */ /* Recover Cache Module */

/** @} */ /* storc_commands_ctrl_recover_cache_module */

/** @defgroup storc_commands_ctrl_debug_logs Get Controller Debug Logs
 * @{
 */

/** @name Controller Last Crash Dump
 * @{ */
/** Collect the controller's last crash dump/log.
 *
 * @post Client must use @ref SA_free on return address to free memory.
 * @param[in]  cda Controller handle.
 * @param[out] n   Size (in bytes) of returned buffer.
 * @return Address & size of controller's last crash dump log.
 * @return NULL if an error occured.
 */
void *SA_GetControllerLastCrashDump(PCDA cda, SA_DWORD *n);
/** @} */ /* Controller Last Crash Dump */

/** @name Complete Controller Serial Output Buffer
 * @{ */
/** Collect the controller's complete serial output buffer.
 *
 * @post Client must use @ref SA_free on return address to free memory.
 * @param[in]  cda Controller handle.
 * @param[out] n   Size (in bytes) of returned buffer.
 * @return Address & size of controller's serial output buffer.
 * @return NULL if an error occured.
 */
void *SA_GetControllerSerialOutputBuffer(PCDA cda, SA_DWORD *n);
/** @} */ /* Complete Controller Serial Output Buffer */

/** @name Controller Persistent Event Log
 * @{ */
/** Collect the controller's persistent event log.
 *
 * @pre Client has called @ref SA_ControllerSupportsPersistentEventLog to verify that the controller supports
 * persistent event logs.
 *
 * @post Client should use @ref SA_EnumeratePersistentEventLogBuffer to parse the returned buffer.
 * @post Client must use @ref SA_free on return address to free memory.
 *
 * @param[in] cda Controller handle.
 * @param[out] n  Size (in bytes) of returned buffer.
 * @return Address & size of controller's persistent event log buffer.
 * @return NULL if unsupported (see @ref SA_ControllerSupportsPersistentEventLog) or an error occurred.
 */
void *SA_GetControllerPersistentEventLog(PCDA cda, SA_DWORD *n);
/** @} */ /* Controller Persistent Event Log */

/** @name Enumerate Persistent Event Log
 * @{ */
/** Enumerate the events in the controller persistent event log buffer.
 * See @ref SA_GetControllerPersistentEventLog.
 *
 * @pre Client must call @ref SA_GetControllerPersistentEventLog to retrieve a persistent event log for this API.
 * @param[in] persistent_event_log  Address of the persistent event log buffer from @ref SA_GetControllerPersistentEventLog.
 * @param[in] log_length            Size of the persistent event log buffer from @ref SA_GetControllerPersistentEventLog.
 * @param[in] event_pointer         Pointer to the last event read from the persistent event log.
 * @return A pointer to the next persistent controller event.
 *
 * __Example__
 * @code{.c}
 * SA_DWORD size = 0;
 * void* persistent_event_log = SA_GetControllerPersistentEventLog(cda, &size);
 * PersistentControllerEvent* persistent_controller_event = NULL;
 * persistent_controller_event = SA_EnumeratePersistentEventLogBuffer(persistent_event_log, size, persistent_controller_event);
 *
 * while (persistent_controller_event != NULL)
 * {
 *    // Read information from the persistent event.
 *    printf("Persistent controller event class %d subclass %d detail %d\n",
 *       persistent_controller_event.event_class_code,
 *       persistent_controller_event.event_subclass_code,
 *       persistent_controller_event.event_detail_code);
 *
 *    // Increment the persistent_controller_event pointer.
 *    persistent_controller_event = SA_EnumeratePersistentEventLogBuffer(persistent_event_log, size, persistent_controller_event);
 * }
 * @endcode
 */
PersistentControllerEvent* SA_EnumeratePersistentEventLogBuffer(void* persistent_event_log, SA_DWORD log_length, PersistentControllerEvent *event_pointer);
/** @} */ /* Enumerate Persistent Event Log */

/** @} */ /* storc_commands_ctrl_debug_logs */

/** @defgroup storc_commands_ctrl_debug_token Get Controller Debug Token
 * @{
 */

/** @name Controller Debug Token
 * @{ */
/** Retrieve the controller's debug token.
 *
 * @param[in]  cda Controller handle.
 * @param[out] n   Size (in bytes) of returned buffer.
 * @returns NULL if error, otherwise returns ADDRESS of debug token and SIZE n of debug token.
 * @post Client must use @ref SA_free on return address to free memory.
 */
void *SA_GetControllerDebugToken(PCDA cda, SA_DWORD *n);
/** @} */ /*Controller Debug Token */

/** @} */ /* storc_commands_ctrl_debug_token */

/** @defgroup storc_commands_ctrl_do_soft_reset_controller OFA Soft Reset Controller
 * @brief Soft Reset the controller using the OFA feature.
 * @{ */
/** @name Can OFA Soft Reset Controller
 * @{ */
/** Can the controller be soft reset?
 *
 * @post If allowed, user may call SA_DoOFASoftResetController
 * @param[in] cda  Controller handle.
 * @return Uses same return codes as [SA_ValidateControllerOnlineFlash](@ref storc_flash_ctrl).
 */
SA_UINT64 SA_CanOFASoftResetController(PCDA cda);
/** @} */ /* Can OFA Soft Reset Controller */
/** @name Do OFA Soft Reset Controller
 * @pre The user has used @ref SA_CanOFASoftResetController to ensure the operation can be performed.
 * @param[in] cda  Controller handle.
 *  @return Uses same return codes as [SA_ValidateControllerOnlineFlash](@ref storc_flash_ctrl).
 *
 __Example__
 * @code
 * if (SA_CanOFASoftResetController(cda) == kControllerOnlineFirmwareActivationValidateOk)
 * {
 *    if (SA_DoOFASoftResetController(cda) != kControllerOnlineFirmwareActivationValidateOk)
 *    {
 *        fprintf(stderr, "Failed to soft reset controller\n");
 *    }
 * }
 * @endcode
 */
SA_UINT64 SA_DoOFASoftResetController(PCDA cda);
/** @} */ /* OFA Soft Reset Controller */
/** @} */ /* storc_commands_ctrl_do_soft_reset_controller */
/** @} */ /* storc_commands_ctrl */

/**************************************************//**
 * @defgroup storc_commands_pd Physical Drive Commands
 * @brief Special commands for a physical drive.
 * @details
 * ### Examples
 * - @ref storc_command_scsi_command_example
 * @{
 *****************************************************/

/** Retrieve the @ref syscore @ref DeviceDescriptor for a physical device.
 * @warning The caller shall not change or free the return descriptor.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Target physical drive number.
 *
 * @return The @ref syscore @ref DeviceDescriptor for the device.
 */
DeviceDescriptor SA_GetPhysicalDeviceDescriptor(PCDA cda, SA_WORD pd_number);

/** @defgroup storc_commands_pd_ata Physical Drive ATA Commands
 * @brief Send ATA passthough commands to a physical drive.
 * @{ */
/** Is it allowed to send a ATA passthrough command to the physical device?
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Number of target physical device. This can be a physical drive, tape drive, or SEP.
 * @return @ref kTrue/@ref kFalse if it is possible to send ATA commands to the physical device.
 */
SA_BOOL SA_CanSendPhysicalDeviceATACommand(PCDA cda, SA_WORD pd_number);

/** Send a ATA passthrough command to the designated physical device
 * @pre The user has used @ref SA_CanSendPhysicalDeviceATACommand to ensure the operation can be performed.
 * @param[in]     cda        Controller data area.
 * @param[in]     pd_number  Number of target physical device. This can be a physical drive, tape drive, or SEP.
 * @param[in,out] ata_request Pointer to a ATA request structure
 * @return @ref kTrue/@ref kFalse if it the command successfully executed on the target device
 */
SA_BOOL SA_SendPhysicalDeviceATACommand(PCDA cda, SA_WORD pd_number, PATARequest ata_request);
/** @} */ /* storc_commands_pd_ata */

/** @defgroup storc_commands_pd_scsi Physical Drive SCSI Commands
 * @brief Send SCSI passthough commands to a physical drive.
 * @{ */
/** Is it allowed to send a SCSI passthrough command to the physical device?
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Number of target physical device. This can be a physical drive, tape drive, or SEP.
 * @return @ref kTrue/@ref kFalse if it is possible to send SCSI commands to the physical device.
 */
SA_BOOL SA_CanSendPhysicalDeviceSCSICommand(PCDA cda, SA_WORD pd_number);

/** Send a SCSI passthrough command to the designated physical device
 * @pre The user has used @ref SA_CanSendPhysicalDeviceSCSICommand to ensure the operation can be performed.
 * @param[in]     cda        Controller data area.
 * @param[in]     pd_number  Number of target physical device. This can be a physical drive, tape drive, or SEP.
 * @param[in,out] scsi_request Pointer to a SCSI request structure
 * @return @ref kTrue/@ref kFalse if it the command successfully executed on the target device
 */
SA_BOOL SA_SendPhysicalDeviceSCSICommand(PCDA cda, SA_WORD pd_number, PSCSIRequest scsi_request);

/** Send SCSI TEST UNIT READY command to target physical drive.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Number of target physical drive.
 * @return @ref kTrue/@ref kFalse if the command successfully executed on the target device.
 */
SA_BOOL SA_PhysicalDriveTestUnitReady(PCDA cda, SA_WORD pd_number);

/** Send SCSI START STOP UNIT command to target physical drive.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Number of target physical drive.
 * @param[in] immediate  Sets the SCSI START STOP UNIT immediate bit.
 * @param[in] start      Sets the SCSI START STOP UNIT start bit.
 * @return @ref kTrue/@ref kFalse if the command successfully executed on the target device.
 */
SA_BOOL SA_PhysicalDriveStartStopUnit(PCDA cda, SA_WORD pd_number, SA_BOOL immediate, SA_BOOL start);

/** Get Standard INQUIRY Data for the target physical drive.
 * @param[in]  cda             Controller handler.
 * @param[in]  pd_number       Number of target physical drive.
 * @param[out] io_buffer       IO buffer that receives the requested physical drive data.
 * @param[in]  io_buffer_size  Size of the IO buffer provided for results.
 * @return @ref kTrue/@ref kFalse if the command successfully executed on the target device.
 */
SA_BOOL SA_PhysicalDriveStandardInquiryData(PCDA cda, SA_WORD pd_number, SA_BYTE *io_buffer, SA_WORD io_buffer_size);

/** Get the specified VPD page for the target physical drive.
 * @param[in]  cda             Controller handler.
 * @param[in]  pd_number       Number of target physical drive.
 * @param[in]  page_code       Vital Product Data (VPD) page code to look up.
 * @param[out] io_buffer       IO buffer that receives the requested physical drive data.
 * @param[in]  io_buffer_size  Size of the IO buffer provided for results.
 * @return @ref kTrue/@ref kFalse if the command successfully executed on the target device.
 */
SA_BOOL SA_PhysicalDriveVPDPage(PCDA cda, SA_WORD pd_number, SA_BYTE page_code, SA_BYTE *io_buffer, SA_WORD io_buffer_size);
   #define kPhysicalDriveInquirySupportedVPDPages 0x00     /**< Supported VPD Pages (mandatory) ; input of @ref SA_PhysicalDriveVPDPage. */
   #define kPhysicalDriveInquiryUnitSerialNumber 0x80      /**< Unit Serial Number (optional) ; input of @ref SA_PhysicalDriveVPDPage. */
   #define kPhysicalDriveInquiryDeviceIdentification 0x83  /**< Device Identification (mandatory) ; input of @ref SA_PhysicalDriveVPDPage. */
   #define kPhysicalDriveInquiryExtendedInquiryData 0x86   /**< Extended INQUIRY Data (optional) ; input of @ref SA_PhysicalDriveVPDPage. */
/** @} */ /* storc_commands_pd_scsi */

/** @defgroup storc_commands_pd_led Physical Drive Identification LED Commands
 * @brief Manage the locate LED of physical drives.
 * @details
 * ## Example
 * @code{.c}
 * // Blink the first n physical drive LEDs for s seconds
 * SA_BOOL BlinkFirstNPhysicalDrives(int n, SA_DWORD s)
 * {
 *    SA_BOOL success = kTrue;
 *    void * ledMgmt = SA_GetIdentificationLEDInfo(cda);
 *    if (ledMgmt)
 *    {
 *       SA_WORD pd_number = kInvalidPDNumber;
 *       int i = 0;
 *       while (i < n && (pd_number = SA_EnumerateIdentificationLEDPhysicalDrives(cda, pd_number)) != kInvalidPDNumber)
 *       {
 *          SA_SetPhysicalDriveIdentificationLEDStatus(cda, pd_number, kTrue);
 *          i++;
 *       }
 *       success = SA_WriteIdentificationLEDStatus(cda, s);
 *
 *       SA_DestroyIdentificationLEDInfo(ledMgmt);
 *    }
 *    else
 *    {
 *       fprintf(stderr, "Failed to get LED management object\n");
 *       success = kFalse;
 *    }
 *    return success;
 * }
 * @endcode
 * @{ */
/** Get identification LED information object.
 * @param[in] cda        Controller data area.
 * @return An led information structure that can be used in other functions:
 * - @ref SA_DestroyIdentificationLEDInfo
 * - @ref SA_GetRemainingIdentificationLEDDurationSeconds
 * - @ref SA_EnumerateIdentificationLEDPhysicalDrives
 * - @ref SA_GetPhysicalDriveIdentificationLEDStatus
 * - @ref SA_SetPhysicalDriveIdentificationLEDStatus
 * - @ref SA_WriteIdentificationLEDStatus
*/
void *SA_GetIdentificationLEDInfo(PCDA cda);

/** Destroy identification LED information object.
 * @param[in] led_status_info  Identification LED information object created by @ref SA_GetIdentificationLEDInfo
*/
void SA_DestroyIdentificationLEDInfo(void *led_status_info);

/** Get Remaining identification LED duration.
 * @param[in] led_status_info  Identification LED information object created by @ref SA_GetIdentificationLEDInfo
 * @return The number of seconds remaining in the blink interval. Once the interval expires, all identification LEDs are turned off
*/
SA_DWORD SA_GetRemainingIdentificationLEDDurationSeconds(void *led_status_info);

/** Enumerate Identification LED physical drives.
 * @param[in] led_status_info  Identification LED information object created by @ref SA_GetIdentificationLEDInfo
 * @param[in] pd_number  Last physical drive number returned from this function. When calling for the first time, use kInvalidPDNumber as a value.
 * @return The next physical drive whose identification LED is turned on. If no more drives are in this state, kInvalidPDNumber
*/
SA_WORD SA_EnumerateIdentificationLEDPhysicalDrives(void *led_status_info, SA_WORD pd_number);

/** Get the identification LED status for the given physical drive.
 * @param[in] led_status_info  Identification LED information object created by @ref SA_GetIdentificationLEDInfo
 * @param[in] pd_number  Physical drive number to query
 * @return The remaining duration (in seconds) that the identification LED will remain on. If 0, then the light is not on.
*/
SA_DWORD SA_GetPhysicalDriveIdentificationLEDStatus(void *led_status_info, SA_WORD pd_number);

/** Set the identification LED status for the given physical drive.
 * The changes are not submitted to the controller until SA_SetPhysicalDriveIdentificationLEDStatus is called.
 * @param[in] led_status_info  Identification LED information object created by @ref SA_GetIdentificationLEDInfo
 * @param[in] pd_number        Physical drive number to set
 * @param[in] enable_led       If kTrue, then turn on the LED, if kFalse, then turn off the LED
*/
void SA_SetPhysicalDriveIdentificationLEDStatus(void *led_status_info, SA_WORD pd_number, SA_BOOL enable_led);

/** Write the identification LED status information to the controller.
 * @param[in] led_status_info  Identification LED information object created by @ref SA_GetIdentificationLEDInfo
 * @param[in] duration_sec     Duration (in seconds) for the LED to remain on, in seconds.
 * @return kTrue if command was successfully executed, kFalse otherwise.
*/
SA_BOOL SA_WriteIdentificationLEDStatus(void *led_status_info, SA_DWORD duration_sec);
   #define kIdentificationLEDDurationTurnOff 0   /**< Use this constant to turn off all identification LED */
/** @} */ /* storc_commands_pd_led */

/** @defgroup storc_commands_pd_requery Requery Physical Device(s)
 * @brief Have the controller requery a physical device for updated device info.
 * @{ */
/** Determine if the controller can requery an attached physical device (or device(s)).
 * @allows SA_RequeryPhysicalDevice.
 *
 * @param[in] cda         Controller handler.
 * @param[in] dev_number  Device number of a SEP or physical drive (or @ref kInvalidPDNumber to requery all physical devices).
 * @return See [Can Requery Physical Device Returns](@ref storc_commands_pd_requery)
 *
 * Example
 * -------
 * @code
 * if (SA_FlashDirectPhysicalDrive(cda, 10, kSCSIWriteBufferDownloadAndActivate, buffer, buffer_size)
 *       == kFlashDirectPhysicalDriveSuccess);
 * {
 *   printf("Update to PD %u successful\n");
 *   if (SA_CanRequeryPhysicalDevice(cda, 10) == kCanRequeryPhysicalDeviceAllowed)
 *   {
 *     if (!SA_RequeryPhysicalDevice(cda, 10))
 *     {
 *       printf("Warning: Requery of PD %u failed\n");
 *     }
 *   }
 * }
 * @endcode
 */
SA_BYTE SA_CanRequeryPhysicalDevice(PCDA cda, SA_WORD dev_number);
   /** @name Can Requery Physical Device Returns
    * @outputof SA_CanRequeryPhysicalDevice.
    * @{ */
   /** Requery can be performed on the target device.
    * @outputof SA_CanRequeryPhysicalDevice.
    * @disallows SA_RequeryPhysicalDevice.
    */
   #define kCanRequeryPhysicalDeviceAllowed              1
   /** The controller does not support requerying devices.
    * @outputof SA_CanRequeryPhysicalDevice.
    * @disallows SA_RequeryPhysicalDevice.
    */
   #define kCanRequeryPhysicalDeviceUnsupported          2
   /** The target device is invalid.
    * @outputof SA_CanRequeryPhysicalDevice.
    * @disallows SA_RequeryPhysicalDevice.
    */
   #define kCanRequeryPhysicalDeviceInvalidDevice        3
   /** The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation.
    * @outputof SA_CanRequeryPhysicalDevice.
    * @disallows SA_RequeryPhysicalDevice.
    */
   #define kCanRequeryPhysicalDeviceControllerOFAActive  4
   /** @} */

/** Requery all or any physical device attached to a controller.
 * Controller will requery a device (or all devices) for updated information.
 * This includes inquerys to standard log pages. This is not a rescan of the target.
 * The target remains in the topology; the controller simply updates its metadata
 * with new information from the device.
 *
 * @param[in] cda         Controller handler.
 * @param[in] dev_number  Device number of a SEP or physical drive (or @ref kInvalidPDNumber to requery all physical devices).
 * @return @ref kTrue if the controller successfully requeried the target device(s).
 * @return @ref kFalse if the command to requery is unsupported or failed.
 *
 * Example
 * -------
 * @code
 * if (SA_FlashDirectPhysicalDrive(cda, 10, kSCSIWriteBufferDownloadAndActivate, buffer, buffer_size)
 *       == kFlashDirectPhysicalDriveSuccess);
 * {
 *   printf("Update to PD %u successful\n");
 *   if (SA_CanRequeryPhysicalDevice(cda, 10) == kCanRequeryPhysicalDeviceAllowed)
 *   {
 *     if (!SA_RequeryPhysicalDevice(cda, 10))
 *     {
 *       printf("Warning: Requery of PD %u failed\n");
 *     }
 *   }
 * }
 * @endcode
 */
SA_BOOL SA_RequeryPhysicalDevice(PCDA cda, SA_WORD dev_number);
/** @} */ /* storc_commands_pd_requery */

/** @defgroup storc_commands_pd_disable Disable Physical Drive
 * @brief Disable/fail a physical drive.
 * @{ */

/** Determine if the specified physical drive can be disabled.
 * @allows SA_CanDisablePhysicalDrive.
 *
 * @param[in] cda         Controller handler.
 * @param[in] dev_number  Device number of the physical drive to be disabled.
 * @return See [Can Disable Physical Drive Returns](@ref storc_commands_pd_disable)
 */
SA_BYTE SA_CanDisablePhysicalDrive(PCDA cda, SA_WORD dev_number);
   /** @name Can Disable Physical Drive Returns
   * @outputof SA_CanDisablePhysicalDrive
   * @{ */
   /** The target physical drive can be disabled.
    * @outputof SA_CanDisablePhysicalDrive
    * @allows SA_DoDisablePhysicalDrive
    */
   #define kCanDisablePhysicalDriveAllowed              0
   /** The target physical drive cannot be disabled. The drive number is not valid, or the device is not a physical drive.
    * @outputof SA_CanDisablePhysicalDrive
    * @disallows SA_DoDisablePhysicalDrive
    */
   #define kCanDisablePhysicalDriveInvalidDevice        1
   /** The target physical drive cannot be disabled. The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanDisablePhysicalDrive
    * @disallows SA_DoDisablePhysicalDrive
    */
   #define kCanDisablePhysicalDriveControllerOFAActive  2
   /** @} */

/** Disables(Fail) a physical drive.
 * After this command completes the selected physical drive is
 *  no longer recognized by the controller.
 *
 * @param[in] cda         Controller data area
 * @param[in] dev_number  Physical drive number of the physical drive to fail.
 * @return @ref kTrue   if the controller successfully failed the target physical drive.
 * @return @ref kFalse  if the command to failed.
 */
SA_BOOL SA_DoDisablePhysicalDrive(PCDA cda, SA_WORD dev_number);
/** @} */ /* storc_commands_pd_disable */

/** @defgroup storc_commands_refresh_physical_drives_security_status Refresh Physical Drive Security Status
 * @brief Refresh the security status of all physical drives attached to the controller.
 * @{ */

 /** Can the security status of all physical drives connected to the controller be refreshed?
 * @post If allowed, user may call @ref SA_DoRefreshAllPhysicalDrivesSecurityStatus.
 * @param[in] cda            Controller data area.
 * @return See [Can Refresh All Physical Drives Security Status Returns](@ref storc_commands_refresh_physical_drives_security_status).
 */
SA_BYTE SA_CanRefreshAllPhysicalDrivesSecurityStatus(PCDA cda);
/** @name Can Refresh All Physical Drives Security Status Returns
 * @outputof SA_CanRefreshAllPhysicalDrivesSecurityStatus.
 * @{ */
/** The security status of physical drives attached to the controller can be refreshed.
 * @outputof SA_CanRefreshAllPhysicalDrivesSecurityStatus.
 * @allows SA_DoRefreshAllPhysicalDrivesSecurityStatus.
 */
#define kCanRefreshAllPhysicalDrivesSecurityStatusOK                  0x00
/** The command is not supported by the controller.
 * @outputof SA_CanRefreshAllPhysicalDrivesSecurityStatus.
 * @disallows SA_DoRefreshAllPhysicalDrivesSecurityStatus.
 */
#define kCanRefreshAllPhysicalDrivesSecurityStatusCommandNotSupported 0x01
/** Online Firmware Activation is running.
 * @outputof SA_CanRefreshAllPhysicalDrivesSecurityStatus.
 * @disallows SA_DoRefreshAllPhysicalDrivesSecurityStatus.
 */
#define kCanRefreshAllPhysicalDrivesSecurityStatusOFARunning          0x02
 /** There are no SEDs attached to the controller.
 * @outputof SA_CanRefreshAllPhysicalDrivesSecurityStatus.
 * @disallows SA_DoRefreshAllPhysicalDrivesSecurityStatus.
 */
#define kCanRefreshAllPhysicalDrivesSecurityStatusNoSEDs              0x03
/** @} */ /* Can Refresh All Physical Drives Security Status Returns */

/** Refresh the security status of all physical drives connected to the controller.
 * @pre The user has called @ref SA_CanRefreshAllPhysicalDrivesSecurityStatus to ensure the operation can be performed.
 * @param[in] cda            Controller data area.
 * @return @ref kTrue if the security status was successfully refreshed, @ref kFalse otherwise.
 */
SA_BOOL SA_DoRefreshAllPhysicalDrivesSecurityStatus(PCDA cda);

/** Can the security status of a single physical drive be refreshed?
 * @post If allowed, user may call @ref SA_DoRefreshPhysicalDriveSecurityStatus.
 * @param[in] cda            Controller data area.
 * @param[in] pd_number      Target physical drive number.
 * @return See [Can Refresh Physical Drive Security Status Returns](@ref storc_commands_refresh_physical_drives_security_status).
 */
SA_BYTE SA_CanRefreshPhysicalDriveSecurityStatus(PCDA cda, SA_WORD pd_number);
/** @name Can Refresh Physical Drive Security Status Returns
 * @outputof SA_CanRefreshPhysicalDriveSecurityStatus.
 * @{ */
/** The security status of the physical drive can be refreshed.
 * @outputof SA_CanRefreshPhysicalDriveSecurityStatus.
 * @allows SA_DoRefreshPhysicalDriveSecurityStatus.
 */
#define kCanRefreshPhysicalDriveSecurityStatusOK                  0x00
/** The target physical drive number is invalid or is not a physical drive.
 * @outputof SA_CanRefreshPhysicalDriveSecurityStatus.
 * @disallows SA_DoRefreshPhysicalDriveSecurityStatus.
 */
#define kCanRefreshPhysicalDriveSecurityStatusInvalidDevice       0x01
/** The command is not supported by the controller
 * @outputof SA_CanRefreshPhysicalDriveSecurityStatus.
 * @disallows SA_DoRefreshPhysicalDriveSecurityStatus.
 */
#define kCanRefreshPhysicalDriveSecurityStatusCommandNotSupported 0x02
/** Online Firmware Activate is running.
 * @outputof SA_CanRefreshPhysicalDriveSecurityStatus.
 * @disallows SA_DoRefreshPhysicalDriveSecurityStatus.
 */
#define kCanRefreshPhysicalDriveSecurityStatusOFARunning          0x03
/** The target physical drive is not a SED.
 * @outputof SA_CanRefreshPhysicalDriveSecurityStatus.
 * @disallows SA_DoRefreshPhysicalDriveSecurityStatus.
 */
#define kCanRefreshPhysicalDriveSecurityStatusNotSED              0x04
/* @} */ /* Can Refresh Physical Drive Security Status Returns */

/** Refresh the security status of a single physical drive.
 * @pre The user has called @ref SA_CanRefreshPhysicalDriveSecurityStatus to ensure the operation can be performed.
 * @param[in] cda            Controller data area.
 * @param[in] pd_number      Target physical drive number.
 * @return @ref kTrue if the security status was successfully refreshed, @ref kFalse otherwise.
 */
SA_BOOL SA_DoRefreshPhysicalDriveSecurityStatus(PCDA cda, SA_WORD pd_number);

/** @} */ /* storc_commands_refresh_physical_drives_security_status */

/** @defgroup storc_commands_pd_reset_mnp_statistics Reset Physical Drive Monitor and Performance Statistics
 * @brief Reset or erase the monitor and performance statistics of a physical drive.
 * @{ */
/** @name MNP Statistics Reset Types
 * @{ */
#define kMNPStatisticsResetTypeFactoryDefault   1  /**< Reset the physical drive's MNP statistics to factory default. */
#define kMNPStatisticsResetTypeErase            2  /**< Completely erase the physical drive's MNP stamp/data (including drive error log). */
/** @} */ /* MNP Statistics Reset Types */
/** Resets a physical drive's monitor and performance statistics.
 * @pre The user has used @ref SA_CanResetPhysicalDriveMNPStatistics to ensure the operation can be performed.
 * @param[in] cda         Controller data area.
 * @param[in] pd_number   Target physical drive number.
 * @param[in] reset_type  Determine what type of reset or erasure of MNP statistics to perform on the physical drive ([MNP Statistics Reset Types](@ref storc_properties_pd_mnp_statistics)).
 * @return kTrue if command was successfully executed, kFalse otherwise.
 */
SA_BOOL SA_DoResetPhysicalDriveMNPStatistics(PCDA cda, SA_WORD pd_number, SA_BYTE reset_type);
/** Determine if a physical drive's monitor and performance statistics can be reset or erased.
 * @param[in] cda         Controller data area.
 * @param[in] pd_number   Target physical drive number.
 * @param[in] reset_type  Determine what type of reset or erasure of MNP statistics to perform on the physical drive ([MNP Statistics Reset Types](@ref storc_properties_pd_mnp_statistics)).
 * @return [Can Refresh Physical Drive Security Status Returns](@ref storc_commands_pd_reset_mnp_statistics)
 */
SA_BYTE SA_CanResetPhysicalDriveMNPStatistics(PCDA cda, SA_WORD pd_number, SA_BYTE reset_type);
/** @name Can Reset Physical Drive Monitor and Performance Statistics Returns
 * @outputof SA_CanResetPhysicalDriveMNPStatistics.
 * @allows SA_DoResetPhysicalDriveMNPStatistics.
 * @{ */
/** The monitor and performance statistics of the physical drive can be reset or erased.
 * @outputof SA_CanResetPhysicalDriveMNPStatistics.
 * @allows SA_DoResetPhysicalDriveMNPStatistics.
 */
#define kCanResetPhysicalDriveMNPStatisticsAllowed          1
/** The input to the function is invalid.
 * @outputof SA_CanResetPhysicalDriveMNPStatistics.
 * @disallows SA_DoResetPhysicalDriveMNPStatistics.
 */
#define kCanResetPhysicalDriveMNPStatisticsInvalidResetType 2
/** The specified physical drive number is not supported for resetting or erasing the monitor and performance statistics.
 * @outputof SA_CanResetPhysicalDriveMNPStatistics.
 * @disallows SA_DoResetPhysicalDriveMNPStatistics.
 */
#define kCanResetPhysicalDriveMNPStatisticsUnsupportedPD    3
/** The specified physical drive number is not in an OK state.
 * @outputof SA_CanResetPhysicalDriveMNPStatistics.
 * @disallows SA_DoResetPhysicalDriveMNPStatistics.
 */
#define kCanResetPhysicalDriveMNPStatisticsNotOKDriveState  4
/** @} */

/** @} */ /* storc_commands_pd_reset_mnp_statistics */

/** @} */ /* storc_commands_pd */

/*********************************************************//**
 * @defgroup storc_commands_pdevice Physical Device Commands
 * @brief Special commands for physical devices like SEPs/Expanders attached to a controller.
 * @details
 * @{
**********************************************************/

/** @defgroup storc_commands_pdevice_smp Physical Device SMP Commands
 * @brief Send an SMP command to a device.
 * @details
 * ## Send SMP Report Manufacturer Information to Expander Example
 * @code
 * if (SA_CanSendPhysicalDeviceSMPCommand(cda, expander_number))
 * {
 *    SA_BYTE response_buffer[1024] = {0};
 *    SMPRequest smp_request;
 *    memset(&smp_request, 0, sizeof(SMPRequest));
 *    smp_request.header_frame_type = 0x40;
 *    smp_request.header_function = 1; // SMP Report Manufacturer Information
 *    smp_request.header_allocation_response_size = 60;
 *    smp_request.header_request_length = 0;
 *    smp_request.additional_request_data = response_buffer;
 *    smp_request.additional_request_data_size = 4;
 *    smp_request.additional_response_data = response_buffer;
 *    smp_request.additional_response_data_size = 60;
 *
 *    if (SA_SendPhysicalDeviceSMPCommand(cda, expander_number, &smp_request))
 *    {
 *       char smp_model[25] = { 0 };
 *       char smp_fw_version[5] = { 0 };
 *       SA_memcpy_s(smp_model, sizeof(smp_model), &smp_request.additional_response_data[8], sizeof(smp_model)-1);
 *       SA_memcpy_s(smp_fw_version,sizeof(smp_fw_version), &smp_request.additional_response_data[32], sizeof(smp_fw_version)-1);
 *       printf("Expander Model: %s\n", smp_model);
 *       printf("Expander FW:    %s\n", smp_fw_version);
 *    }
 *    else
 *    {
 *       printf("SMP request failed.\n");
 *    }
 * }
 * else
 * {
 *    printf("Not allowed on this device.\n");
 * }
 * @endcode
 * @{ */

/** Can a SMP passthrough command be sent to the physical device?
 * @param[in] cda            Controller data area.
 * @param[in] device_number  Number of target physical device. This must be an expander device.
 * @return @ref kTrue/@ref kFalse if it is possible to send SMP commands to the physical device.
 */
SA_BOOL SA_CanSendPhysicalDeviceSMPCommand(PCDA cda, SA_WORD device_number);

/** Send a SMP passthrough command to the designated physical device.
 * @pre The user has used @ref SA_CanSendPhysicalDeviceSMPCommand to ensure the operation can be performed.
 * @param[in]     cda            Controller data area.
 * @param[in]     device_number  Number of target physical device. This must be an expander device.
 * @param[in,out] smp_request    Pointer to a [SMP request structure](@ref syscore_commands_smp).
 * Clients must populate all required fields except for the sas_address.
 * @return @ref kTrue/@ref kFalse if it the command successfully executed on the target device.
 */
SA_BOOL SA_SendPhysicalDeviceSMPCommand(PCDA cda, SA_WORD device_number, PSMPRequest smp_request);
/** @} */ /* storc_commands_pdevice_smp */

/** @} */ /* storc_commands_pdevice */

/*********************************************************//**
 * @defgroup storc_commands_ld Logical Drive Commands
 * @brief Special commands for a logical drive.
 * @details
 * @{
**********************************************************/

/** @defgroup storc_commands_ld_enable_failed_ld Enable Failed Logical Drive
 * @brief Re-enable a failed logical drive after replacing failed physical drives.
 * @warning Accepts data loss since the logical drive failed.
 * @{ */
/** Check if targeted logical drive can be enabled.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Number of target logical drive.
 * @return See [Can Enable Failed Logical Drive Returns](@ref storc_commands_ld_enable_failed_ld).
 */
SA_BYTE SA_CanEnableFailedLogicalDrive(PCDA cda, SA_WORD ld_number);
/** @name Can Enable Failed Logical Drive Returns
 * @outputof SA_CanEnableFailedLogicalDrive.
 * @{ */
   /** Logical drive can be enabled.
    * @outputof SA_CanEnableFailedLogicalDrive.
    * @allows SA_DoEnableFailedLogicalDrive.
    */
   #define kCanEnableFailedLogicalDriveAllowed                 0x01
   /** Logical drive is not failed.
    * @outputof SA_CanEnableFailedLogicalDrive.
    * @disallows SA_DoEnableFailedLogicalDrive.
    */
   #define kCanEnableFailedLogicalDriveIsNotFailed             0x02
   /** Logical drive is a mirror backup that cannot be re-enabled.
    * @outputof SA_CanEnableFailedLogicalDrive.
    * @disallows SA_DoEnableFailedLogicalDrive.
    */
   #define kCanEnableFailedLogicalDriveIsMirrorBackup          0x03
   /** Logical drive has invalid array status.
    * @outputof SA_CanEnableFailedLogicalDrive.
    * @disallows SA_DoEnableFailedLogicalDrive.
    */
   #define kCanEnableFailedLogicalDriveInvalidArrayStatus      0x04
   /** Logical drive has failed physical drive that has not been replaced.
    * @outputof SA_CanEnableFailedLogicalDrive.
    * @disallows SA_DoEnableFailedLogicalDrive.
    */
   #define kCanEnableFailedLogicalDriveFailedDriveNotReplaced  0x05
   /** Logical drive's array is undergoing Rapid Parity Initializiation (RPI).
    * @outputof SA_CanEnableFailedLogicalDrive.
    * @disallows SA_DoEnableFailedLogicalDrive.
    */
   #define kCanEnableFailedLogicalDriveArrayUndergoingRPI      0x06
   /** Logical drive is encrypted and controller is encryption locked.
    * @outputof SA_CanEnableFailedLogicalDrive.
    * @disallows SA_DoEnableFailedLogicalDrive.
    */
   #define kCanEnableFailedLogicalDriveEncryptionLocked        0x07
   /** Controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanEnableFailedLogicalDrive.
    * @disallows SA_DoEnableFailedLogicalDrive.
    */
   #define kCanEnableFailedLogicalDriveControllerOFAActive     0x08
/** @} */

/** Enable targeted logical drive.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Number of target logical drive.
 * @return @ref kTrue if logical drive was enabled, @ref kFalse otherwise.
 */
SA_BOOL SA_DoEnableFailedLogicalDrive(PCDA cda, SA_WORD ld_number);

/** @} */ /* storc_commands_ld_enable_failed_ld */

/** @} */ /* storc_commands_ld */

#if defined(__cplusplus)
}
#endif

/** @} */ /* storc_commands */

/** @} */ /* storcore */

#endif /* PSTORC_COMMANDS_H */
