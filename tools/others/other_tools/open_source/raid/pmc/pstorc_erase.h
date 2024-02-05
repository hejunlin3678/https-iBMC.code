/** @file pstorc_erase.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore erase header file
   Defines functions for erasing a device.

*/

#ifndef INC_PSTORC_ERASE_H_
#define INC_PSTORC_ERASE_H_

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/************************//**
 * @defgroup storc_erase Erase
 * @brief Erase data on drives.
 * @{
 ***************************/

/** @defgroup storc_erase_support Erase Support
 * @brief Get device's erase support information.
 * @{
 * @addtogroup storc_features_ctrl_pd_erase
 * @addtogroup storc_features_pd_erase_patterns_supported
 * @addtogroup storc_features_ctrl_support_sanitize_lock
 * @}
 */

#if defined(__cplusplus)
extern "C" {
#endif

/** @defgroup storc_erase_pd Erase Physical Drive
 * @brief Start physical drive erase operation.
 * @details
 * @see storc_erase_support
 * @see storc_properties_ctrl_sanitize_lock
 * @see storc_configuration_edit_ctrl_sanitize_lock
 * @see storc_stop_erase_pd
 * @see storc_enable_erased_pd
 * @{
 */

/** Default erase pattern. */
#define kErasePhysicalDriveDefaultPattern    kErasePattern3Pass

/** Can erase the physical drive.
 * @allows SA_ErasePhysicalDrive.
 * @param[in]  cda                Controller data area.
 * @param[in]  pd_number          Target physical drive number.
 * @param[in]  erase_pattern      Desired erase pattern. See [Erase Pattern Values] (@ref storc_features_pd_erase_patterns_supported).
 * @return See [Can Erase Physical Drive Returns](@ref storc_erase_pd).
 *
 * __Example__
 * @code
 * if (SA_CanErasePhysicalDrive(cda, pd_number, erase_pattern) == kCanErasePhysicalDriveOK)
 * {
 *    if (SA_ErasePhysicalDrive(cda, pd_number, erase_pattern) != kErasePhysicalDriveSuccess)
 *    {
 *        fprintf(stderr, "Failed to erase physical drive\n");
 *    }
 * }
 * @endcode
 */
SA_WORD SA_CanErasePhysicalDrive(PCDA cda, SA_WORD pd_number, SA_WORD erase_pattern);
   /** @name Can Erase Physical Drive Returns
    * @outputof SA_CanErasePhysicalDrive.
    * @{ */
   #define kCanErasePhysicalDriveOK                   0x0000 /**< The physical drive can be erased using the desired erase pattern; @outputof SA_CanErasePhysicalDrive. */
   #define kCanErasePhysicalDriveNotUnassigned        0x0001 /**< The desired erase pattern is not supported on the target physical drive; @outputof SA_CanErasePhysicalDrive. */
   #define kCanErasePhysicalDriveIsHBA                0x0002 /**< @deprecated Use @ref SA_PhysicalDriveSupportsErasePattern to determine erase pattern support for HBA drives. */
   #define kCanErasePhysicalDriveIsFailed             0x0003 /**< The desired erase pattern is not supported on the target physical drive; @outputof SA_CanErasePhysicalDrive. */
   #define kCanErasePhysicalDrivePatternNotSupported  0x0004 /**< The desired erase pattern is not supported on the target physical drive; @outputof SA_CanErasePhysicalDrive. */
   #define kCanErasePhysicalDriveIsErasing            0x0005 /**< The desired erase pattern is not supported on the target physical drive; @outputof SA_CanErasePhysicalDrive. */
   #define kCanErasePhysicalDriveSanitizeLockFreeze   0x0006 /**< Sanitize not allowed on this drive due to the sanitize lock state (ATA Freeze); @outputof SA_CanErasePhysicalDrive. */
   #define kCanErasePhysicalDriveControllerOFAActive  0x0007 /**< Sanitize not allowed on this drive due to an active or pending online firmware activation operation; @outputof SA_CanErasePhysicalDrive. */
   #define kCanErasePhysicalDriveIsSEDLocked          0x0008 /**< Sanitize not allowed on this drive due to it being SED Locked. @see SA_GetPhysicalDriveEncryptionInfo. */
   /** @} */

/** Erase the physical drive.
 * @param[in]  cda                Controller data area.
 * @param[in]  pd_number          Target physical drive number.
 * @param[in]  erase_pattern      Desired erase pattern. See [Erase Pattern Values] (@ref storc_features_pd_erase_patterns_supported).
 * @return See [Erase Physical Drive Returns](@ref storc_erase_pd).
 *
 * __Example__
 * @code
 * if (SA_CanErasePhysicalDrive(cda, pd_number, erase_pattern) == kCanErasePhysicalDriveOK)
 * {
 *    if (SA_ErasePhysicalDrive(cda, pd_number, erase_pattern) != kErasePhysicalDriveSuccess)
 *    {
 *        fprintf(stderr, "Failed to erase physical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_ErasePhysicalDrive(PCDA cda, SA_WORD pd_number, SA_WORD erase_pattern);
   /** @name Erase Physical Drive Returns
    * @outputof SA_ErasePhysicalDrive.
    * @{ */
   #define kErasePhysicalDriveFailed   0x00 /**< The erase pattern was not successfully started on the target physical drive.; @outputof SA_ErasePhysicalDrive. */
   #define kErasePhysicalDriveSuccess  0x01 /**< The erase pattern was successfully started on the target physical drive.; @outputof SA_ErasePhysicalDrive. */
   /** @} */

/** @} */ /* storc_erase_pd */

/** @defgroup storc_stop_erase_pd Stop Erase Physical Drive
 * @brief Stop/abort an ongoing erase on a physical drive.
 * @details
 * @see storc_erase_pd
 * @see storc_enable_erased_pd
 * @{
 */

/** Can stop erase physical drive.
 * Alias for @ref SA_CanEnableErasedPhysicalDrive.
 * @see SA_CanEnableErasedPhysicalDrive.
 * @post If valid, user may call @ref SA_StopErasePhysicalDrive.
 * @param[in]  cda                Controller data area.
 * @param[in]  pd_number          Target physical drive number.
 * @return [Can Stop Erase Physical Drive Returns](@ref storc_stop_erase_pd).
 */
SA_BYTE SA_CanStopErasePhysicalDrive(PCDA cda, SA_WORD pd_number);
   /** @name Can Stop Erase Physical Drive Returns
    * @outputof SA_CanStopErasePhysicalDrive.
    * @outputof SA_CanEnableErasedPhysicalDrive.
    * @{ */
   #define kCanStopEnableErasePhysicalDriveOK               0x00 /**< @deprecated Alias for @ref kCanStopErasePhysicalDriveOK. */
   #define kCanEnableErasedPhysicalDriveRestrictedSanitize  0x03 /**< @deprecated Alias for @ref kCanStopErasePhysicalDriveRestrictedSanitize. */

   #define kCanStopErasePhysicalDriveOK                     0x00 /**< The physical drive erase can be stopped and enabled; @outputof SA_CanStopErasePhysicalDrive. */
   #define kCanStopErasePhysicalDriveNotErasing             0x01 /**< The drive erase cannot be stopped or enabled because the drive is not erasing.; @outputof SA_CanStopErasePhysicalDrive. */
   #define kCanStopErasePhysicalDriveSanitizing             0x02 /**< A sanitize erase is in progress and cannot be stopped; @outputof SA_CanStopErasePhysicalDrive. */
   #define kCanStopErasePhysicalDriveRestrictedSanitize     0x03 /**< A restricted sanitize erase has failed on the physical drive and the drive cannot be enabled; @outputof SA_CanStopErasePhysicalDrive. */
   #define kCanStopErasePhysicalDriveControllerOFAActive    0x04 /**< The physical drive cannot be enabled due to an active or pending online firmware activation.; @outputof SA_CanStopErasePhysicalDrive. */
   /** @} */

/** Stop erase physical drive.
 * Alias for @ref SA_EnableErasedPhysicalDrive.
 * @see SA_EnableErasedPhysicalDrive.
 * @param[in]  cda                Controller data area.
 * @param[in]  pd_number          Target physical drive number.
 * @return [Stop Erase Physical Drive Returns](@ref storc_stop_erase_pd).
 */
SA_BYTE SA_StopErasePhysicalDrive(PCDA cda, SA_WORD pd_number);
   /** @name Stop Erase Physical Drive Returns
    * @outputof SA_StopErasePhysicalDrive.
    * @outputof SA_EnableErasedPhysicalDrive.
    * @{ */
   #define kStopEnabledErasePhysicalDriveFailed  0x00 /**< The physical drive erase was not stopped/enabled successfully.; @outputof SA_StopErasePhysicalDrive. */
   #define kStopEnabledErasePhysicalDriveSuccess 0x01 /**< The physical drive erase was stopped/enabled successfully.; @outputof SA_StopErasePhysicalDrive. */
   /** @} */

/** @} */ /* storc_stop_erase_pd */

/** @defgroup storc_enable_erased_pd Enable Erased Physical Drive
 * @brief Reenable a physical drive after erase is complete or aborted.
 * @details
 * @see storc_erase_pd
 * @see storc_stop_erase_pd
 * @{
 */

/** Can enable erased physical drive.
 * Alias for @ref SA_CanStopErasePhysicalDrive.
 * @see SA_CanStopErasePhysicalDrive.
 * @post If valid, user may call @ref SA_EnableErasedPhysicalDrive.
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Target physical drive number.
 * @return [Can Stop Erase Physical Drive Returns](@ref storc_stop_erase_pd).
 */
SA_BYTE SA_CanEnableErasedPhysicalDrive(PCDA cda, SA_WORD pd_number);

/** Enable erased physical drive.
 * Alias for @ref SA_StopErasePhysicalDrive.
 * @see SA_StopErasePhysicalDrive.
 * @param[in]  cda         Controller data area.
 * @param[in]  pd_number   Target physical drive number.
 * @return [Stop Erase Physical Drive Returns](@ref storc_stop_erase_pd).
 */
SA_BYTE SA_EnableErasedPhysicalDrive(PCDA cda, SA_WORD pd_number);

/** @} */ /* storc_enable_erased_pd */

/** @} */ /* storc_erase */

/** @} */ /* storcore */


#if defined(__cplusplus)
}
#endif

#endif /* INC_PSTORC_FLASH_H_ */

