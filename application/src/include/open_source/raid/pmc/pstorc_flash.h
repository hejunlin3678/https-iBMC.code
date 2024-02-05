/** @file pstorc_flash.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore flash header file
   Defines functions for updating device firmware.

*/

#ifndef INC_PSTORC_FLASH_H_
#define INC_PSTORC_FLASH_H_

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/************************************//**
 * @defgroup storc_flash Flash
 * @brief Update device firmware.
 * @details
 * @{
 ***************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/** @defgroup storc_flash_ctrl Flash Controller
 * @brief Update controller firmware.
 * @details
 * @{
 */

/** @name Validate Controller Image
 * @{ */
/** Perform a validation on the provided controller image.
 * @pre `image` & `image_size` point to a valid controller firmware image in memory.
 * @warning Do not call this API while performing any other flash operation(s).
 * @param[in]  cda         Controller data area.
 * @param[in]  image       Address of firmware image.
 * @param[in]  image_size  Size (in bytes) of `image`.
 * @return See [Validate Controller Image Returns](@ref storc_flash_ctrl).
 */
SA_BYTE SA_ValidateControllerImage(PCDA cda, void* image, SA_UINT32 image_size);
    /** Image passed validation.
     * @outputof SA_ValidateControllerImage.
     */
    #define kValidateControllerImageOK                  1
    /** Image failed validation.
     * @outputof SA_ValidateControllerImage.
     */
    #define kValidateControllerImageFailed              2
    /** Cannot validate the provided image.
     * @outputof SA_ValidateControllerImage.
     */
    #define kValidateControllerImageCannotValidateImage 3
/** @} */

/** @name Can Flash Controller
 * @{ */
/** Can flash the controller.
 * @post If valid, user may call @ref SA_FlashController.
 * @deprecated `image_size` parameter which is ignored.
 * @param[in]  cda                Controller data area.
 * @param[in]  image_size         deprecated.
 * @return See [Can Flash Controller Returns](@ref storc_flash_ctrl).
 *
 * __Example__
 * @code
 * if (SA_CanFlashController(cda, 0) == kCanFlashControllerOK)
 * {
 *    if (SA_FlashController(cda, image_addr, 0) != kFlashControllerSuccess)
 *    {
 *        fprintf(stderr, "Failed flash controller\n");
 *    }
 * }
 * @endcode
 */
SA_WORD SA_CanFlashController(PCDA cda, SA_DWORD image_size);
   /** Drive flash with given parameters allowed.
    * @outputof SA_CanFlashController.
    * @allows SA_FlashController.
    */
   #define kCanFlashControllerOK                       0x0000
   /** Given image size is not valid or missing.
    * @deprecated Not longer used as `image_size` is no longer validated.
    */
   #define kCanFlashControllerInvalidImageSize         0x0001
   /** The target controller does not support BMIC commands.
    * @outputof SA_CanFlashController.
    * @disallows SA_FlashController.
    */
   #define kCanFlashControllerBMICCommandNotSupported  0x0002
   /** The target controller's firmware is locked.
    * @outputof SA_CanFlashController.
    * @disallows SA_FlashController.
    */
   #define kCanFlashControllerFWLocked                 0x0004
   /** The target controller as a pending or running online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanFlashController.
    * @disallows SA_FlashController.
    */
   #define kCanFlashControllerOFAActive                0x0008
/** @} */


/** @name Controller Flash
 * @{ */
/** Callback signature for @ref SA_FlashController. */
typedef void(*FlashProgressCallback)(SA_BYTE);
/** Flash the controller.
 * @pre The user has used @ref SA_CanFlashController to ensure the operation can be performed.
 * @param[in]  cda                Controller data area.
 * @param[in]  image              Address of firmware image.
 * @param[in]  image_size         Size (in bytes) of `image`.
 * @param[in]  progress_callback  Address to a function of type @ref FlashProgressCallback to receive live flash progress (optional).
 * @param[in]  progress_callback    The callback will be called repeatedly with an increasing value between 0-100 showing overall flash progress.
 * @param[in]  progress_callback    Can be NULL to disable flash progress reporting.
 * @return See [Controller Flash Returns](@ref storc_flash_ctrl).
 *
 * __Example__
 * @code
 * void ReportFlashProgressPercent(SA_BYTE percent)
 * {
 *    printf("Flash Progress: %d%%\n", percent);
 * }
 * @endcode
 * @code
 * // Validate the image
 * if (SA_ValidateControllerImage(cda, image_addr, image_size) == kValidateControllerImageOK)
 * {
 *    if (SA_CanFlashController(cda, image_size) == kCanFlashControllerOK)
 *    {
 *       if (SA_FlashController(cda, image_addr, image_size, ReportFlashProgressPercent) != kFlashControllerSuccess)
 *       {
 *           fprintf(stderr, "Failed flash controller\n");
 *       }
 *       else if (SA_ControllerFlashOnce(cda) == kFalse)
 *       {
 *           // Need to flash the controller a second time.
 *           if (SA_FlashController(cda, image_addr, image_size, ReportFlashProgressPercent) != kFlashControllerSuccess)
 *           {
 *               fprintf(stderr, "Failed flash controller (second flash)\n");
 *           }
 *       }
 *    }
 * }
 * else
 * {
 *    fprintf(stderr, "Firmware image failed to pass validation.")
 * }
 * @endcode
 */
SA_BYTE SA_FlashController(PCDA cda, SA_BYTE * image, SA_DWORD image_size, FlashProgressCallback progress_callback);
   #define kFlashControllerFailed                0 /**< Flash command(s) did __NOT__ complete successfully; @outputof SA_FlashController. */
   #define kFlashControllerSuccess               1 /**< Flash command(s) completed successfully; @outputof SA_FlashController. */
/** @} */

/** @name Online Firmware Activation Validation
 * @{ */

/** Check if firmware can be validated for online firmware activation.
 * Runs a subset of checks performed in @ref SA_ValidateControllerOnlineFlash/@ref SA_ValidateControllerPendingImageForOnlineFlash
 * without sending any commands to the controller.
 * @post If allowed, user may call @ref SA_ValidateControllerOnlineFlash.
 * @post If allowed, user may call @ref SA_ValidateControllerPendingImageForOnlineFlash.
 * @return @ref kControllerOnlineFirmwareActivationValidateOk if user may perform @ref SA_ValidateControllerOnlineFlash.
 * @return @ref kControllerOnlineFirmwareActivationValidateOk if user may perform @ref SA_ValidateControllerPendingImageForOnlineFlash.
 * @return Returns a subset of the return codes used by [SA_ValidateControllerOnlineFlash](@ref storc_flash_ctrl).
 */
SA_UINT64 SA_CanValidateControllerOnlineFlash(PCDA cda);

/** Check if any pending firmware image is valid to perform an Online Firmware Activation.
 * This validates that a previously queued (pending) firmware image can be activated without a reboot.
 * @pre User has used @ref SA_CanValidateControllerOnlineFlash to ensure the operation can be performed.
 * @pre @ref SA_ControllerSupportsOnlineFirmwareActivation reports both 'supported' and 'enabled'.
 * @pre The client as queued a pending firmware image via @ref SA_FlashController.
 * @warning Do not call this API while performing any other flash operation(s).
 * @param[in]  cda         Controller data area.
 * @return @ref kControllerOnlineFirmwareActivationValidateOk if the controller and image can be online flashed.
 * @return Bitmap listing reasons why the controller and/or image cannot be flashed online.
 * @return Same as @ref SA_ValidateControllerOnlineFlash.
 */
SA_UINT64 SA_ValidateControllerPendingImageForOnlineFlash(PCDA cda);


/** Check if a given firmware image can be flash online via Online Firmware Activation.
 * This validates that a firmware image can be activated without a reboot (without queuing the firmware image to the controller).
 * @pre User has used @ref SA_CanValidateControllerOnlineFlash to ensure the operation can be performed.
 * @pre @ref SA_ControllerSupportsOnlineFirmwareActivation reports both 'supported' and 'enabled'.
 * @pre `image` & `image_size` point to a valid controller firmware image in memory.
 * @warning Do not call this API while performing any other flash operation(s).
 * @param[in]  cda         Controller data area.
 * @param[in]  image       Address of firmware image.
 * @param[in]  image_size  Size (in bytes) of `image`.
 * @return @ref kControllerOnlineFirmwareActivationValidateOk if the controller and image can be online flashed.
 * @return Bitmap listing reasons why the controller and/or image cannot be flashed online.
 * @return Same as @ref SA_ValidateControllerPendingImageForOnlineFlash.
 */
SA_UINT64 SA_ValidateControllerOnlineFlash(PCDA cda, void *image, SA_UINT32 image_size);
   /** The controller and provided image are capable of online firmware activation.
    * @outputof SA_CanValidateControllerOnlineFlash
    * @outputof SA_CanInitiateControllerOnlineFlash
    * @outputof SA_ValidateControllerOnlineFlash
    * @outputof SA_InitiateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateOk                         0ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because an unknown/unexpected error occurred.
    * @outputof SA_ValidateControllerOnlineFlash
    * @outputof SA_InitiateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateUnknownError               0x0000000000000001ull
   /** The controller is _not_ capable of online firmware activation
    * because @ref SA_CanFlashController does not report the controller as flashable.
    * @see SA_CanFlashController.
    * @outputof SA_CanValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateCannotFlash                0x0000000000000002ull
   /** The controller is _not_ capable of online firmware activation
    * because it online firmware activation is not supported by the controller.
    * @outputof SA_CanValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateNotSupported               0x0000000000000004ull
   /** The controller is _not_ capable of online firmware activation
    * because the required driver is not loaded.
    * @outputof SA_CanValidateControllerOnlineFlash
    * @outputof SA_ValidateControllerOnlineFlash*/
   #define kControllerOnlineFirmwareActivationValidateNotEnabled                 0x0000000000000008ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because of the controller's physical configuration.
    * @outputof SA_ValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateUnsupportedPhysicalConfig  0x0000000000000010ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because of the controller's logical configuration.
    * @outputof SA_ValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateUnsupportedLogicalConfig   0x0000000000000020ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because of an enabled feature on the controller.
    * @outputof SA_ValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateUnsupportedFeature         0x0000000000000040ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because of a current controller state/status.
    * @outputof SA_ValidateControllerOnlineFlash*/
   #define kControllerOnlineFirmwareActivationValidateProhibitedControllerState  0x0000000000000080ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because an outstanding operation cannot be paused/stopped.
    * @outputof SA_ValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateUninterruptibleProcess     0x0000000000000100ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because the image contains an incompatible header version.
    * @outputof SA_ValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateIncompatibleHeaderVersion  0x0000000000000200ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because 1 one or more image component would require a reboot.
    * @outputof SA_ValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateComponentRequiresReboot    0x0000000000000400ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because the image is incompatible for online activation. */
   #define kControllerOnlineFirmwareActivationValidateIncompatible               0x0000000000000800ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because of an unidentified reason.
    * @outputof SA_ValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateUndefined                  0x0000000000001000ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because online firmware activation is currently in progress.
    * @outputof SA_CanValidateControllerOnlineFlash
    * @outputof SA_CanInitiateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateAlreadyRunning             0x0000000000002000ull
   /** The controller and/or provided image are _not_ capable of online firmware activation
    * because the internal activation manager encountered an unexpected error.
    * @outputof SA_ValidateControllerOnlineFlash() */
   #define kControllerOnlineFirmwareActivationValidateFailedInit                 0x0000000000004000ull
   /** The controller is _not_ capable of online firmware activation
    * because the controller is not capable of running an online activation operation.
    * @outputof SA_ValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateNotCapable                 0x0000000000008000ull
   /** The image is _not_ capable of online firmware activation
    * because the image does not have a valid header.
    * @outputof SA_ValidateControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationValidateInvalidHeader              0x0000000000010000ull
/** @} */ /* Online Firmware Activation Validation */

/** @name Online Firmware Activation Initiate
 * @{ */

/** Check if Online Firmware Activation can be initiated.
 * Runs a subset of checks performed in @ref SA_InitiateControllerOnlineFlash
 * without sending any commands to the controller.
 * @post If allowed, user may call @ref SA_InitiateControllerOnlineFlash.
 * @return @ref kControllerOnlineFirmwareActivationValidateOk if user may perform @ref SA_InitiateControllerOnlineFlash.
 * @return Returns a subset of the return codes used by [SA_InitiateControllerOnlineFlash](@ref storc_flash_ctrl).
 */
SA_UINT64 SA_CanInitiateControllerOnlineFlash(PCDA cda);

/** Perform an Online Firmware Activation initiate to reset the controller and activate the new firmware.
 * @pre User has used @ref SA_CanInitiateControllerOnlineFlash to ensure the operation can be performed.
 * @pre The client as queued a pending firmware image via @ref SA_FlashController.
 * @pre @ref SA_ControllerSupportsOnlineFirmwareActivation reports both 'supported' and 'enabled'.
 * @post The controller may reset immediately after running this API. Most operations will be unavailable until the controller returns from a reset.
 * @warning Do not call this API while performing any other flash operation(s).
 * @warning Most operations will be unavailable while a controller performs an Online Firmware Activation.
 * @warning The Online Firmware Activation cannot be aborted once the delay expires and the process starts.
 * @param[in]  cda            Controller data area.
 * @param[in]  delay_minutes  Schedule the controller reset by delaying the initiate for the specified minutes.
 * @return @ref kControllerOnlineFirmwareActivationValidateOk if the controller as started or scheduled the Online Firmware Activation.
 * @return Bitmap listing reasons why the controller and/or image could not be activated.
 * @return Same as @ref SA_ValidateControllerOnlineFlash.
 */
SA_UINT64 SA_InitiateControllerOnlineFlash(PCDA cda, SA_UINT16 delay_minutes);

/** @} */ /* Online Firmware Activation Initiate */

/** @name Online Firmware Activation Abort
 * @{ */

/** Check if Online Firmware Activation can be aborted.
 * Runs a subset of checks performed in @ref SA_AbortControllerOnlineFlash
 * without sending any commands to the controller.
 * @post If allowed, user may call @ref SA_AbortControllerOnlineFlash.
 * @return @ref kControllerOnlineFirmwareActivationAbortSuccess if user may perform @ref SA_AbortControllerOnlineFlash.
 * @return Uses same return codes as [SA_AbortControllerOnlineFlash](@ref storc_flash_ctrl).
 */
SA_UINT32 SA_CanAbortControllerOnlineFlash(PCDA cda);

/** Abort a scheduled Online Firmware Activation operation.
 * @pre User has used @ref SA_CanAbortControllerOnlineFlash to ensure the operation can be performed.
 * @pre The client has scheduled an Online Firmware Activation with @ref SA_InitiateControllerOnlineFlash.
 * @pre @ref SA_ControllerSupportsOnlineFirmwareActivation reports both 'supported' and 'enabled'.
 * @warning Do not call this API while performing any other flash operation(s).
 * @param[in]  cda  Controller data area.
 * @return A list of failure reasons (see [Online Firmware Activation Abort](@ref storc_flash_ctrl)) or
 * @ref kControllerOnlineFirmwareActivationAbortSuccess if the controller successfully aborted a scheduled Online Firmware Activation.
 */
SA_UINT32 SA_AbortControllerOnlineFlash(PCDA cda);
   /** Successfully aborted the outstanding Online Firmware Activation process/operation.
    * @outputof SA_AbortControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationAbortSuccess       0
   /** Cannot abort the Online Firmware Activation because it is not supported on this controller.
    * @outputof SA_AbortControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationAbortNotSupported  1
   /** Failed to abort the Online Firmware Activation because the required driver is not loaded.
    * @outputof SA_AbortControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationAbortNotEnabled    2
   /** Cannot abort the Online Firmware Activation because there is no Online Firmware Activation process running or
    * the OFA process cannot be aborted.
    * @outputof SA_AbortControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationAbortNotRunning    3
   /** Attempted but failed to abort the Online Firmware Activation.
    * @outputof SA_AbortControllerOnlineFlash */
   #define kControllerOnlineFirmwareActivationAbortFailed        4

/** @} */ /* Online Firmware Activation Abort */

/** @name Controller Flash Once
 * @{ */

/** Check if the controller should be flashed only once.
 * @return If @ref kTrue, then the controller should only be flashed once (see @ref SA_FlashController).
 * If @ref kFalse, then the controller should be flashed twice.
 */
SA_BOOL SA_ControllerFlashOnce(PCDA cda);

/** @} */ /* Controller Flash Once */

/** @name Toggle Active ROM Image
 * @{ */

/** Check if the Active ROM image can be toggled from the currently active image to the backup image.
 * @post If allowed, user may call @ref SA_ToggleControllerActiveROMImage.
 * @param[in]  cda Controller data area.
 * @return A list of failure reasons (see [SA_CanToggleControllerActiveROMImage Returns](@ref storc_flash_ctrl)) or
 * @ref kCanToggleControllerActiveROMImageOK if a user may perform @ref SA_ToggleControllerActiveROMImage.
 */
SA_BYTE SA_CanToggleControllerActiveROMImage(PCDA cda);
   /** @name SA_CanToggleControllerActiveROMImage Returns
    * @{ */
   /** Toggle controller active ROM image allowed.
    * @outputof SA_CanToggleControllerActiveROMImage.
    * @allows SA_ToggleControllerActiveROMImage.
    */
   #define kCanToggleControllerActiveROMImageOK                   0x00
   /** Controller does not support toggling the active ROM image.
    * @outputof SA_CanToggleControllerActiveROMImage.
    * @disallows SA_ToggleControllerActiveROMImage.
    */
   #define kCanToggleControllerActiveROMImageNotSupported         0x01
   /** Controller is not in an OK state.
    * @outputof SA_CanToggleControllerActiveROMImage.
    * @disallows SA_ToggleControllerActiveROMImage.
    */
   #define kCanToggleControllerActiveROMImageControllerStateNotOK 0x02
   /** Online firmware activation (OFA) is in progress.
    * @outputof SA_CanToggleControllerActiveROMImage.
    * @disallows SA_ToggleControllerActiveROMImage.
    */
   #define kCanToggleControllerActiveROMImageOFAInProgress        0x03
   /** Controller firmware is locked.
    * @outputof SA_CanToggleControllerActiveROMImage.
    * @disallows SA_ToggleControllerActiveROMImage.
    */
   #define kCanToggleControllerActiveROMImageFirmwareIsLocked     0x04
   /** @} */ /* SA_CanToggleControllerActiveROMImage Returns */

/** Toggle the active ROM image to the backup image.
 * @pre User has used @ref SA_CanToggleControllerActiveROMImage to ensure the operation can be performed.
 * @param[in]  cda Controller data area.
 * @return See [SA_ToggleControllerActiveROMImage Returns](@ref storc_flash_ctrl).
 */
SA_BYTE SA_ToggleControllerActiveROMImage(PCDA cda);
   /** @name SA_ToggleControllerActiveROMImage Returns
    * @{ */
   #define kToggleControllerActiveROMImageFailed   0x00  /**< Toggle active ROM Image did __NOT__ complete successfully; @outputof SA_ToggleControllerActiveROMImage. */
   #define kToggleControllerActiveROMImageSuccess  0x01  /**< Toggle active ROM Image completed successfully; @outputof SA_ToggleControllerActiveROMImage. */
   /** @} */ /* SA_ToggleControllerActiveROMImage Returns */
/** @} */ /* Toggle Active ROM Image */

/** @} */ /* storc_flash_ctrl */

/** @name Supported SCSI WRITE BUFFER modes
 * Input to:
 *  - @ref SA_CanFlashDirectPhysicalDrive
 *  - @ref SA_FlashDirectPhysicalDrive
 *  - @ref SA_CanFlashDeferredPhysicalDrive
 *  - @ref SA_FlashDeferredPhysicalDrive
 *  - @ref SA_CanFlashDirectSEP
 *  - @ref SA_FlashDirectSEP
 *
 * Mode                                                    | Flash Method    | Transfer Size | Image/Size
 * ------------------------------------------------------- | --------------- | ------------- | ----------
 * @ref kSCSIWriteBufferDownloadAndActivate                | Direct/Deferred | Ignored       | Required
 * @ref kSCSIWriteBufferDownloadWithOffsetsAndActivate     | Direct/Deferred | Required      | Required
 * @ref kSCSIWriteBufferDownloadWithOffsetsAndDefer        | Direct/Deferred | Required      | Required
 * @ref kSCSIWriteBufferActivateSavedFirmware              | Direct Only     | Ignored       | Ignored
 * @ref kSCSIWriteBufferSaveWithOffsetsAndActivateFirmware | Direct Only     | Required      | Required
 *
 * @{ */
/** Download/activate FW image on the device using 1 large transfer.
 * Input to @ref SA_FlashDirectPhysicalDrive and @ref SA_FlashDeferredPhysicalDrive.
 *
 * Using this mode requires the image be small enough to be
 * fully addressed in a single SCSI command.
 *
 * This mode is more reliable for SATA drives since intermixed I/O between
 * WRITE BUFFER commands will invalidate the FW image however, SATA drives
 * usually cannot completely address modern FW images.
 *
 * Usually after the command completes the new FW takes effect immediately,
 * which may not have been safe for the drive; therefore, it is recommended
 * to use this mode only if user I/O has been quiesced, in an offline environment,
 * or when doing a [deferred flash](@ref SA_FlashDeferredPhysicalDrive).
 */
#define kSCSIWriteBufferDownloadAndActivate                0x0005
/** Download/activate FW image on the device using mutliple transfers.
 * Input to @ref SA_FlashDirectPhysicalDrive and @ref SA_FlashDeferredPhysicalDrive.
 *
 * Using this mode requires the transfer size be small enough to be
 * fully addressed in a SCSI command.
 *
 * This mode is less reliable for SATA drives since intermixed I/O between
 * WRITE BUFFER commands will invalidate the FW image but allows SATA
 * drives to fully address the [smaller] FW image segments.
 *
 * Usually after the command completes the new FW takes effect immediately,
 * which may not have been safe for the drive; therefore, it is recommended
 * to use this mode only if user I/O has been quiesced, in an offline environment,
 * or when doing a [deferred flash](@ref SA_FlashDeferredPhysicalDrive).
 */
#define kSCSIWriteBufferDownloadWithOffsetsAndActivate     0x0007
/** Download the FW image to the device using mutliple transfers.
 * Input to @ref SA_FlashDirectPhysicalDrive and @ref SA_FlashDeferredPhysicalDrive.
 *
 * Unlike @ref kSCSIWriteBufferDownloadWithOffsetsAndActivate, this mode
 * never activates the FW until device is reset by a system reboot or
 * SCSI WRITE BUFFER command using @ref kSCSIWriteBufferActivateSavedFirmware.
 * This make this mode the safest when flashing a device online.
 *
 * Using this mode requires the transfer size be small enough to be
 * fully addressed in a SCSI command.
 *
 * This mode is less reliable for SATA drives since intermixed I/O between
 * WRITE BUFFER commands will invalidate the FW image but allows SATA
 * drives to fully address the [smaller] FW image segments.
 */
#define kSCSIWriteBufferDownloadWithOffsetsAndDefer        0x000E
/** Activate a FW image previously downloaded with @ref kSCSIWriteBufferDownloadWithOffsetsAndDefer.
 * Input to @ref SA_FlashDirectPhysicalDrive.
 * Using this mode requires no transfer size or FW image.
 */
#define kSCSIWriteBufferActivateSavedFirmware              0x000F
/** Flashes a device using @ref kSCSIWriteBufferDownloadWithOffsetsAndDefer and then activates.
 * the FW using @ref kSCSIWriteBufferActivateSavedFirmware.
 */
#define kSCSIWriteBufferSaveWithOffsetsAndActivateFirmware 0x00EF
/** @} */

/** @name Supported ATA DOWNLOAD MICROCODE subcommands.
 * @ingroup storc_flash_drive
 * @ingroup storc_flash_drive_direct
 * Input to @ref SA_FlashDirectPhysicalDrive for __SATA drives only__.
 * __Usable for SATA drives only__
 * Mode                                                     | Flash Method    | Transfer Size | Image/Size
 * -------------------------------------------------------- | --------------- | ------------- | ----------
 * @ref kATADownloadUCodeDownloadAndActivate                | Direct Only     | Ignored       | Required
 * @ref kATADownloadUCodeDownloadWithOffsetsAndActivate     | Direct Only     | Required      | Required
 * @ref kATADownloadUCodeDownloadWithOffsetsAndDefer        | Direct Only     | Required      | Required
 * @ref kATADownloadUCodeActivateSavedFirmware              | Direct Only     | Ignored       | Ignored
 * @ref kATADownloadUCodeSaveWithOffsetsAndActivateFirmware | Direct Only     | Required      | Required
 *
 * @{ */
/** Download/activate FW image on the SATA drive using 1 large transfer.
 * Input to @ref SA_FlashDirectPhysicalDrive for SATA drives only.
 *
 * Using this mode requires the image be small enough to be
 * fully addressed in a single ATA command.
 *
 * This mode is more reliable for SATA drives since intermixed I/O between
 * ATA DOWNLOAD MICROCODE commands will invalidate the FW image however, SATA drives
 * usually cannot completely address modern FW images.
 *
 * Usually after the command completes the new FW takes effect immediately,
 * which may not have been safe for the drive; therefore, it is recommended
 * to use this mode only if user I/O has been quiesced or in an offline environment.
 */
#define kATADownloadUCodeDownloadAndActivate                0x1007
/** Download/activate FW image on the SATA drive using mutliple transfers.
 * Input to @ref SA_FlashDirectPhysicalDrive for SATA drives only.
 *
 * Using this mode requires the transfer size be small enough to be
 * fully addressed in an ATA command.
 *
 * This mode is less reliable for SATA drives since intermixed I/O between
 * ATA DOWNLOAD MICROCODE commands will invalidate the FW image but allows SATA
 * drives to fully address the [smaller] FW image segments.
 *
 * Usually after the command completes the new FW takes effect immediately,
 * which may not have been safe for the drive; therefore, it is recommended
 * to use this mode only if user I/O has been quiesced or in an offline environment..
 */
#define kATADownloadUCodeDownloadWithOffsetsAndActivate     0x1003
/** Download the FW image to the device using mutliple transfers.
 * Input to @ref SA_FlashDirectPhysicalDrive for SATA drives only.
 *
 * Unlike @ref kATADownloadUCodeDownloadWithOffsetsAndActivate, this mode
 * never activates the FW until device is reset by a system reboot or
 * ATA DOWNLOAD MICROCODE command using @ref kATADownloadUCodeActivateSavedFirmware.
 * This make this mode the safest when flashing a device online.
 *
 * Using this mode requires the transfer size be small enough to be
 * fully addressed in an ATA command.
 *
 * This mode is less reliable for SATA drives since intermixed I/O between
 * ATA DOWNLOAD MICROCODE commands will invalidate the FW image but allows SATA
 * drives to fully address the [smaller] FW image segments.
 */
#define kATADownloadUCodeDownloadWithOffsetsAndDefer        0x100E
/** Activate a FW image previously downloaded with @ref kATADownloadUCodeDownloadWithOffsetsAndDefer.
 * Input to @ref SA_FlashDirectPhysicalDrive for SATA drives only.
 * Using this mode requires no transfer size or FW image.
 */
#define kATADownloadUCodeActivateSavedFirmware              0x100F
/** Flashes a device using @ref kATADownloadUCodeDownloadWithOffsetsAndDefer and then activates.
 * the FW using @ref kATADownloadUCodeActivateSavedFirmware.
 */
#define kATADownloadUCodeSaveWithOffsetsAndActivateFirmware 0x10EF
/** @} */

/**********************************//**
 * @defgroup storc_flash_sep Flash SEP
 * @brief Update SEP firmware.
 * @details
 * @{
 *************************************/

/** Default mode to use for SEP flash commands. */
#define kFlashSEPDefaultMode                   kSCSIWriteBufferSaveWithOffsetsAndActivateFirmware
/** Default transfer size to use for SEP flash commands. */
#define kFlashSEPDefaultTransferSize           4096

/** Default buffer ID to use for SEP flash commands.
 * Input to @ref SA_FlashDirectSEP
 */
#define kFlashSEPFlashDefaultBufferID          0

/** @name Can Direct Flash SEP
 * @{ */
/** Can direct flash the SEP?
 * @post If valid, user may call @ref SA_FlashDirectSEP.
 * @param[in]  cda                Controller data area.
 * @param[in]  sep_number         SEP number of target.
 * @param[in]  mode               Controls which SCSI WRITE BUFFER mode is used
 * (see [Supported SCSI WRITE BUFFER modes](@ref storc_flash)).
 * @param[in]  transfer_size      Transfer size to use when sending image to SEP.
 * @param[in]  image_size         Size (in bytes) of `image`.
 * @return See [Can Direct Flash SEP Returns](@ref storc_flash_sep).
 *
 * __Example__
 * @code
 * if (SA_CanFlashDirectSEP(cda, sep_number, kFlashSEPDefaultMode, kFlashSEPDefaultTransferSize, image_size) == kCanFlashDirectSEPOK)
 * {
 *    if (SA_FlashDirectSEP(cda, sep_number, kFlashSEPDefaultMode, kFlashSEPDefaultTransferSize, image_addr, image_size)
 *          != kFlashDirectSEPSuccess)
 *    {
 *        fprintf(stderr, "Failed flash SEP\n");
 *    }
 * }
 * @endcode
 */
SA_WORD SA_CanFlashDirectSEP(PCDA cda, SA_WORD sep_number, SA_WORD mode, SA_DWORD transfer_size, SA_DWORD image_size);
   /** SEP flash with given parameters allowed.
    * @outputof SA_CanFlashDirectSEP.
    * @allows SA_FlashDirectSEP.
    */
   #define kCanFlashDirectSEPOK                       0x0000
   /** Given mode is not supported.
    * @outputof SA_CanFlashDirectSEP.
    * @allows SA_FlashDirectSEP.
    */
   #define kCanFlashDirectSEPModeUnsupported          0x0001
   /** Given transfer size is not valid or missing.
    * @outputof SA_CanFlashDirectSEP.
    * @disallows SA_FlashDirectSEP.
    */
   #define kCanFlashDirectSEPInvalidTransferSize      0x0002
   /** Given transfer size is too large for command.
    * @outputof SA_CanFlashDirectSEP.
    * @disallows SA_FlashDirectSEP.
    */
   #define kCanFlashDirectSEPTransferSizeTooLarge     0x0004
   /** Given transfer size is not aligned to blocksize.
    * @outputof SA_CanFlashDirectSEP.
    * @disallows SA_FlashDirectSEP.
    */
   #define kCanFlashDirectSEPTransferSizeUnaligned    0x0008
   /** Given image is too large given other parameters.
    * @outputof SA_CanFlashDirectSEP.
    * @disallows SA_FlashDirectSEP.
    */
   #define kCanFlashDirectSEPImageTooLarge            0x0010
   /** Given image size is not aligned to blocksize.
    * @outputof SA_CanFlashDirectSEP.
    * @disallows SA_FlashDirectSEP.
    */
   #define kCanFlashDirectSEPImageSizeUnaligned       0x0020
   /** Given image size is not valid or missing.
    * @outputof SA_CanFlashDirectSEP.
    * @disallows SA_FlashDirectSEP.
    */
   #define kCanFlashDirectSEPInvalidImageSize         0x0040
   /** The target SEP does not support SCSI commands.
    * @outputof SA_CanFlashDirectSEP.
    * @disallows SA_FlashDirectSEP.
    */
   #define kCanFlashDirectSEPSCSICommandNotSupported  0x0080
   /** The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanFlashDirectSEP.
    * @disallows SA_FlashDirectSEP.
    */
   #define kCanFlashDirectSEPOFAActive                0x0100
 /** @} */

/** @name Direct SEP Flash
 * @{ */
/** Flash the SEP directly.
 * @pre The user has used @ref SA_CanFlashDirectSEP to ensure the operation can be performed.
 *
 * @param[in]  cda                Controller data area.
 * @param[in]  sep_number         SEP number of target.
 * @param[in]  mode               Controls which SCSI WRITE BUFFER mode is used
 * (see [Supported SCSI WRITE BUFFER modes](@ref storc_flash)).
 * @param[in]  buffer_id          Buffer ID of the SEP (usually @ref kFlashSEPFlashDefaultBufferID).
 * @param[in]  transfer_size      Transfer size to use when sending image to SEP.
 * @param[in]  image              Firmware image to send to SEP.
 * @param[in]  image_size         Size (in bytes) of `image`.
 * @return See [Direct SEP Flash Returns](@ref storc_flash_sep).
 *
 * __Example__
 * @code
 * if (SA_CanFlashDirectSEP(cda, sep_number, kFlashSEPDefaultMode, kFlashSEPDefaultTransferSize, image_size) == kCanFlashDirectSEPOK)
 * {
 *    if (SA_FlashDirectSEP(cda, sep_number, kFlashSEPDefaultMode, kFlashSEPDefaultTransferSize, image_addr, image_size)
 *          != kFlashDirectSEPSuccess)
 *    {
 *        fprintf(stderr, "Failed flash SEP\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_FlashDirectSEP(PCDA cda, SA_WORD sep_number, SA_WORD mode, SA_BYTE buffer_id, SA_DWORD transfer_size, SA_BYTE * image, SA_DWORD image_size);
   #define kFlashDirectSEPFailed                0 /**< Flash command(s) did __NOT__ complete successfully; @outputof SA_FlashDirectSEP. */
   #define kFlashDirectSEPSuccess               1 /**< Flash command(s) completed successfully; @outputof SA_FlashDirectSEP. */
/** @} */

 /** @} */ /* storc_flash_sep */

/**********************************//**
 * @defgroup storc_flash_storage_enclosure_backplane Flash Storage Enclosure Backplane
 * @brief Update storage enclosure backplane firmware.
 * @details
 * @{
 *************************************/

/** @name Can Flash Storage Enclosure Backplane
 * @{ */
/** Can flash the storage enclosure backplane?
 *
 * @post If valid, user may call @ref SA_FlashStorageEnclosureBackplane.
 *
 * @param[in]  cda                Controller data area.
 * @param[in]  se_number          Storage enclosure number.
 * @param[in]  image              Firmware image to send to storage enclosure backplane.
 * @param[in]  image_size         Size (in bytes) of `image`.
 * @return See [Flash Storage Enclosure Backplane Returns](@ref storc_flash_storage_enclosure_backplane).
 *
 * __Example__
 * @code
 * if (SA_CanFlashStorageEnclosureBackplane(cda, se_number, image_addr, image_size) == kCanFlashStorageEnclosureBackplaneOK)
 * {
 *    if (SA_FlashStorageEnclosureBackplane(cda, se_number, image_addr, image_size) != kFlashStorageEnclosureBackplaneSuccess)
 *    {
 *       fprintf(stderr, "Failed to flash storage enclosure backplane\n");
 *    }
 * }
 * else
 * {
 *    fprintf(stderr, "Cannot flash storage enclosure backplane\n");
 * }
 * @endcode
 */
/* @} */
SA_WORD SA_CanFlashStorageEnclosureBackplane(PCDA cda, SA_WORD se_number, SA_BYTE * image, SA_DWORD image_size);
   /** Storage enclosure backplane flash allowed.
    * @outputof SA_CanFlashStorageEnclosureBackplane.
    * @allows SA_FlashStorageEnclosureBackplane.
    */
   #define kCanFlashStorageEnclosureBackplaneOK                         0x0000
   /** The storage enclosure number provided is invalid.
    * @outputof SA_CanFlashStorageEnclosureBackplane.
    * @disallows SA_FlashStorageEnclosureBackplane.
    */
   #define kCanFlashStorageEnclosureBackplaneStorageEnclosureInvalid    0x0001
   /** The storage enclosure does not have a valid backplane target present.
    * @outputof SA_CanFlashStorageEnclosureBackplane.
    * @disallows SA_FlashStorageEnclosureBackplane.
    */
   #define kCanFlashStorageEnclosureBackplaneNoValidBackplanePresent    0x0002
   /** The image size provided is invalid or missing.
    * @outputof SA_CanFlashStorageEnclosureBackplane.
    * @disallows SA_FlashStorageEnclosureBackplane.
    */
   #define kCanFlashStorageEnclosureBackplaneInvalidImageSize           0x0004
   /** The image provided is null.
    * @outputof SA_CanFlashStorageEnclosureBackplane.
    * @disallows SA_FlashStorageEnclosureBackplane.
    */
   #define kCanFlashStorageEnclosureBackplaneImageIsNull                0x0008
   /** Cannot flash the storage enclosure backplane because of an active or pending online firmware activation operation.
    * @outputof SA_CanFlashStorageEnclosureBackplane.
    * @disallows SA_FlashStorageEnclosureBackplane.
    */
   #define kCanFlashStorageEnclosureBackplaneOFAActive                  0x0010

/** @name Flash Storage Enclosure Backplane
 * @{ */
/** Flash the storage enclosure backplane.
 *
 * @pre The user has used @ref SA_CanFlashStorageEnclosureBackplane to ensure the operation can be performed.
 *
 * @param[in]  cda            Controller data area.
 * @param[in]  se_number      Storage enclosure number.
 * @param[in]  image          Firmware image to send to storage enclosure.
 * @param[in]  image_size     Size (in bytes) of `image`.
 * @return See [Flash Storage Enclosure Backplane Returns](@ref storc_flash_storage_enclosure_backplane).
 */
/* @} */
SA_BYTE SA_FlashStorageEnclosureBackplane(PCDA cda, SA_WORD se_number, SA_BYTE * image, SA_DWORD image_size);
#define kFlashStorageEnclosureBackplaneFailed      0 /**< Flash command(s) did __NOT__ complete successfully; @outputof SA_FlashStorageEnclosureBackplane. */
#define kFlashStorageEnclosureBackplaneSuccess     1 /**< Flash command(s) completed successfully; @outputof SA_FlashStorageEnclosureBackplane. */

 /** @} */ /* storc_flash_storage_enclosure_backplane */

/***********************************************//**
 * @defgroup storc_flash_drive Flash Physical Drive
 * @brief Update physical drive/disk firmware.
 * @details
 * @{
 **************************************************/

/** @defgroup storc_flash_drive_direct Direct Flash Physical Drive
 * @{
 */

/** Default mode to use for physical drive flash commands. */
#define kFlashPhysicalDriveDefaultMode                   kSCSIWriteBufferDownloadWithOffsetsAndDefer
/** Default transfer size to use for physical drive flash commands. */
#define kFlashPhysicalDriveDefaultTransferSize           32768

/** @name Can Direct Flash Physical Drive
 * @{ */
/** Can direct flash the physical drive?
 * @post If valid, user may call @ref SA_FlashDirectPhysicalDrive.
 * @param[in]  cda                Controller data area.
 * @param[in]  pd_number          PD number of target physical drive.
 * @param[in]  mode               Controls which SCSI WRITE BUFFER mode or ATA DOWNLOAD MICROCODE subcommand is used
 * (see [Supported SCSI WRITE BUFFER modes](@ref storc_flash) and [Supported ATA DOWNLOAD MICROCODE subcommands](@ref storc_flash)).
 * @param[in]  transfer_size      Transfer size to use when sending image to drive.
 * @param[in]  image_size         Size (in bytes) of `image`.
 * @return See [Can Direct Flash Physical Drive Returns](@ref storc_flash_drive_direct).
 *
 * __Example__
 * @code
 * if (SA_CanFlashDirectPhysicalDrive(cda, pd_number, kFlashPhysicalDriveDefaultMode, kFlashPhysicalDriveDefaultTransferSize, image_size) == kCanFlashDirectPhysicalDriveOK)
 * {
 *    if (SA_FlashDirectPhysicalDrive(cda, pd_number, kFlashPhysicalDriveDefaultMode, kFlashPhysicalDriveDefaultTransferSize, image_addr, image_size)
 *          != kFlashDirectPhysicalDriveSuccess)
 *    {
 *        fprintf(stderr, "Failed flash physical drive\n");
 *    }
 * }
 * @endcode
 */
SA_WORD SA_CanFlashDirectPhysicalDrive(PCDA cda, SA_WORD pd_number, SA_WORD mode, SA_DWORD transfer_size, SA_DWORD image_size);
   /** Drive flash with given parameters allowed.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @allows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveOK                       0x0000
   /** Given mode is not supported.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveModeUnsupported          0x0001
   /** Given mode is not supported for target SAS drive.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveModeUnsupportedOnSAS     0x0002
   /** Given transfer size is not valid or missing.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveInvalidTransferSize      0x0004
   /** Given transfer size is too large for command.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveTransferSizeTooLarge     0x0008
   /** Given transfer size is not aligned to blocksize.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveTransferSizeUnaligned    0x0010
   /** Given image is too large given other parameters.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveImageTooLarge            0x0020
   /** Given image size is not aligned to blocksize.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveImageSizeUnaligned       0x0040
   /** Given image size is not valid or missing.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveInvalidImageSize         0x0080
   /** The target drive does not support SCSI commands.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveSCSICommandNotSupported  0x0100
   /** The target drive does not exists or is not responding.
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveInvalidDrive             0x0200
   /** The target drive's controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanFlashDirectPhysicalDrive.
    * @disallows SA_FlashDirectPhysicalDrive.
    */
   #define kCanFlashDirectPhysicalDriveOFAActive                0x0400
/** @} */

/** @name Direct Physical Drive Flash
 * @{ */
/** Flash the physical drive directly.
 * @pre The user has used @ref SA_CanFlashDirectPhysicalDrive to ensure the operation can be performed.
 *
 * @param[in]  cda                Controller data area.
 * @param[in]  pd_number          PD number of target physical drive.
 * @param[in]  mode               Controls which SCSI WRITE BUFFER mode or ATA DOWNLOAD MICROCODE subcommand is used
 * (see [Supported SCSI WRITE BUFFER modes](@ref storc_flash) and [Supported ATA DOWNLOAD MICROCODE subcommands](@ref storc_flash)).
 * @param[in]  transfer_size      Transfer size to use when sending image to drive.
 * @param[in]  image              Firmware image to send to drive.
 * @param[in]  image_size         Size (in bytes) of `image`.
 * @return See [Direct Physical Drive Flash Returns](@ref storc_flash_drive_direct).
 *
 * __Example__
 * @code
 * if (SA_CanFlashDirectPhysicalDrive(cda, pd_number, kFlashPhysicalDriveDefaultMode, kFlashPhysicalDriveDefaultTransferSize, image_size) == kCanFlashDirectPhysicalDriveOK)
 * {
 *    if (SA_FlashDirectPhysicalDrive(cda, pd_number, kFlashPhysicalDriveDefaultMode, kFlashPhysicalDriveDefaultTransferSize, image_addr, image_size)
 *          != kFlashDirectPhysicalDriveSuccess)
 *    {
 *        fprintf(stderr, "Failed flash physical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_FlashDirectPhysicalDrive(PCDA cda, SA_WORD pd_number, SA_WORD mode, SA_DWORD transfer_size, SA_BYTE * image, SA_DWORD image_size);
   #define kFlashDirectPhysicalDriveFailed                0 /**< Flash command(s) did __NOT__ complete successfully; @outputof SA_FlashDirectPhysicalDrive. */
   #define kFlashDirectPhysicalDriveSuccess               1 /**< Flash command(s) completed successfully; @outputof SA_FlashDirectPhysicalDrive. */
/** @} */

/** @} */ /* storc_flash_drive_direct */

/** @defgroup storc_flash_drive_deferred Flash Deferred Physical Drive
 * @brief Defer/queue physical drive firmware on next boot.
 * @{
 */

/** @name Can Defer Flash Physical Drive
 * @{ */
/** Can defer flash the physical drive?
 * @post If valid, user may call @ref SA_FlashDeferredPhysicalDrive.
 * @param[in]  cda                Controller data area.
 * @param[in]  pd_number          PD number of target physical drive.
 * @param[in]  mode               Controls which SCSI WRITE BUFFER mode is used (see [Supported SCSI WRITE BUFFER modes](@ref storc_flash)).
 * @param[in]  transfer_size      Transfer size to use when sending image to drive.
 * @param[in]  image_size         Size (in bytes) of `image`.
 * @return See [Can Defer Flash Physical Drive Returns](@ref storc_flash_drive_deferred).
 *
 * __Example__
 * @code
 * if (SA_CanFlashDeferredPhysicalDrive(cda, pd_number, kFlashPhysicalDriveDefaultMode, kFlashPhysicalDriveDefaultTransferSize, image_size) == kCanFlashDeferredPhysicalDriveOK)
 * {
 *    if (SA_FlashDeferredPhysicalDrive(cda, pd_number, kFlashPhysicalDriveDefaultMode, kFlashPhysicalDriveDefaultTransferSize, image_addr, image_size)
 *          != kFlashDeferredPhysicalDriveSuccess)
 *    {
 *        fprintf(stderr, "Failed to queue deferred flash to physical drive\n");
 *    }
 * }
 * @endcode
 */
SA_WORD SA_CanFlashDeferredPhysicalDrive(PCDA cda, SA_WORD pd_number, SA_WORD mode, SA_DWORD transfer_size, SA_DWORD image_size);
   /** Drive flash with given parameters allowed.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @allows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveOK                         0x0000
   /** Given mode is not supported.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveModeUnsupported            0x0001
   /** Given mode is not supported for target HBA drive.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveUnsupportedForHBADrive     0x0002
   /** Given transfer size is not valid or missing.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveInvalidTransferSize        0x0004
   /** Given transfer size is too large for command.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveTransferSizeTooLarge       0x0008
   /** Given image is too large given other parameters.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveImageTooLarge              0x0010
   /** Given image size is not valid or missing.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveInvalidImageSize           0x0020
   /** Deferred flashing is not supported on generic HBA controllers.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveUnsupportedOnNonSmartArray 0x0040
   /** The controller does not support BMIC commands.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveBMICCommandNotSupported    0x0080
   /** The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveOFAActive                  0x0100
   /** The specified physical drive does not exist.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveInvalidPhysicalDrive       0x0200
   /** The length of the current firmware revision of the drive is too long and is not supported by deferred flashing.
    * @outputof SA_CanFlashDeferredPhysicalDrive.
    * @disallows SA_FlashDeferredPhysicalDrive.
    */
   #define kCanFlashDeferredPhysicalDriveFWRevisionTooLong          0x0400
/** @} */

/** @name Deferred Physical Drive Flash
 * @{ */
/** Queue physical drive flash to next reboot.
 * @pre The user has used @ref SA_CanFlashDeferredPhysicalDrive to ensure the operation can be performed.
 *
 * @param[in]  cda                Controller data area.
 * @param[in]  pd_number          PD number of target physical drive.
 * @param[in]  mode               Controls which SCSI WRITE BUFFER mode is used (see [Supported SCSI WRITE BUFFER modes](@ref storc_flash)).
 * @param[in]  transfer_size      Transfer size to use when sending image to drive.
 * @param[in]  image              Firmware image to send to drive.
 * @param[in]  image_size         Size (in bytes) of `image`.
 * @return See [Deferred Physical Drive Flash Returns](@ref storc_flash_drive_deferred).
 *
 * __Example__
 * @code
 * if (SA_CanFlashDeferredPhysicalDrive(cda, pd_number, kFlashPhysicalDriveDefaultMode, kFlashPhysicalDriveDefaultTransferSize, image_size) == kCanFlashDeferredPhysicalDriveOK)
 * {
 *    if (SA_FlashDeferredPhysicalDrive(cda, pd_number, kFlashPhysicalDriveDefaultMode, kFlashPhysicalDriveDefaultTransferSize, image_addr, image_size)
 *          != kFlashDeferredPhysicalDriveSuccess)
 *    {
 *        fprintf(stderr, "Failed to queue deferred flash to physical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_FlashDeferredPhysicalDrive(PCDA cda, SA_WORD pd_number, SA_WORD mode, SA_DWORD transfer_size, SA_BYTE * image, SA_DWORD image_size);
   #define kFlashDeferredPhysicalDriveFailed                0 /**< Flash command(s) did __NOT__ complete successfully; @outputof SA_FlashDeferredPhysicalDrive. */
   #define kFlashDeferredPhysicalDriveSuccess               1 /**< Flash command(s) completed successfully; @outputof SA_FlashDeferredPhysicalDrive. */
/** @} */

/** @name Deferred Physical Drive Flash Information
 * @{ */
/** Get information on the deferred update status of a physical drive.
 * @pre The user has queued at least 1 drive for a deferred update using @ref SA_FlashDeferredPhysicalDrive.
 *
 * @param[in]  cda                Controller data area.
 * @param[in]  pd_number          PD number of target physical drive.
 * @return See [Deferred Physical Drive Flash Information](@ref storc_flash_drive_deferred).
 */
SA_WORD SA_GetPhysicalDriveDeferredFlashInfo(PCDA cda, SA_WORD pd_number);
   #define kPhysicalDriveDeferredFlashInvalid        0x0000 /**< No deferred update has been queued or in progress; @outputof SA_GetPhysicalDriveDeferredFlashInfo. */
   #define kPhysicalDriveDeferredFlashQueueing       0x0001 /**< An update is currently being queued to the drive; @outputof SA_GetPhysicalDriveDeferredFlashInfo. */
   #define kPhysicalDriveDeferredFlashReadyForReboot 0x0002 /**< An update is queued to a drive and is awaiting a system reboot; @outputof SA_GetPhysicalDriveDeferredFlashInfo. */
   #define kPhysicalDriveDeferredFlashInProgress     0x0004 /**< A queued update is currently being applied to the drive; @outputof SA_GetPhysicalDriveDeferredFlashInfo. */
   #define kPhysicalDriveDeferredFlashCancelled      0x0008 /**< A queued update for a drive has been cancelled; @outputof SA_GetPhysicalDriveDeferredFlashInfo. */
   #define kPhysicalDriveDeferredFlashCompleted      0x0010 /**< The last deferred update has been completed successfully; @outputof SA_GetPhysicalDriveDeferredFlashInfo. */
   #define kPhysicalDriveDeferredFlashFailed         0x0020 /**< The last deferred update failed with an error; @outputof SA_GetPhysicalDriveDeferredFlashInfo. */
   #define kPhysicalDriveDeferredFlashPaused         0x0040 /**< A queue update for a drive has been paused; @outputof SA_GetPhysicalDriveDeferredFlashInfo. */

   /** Used to decode output of @ref SA_GetPhysicalDriveDeferredFlashInfo to get deferred update status of a drive.
    * @see PHYSICAL_DRIVE_DEFERRED_UPDATE_STATUS. */
   #define kPhysicalDriveDeferredFlashStatusMask     0x00FF
   /** Decode output of @ref SA_GetPhysicalDriveDeferredFlashInfo to get deferred update status of a drive. */
   #define PHYSICAL_DRIVE_DEFERRED_UPDATE_STATUS(info) (info & kPhysicalDriveDeferredFlashStatusMask)

   /** Used to decode output of @ref SA_GetPhysicalDriveDeferredFlashInfo to get global deferred update progress.
    * @see CONTROLLER_DEFERRED_UPDATE_PROGRESS. */
   #define kPhysicalDriveDeferredFlashProgressMask      0xFF00
   /** Used to decode output of @ref SA_GetPhysicalDriveDeferredFlashInfo to get global deferred update progress.
    * @see CONTROLLER_DEFERRED_UPDATE_PROGRESS. */
   #define kPhysicalDriveDeferredFlashProgressBitShift  8
   /** Decode output of @ref SA_GetPhysicalDriveDeferredFlashInfo to get global update progress of all drives. */
   #define CONTROLLER_DEFERRED_UPDATE_PROGRESS(info) ((info & kPhysicalDriveDeferredFlashProgressMask) >> kPhysicalDriveDeferredFlashProgressBitShift)
/** @} */

/** @} */ /* storc_flash_drive_deferred */

/** @} */ /* storc_flash_drive */

/** @} */ /* storc_flash */

/** @} */ /* storcore */


#if defined(__cplusplus)
}
#endif

#endif /* INC_PSTORC_FLASH_H_ */

