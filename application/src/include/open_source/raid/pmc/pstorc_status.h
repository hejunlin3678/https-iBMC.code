/** @file pstorc_status.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore status header file
   Defines functions for obtaining device status

*/

#ifndef PSTORC_STATUS_H
#define PSTORC_STATUS_H

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/****************************************//**
 * @defgroup storc_status Status
 * @brief
 * @details
 * @{
 *******************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*************************************//**
 * @defgroup storc_status_ctrl Controller
 * @brief
 * @details
 * @{
 *****************************************/

/** @defgroup storc_status_ctrl_status Controller Status
 * @{ */
/** Obtain status info for a Controller.
 *
 * @param[in] cda  Controller data area.
 * @return Status map for controller (in @ref storc_status_ctrl_status)
 * - [Controller Statuses](@ref storc_status_ctrl_status)
 * - [Controller Cache Statuses](@ref storc_status_ctrl_status)
 * - [Controller Cache States If Cache Disabled](@ref storc_status_ctrl_status)
 * - [Controller Cache States Under Any Cache Status](@ref storc_status_ctrl_status)
 * - [Controller Configuration Statuses](@ref storc_status_ctrl_status)
 * @return Use @ref CONTROLLER_STATUS, @ref CACHE_STATUS, @ref CONTROLLER_CACHE_STATE,
 * @ref CONTROLLER_CACHE_FLAGS, and @ref CONTROLLER_CONFIG_STATUS to decode (in @ref storc_status_ctrl_status).
 * Use @ref SA_GetControllerNVRAMErrors to check for NVRAM errors (@ref storc_properties_ctrl_nvram_errors).
 *
 * __Example__
 * @code
 * SA_DWORD ctl_status = SA_GetControllerStatusInfo(cda);
 * printf("Controller Status: %s\n",
 *             CONTROLLER_STATUS(ctl_status) == kControllerStatusOK ? "OK"
 *             : CONTROLLER_STATUS(ctl_status) == kControllerStatusFailed ? "Failed"
 *             : CONTROLLER_STATUS(ctl_status) == kControllerStatusBadZMRMode ? "Bad ZMR"
 *             : CONTROLLER_STATUS(ctl_status) == kControllerStatusCacheProblem ? "Cache Problem"
 *             : CONTROLLER_STATUS(ctl_status) == kControllerStatusIncompatibleCacheModule ? "Incompatible Cache"
               : CONTROLLER_STATUS(ctl_status) == kControllerStatusDirtyCacheNeedsFirmwareDowngrade ? "Dirty Cache Requires Downgrade"
 *             : "UNKNOWN");
 *
 * if (CONTROLLER_STATUS(ctl_status) != kControllerStatusFailed)
 * {
 *     printf("Cache Status: %s\n",
 *                 CACHE_STATUS(ctl_status) == kControllerCacheStatusOK ? "OK"
 *                 : CACHE_STATUS(ctl_status) == kControllerCacheStatusNotConfigured ? "Not Configured"
 *                 : CACHE_STATUS(ctl_status) == kControllerCacheStatusTemporarilyDisabled ? "Temporarily Disabled"
 *                 : CACHE_STATUS(ctl_status) == kControllerCacheStatusPermanentlyDisabled ? "Permanently Disabled"
 *                 : "UNKNOWN");
 *
 *     if (CACHE_STATUS(ctl_status) == kControllerCacheStatusTemporarilyDisabled ||
 *         CACHE_STATUS(ctl_status) == kControllerCacheStatusPermanentlyDisabled)
 *     {
 *         printf("Cache State: %s\n",
 *                     CONTROLLER_CACHE_STATE(ctl_status) == kControllerCacheState0BoardMismatch ? "Board/Cache mismatch"
 *                     : CONTROLLER_CACHE_STATE(ctl_status) == kControllerCacheState17CannotReadData ? "Failed Read"
 *                     : CONTROLLER_CACHE_STATE(ctl_status) == kControllerCacheState18CannotWriteData ? "Failed Write"
 *                     : "UNKNOWN");
 *     }
 *
 *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsValidDataFoundInCache)
 *         printf("Cache Flags: Valid Data Exists\n");
 *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsLowBatteryAtBoot)
 *         printf("Cache Flags: Low battery at boot\n");
 *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsFlashMemoryDetached)
 *         printf("Cache Flags: Flash memory detached\n");
 *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsModuleRunningAtFailsafeSpeed)
 *         printf("Cache Flags: Running at failsafe speed\n");
 *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsOtherState)
 *         printf("Cache Flags: other\n");
 * #if defined(ENABLE_SOFTWARE_RAID_SUPPORT)
 *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsDirtySoftwareRAIDCache)
 *         printf("Cache Flags: Dirty software RAID cache\n");
 * #endif
 * }
 *
 * printf("Controller Config Statuses\n");
 * if (CONTROLLER_CONFIG_STATUS(ctl_status) & kControllerConfigStatusOK)
 *     printf(" - OK\n");
 * if (CONTROLLER_CONFIG_STATUS(ctl_status) & kControllerConfigStatusVersionMismatch)
 *     printf(" - Version mismatch\n");
 * if (CONTROLLER_CONFIG_STATUS(ctl_status) & kControllerConfigStatusBadEncryptionOverride)
 *     printf(" - Bad Encryption\n");
 * if (CONTROLLER_CONFIG_STATUS(ctl_status) & kControllerConfigStatusBadLUCacheOverride)
 *     printf(" - Bad Smart Cache\n");
 * @endcode
 */
SA_DWORD SA_GetControllerStatusInfo(PCDA cda);
   /** @name Controller Statuses
    * @outputof SA_GetControllerStatusInfo for Controller status.
    * @{ */
   #define kControllerStatusOK                               0x01  /**< Controller status OK; @outputof SA_GetControllerStatusInfo. */
   #define kControllerStatusFailed                           0x02  /**< Controller status Failed; @outputof SA_GetControllerStatusInfo. */
   #define kControllerStatusBadZMRMode                       0x03  /**< Controller status unsupported in ZMR mode; @outputof SA_GetControllerStatusInfo. */
   #define kControllerStatusCacheProblem                     0x04  /**< Controller status cache problem; see [Controller Cache Statuses](@ref storc_status_ctrl_status). */
   #define kControllerStatusIncompatibleCacheModule          0x05  /**< Controller status incompatible cache module attached; @outputof SA_GetControllerStatusInfo. */
   #define kControllerStatusDirtyCacheNeedsFirmwareDowngrade 0x06  /**< Controller status dirty cache exists and requires a firmware downgrade to flush; @outputof SA_GetControllerStatusInfo. */
   /** Use to mask output of @ref SA_GetControllerStatusInfo for the status of the Controller.
    * @see CONTROLLER_STATUS.
    */
   #define kControllerStatusMask                    0x0000000F
   /** Use to decode output of @ref SA_GetControllerStatusInfo for the status of the Controller.
    * @see CONTROLLER_STATUS.
    */
   #define kControllerStatusMaskBitShift            0
   /** Use to decode Controller status output of @ref SA_GetControllerStatusInfo.
    *
    * __Example__
    * @code
    * SA_DWORD ctl_status = SA_GetControllerStatusInfo(cda);
    * printf("Controller Status: %s\n",
    *             CONTROLLER_STATUS(ctl_status) == kControllerStatusOK           ? "OK"
    *           : CONTROLLER_STATUS(ctl_status) == kControllerStatusFailed       ? "Failed"
    *           : CONTROLLER_STATUS(ctl_status) == kControllerStatusBadZMRMode   ? "Bad ZMR"
    *           : CONTROLLER_STATUS(ctl_status) == kControllerStatusCacheProblem ? "Cache Problem"
    *           : CONTROLLER_STATUS(ctl_status) == kControllerStatusIncompatibleCacheModule ? "Incompatible Cache"
    *           : CONTROLLER_STATUS(ctl_status) == kControllerStatusDirtyCacheNeedsFirmwareDowngrade ? "Dirty Cache Requires Downgrade"
    *           : "UNKNOWN");
    * @endcode
    */
   #define CONTROLLER_STATUS(info_value) ((SA_BYTE)(info_value & kControllerStatusMask))
   /** @} */

   /** @name Controller Cache Statuses
    * @outputof SA_GetControllerStatusInfo for Controller cache status.
    * @attention Valid only if controller status is not failed (see @ref CONTROLLER_STATUS).
    * @{ */
   #define kControllerCacheStatusOK                    0x01  /**< Controller cache OK; @outputof SA_GetControllerStatusInfo. */
   #define kControllerCacheStatusNotConfigured         0x02  /**< Controller cache not configured; @outputof SA_GetControllerStatusInfo. */
   #define kControllerCacheStatusTemporarilyDisabled   0x03  /**< Controller cache temporarily disabled; @outputof SA_GetControllerStatusInfo. */
   #define kControllerCacheStatusPermanentlyDisabled   0x04  /**< Controller cache permanently disabled; @outputof SA_GetControllerStatusInfo. */
   /** Use to mask output of @ref SA_GetControllerStatusInfo for the status of the cache module of a Controller.
    * @see CACHE_STATUS.
    */
   #define kControllerCacheStatusMask                  0x000000F0
   /** Use to decode output of @ref SA_GetControllerStatusInfo for the status of the Controller.
    * @see CACHE_STATUS.
    */
   #define kControllerCacheStatusBitShift              4
   /** Use to decode Controller cache status output of @ref SA_GetControllerStatusInfo.
    *
    * __Example__
    * @code
    * SA_DWORD ctl_status = SA_GetControllerStatusInfo(cda);
    * if (CONTROLLER_STATUS(ctl_status) != kControllerStatusFailed)
    * {
    *     printf("Cache Status: %s\n",
    *                 CACHE_STATUS(ctl_status) == kControllerCacheStatusOK ? "OK"
    *                 : CACHE_STATUS(ctl_status) == kControllerCacheStatusNotConfigured ? "Not Configured"
    *                 : CACHE_STATUS(ctl_status) == kControllerCacheStatusTemporarilyDisabled ? "Temporarily Disabled"
    *                 : CACHE_STATUS(ctl_status) == kControllerCacheStatusPermanentlyDisabled ? "Permanently Disabled"
    *                 : "UNKNOWN");
    * }
    * @endcode
    */
   #define CACHE_STATUS(info_value) ((SA_BYTE)((info_value & kControllerCacheStatusMask) >> kControllerCacheStatusBitShift))
   /** @} */

   /** @name Controller Cache States If Cache Disabled
    * @outputof SA_GetControllerStatusInfo for Controller disabled cache state.
    * @attention Valid only if cache is disabled (see @ref kControllerCacheStatusTemporarilyDisabled and @ref kControllerCacheStatusPermanentlyDisabled).
    * @{ */
   /** Cache temporarily disabled; data on the cache does not match the controller.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState0BoardMismatch      0x00
   /** Cache temporarily disabled; battery charge level too low.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState1                   0x01
   /** Cache temporarily disabled; cache disabled by Flush/Disabled Posted-Write Cache command.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState2                   0x02
   /** Cache temporarily disabled; not enough resources.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState3                   0x03
   /** Cache temporarily disabled; cache module not attached.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState4                   0x04
   /** Cache temporarily disabled; transformation in progress.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState5                   0x05
   /** Cache temporarily disabled; green backup version running does not match the ROM. Power cycle (cold boot) the
    * system to complete the firmware update process.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState6                   0x06
   /** Cache temporarily disabled; green backup processor not responding.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState7                   0x07
   /** Cache temporarily disabled; a cache size mismatch exists between the two controllers in a redundant environment.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState8                   0x08
   /** Cache temporarily disabled; cache failure has occurred on the other controller in a redundant environment.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState9                   0x09
   /** Cache temporarily disabled; RAID 6 enablement mechanism is broken or missing.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState10                  0x0A
   /** Deprecated.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState11                  0x0B
   /** Cache temporarily disabled; capacitor charge level too low.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState12                  0x0C
   /** Cache temporarily disabled; cache module flash memory erase in progress.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState13                  0x0D
   /** Cache temporarily disabled; backup failure has been cleared and controller needs to be rebooted to complete recovery.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState14                  0x0E
   /** Deprecated.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState15                  0x0F
   /** Deprecated.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState16                  0x10
   /** Cache permanently disabled; error reading from cache module.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState17CannotReadData    0x11
   /** Cache permanently disabled; error writing to cache module.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState18CannotWriteData   0x12
   /** Cache permanently disabled; Set Configuration was issued. Issue a Set Cache Configuration command to re-enable
    * cache.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState19                  0x13
   /** Cache permanently disabled; large number of ECC errors detected.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState20                  0x14
   /** Cache permanently disabled; large number of ECC errors detected while testing the cache during POST.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState21                  0x15
   /** Cache permanently disabled; battery was hot removed.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState22                  0x16
   /** Cache permanently disabled; not enough batteries attached to the controller to ensure write-cache will be held
    * without power for the required time period.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState23                  0x17
   /** Cache permanently disabled; cache module not supported by firmware.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState24                  0x18
   /** Cache permanently disabled; wrong backup power source attached.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState25                  0x19
   /** Cache permanently disabled; backup power source not attached.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState26                  0x1A
   /** Cache permanently disabled; backup to flash memory failed.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState27                  0x1B
   /** Cache permanently disabled; restore from flash memory failed.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState28                  0x1C
   /** Cache permanently disabled; flash memory/power source hardware failure.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState29                  0x1D
   /** Cache permanently disabled; power source failed to charge.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState30                  0x1E
   /** Cache permanently disabled; incompatible flash module is being used.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState31                  0x1F
   /** Cache permanently disabled; charger circuit on the flash module has failed.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState32                  0x20
   /** Cache permanently disabled; cable error.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheState33                  0x21
   /** Use to mask output of @ref SA_GetControllerStatusInfo for the state of the disabled cache module of a Controller.
    * @see CONTROLLER_CACHE_STATE.
    */
   #define kControllerCacheStateMask                0x0000FF00
   /** Use to decode output of @ref SA_GetControllerStatusInfo for the state of the disabled cache module of a Controller.
    * @see CONTROLLER_CACHE_STATE.
    */
   #define kControllerCacheStateBitShift            8
   /** Use to decode Controller's disabled cache state output of @ref SA_GetControllerStatusInfo.
    *
    * __Example__
    * @code
    * SA_DWORD ctl_status = SA_GetControllerStatusInfo(cda);
    * if (CONTROLLER_STATUS(ctl_status) != kControllerStatusFailed)
    * {
    *     if (CACHE_STATUS(ctl_status) == kControllerCacheStatusTemporarilyDisabled ||
    *         CACHE_STATUS(ctl_status) == kControllerCacheStatusPermanentlyDisabled)
    *     {
    *         printf("Cache State: %s\n",
    *                     CONTROLLER_CACHE_STATE(ctl_status) == kControllerCacheState0BoardMismatch ? "Board/Cache mismatch"
    *                     : CONTROLLER_CACHE_STATE(ctl_status) == kControllerCacheState17CannotReadData ? "Failed Read"
    *                     : CONTROLLER_CACHE_STATE(ctl_status) == kControllerCacheState18CannotWriteData ? "Failed Write"
    *                     : "UNKNOWN");
    *     }
    * }
    * @endcode
    */
   #define CONTROLLER_CACHE_STATE(info_value) ((SA_BYTE)((info_value & kControllerCacheStateMask) >> kControllerCacheStateBitShift))
   /** @} */

   /** @name Controller Cache States Under Any Cache Status
    * @outputof SA_GetControllerStatusInfo for Controller cache state.
    * @attention Valid for any cache state.
    * @{ */
   /** Controller cache state: Valid data was found in the cache.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheFlagsValidDataFoundInCache        0x01
   /** Controller cache state: Low battery at boot.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheFlagsLowBatteryAtBoot             0x02
   /** Controller cache state: Flash memory detached. Cache Module flash memory is not installed.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheFlagsFlashMemoryDetached          0x04
   /** Controller cache state: The cache module is degraded and running at failsafe speeds.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheFlagsModuleRunningAtFailsafeSpeed 0x08
   /** Controller cache state: Other.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheFlagsOtherState                   0x10
   /** Controller cache state: Dirty software RAID cache.
    * @outputof SA_GetControllerStatusInfo.
    */
   #define kControllerCacheFlagsDirtySoftwareRAIDCache       0x20
   /** Use to mask output of @ref SA_GetControllerStatusInfo for the state of the cache module of a Controller.
    * @see CONTROLLER_CACHE_FLAGS.
    */
   #define kControllerCacheFlagsMask                         0x00FF0000
   /** Use to decode output of @ref SA_GetControllerStatusInfo for the state of the cache module of a Controller.
    * @see CONTROLLER_CACHE_FLAGS.
    */
   #define kControllerCacheFlagsBitShift                     16
   /** Use to decode Controller's cache state output of @ref SA_GetControllerStatusInfo.
    *
    * __Example__
    * @code
    * SA_DWORD ctl_status = SA_GetControllerStatusInfo(cda);
    * if (CONTROLLER_STATUS(ctl_status) != kControllerStatusFailed)
    * {
    *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsValidDataFoundInCache)
    *         printf("Cache Flags: Valid Data Exists\n");
    *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsLowBatteryAtBoot)
    *         printf("Cache Flags: Low battery at boot\n");
    *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsFlashMemoryDetached)
    *         printf("Cache Flags: Flash memory detached\n");
    *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsModuleRunningAtFailsafeSpeed)
    *         printf("Cache Flags: Running at failsafe speed\n");
    *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsOtherState)
    *         printf("Cache Flags: other\n");
    * #if defined(ENABLE_SOFTWARE_RAID_SUPPORT)
    *     if (CONTROLLER_CACHE_FLAGS(ctl_status) & kControllerCacheFlagsDirtySoftwareRAIDCache)
    *         printf("Cache Flags: Dirty software RAID cache\n");
    * #endif
    * }
    * @endcode
    */
   #define CONTROLLER_CACHE_FLAGS(info_value) ((SA_BYTE)((info_value & kControllerCacheFlagsMask) >> kControllerCacheFlagsBitShift))
   /** @} */

   /** @name Controller Configuration Statuses
    * @outputof SA_GetControllerStatusInfo for Controller configuration status.
    * @{ */
   #define kControllerConfigStatusOK                      0x01  /**< Controller configuration status: OK; @outputof SA_GetControllerStatusInfo. */
   #define kControllerConfigStatusVersionMismatch         0x02  /**< Controller configuration status: Version mismatch; @outputof SA_GetControllerStatusInfo. */
   #define kControllerConfigStatusBadEncryptionOverride   0x04  /**< Controller configuration status: Bad encryption; @outputof SA_GetControllerStatusInfo. */
   #define kControllerConfigStatusBadLUCacheOverride      0x08  /**< Controller configuration status: Bad LU cache; @outputof SA_GetControllerStatusInfo. */
   #define kControllerConfigStatusAbnormalVolumeState     0x10  /**< Controller configuration status: A volume on the controller is in abnormal state; @outputof SA_GetControllerStatusInfo. */
   #define kControllerConfigStatusAbnormalVolumeStateSEDLockedWithDirtyCache  0x20 /**< Controller configuration status: A volume on the controller is in a SED related abnormal state; @outputof SA_GetControllerStatusInfo. */
   /** Use to mask output of @ref SA_GetControllerStatusInfo for the configuration status of the Controller.
    * @see CONTROLLER_CONFIG_STATUS.
    */
   #define kControllerConfigStatusMask                    0xFF000000
   /** Use to decode output of @ref SA_GetControllerStatusInfo for the configuration status of the Controller.
    * @see CONTROLLER_CONFIG_STATUS.
    */
   #define kControllerConfigStatusBitShift                24
   /** Use to decode Controller's configuration status output of @ref SA_GetControllerStatusInfo.
    *
    * __Example__
    * @code
    * SA_DWORD ctl_status = SA_GetControllerStatusInfo(cda);
    * printf("Controller Config Statuses\n");
    * if (CONTROLLER_CONFIG_STATUS(ctl_status) & kControllerConfigStatusOK)
    *     printf(" - OK\n");
    * if (CONTROLLER_CONFIG_STATUS(ctl_status) & kControllerConfigStatusVersionMismatch)
    *     printf(" - Version mismatch\n");
    * if (CONTROLLER_CONFIG_STATUS(ctl_status) & kControllerConfigStatusBadEncryptionOverride)
    *     printf(" - Bad Encryption\n");
    * if (CONTROLLER_CONFIG_STATUS(ctl_status) & kControllerConfigStatusBadLUCacheOverride)
    *     printf(" - Bad Smart Cache\n");
    * @endcode
    */
   #define CONTROLLER_CONFIG_STATUS(info_value) ((SA_BYTE)((info_value & kControllerConfigStatusMask) >> kControllerConfigStatusBitShift))
   /** @} */

/** @} */ /* storc_status_ctrl_status */

/** @defgroup storc_status_ctrl_online_firmware_activation Controller Online Firmware Activation Status
 * @{ */
/** @name Controller Online Firmware Activation Statuses
 * Values for @ref SA_CONTROLLER_OFA_STATUS.status
 * @see storc_status_ctrl_online_firmware_activation
 * @{ */
/** Online firmware activation is not running. */
#define kControllerOnlineFirmwareActivationStatusNotRunning                     0x00
/** Online firmware activation is not running and the last attempted failed. */
#define kControllerOnlineFirmwareActivationStatusNotRunningLastAttemptFailed    0x01
/** Online firmware activation is not running and the last attempt was aborted. */
#define kControllerOnlineFirmwareActivationStatusNotRunningLastAttemptAborted   0x02
/** Online firmware activation is scheduled to begin in @ref SA_CONTROLLER_OFA_STATUS.delay_remaining_minutes minutes.
 * @attention Most operations will be blocked during this status.
 */
#define kControllerOnlineFirmwareActivationStatusPendingInitiate                0x03
/** Online firmware activation is in progress: flashing & disabling cache. */
#define kControllerOnlineFirmwareActivationStatusInProgressFlushingCache        0x04
/** Online firmware activation is in progress: pausing background operations. */
#define kControllerOnlineFirmwareActivationStatusInProgressPausingBackgroundOps 0x05
/** Online firmware activation is in progress: performing final checks. */
#define kControllerOnlineFirmwareActivationStatusInProgressFinalCheck           0x06
/** Online firmware activation is in progress: pausing host I/O. */
#define kControllerOnlineFirmwareActivationStatusInProgressPausingHostIO        0x07
/** Online firmware activation is in progress: preparing metadata. */
#define kControllerOnlineFirmwareActivationStatusInProgressPreparingMetadata    0x08
/** Online firmware activation is in progress: issuing reset. */
#define kControllerOnlineFirmwareActivationStatusInProgressIssuingReset         0x09
/** Online firmware activation is scheduled to begin in @ref SA_CONTROLLER_OFA_STATUS.delay_remaining_minutes minutes.
 * @attention Most operations will be allowed during this status.
 */
#define kControllerOnlineFirmwareActivationStatusDelayingInitiate               0x0A
/** Online firmware activation status/progress is unknown. */
#define kControllerOnlineFirmwareActivationStatusUnknown                        0xFF
/** Using @ref SA_CONTROLLER_OFA_STATUS.status, return if OFA is pending/in-progress/running. */
#define SA_IS_CONTROLLER_OFA_RUNNING(status) \
      ((SA_BOOL)(((status) != kControllerOnlineFirmwareActivationStatusNotRunning && \
                  (status) != kControllerOnlineFirmwareActivationStatusNotRunningLastAttemptFailed && \
                  (status) != kControllerOnlineFirmwareActivationStatusDelayingInitiate && \
                  (status) != kControllerOnlineFirmwareActivationStatusNotRunningLastAttemptAborted) ? kTrue : kFalse))
/** Using @ref SA_CONTROLLER_OFA_STATUS.status, return if OFA is pending/scheduled. */
#define SA_IS_CONTROLLER_OFA_SCHEDULED(status) \
      ((SA_BOOL)(((status) == kControllerOnlineFirmwareActivationStatusPendingInitiate || \
                  (status) == kControllerOnlineFirmwareActivationStatusDelayingInitiate) ? kTrue : kFalse))
/** Using @ref SA_CONTROLLER_OFA_STATUS.status, return if OFA has started (in-progress). */
#define SA_HAS_CONTROLLER_OFA_STARTED(status) \
      ((SA_BOOL)((SA_IS_CONTROLLER_OFA_RUNNING(status) && \
                  !SA_IS_CONTROLLER_OFA_SCHEDULED(status)) ? kTrue : kFalse))
/** Using @ref SA_CONTROLLER_OFA_STATUS.status, return if the last OFA attempt failed or was aborted. */
#define SA_LAST_CONTROLLER_OFA_ATTEMPT_FAILED(status) \
      ((SA_BOOL)(((status) == kControllerOnlineFirmwareActivationStatusNotRunningLastAttemptFailed || \
                  (status) == kControllerOnlineFirmwareActivationStatusNotRunningLastAttemptAborted) ? kTrue : kFalse))
/** @} */

/** @name Controller Online Firmware Activation Progress Percent
 * Values for @ref SA_CONTROLLER_OFA_STATUS.progress_percent
 * @see storc_status_ctrl_online_firmware_activation
 * @{ */
/** Defines progress/percent when the online firmware activation is not running. */
#define kControllerOnlineFirmwareActivationProgressPercentInvalid  101
/** @} */

/** Controller Online Firmware Activation status information.
 * @outputof SA_GetControllerOnlineFirmwareActivationStatus
 * @see storc_status_ctrl_online_firmware_activation
 */
struct _SA_CONTROLLER_OFA_STATUS
{
   SA_UINT8 status; /**< Controller Online Firmware Activation status.
                      * One of [Controller Online Firmware Activation Statuses](@ref storc_status_ctrl_online_firmware_activation).
                      * @see storc_status_ctrl_online_firmware_activation
                      */

   SA_UINT8 progress_percent; /**< For a running online firmware activation, report the overall progress percent.
                               * @pre @ref SA_CONTROLLER_OFA_STATUS.status reports online firmware activation as running.
                               * @pre @ref SA_IS_CONTROLLER_OFA_RUNNING on @ref SA_CONTROLLER_OFA_STATUS.status is @ref kTrue.
                               * Will be set to @ref kControllerOnlineFirmwareActivationProgressPercentInvalid
                               * when online firmware activation is not running.
                               */

   SA_UINT16 delay_requested_minutes; /**< Report the original/requested online firmware activation delay (in minutes).
                                       * @pre @ref SA_CONTROLLER_OFA_STATUS.status is @ref kControllerOnlineFirmwareActivationStatusPendingInitiate.
                                       */
   SA_UINT16 delay_remaining_minutes; /**< Report the remaining online firmware activation countdown (in minutes).
                                       * @pre @ref SA_CONTROLLER_OFA_STATUS.status is @ref kControllerOnlineFirmwareActivationStatusPendingInitiate.
                                       */
};
typedef struct _SA_CONTROLLER_OFA_STATUS SA_CONTROLLER_OFA_STATUS;
typedef struct _SA_CONTROLLER_OFA_STATUS *PSA_CONTROLLER_OFA_STATUS;

/** Update the information returned by the next call to @ref SA_GetControllerOnlineFirmwareActivationStatus.
 * Use to update controller online firmware activation status without calling @ref SA_ReInitializeCDA.
 * @param[in] cda Controller data area.
 * @see storc_status_ctrl_online_firmware_activation
 */
void SA_UpdateControllerOnlineFirmwareActivationStatus(PCDA cda);

/** Get the controller online firmware activation.
 * Online firmware activation status information can be updated by first calling:
 * - @ref SA_ReInitializeCDA
 * - @ref SA_UpdateControllerOnlineFirmwareActivationStatus
 * @param[in]  cda         Controller data area.
 * @param[out] ofa_status  Controller online firmware activation information.
 * @return @ref kTrue on success, @ref kFalse otherwise.
 * @return `ofa_status` will contain the online firmware activation status (iff return is @ref kTrue).
 * @see storc_status_ctrl_online_firmware_activation
 */
SA_BOOL SA_GetControllerOnlineFirmwareActivationStatus(PCDA cda, PSA_CONTROLLER_OFA_STATUS ofa_status);

/** @} */ /* storc_status_ctrl_online_firmware_activation */

/** @defgroup storc_status_ctrl_backup_power_source_status Controller Backup Power Source Status
 * @{ */
/** Get the status of the battery or capacitor.
 *
 * @param[in] cda  Controller data area.
 * @return Backup power source status (see [Controller Backup Power Source Status Returns](@ref storc_status_ctrl_backup_power_source_status).
 */
SA_WORD SA_GetControllerBackupPowerSourceStatus(PCDA cda);
   /** @name Controller Backup Power Source Status Returns
    * @outputof SA_GetControllerBackupPowerSourceStatus.
    * @{ */
   #define kControllerBackupPowerSourceFailed               0  /**< Backup power failed on Controller; @outputof SA_GetControllerBackupPowerSourceStatus. */
   #define kControllerBackupPowerSourceNotPresent           1  /**< Backup power no present on Controller; @outputof SA_GetControllerBackupPowerSourceStatus. */
   #define kControllerBackupPowerSourcePresentAndCharging   2  /**< Backup power charging on Controller; @outputof SA_GetControllerBackupPowerSourceStatus. */
   #define kControllerBackupPowerSourcePresentAndCharged    3  /**< Backup power charged on Controller; @outputof SA_GetControllerBackupPowerSourceStatus. */
   /** @} */
/** @} */ /* storc_status_ctrl_backup_power_source_status */

/** @defgroup storc_properties_ctrl_backup_power_source_info Controller Backup Power Source Info
 * @brief Get a controller's backup power source info.
 * @details
 * @see storc_features_ctrl_backup_power_source_info_supported
 * @{ */

/** @name Controller Backup Power Source Info
 * @{ */
/** Controller Backup Power Source Info.
 * Returned by @ref SA_GetControllerBackupPowerSourceInfo.
 */
struct _SA_BACKUP_POWER_SOURCE_INFO
{
   SA_INT16   max_temperature;     /**< Maximum temperature in degrees Celsius recorded during the super capacitor's lifetime. */
   SA_UINT16  max_voltage;         /**< Maximum voltage in milivolts recorded during the super capacitor's lifetime. */
   SA_INT16   temperature;         /**< Current temperature in degrees Celsius of the super capacitor pack. */
   SA_UINT16  voltage;             /**< Current voltage in millivolts of the super capacitor pack. */
   SA_INT16   current;             /**< Active current draw in milliamps of the super capacitor charging circuit. */
   SA_INT16   temp_threshold;      /**< Maximum allowable temperature in degrees Celsius for the super capacitor. */
   SA_UINT16  health;              /**< Predicted health of the super capacitor expressed as a percentage of initial capacitance. */
   SA_UINT16  charge_state;        /**< Relative charge expressed as a percentage. */
   SA_UINT16  status;              /**< Backup power status. See [Backup Power Source Status](@ref storc_properties_ctrl_backup_power_source_info) */
};
typedef struct _SA_BACKUP_POWER_SOURCE_INFO SA_BACKUP_POWER_SOURCE_INFO;
/** @} */ /* Controller Backup Power Source Info */

/** @name Backup Power Source Status
 * @outputof SA_GetControllerBackupPowerSourceInfo
 * @{ */
#define kBackupPowerSourceInfoStatusUninitialized           ( 1 << 0 )  /**< Backup power is uninitialized. */
#define kBackupPowerSourceInfoStatusReady                   ( 1 << 1 )  /**< Backup power is ready. */
#define kBackupPowerSourceInfoStatusLearningCyle            ( 1 << 2 )  /**< Backup power is executing a learning cycle */
#define kBackupPowerSourceInfoStatusFailed                  ( 1 << 3 )  /**< Backup power subsystem has failed. */
#define kBackupPowerSourceInfoStatusAboveMaxTemperature     ( 1 << 4 )  /**< Backup power has exceeded the maximmum temperature threshold. */
#define kBackupPowerSourceInfoStatusAboveWarningTemperature ( 1 << 5 )  /**< Backup power has exceeded the warning temperature threshold. */
#define kBackupPowerSourceInfoStatusOverVoltage             ( 1 << 6 )  /**< Backup power is over voltage. */
#define kBackupPowerSourceInfoStatusAboveMaxChargingCurrent ( 1 << 7 )  /**< Backup power has exceeded maximum charging current. */
#define kBackupPowerSourceInfoStatusLearningCyclePassed     ( 1 << 8 )  /**< Backup power learning cycle has passed. */
#define kBackupPowerSourceInfoStatusLearningCyleFailed      ( 1 << 9 )  /**< Backup power learning cycle has failed. */
#define kBackupPowerSourceInfoStatusSupercapacitorFailed    ( 1 << 10 ) /**< Backup power super capacitor has failed. */
#define kBackupPowerSourceInfoStatusSupercapacitorNearEOL   ( 1 << 11 ) /**< Backup power super capacitor is nearing the ened of its life. */
#define kBackupPowerSourceInfoStatusSupercapacitorPassedEOL ( 1 << 12 ) /**< Backup power super capacitor has reached the end of its life. */
#define kBackupPowerSourceInfoStatusSupercapacitorMissing   ( 1 << 13 ) /**< One of the super capacitors in the super capacitor pack is missing. */
 /** @} */

/** Get the controller backup power source info.
 *
 * @param[in] cda  Controller data area.
 * @param[in] backup_power_source_info  Pointer to pre-allocated BACKUP_POWER_SOURCE_INFO struct where backup power source info will be saved.
 * @return [Controller Backup Power Source Info Returns](@ref storc_properties_ctrl_backup_power_source_info).
 */
SA_BYTE SA_GetControllerBackupPowerSourceInfo(PCDA cda, SA_BACKUP_POWER_SOURCE_INFO* backup_power_source_info);

/** @name Controller Backup Power Source Info Returns
 * @outputof SA_GetControllerBackupPowerSourceInfo.
 * @{ */
#define kGetControllerBackupPowerSourceInfoSuccess                   0x01   /**< Get the OOB configuration successfully. */
#define kGetControllerBackupPowerSourceInfoNotSupported              0x02   /**< Backup power source info is not supported. */
#define kGetControllerBackupPowerSourceInfoPowerSourceNotConnected   0x03   /**< Backup power source is not connected. */
#define kGetControllerBackupPowerSourceInfoPowerSourceDataFailure    0x04   /**< Failure to retrieve supercap data. */
#define kGetControllerBackupPowerSourceInfoInvalidInfoBuffer         0x05   /**< Invalid backup power source info buffer given. */
 /** @} */ /* Controller Backup Power Source Info */

/** @} */ /* storc_properties_ctrl_backup_power_source_info */

/** @defgroup storc_properties_ctrl_green_backup_info Green Backup Power Info
 * @{ */
/** Controller's green backup power information filled by calling @ref SA_GetControllerGreenBackupInfo
 */
struct _GREEN_BACKUP_INFO
{
   SA_BYTE  status;                                /**< Backup power status. See [Green Backup Power Status](@ref storc_properties_ctrl_green_backup_info) */
   SA_WORD  total_backups;                         /**< Total number of backups. */
   SA_DWORD last_backup_time;                      /**< Last backup time in milliseconds. */
   SA_DWORD last_restore_time;                     /**< Last restore time in milliseconds. */
   SA_BYTE  processor_version_major;               /**< Green backup processor version major. */
   SA_BYTE  processor_version_minor;               /**< Green backup processor version minor. */
   SA_WORD  hardware_error;                        /**< Hardware error reasons. See [Green Backup Power Hardware Errors](@ref storc_properties_ctrl_green_backup_info) */
   SA_BYTE  flags;                                 /**< Green backup preservation flags. See [Green Backup Flags](@ref storc_properties_ctrl_green_backup_info) */
};
typedef struct _GREEN_BACKUP_INFO GREEN_BACKUP_INFO;

/** @name Green Backup Power Status
 * @outputof SA_GetControllerGreenBackupInfo
 * @{ */
#define kGreenBackupInfoStatusUninitialized  0 /**< Backup power is uninitialized. */
#define kGreenBackupInfoStatusDisconnected   1 /**< Backup power is disconnected. */
#define kGreenBackupInfoStatusCharging       2 /**< Backup power is charging. */
#define kGreenBackupInfoStatusCharged        3 /**< Backup power is charged. */
#define kGreenBackupInfoStatusNotCharged     4 /**< Backup power is not charged. */
#define kGreenBackupInfoStatusHotRemoved     5 /**< Backup power has been hot removed. */
#define kGreenBackupInfoStatusChargeTimeout  6 /**< Backup power timed out while charging. */
#define kGreenBackupInfoStatusFailure        7 /**< Backup power has failed. */
 /** @} */

/** @name Green Backup Power Hardware Errors
 * @outputof SA_GetControllerGreenBackupInfo
 * @{ */
#define kGreenBackupInfoHardwareNoError                  0x0000 /**< Hardware status is OK. */
#define kGreenBackupInfoHardwarePowerSupplyNotResponding 0x0001 /**< Backup power supply is not responding. */
#define kGreenBackupInfoHardwareSetBackupRegionError     0x0002 /**< Set backup region error. */
#define kGreenBackupInfoHardwareEnableProtectionError    0x0004 /**< Enable protection error. */
#define kGreenBackupInfoHardwareNANDFlashCleanupError    0x0008 /**< NAND flash cleanup erro. */
#define kGreenBackupInfoHardwareSetPreservationError     0x0010 /**< Set preservation error. */
#define kGreenBackupInfoHardwareChargeStartError         0x0020 /**< Charge start error. */
#define kGreenBackupInfoHardwareCheckPreservationError   0x0040 /**< Check preservation error. */
#define kGreenBackupInfoHardwareNANDEraseError           0x0080 /**< NAND erase error. */
#define kGreenBackupInfoHardwareUnknownError             0x8000 /**< Unknown error. */
 /** @} */

/** @name Green Backup Flags
 * @outputof SA_GetControllerGreenBackupInfo
 * @{ */
#define kGreenBackupInfoFlagsPreservationEnable  1  /**< Preservation enable. */
#define kGreenBackupInfoFlagsPreservationSet     2  /**< Preservation set. */
 /** @} */

/** Get the controller green backup information.
 *
 * @param[in] cda  Controller data area.
 * @param[in] green_backup_info  Pointer to pre-allocated GREEN_BACKUP_INFO struct where backup power source info will be saved. See [Green Backup Power Info](@ref storc_properties_ctrl_green_backup_info).
 * @return [Controller Green Backup Power Info Returns](@ref storc_properties_ctrl_green_backup_info).
 */
SA_BYTE SA_GetControllerGreenBackupInfo(PCDA cda, GREEN_BACKUP_INFO* green_backup_info);
/** @name Controller Green Backup Power Info Returns
 * @outputof SA_GetControllerGreenBackupInfo.
 * See @ref SA_ControllerSupportsGreenBackupInfo.
 * @{ */
#define kGetControllerGreenBackupInfoSuccess             0x01   /**< Green backup info retrieved successfully. */
#define kGetControllerGreenBackupInfoInvalidInfoBuffer   0x02   /**< Invalid Green backup info buffer given. */
#define kGetControllerGreenBackupInfoNotSupported        0x03   /**< Green backup info is not supported by the controller. */
#define kGetControllerGreenBackupInfoFailure             0x04   /**< Failure to retrieve green back up info. */
 /** @} */ /* Controller Green Backup Power Info Returns */

/** @} */ /* storc_properties_ctrl_green_backup_info */

/** @defgroup storc_status_ctrl_health Controller Health
 * @{ */
/** Get the controller health.
 *
 * This is the health of the controller only, not including dependent resources.
 *
 * @param[in] cda  Controller data area.
 * @return Controller health value (see [Controller Health Values](@ref storc_status_ctrl_health)).
 */
SA_BYTE SA_GetControllerHealth(PCDA cda);
  /** @name Controller Health Values
   * @outputof SA_GetControllerHealth.
   * @{ */
   #define kControllerHealthOK         0 /**< Controller OK health status; @outputof SA_GetControllerHealth. */
   #define kControllerHealthWarning    1 /**< Controller warning health status; @outputof SA_GetControllerHealth. */
   #define kControllerHealthCritical   2 /**< Controller critical health status; @outputof SA_GetControllerHealth. */
   /** @} */

/** Get the controller health rollup.
 *
 * This is the health of the controller including dependent resources.
 *
 * @param[in] cda          Controller CDA pointer
 * @return                 Controller health rollup value
 */
SA_BYTE SA_GetControllerHealthRollup(PCDA cda);
   /** @name Controller Health Rollup Values
   * @{ */
   #define kControllerHealthRollupOK         0x0
   #define kControllerHealthRollupWarning    0x1
   #define kControllerHealthRollupCritical   0x2
   #define IS_CONTROLLER_ROLLUP_CRITICAL(info_value) ((info_value & kControllerHealthRollupCritical) ? kTrue : kFalse)
   #define IS_CONTROLLER_ROLLUP_WARNING(info_value) ((info_value & kControllerHealthRollupWarning) ? kTrue : kFalse)
   /** @} */
/** @} */ /* storc_status_controller_health_rollup */

/** @defgroup storc_status_controller_reboot_required Controller Reboot Required
 * @{ */
/** Report reaons the system needs to reboot reported by the controller.
 * @param[in] cda  Controller data area.
 * @return [Reboot Required Reasons](@ref storc_status_controller_reboot_required).
 */
SA_QWORD SA_ControllerRebootRequired(PCDA cda);
/** @name Reboot Required Reasons
 * @outputof SA_ControllerRebootRequired
 * @{ */
   /** No reboot is needed.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerNoRebootRequired                              0x0000000000000000ULL
   /** At least one reboot reason requires a cold boot.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRequiredColdBoot                              0x0000000000000001ULL
   /** Reboot is recommended for power mode changes.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRebootRecommendedPowerMode                    0x0000000000000002ULL
   /** Reboot is needed for power mode changes.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRebootRequiredPowerMode                       0x0000000000000004ULL
   /** Reboot is needed for changes to controller or port mode changes.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRebootRequiredOperatingMode                   0x0000000000000008ULL
   /** Reboot is needed for changes to the OOB configuration.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRebootRequiredOOBInterface                    0x0000000000000010ULL
   /** Reboot is needed for changes to the connector discovery configuration.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRebootRequiredConnectorDiscoveryConfiguration 0x0000000000000020ULL
   /** Reboot is needed to complete a controller firmware update.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRebootRequiredControllerFirmwareUpdate        0x0000000000000040ULL
   /** Reboot is needed due to a CPLD update.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRebootRequiredCPLDUpdate                      0x0000000000000080ULL
   /** Reboot is needed for changes to the PCIe ordering mode.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRebootRequiredPCIeOrdering                    0x0000000000000100ULL
   /** Reboot is needed because an unlocked SED drive was found with configuration metadata.
    * Logical drives contained on any unlocked SED drives may not be available until a reboot
    * occurs.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRebootRequiredUnlockedSEDWithConfigMetadata   0x0000000000000200ULL
   /** Reboot is needed because controller's sanitize lock policy has changed.
    * @outputof SA_ControllerRebootRequired.
    * @attention If there are any physical SATA drives which support freeze/anti-freeze connected to
    * this controller, they must also be power-cycled or hot-plugged for the lock state to be applied.
    */
   #define kControllerRebootRequiredControllerSanitizeLockChange    0x0000000000000400ULL
   /** Reboot is needed for an unknown reason.
    * @outputof SA_ControllerRebootRequired.
    */
   #define kControllerRebootRequiredUnknownReason                   0x8000000000000000ULL
/** @} */ /* Reboot Required Reasons */

/** @} */ /* storc_status_controller_reboot_required */

/** @} */ /* storc_status_ctrl */

/**************************************//**
 * @defgroup storc_status_ld Logical Drive
 * @brief
 * @details
 * @{
 *****************************************/

/** @defgroup storc_status_ld_present Logical Drive Present Check
* @{ */
/** Determine if drive number is a configured logical drive.
 * Should return @ref kTrue for any ld_number returned by @ref SA_EnumerateControllerLogicalDrives.
 * @param[in] cda Controller data area.
 * @param[in] ld_number Logical drive number to test.
 * @return @ref kTrue iff the test `ld_number` is a configured logical drive, @ref kFalse otherwise.
 */
SA_BOOL SA_IsLogicalDrivePresent(PCDA cda, SA_WORD ld_number);
/** @} */ /* storc_status_ld_present */

/** @defgroup storc_status_ld_status Logical Drive Status
* @{ */
/** Obtain status info for a Logical Drive.
 * @pre Logical drive `ld_number` exists.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Valid logical drive number.
 * @return Status map; see [Logical Drive Statuses](@ref storc_status_ld) & [Logical Drive Encryption Statuses](@ref storc_status_ld).
 * @return Use @ref LOGICAL_DRIVE_UNIT_STATUS and @ref LOGICAL_DRIVE_ENCRYPTION_STATUS to decode (in @ref storc_status_ld).
 *
 * __Example__
 * @code
 * SA_DWORD ld_status = SA_GetLogicalDriveStatusInfo(cda, ld_number);
 * printf("Logical Drive %u Status: %s\n", ld_number,
 *             LOGICAL_DRIVE_UNIT_STATUS(ld_status) == kLogicalDriveUnitStatusOK ? "OK"
 *             : LOGICAL_DRIVE_UNIT_STATUS(ld_status) == kLogicalDriveUnitStatus1Failed ? "Failed"
 *             : LOGICAL_DRIVE_UNIT_STATUS(ld_status) == kLogicalDriveUnitStatus2NotConfigured ? "Not Configured"
 *             : ... );
 * // ...
 * @endcode
 */
SA_DWORD SA_GetLogicalDriveStatusInfo(PCDA cda, SA_WORD ld_number);
   /** @name Logical Drive Statuses
    * @outputof SA_GetLogicalDriveStatusInfo.
    * @{ */
   #define kLogicalDriveUnitStatusOK                                        0x00  /**< Logical Drive status OK; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus1Failed                                   0x01  /**< Logical Drive status failed; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus2NotConfigured                            0x02  /**< Logical Drive status not configured; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus3InterimRecovery                          0x03  /**< Logical Drive status interim recovery; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus4ReadyForRecovery                         0x04  /**< Logical Drive status ready for recovery; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus5Recovering                               0x05  /**< Logical Drive status recovering; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus6WrongDriveReplaced                       0x06  /**< Logical Drive status wrong drive replaced; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus7DriveImproperlyConnected                 0x07  /**< Logical Drive status drive improperly connected; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus8HardwareIsOverheating                    0x08  /**< Logical Drive status hardware is over-heating; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus9HardwareHasOverheated                    0x09  /**< Logical Drive status hardware has over-heated; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus10Expanding                               0x0A  /**< Logical Drive status expanding; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus11NotYetAvailable                         0x0B  /**< Logical Drive status not available; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus12QueuedForExpansion                      0x0C  /**< Logical Drive status queued for expansion; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus13DisabledFromSCSIIDConflict              0x0D  /**< Logical Drive status disabled from SCSI conflict; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus14Ejected                                 0x0E  /**< Logical Drive status ejected; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus15EraseInProgress                         0x0F  /**< Logical Drive status erase in progress; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus16Unused                                  0x10  /**< Logical Drive status unused; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus17ReadyToPerformPredictiveSpareRebuild    0x11  /**< Logical Drive status ready to perform predictive spare rebuild; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus18RPIInProgress                           0x12  /**< Logical Drive status RPI in progress; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus19RPIQueued                               0x13  /**< Logical Drive status RPI queued; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus20EncryptedVolumeWithoutKey               0x14  /**< Logical Drive status encrypted without key (missing boot password or foreign key needed); @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus21PlaintextVolumeRejectedInEncryptingMode 0x15  /**< Logical Drive status plaintext volume rejected in encryption mode; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus22EncryptionMigration                     0x16  /**< Logical Drive status encryption migration; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus23EncryptedVolumeRekeying                 0x17  /**< Logical Drive status encrypted & re-keying; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus24EncryptedVolumeEncryptionOff            0x18  /**< Logical Drive status encrypted with encryption off; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus25VolumeEncodeRequested                   0x19  /**< Logical Drive status volume encode requested; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus26EncryptedVolumeRekeyRequested           0x1A  /**< Logical Drive status encrypted with re-key request; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus27UnsupportedOnThisController             0x1B  /**< Logical Drive status unsupported; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus28HasLockedSED                            0x1C  /**< Logical Drive status has locked SED; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus29SEDQualificationOngoing                 0x1D  /**< Logical Drive status SED qualification ongoing; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveUnitStatus30SEDQualificationFailed                  0x1E  /**< Logical Drive status SED qualification failed - use @ref SA_GetPhysicalDriveEncryptionInfo to know which physical drive caused this failure; @outputof SA_GetLogicalDriveStatusInfo. */
   /** Use to mask output of @ref SA_GetLogicalDriveStatusInfo for the status of a logical drive.
    * @see LOGICAL_DRIVE_UNIT_STATUS.
    */
   #define kLogicalDriveUnitStatusMask                                0x000000FF
   /** Use to decode logical drive's unit status output of @ref SA_GetLogicalDriveStatusInfo.
    *
    * __Example__
    * @code
    * SA_DWORD ld_status = SA_GetLogicalDriveStatusInfo(cda, ld_number);
    * printf("Logical Drive %u Unit Status: %s\n", ld_number,
    *             LOGICAL_DRIVE_UNIT_STATUS(ld_status) == kLogicalDriveUnitStatusOK ? "OK"
    *             : LOGICAL_DRIVE_UNIT_STATUS(ld_status) == kLogicalDriveUnitStatus1Failed ? "Failed"
    *             : LOGICAL_DRIVE_UNIT_STATUS(ld_status) == kLogicalDriveUnitStatus2NotConfigured ? "Not Configured"
    *             : ... );
    * @endcode
    */
   #define LOGICAL_DRIVE_UNIT_STATUS(info_value) ((SA_BYTE)(info_value & kLogicalDriveUnitStatusMask))

   /** The logical drive failed during a transformation operation. @outputof SA_GetLogicalDriveStatusInfo  */
   #define kLogicalDriveStatusFailedDuringTransformation    0x00000100
   /** The logical drive is failed and is ready to be re-enabled. @outputof SA_GetLogicalDriveStatusInfo  */
   #define kLogicalDriveStatusMediaExchangedReadyToReenable 0x00000200

   /** @} */ /* Logical Drive Statuses */
/** @} */ /* storc_status_ld_status */

/** @defgroup storc_status_ld_health Logical Drive Health
 * @{ */
/** Get the logical drive health.
 * @pre `ld_number` is a valid logical drive number.
 * This is the health of the logical drive only, not including dependent resources.
 *
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @return Controller health value (see [Logical Drive Health Values](@ref storc_status_ld_health)).
 */
SA_BYTE SA_GetLogicalDriveHealth(PCDA cda, SA_WORD ld_number);
   /** @name Logical Drive Health Values
    * @outputof SA_GetLogicalDriveHealth.
    * @{ */
   #define kLogicalDriveHealthOK         0 /**< Logical drive OK health status; @outputof SA_GetLogicalDriveHealth. */
   #define kLogicalDriveHealthWarning    1 /**< Logical drive warning health status; @outputof SA_GetLogicalDriveHealth. */
   #define kLogicalDriveHealthCritical   2 /**< Logical drive critical health status; @outputof SA_GetLogicalDriveHealth. */
   /** @} */
/** @} */ /* storc_status_ld_health */

/** @defgroup storc_status_ld_blocks_left_to_recover Logical Drive Blocks Left To Recover
 * @{ */
/** Get the number of blocks left to recover on a logical drive.
 *
 * This is used for transformations, expansions, rebuilds, and parity initialization.
 *
 * @pre `ld_number` is a valid logical drive number.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @return Blocks left to recover.
 */
SA_QWORD SA_GetLogicalDriveBlocksLeftToRecover(PCDA cda, SA_WORD ld_number);
/** @} */ /* storc_status_ld_blocks_left_to_recover */

/** @defgroup storc_status_ld_lu_cache_status LU Cache Status
 * @brief Get status/state information of a data/cache logical drive pair.
 * @details
 * @see storc_properties_lu_ld
 * @{ */
/** @name LU Cache State/Status
 * @{ */
/** Returns information of a LU cache status/state.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Current logical drive which is either a LU cached or caching logical drive.
 * @return Decode with @ref LU_CACHE_STATE to get [possible cache states](@ref storc_status_ld_lu_cache_status).
 * @return Decode with @ref LU_CACHE_WRITE_POLICY_REASON to get [possible cache write policy reasons](@ref storc_status_ld_lu_cache_status).
 */
SA_WORD SA_GetLogicalDriveLUCacheStatusInfo(PCDA cda, SA_WORD ld_number);
   /** LU cache state 'OK'; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kLUCacheStateGood                  0x01
   /** LU cache state 'limited'; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kLUCacheStateLimited               0x02
   /** LU cache state 'DDR unsage'; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kLUCacheStateDDRUnsafe             0x03
   /** LU cache state 'cache LD offline'; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kLUCacheStateCacheLUNOffline       0x04
   /** LU cache state 'data LD offline'; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kLUCacheStateDataLUNOffline        0x08
   /** LU cache state 'destroyed'; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kLUCacheStateDestroyed             0x10
   /** LU cache state 'flushing'; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kLUCacheStateFlushing              0x20
   /** LU cache state 'configuring'; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kLUCacheStateConfiguring           0x40
   /** LU cache state 'powerup fail'; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kLUCacheStatePowerupFail           0x80

   /** Used by @ref LU_CACHE_STATE to decode @ref SA_GetLogicalDriveLUCacheStatusInfo. */
   #define kLUCacheStateMask                  0x00FF
   /** Used by @ref LU_CACHE_STATE to decode @ref SA_GetLogicalDriveLUCacheStatusInfo. */
   #define kLUCacheStateBitshift              0
   /** Decode LU cache state from [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define LU_CACHE_STATE(info) ((info & kLUCacheStateMask) >> kLUCacheStateBitshift)

   /** LU cache write policy is set to desired setting; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kWritePolicyStatusOK               0x00
   /** LU cache write policy set because DDR is unsafe; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kWritePolicyStatusDDRUnsafe        0x01
   /** LU cache write policy set because missing license; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kWritePolicyStatusNoLicense        0x02
   /** LU cache write policy set because LD is degraded; Output of [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define kWritePolicyStatusCacheLUNDegraded 0x04

   /** Used by @ref LU_CACHE_WRITE_POLICY_REASON to decode @ref SA_GetLogicalDriveLUCacheStatusInfo. */
   #define kLUCacheWritePolicyReasonMask      0x0700
   /** Used by @ref LU_CACHE_WRITE_POLICY_REASON to decode @ref SA_GetLogicalDriveLUCacheStatusInfo. */
   #define kLUCacheWritePolicyReasonBitshift  8
   /** Decode LU cache write polocy reason from [SA_GetLogicalDriveLUCacheStatusInfo](@ref storc_status_ld_lu_cache_status). */
   #define LU_CACHE_WRITE_POLICY_REASON(info) ((info & kLUCacheWritePolicyReasonMask) >> kLUCacheWritePolicyReasonBitshift)
/** @} */ /* LU Cache State/Status */

/** @name LU Cache Dirty Lines/Flush Progress
 * @{ */
/** Returns information on the number of dirty cache lines of LU cache.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Current logical drive which is either a LU cached or caching logical drive.
 * @return Decode with @ref LU_CACHE_DIRTY_LINES to get number of dirty LU cache lines (in thousands).
 * @return Decode with @ref LU_CACHE_STARTING_DIRTY_LINES to get starting number of dirty LU cache lines (in thousands).
 * @return Decode with @ref LU_CACHE_FLUSH_PROGRESS to get LU cache flush progress percent (as a double [0.0,100.0]).
 * @return Decode with @ref LU_CACHE_FLUSH_PROGRESS_INT to get LU cache flush progress percent (as a whole integer [0,100]).
 */
SA_QWORD SA_GetLogicalDriveLUCacheDirtyLinesCount(PCDA cda, SA_WORD ld_number);
   /** Get number of dirty LU cache lines (in thousands) from [SA_GetLogicalDriveLUCacheDirtyLinesCount](@ref storc_status_ld_lu_cache_status). */
   #define LU_CACHE_DIRTY_LINES(info) (info & 0x00000000FFFFFFFFULL)
   /** Get starting number of dirty LU cache lines (in thousands) from [SA_GetLogicalDriveLUCacheDirtyLinesCount](@ref storc_status_ld_lu_cache_status). */
   #define LU_CACHE_STARTING_DIRTY_LINES(info) ((info & 0xFFFFFFFF00000000ULL) >> 32)
   /** Get LU cache flush progress percent from [SA_GetLogicalDriveLUCacheDirtyLinesCount](@ref storc_status_ld_lu_cache_status). */
   #define LU_CACHE_FLUSH_PROGRESS(info) (LU_CACHE_STARTING_DIRTY_LINES(info) != 0 && LU_CACHE_STARTING_DIRTY_LINES(info) >= LU_CACHE_DIRTY_LINES(info) ? \
      ((100.0 * (double)(LU_CACHE_STARTING_DIRTY_LINES(info) - LU_CACHE_DIRTY_LINES(info))) / (double)LU_CACHE_STARTING_DIRTY_LINES(info)) : 0.0)
   #define LU_CACHE_FLUSH_PROGRESS_INT(info) (LU_CACHE_STARTING_DIRTY_LINES(info) != 0 && LU_CACHE_STARTING_DIRTY_LINES(info) >= LU_CACHE_DIRTY_LINES(info) ? \
      ((100 * (LU_CACHE_STARTING_DIRTY_LINES(info) - LU_CACHE_DIRTY_LINES(info))) / LU_CACHE_STARTING_DIRTY_LINES(info)) : 0)
/** @} */ /* LU Cache Dirty Lines/Flush Progress */

/** @} */ /* storc_status_ld_lu_cache_status */

/** @defgroup storc_status_surface_scan Logical Drive Surface Scan
 * @{ */

/** Refresh surface scan analysis information.
 *
 * Refreshes surface scan information for _all_ logical drives.
 *
 * Refreshes information given by:
 *
 * - @ref SA_GetLogicalDriveSurfaceStatus.
 *
 * @param[in] cda Controller data area.
 *
 * Calling @ref SA_ReInitializeCDA will also refresh surface analysis information.
 */
void SA_ResetSurfaceStatusInformation(PCDA cda);

/** @name Logical Drive Surface Scan Status
 * @{
 */
/** Return information on the surface scan analysis for a logical drive.
 *
 * @pre Surface scan analysis is active on the controller (see @ref storc_configuration_edit_ctrl_ssap).
 * @pre Surface scan analysis information has been updated with @ref SA_ReInitializeCDA or @ref SA_ResetSurfaceStatusInformation.
 *
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Target logical drive number.
 * @return [Bit map with information on surface scan analysis for a logical drive](@ref storc_status_surface_scan).
 */
SA_WORD SA_GetLogicalDriveSurfaceStatus(PCDA cda, SA_WORD ld_number);
   /** Surface analysis consistency initialization has completed on the logical drive.
    * @outputof SA_GetLogicalDriveSurfaceStatus.
    */
   #define kLogicalDriveSurfaceStatusScanCompleted         0x0001
   /** Surface analysis consistency check failed.
    * @outputof SA_GetLogicalDriveSurfaceStatus.
    */
   #define kLogicalDriveSurfaceStatusCheckFailed           0x0002
   /** Surface analysis has not started on the logical drive because it has not been written to yet.
    * @outputof SA_GetLogicalDriveSurfaceStatus.
    */
   #define kLogicalDriveSurfaceStatusNotStarted            0x0004
   /** The logical drive is using a fault-tolerance mode that has "parity" sectors.
    * @outputof SA_GetLogicalDriveSurfaceStatus.
    */
   #define kLogicalDriveSurfaceStatusUsesParity            0x0008
   /** Uncorrectable media defects (URE: unrecoverable read errors) were encountered on this logical drive during the previous
    * surface analysis or rebuild operation.
    *
    * If this occurred during rebuild, the media defects were propagated to the rebuilding drive so rebuild could complete
    * and make the volume fault tolerant again.
    * This bit will be cleared after any pass of surface analysis that is 100% successful (i.e. after all media errors have
    * been overwritten by the host).
    * @outputof SA_GetLogicalDriveSurfaceStatus.
    */
   #define kLogicalDriveSurfaceStatusURELastPass           0x0010
   /** Uncorrectable media defects (URE: unrecoverable read errors) were encountered on this logical drive during the current
    * surface analysis pass.
    *
    * This bit will be cleared every time Surface Analysis begins a new pass through this volume.
    * @outputof SA_GetLogicalDriveSurfaceStatus.
    */
   #define kLogicalDriveSurfaceStatusURECurrentPass        0x0020
   /** The logical drive is currently undergoing Rapid Parity Initialization (RPI).
    * @outputof SA_GetLogicalDriveSurfaceStatus.
    */
   #define kLogicalDriveSurfaceStatusRPIInProgress         0x0080
   /** The logical drive is currently being scanned.
    * @outputof SA_GetLogicalDriveSurfaceStatus.
    */
   #define kLogicalDriveSurfaceStatusScanning              0x0100
   /** The logical drive is currently undergoing parity initialization.
    * @outputof SA_GetLogicalDriveSurfaceStatus.
    */
   #define kLogicalDriveSurfaceStatusParityInitInProgress  0x0200
/** @} */ /* Logical Drive Surface Scan Status */

/** @} */ /* storc_status_surface_scan */

/** @} */ /* storc_status_ld */

/*********************************//**
 * @defgroup storc_status_array Array
 * @brief
 * @details
 * @{
 ************************************/

/** @defgroup storc_status_array_status Array Status
 * @{ */
/** Obtain status info for an Array.
 * @pre Array `array_number` exists.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Valid Array number.
 * @return Bitmap of status; see [Array Statuses](@ref storc_status_array).
 *
 * __Example__
 * @code
 * SA_DWORD array_status = SA_GetArrayStatusInfo(cda, array_number);
 * printf("Array %u Statuses:\n", array_number);
 * if (array_status & kArrayStatusOK)
 *     printf(" - Ok\n");
 * if (array_status & kArrayStatusHasFailedLogicalDrive)
 *     printf(" - Failed\n");
 * if (array_status & kArrayStatusHasFailedDataDrive)
 *     printf(" - Has Failed Data Drive\n");
 * if (array_status & kArrayStatusHasErasingDataDrive)
 *     printf(" - Has Erasing Logical Drive\n");
 * //...
 * @endcode
 */
SA_DWORD SA_GetArrayStatusInfo(PCDA cda, SA_WORD array_number);
   /** @name Array Statuses
    * @outputof SA_GetArrayStatusInfo.
    * @{ */
   #define kArrayStatusOK                                       0x00000001  /**< Array status OK; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasFailedLogicalDrive                    0x00000002  /**< Array status has a failed logical drive; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasFailedDataDrive                       0x00000004  /**< Array status has a failed data drive; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasFailedSpareDrive                      0x00000008  /**< Array status has a failed spare drive; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasErasingDataDrive                      0x00000010  /**< Array status has an erasing data drive; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasErasingLogicalDrive                   0x00000020  /**< Array status has an erasing logical drive; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasGaps                                  0x00000040  /**< Array status has gaps; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasInvalidSpare                          0x00000080  /**< Array status has invalid spare configuration; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasActiveSpare                           0x00000100  /**< Array status has active spare; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasInitializingLogicalDrive              0x00000200  /**< Array status has an initializing logical drive; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasRecoveringLogicalDrive                0x00000400  /**< Array status has a recovering logical drive; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasTransformingLogicalDrive              0x00000800  /**< Array status has a transforming logical drive; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasRekeyingLogicalDrive                  0x00001000  /**< Array status has a logical drive undergoing an encryption rekey/encode; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasQueuedLogicalDriveTransformation      0x00002000  /**< Array status has a logical drive queued for transformation; @outputof SA_GetEditableArrayStatusInfo. */
   #define kArrayStatusHasUnlockedLogicalDrive                  0x00004000  /**< Array status has an unlocked logical drive; @outputof SA_GetEditableArrayStatusInfo. */
   #define kArrayStatusUnsupportedBlockSizeForRAIDConfiguration 0x00008000  /**< Array status has a drive with an unsupported blocksize; @outputof SA_GetEditableArrayStatusInfo. */
   #define kArrayStatusHasOfflineLogicalDrive                   0x00010000  /**< Array status has a logical drive offline/locked; @outputof SA_GetEditableArrayStatusInfo. */
   #define kArrayStatusHasLogicalDriveWithVolatileKeyOp         0x00020000  /**< Array status has a logical drive with an outstanding/pending volatile key operation; @outputof SA_GetEditableArrayStatusInfo. */
   #define kArrayStatusHasFlushingLogicalDrive                  0x00040000  /**< Array status has a flushing lu cache logical drive; @outputof SA_GetEditableArrayStatusInfo. */
   #define kArrayStatusHasDegradedLogicalDrive                  0x00080000  /**< Array status has a degraded logical drive; @outputof SA_GetArrayStatusInfo. */
   #define kArrayStatusHasLogicalDriveHasSEDStatus              0x00100000  /**< Array status has a logical drive with ongoing or failed SED qualification or locked SED; @outputof SA_GetArrayStatusInfo. @see SA_GetLogicalDriveStatusInfo */
   /** @} */

/** @defgroup storc_status_array_health Array Health
 * @{ */
/** Get the array health.
 *
 * This is the health of the array only, not including dependent resources.
 *
 * @param[in] cda          Controller CDA pointer
 * @param[in] array_number Array number
 * @return                 Array health value (see [Array Health Values](@ref storc_status_array_health))
 */
SA_BYTE SA_GetArrayHealth(PCDA cda, SA_WORD array_number);
   /** @name Array Health Values
    * @outputof SA_GetArrayHealth.
    * @{ */
   #define kArrayHealthOK         0 /**< Array health 'OK'; @outputof SA_GetArrayHealth. */
   #define kArrayHealthWarning    1 /**< Array health 'Warning'; @outputof SA_GetArrayHealth. */
   #define kArrayHealthCritical   2 /**< Array health 'Critical'; @outputof SA_GetArrayHealth. */
   /** @} */

/** Get the array health rollup.
 *
 * This is the health of the array including dependent resources.
 *
 * @param[in] cda          Controller CDA pointer
 * @param[in] array_number Array number
 * @return                 Array health rollup value map (see [Array Health Rollup Values](@ref storc_status_array_health))
 * @return                 Decode with @ref IS_ARRAY_ROLLUP_CRITICAL and @ref IS_ARRAY_ROLLUP_WARNING.
 */
SA_BYTE SA_GetArrayHealthRollup(PCDA cda, SA_WORD array_number);
   /** @name Array Health Rollup Values
    * @outputof SA_GetArrayHealthRollup.
    * @{ */
   #define kArrayHealthRollupOK         0x0 /**< Array health rollup bitmap for 'OK'; @outputof SA_GetArrayHealthRollup. */
   #define kArrayHealthRollupWarning    0x1 /**< Array health rollup bitmap for 'Warning'; @outputof SA_GetArrayHealthRollup. */
   #define kArrayHealthRollupCritical   0x2 /**< Array health rollup bitmap for 'Critical'; @outputof SA_GetArrayHealthRollup. */
   /** Decode return of @ref SA_GetArrayHealthRollup and return @ref kTrue/@ref kFalse if Array health rollup includes a warning health. */
   #define IS_ARRAY_ROLLUP_CRITICAL(info_value) ((info_value & kArrayHealthRollupCritical) ? kTrue : kFalse)
   /** Decode return of @ref SA_GetArrayHealthRollup and return @ref kTrue/@ref kFalse if Array health rollup includes a critical health. */
   #define IS_ARRAY_ROLLUP_WARNING(info_value) ((info_value & kArrayHealthRollupWarning) ? kTrue : kFalse)
   /** @} */
/** @} */ /* storc_status_array_health */

/** @} */ /* storc_status_array_status */

/** @} */ /* storc_status_array */

/***************************************//**
 * @defgroup storc_status_pd Physical Drive
 * @brief
 * @details
 * @{
 ******************************************/

/** @defgroup storc_status_pd_status Physical Drive Status
 * @{ */
/** Obtain status information for the physical drive.
 * @pre Physical drive `pd_number` exists.
 * @post If @ref kPhysicalDriveStatusUnsupportedForConfig is set, user may call @ref SA_GetPhysicalDriveUnsupportedReasons.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Valid physical drive number.
 * @return Bitmap of status; see [Physical Drive Status Levels](@ref storc_status_pd) & [Physical Drive Statuses](@ref storc_status_pd).
 * @return Use @ref PHYSICAL_DRIVE_STATUS_LEVEL & @ref PHYSICAL_DRIVE_STATUS_IS_DRIVE_USABLE to decode (in @ref storc_status_pd).
 * @remark Use @ref SA_GetPhysicalDriveLastFailureReason to determine the reason for a drive being failed.
 *
 * __Example__
 * @code
 * SA_DWORD pd_status = SA_GetPhysicalDriveStatusInfo(cda, pd_number);
 * printf("PD %u Status Level %s", pd_number,
 *             PHYSICAL_DRIVE_STATUS_LEVEL(pd_status) == kPhysicalDriveStatusLevelOK ? "OK: "
 *             : PHYSICAL_DRIVE_STATUS_LEVEL(pd_status) == kPhysicalDriveStatusLevelInformational ? "Info: "
 *             : PHYSICAL_DRIVE_STATUS_LEVEL(pd_status) == kPhysicalDriveStatusLevelWarning ? "Warning: "
 *             : PHYSICAL_DRIVE_STATUS_LEVEL(pd_status) == kPhysicalDriveStatusLevelFailed ? "Failed: "
 *             : "UNKNOWN");
 * printf("%s", pd_status,
 *             pd_status & kPhysicalDriveStatusFailed ? "Failed"
 *             : pd_status & kPhysicalDriveStatusUnsupportedNoLicense ? "Missing needed license"
 *             : pd_status & kPhysicalDriveStatusPredictiveFailure ? "Predictive failure"
 *             : pd_status & kPhysicalDriveStatusIsTransientDrive ? "Is a transient drive"
 *             // ...
 *             : "UNKNOWN");
 * printf(" (is usable? %s)\n", PHYSICAL_DRIVE_STATUS_IS_DRIVE_USABLE(pd_status) ? "yes" : "no");
 * @endcode
 */
SA_DWORD SA_GetPhysicalDriveStatusInfo(PCDA cda, SA_WORD pd_number);
   /** @name Physical Drive Status Levels
    * @outputof SA_GetPhysicalDriveStatusInfo.
    * @{ */
   #define kPhysicalDriveStatusLevelOK                     0x0    /**< Physical drive status level OK; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusLevelInformational          0x1    /**< Physical drive status level informational; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusLevelWarning                0x2    /**< Physical drive status level warning; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusLevelFailed                 0x3    /**< Physical drive status level failed; @outputof SA_GetPhysicalDriveStatusInfo. */
   /** Use to mask output of @ref SA_GetPhysicalDriveStatusInfo for the status level of a physical drive.
    * @see PHYSICAL_DRIVE_STATUS_LEVEL.
    */
   #define kPhysicalDriveStatusLevelMask                   0x3
   /** Use to decode physical drive's status level output of @ref SA_GetPhysicalDriveStatusInfo.
    * __Example__
    * @code
    * SA_DWORD pd_status = SA_GetPhysicalDriveStatusInfo(cda, array_number);
    * printf("PD %u Status Level %s", pd_status,
    *             PHYSICAL_DRIVE_STATUS_LEVEL(pd_status)   == kPhysicalDriveStatusLevelOK ?            "OK: "
    *             : PHYSICAL_DRIVE_STATUS_LEVEL(pd_status) == kPhysicalDriveStatusLevelInformational ? "Info: "
    *             : PHYSICAL_DRIVE_STATUS_LEVEL(pd_status) == kPhysicalDriveStatusLevelWarning ?       "Warning: "
    *             : PHYSICAL_DRIVE_STATUS_LEVEL(pd_status) == kPhysicalDriveStatusLevelFailed ?        "Failed: "
    *             : "UNKNOWN");
    * @endcode
    */
   #define PHYSICAL_DRIVE_STATUS_LEVEL(status_info) ((SA_BYTE)((status_info & kPhysicalDriveStatusLevelMask)))
   /** @} */

   /** @name Physical Drive Statuses
    * @outputof SA_GetPhysicalDriveStatusInfo.
    * @{ */
   #define kPhysicalDriveStatusFailed                     0x00000100  /**< Physical drive failed status failed; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusFailedDueToPSA             0x00000200  /**< Physical drive failed status failed PSA; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusUnsupportedNoLicense       0x00000400  /**< Physical drive failed status unsupported no license; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusPredictiveFailure          0x00000800  /**< Physical drive warning status predictive failure; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusPredictiveFailureWornOut   0x00001000  /**< Physical drive warning status predictive failure wornout; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusIsTransientDrive           0x00002000  /**< Physical drive informational status is transient drive; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusErasingOffline             0x00004000  /**< Physical drive warning status offline/erasing; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusInvalidDriveSize           0x00008000  /**< Physical drive failed invalid size; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusFirmwareUpdateRequired     0x00010000  /**< Physical drive informational status firmware out-of-date; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusExposedToOS                0x00020000  /**< Physical drive informational status exposed to OS; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusMaskedWithInvalidCCM       0x00040000  /**< Physical drive informational status HBA drive masked from OS due to invalid/stale CCM; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusUnsupportedForConfig       0x00080000  /**< Physical drive informational status drive is not supported for RAID configuration; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusRebuilding                 0x00100000  /**< Physical drive informational status is rebuilding; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusRebuildQueued              0x00200000  /**< Physical drive informational status is rebuild queued; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusFailedDueToHotRemoval      0x00400000  /**< Physical drive failed status failed due to hot removal; @outputof SA_GetPhysicalDriveStatusInfo. */
   #define kPhysicalDriveStatusFailedDueSEDQualification  0x00800000  /**< Physical drive failed status failed due to SED qualification failure; @outputof SA_GetPhysicalDriveStatusInfo. */

   /** Use to decode output of @ref SA_GetPhysicalDriveStatusInfo for the status of a physical drive.
    * @see PHYSICAL_DRIVE_STATUS_IS_DRIVE_USABLE.
    */
   #define kPhysicalDriveStatusExcludeFromConfigurationBit 0x4
   /** @return @ref kTrue if drive status reflects the drive is still usable, @ref kFalse otherwise.
    * Use to decode physical drive's status output of @ref SA_GetPhysicalDriveStatusInfo.
    */
   #define PHYSICAL_DRIVE_STATUS_IS_DRIVE_USABLE(status_info) ((status_info & kPhysicalDriveStatusExcludeFromConfigurationBit) == kPhysicalDriveStatusExcludeFromConfigurationBit ? kFalse : kTrue)
   /** @} */

/** @defgroup storc_status_pd_health Physical Drive Health
 * @{ */
/** Get the physical drive health.
 *
 * This is the health of the physical drive only, not including dependent resources.
 *
 * @param[in] cda       Controller CDA pointer
 * @param[in] pd_number Physical drive number
 * @return              Physical drive health value
 * @remark Use @ref SA_GetPhysicalDriveLastFailureReason to determine the reason for critical health values.
 */
SA_BYTE SA_GetPhysicalDriveHealth(PCDA cda, SA_WORD pd_number);
   /** @name Physical Drive Health Values
    * @{ */
   #define kPhysicalDriveHealthOK         0
   #define kPhysicalDriveHealthWarning    1
   #define kPhysicalDriveHealthCritical   2
   /** @} */
/** @} */ /* storc_status_pd_health */

/** @defgroup storc_status_pd_last_failure_reason Physical Drive Last Failure Reason
 * @{ */
/** Get the reason the physical drive was last failed by the controller.
 *
 * @param[in] cda       Controller CDA pointer
 * @param[in] pd_number Physical drive number
 * @return Use @ref GET_PHYSICAL_DRIVE_LAST_FAILURE_REASON_SUCCESS to determine success or failure of API call. (@ref storc_status_pd_last_failure_reason)
 * @return If valid, use @ref PHYSICAL_DRIVE_LAST_FAILURE_REASON to get physical drive's last failure reason code. (@ref storc_status_pd_last_failure_reason)
 *
 * __Example__
 * @code
 * SA_WORD return_value = SA_GetPhysicalDriveLastFailureReason(cda, pd_number);
 * if (GET_PHYSICAL_DRIVE_LAST_FAILURE_REASON_SUCCESS(return_value) == kGetPhysicalDriveLastFailureReasonSuccess)
 * {
 *    printf("PD %u Last Failure Reason %s", pd_number,
 *       PHYSICAL_DRIVE_LAST_FAILURE_REASON(last_failure_reason) == kPhysicalDriveLastFailureReasonTimeout
 *          ? "Too Many SCSI Timeouts" : "Other");
 * }
 * @endcode
 */
SA_WORD SA_GetPhysicalDriveLastFailureReason(PCDA cda, SA_WORD pd_number);
   /** @name Physical Drive Last Failure Reason Values
    * @{ */
   #define kPhysicalDriveLastFailureReasonTooSmallInLoadedConfig     0x01  /**< Replacement physical drive is too small for configured volumes; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonErrorErasingRIS            0x02  /**< Couldn't write to RIS area after multiple tries during drive erase; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonErrorSavingRIS             0x03  /**< Couldn't write to RIS area after multiple tries during RIS data save; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonFailDriveCommand           0x04  /**< Controller received Fail Drive command from host; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonMarkBadFailed              0x05  /**< Write Long command failed after multiple tries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonMarkBadFailedInFinishRemap 0x06  /**< Write Long command failed after multiple tries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonTimeout                    0x07  /**< Too many SCSI command timeouts; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonAutosenseFailed            0x08  /**< Drive is failing commands but not returning SCSI sense data after multiple tries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonMediumError1               0x09  /**< A particular set of SCSI sense codes indicate the drive is in bad shape; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonMediumError2               0x0A  /**< A particular set of SCSI sense codes indicate the drive is in bad shape; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonNotReadyBadSense           0x0B  /**< Drive is returning unrecognized Not Ready sense codes; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonNotReady                   0x0C  /**< Drive is not ready and will not spin up; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonHardwareError              0x0D  /**< Drive returns Hardware Error sense key, controller unable to get drive back online; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonAbortedCommand             0x0E  /**< Drive returns Aborted Command sense key, controller unable to get drive back online; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonWriteProtected             0x0F  /**< Drive returns a sense key indicating it is write protected; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonSpinUpFailureInRecover     0x10  /**< Start Unit command failed during error recovery; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonRebuildWriteError          0x11  /**< Drive failed write command after multiple tries during rebuild; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonTooSmallInHotPlug          0x12  /**< Replacement drive is too small for configured volumes; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonBusResetRecoveryAborted    0x13  /**< Unable to communicate with drive after multiple bus resets and retries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonRemovedInHotPlug           0x14  /**< Drive has been removed in hot plug; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonInitRequestSenseFailed     0x15  /**< Request Sense command failed during device discovery / initialization; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonInitStartUnitFailed        0x16  /**< Start Unit command failed during device discovery / initialization; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonInquiryFailed              0x17  /**< Inquiry command failed after multiple tries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonNonDiskDevice              0x18  /**< Attached device is not a hard disk per its inquiry data; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonReadCapacityFailed         0x19  /**< Read Capacity command failed after multiple tries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonInvalidBlockSize           0x1A  /**< Drive indicates it's not formatted for 512 bytes per sector; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonHotPlugRequestSenseFailed  0x1B  /**< Request Sense command failed after drive was hot-added; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonHotPlugStartUnitFailed     0x1C  /**< Start Unit command failed after drive was hot-added; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonWriteErrorAfterRemap       0x1D  /**< After reassigning a media error reported during a write, the write command failed with another media error; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonInitResetRecoveryAborted   0x1E  /**< Unable to communicate with drive during initialization after multiple bus resets and tries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonDeferredWriteError         0x1F  /**< Drive with write cache enabled could not complete a write command it previously indicated was successful; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonMissingInSaveRIS           0x20  /**< Couldn't write to RIS area after multiple tries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonWrongReplace               0x21  /**< A hot-plug tape drive was plugged into a bay that a hard disk was removed from; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonGDPVPDInquiryFailed        0x22  /**< Couldn't read drive serial number after multiple tries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonGDPModeSenseFailed         0x23  /**< Couldn't read certain sense pages after multiple tries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonDriveNotIn48BitMode        0x24  /**< SATA drive is not supported, not in 48-bit LBA mode; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonDriveTypeMixInHotPlug      0x25  /**< Attempt to hot-add a SATA drive as a replacement in a SAS-only volume, or vice versa; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonDriveTypeMixInLoadCfg      0x26  /**< Attempt to use a SATA drive as a replacement in a SAS-only volume, or vice versa; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonProtocolAdapterFailed      0x27  /**< Protocol layer reports a protocol hardware failure, may be a controller failure; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonFaultyIDBayEmpty           0x28  /**< Drive responds to SCSI ID, but the corresponding bay is empty; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonFaultyIDBayOccupied        0x29  /**< Bay is occupied by a drive that does not respond to the corresponding SCSI ID; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonFaultyIDInvalidBay         0x2A  /**< Drive responds to an ID that doesn't have a valid corresponding bay; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonWriteRetriesFailed         0x2B  /**< Unable to complete a write operation after several tries; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonErrorWriteExpand           0x2F  /**< Media failed a write to metadata region; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonSmartErrorReported         0x37  /**< Drive has reported a predictive failure error when controller is configured to automatically fail drives with this error; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonPhyResetFailed             0x38  /**< Phy reset request failed; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonCLIFailDriveCommand        0x39  /**< Drive marked as failed via CLI command; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonChkBlkFailedWrite          0x3A  /**< Drive failed write command while checking for media errors; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonATITestFailedWrite         0x3B  /**< Drive failed write command while checking for errors; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonNotInSASTopology           0x43  /**< Drive is installed but not detected in the SAS topology; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonSEHardwareError            0x44  /**< Drive is returning a Hardware Error sense key while checking sanitize support; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonPowerCycleFailed           0x45  /**< Drive failed rediscovery following enclosure bay power cycle; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonFRSEDQualFailed            0x4C  /**< Drive is failed on spare activation or hot plug due to SED qualification failure; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonFRSEDCmdFailed             0x4D  /**< SED protocol command failure; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonOfflineErase               0x80  /**< Drive is offline due to a Secure Erase operation; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonOfflineTooSmall            0x81  /**< Drive is offline because it's a replacement drive that is too small; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonOfflineDriveTypeMix        0x82  /**< Drive is offline because it's not the correct type for the array [SATA vs. SAS]; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kPhysicalDriveLastFailureReasonOfflineEraseComplete       0x83  /**< Drive is offline because a Secure Erase operation has completed on it, but it has not been replaced yet; @outputof SA_GetPhysicalDriveLastFailureReason. */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
   /** Use to mask output of @ref SA_GetPhysicalDriveLastFailureReason for the physical drive's last failure reason.
    * @see PHYSICAL_DRIVE_LAST_FAILURE_REASON.
    */
   #define kPhysicalDriveLastFailureReasonCodeMask                 0x00FF
#endif
   /** Use to decode physical drive's last failure reason code output of @ref SA_GetPhysicalDriveLastFailureReason. */
   #define PHYSICAL_DRIVE_LAST_FAILURE_REASON(reason) ((SA_BYTE)(reason & kPhysicalDriveLastFailureReasonCodeMask))
   /** @} */

   /** @name SA_GetPhysicalDriveLastFailureReason Return Codes
    * @{ */
   #define kGetPhysicalDriveLastFailureReasonSuccess              0x00     /**< Call to SA_GetPhysicalDriveLastFailureReason was successful; @outputof SA_GetPhysicalDriveLastFailureReason. Use @ref PHYSICAL_DRIVE_LAST_FAILURE_REASON to get physical drive's last failure reason.*/
   #define kGetPhysicalDriveLastFailureReasonInvalidPDNumber      0x01     /**< pd_number supplied to SA_GetPhysicalDriveLastFailureReason was invalid; @outputof SA_GetPhysicalDriveLastFailureReason. */
   #define kGetPhysicalDriveLastFailureReasonFailed               0x02     /**< Call to SA_GetPhysicalDriveLastFailureReason failed; @outputof SA_GetPhysicalDriveLastFailureReason. */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
   /** Use to mask output of @ref SA_GetPhysicalDriveLastFailureReason for the API call success code. */
   #define kPhysicalDriveLastFailureReasonSuccessBitshift         8
   #define kPhysicalDriveLastFailureReasonSuccessMask             (0xFF << kPhysicalDriveLastFailureReasonSuccessBitshift)
#endif
   /** Use to decode API success code output of @ref SA_GetPhysicalDriveLastFailureReason.
    */
   #define GET_PHYSICAL_DRIVE_LAST_FAILURE_REASON_SUCCESS(reason) ((SA_BYTE)((reason & kPhysicalDriveLastFailureReasonSuccessMask) >> kPhysicalDriveLastFailureReasonSuccessBitshift))
   /** @} */
/** @} */ /* storc_status_pd_last_failure_reason */

/** @defgroup storc_properties_pd_unsupported_reasons Physical Drive Unsupported Reasons
 * @{ */
/** Get the reasons a physical drive is unsupported.
 * @pre Physical drive `pd_number` exists.
 * @pre @ref kPhysicalDriveStatusUnsupportedForConfig is set by @ref SA_GetPhysicalDriveStatusInfo.
 * @param[in] cda         Controller Data Area
 * @param[in] pd_number   Physical drive number.
 * @return Returns a bitmap of reasons the physical drive is not supported. See [Physical Drive Unsupported Reasons] (@ref storc_properties_pd_unsupported_reasons).
 * @return Returns 0 if the physical drive number is invalid or unsupported.
 */
SA_WORD SA_GetPhysicalDriveUnsupportedReasons(PCDA cda, SA_WORD pd_number);
   /** @name Physical Drive Unsupported Reasons
    * @outputof SA_GetPhysicalDriveUnsupportedReasons
    * @{ */
   #define kPhysicalDriveUnsupportedReasonsFormat               0x0001 /**< Unsupported physical drive due to an unsupported drive format;  @outputof SA_GetPhysicalDriveUnsupportedReasons. */
   #define kPhysicalDriveUnsupportedReasonsSecurityState        0x0002 /**< Unsupported physical drive due to the drive security state; @outputof SA_GetPhysicalDriveUnsupportedReasons. */
   #define kPhysicalDriveUnsupportedReasonsBackplane            0x0004 /**< Physical drive unsupported by the backplane; @outputof SA_GetPhysicalDriveUnsupportedReasons. */
   #define kPhysicalDriveUnsupportedReasonsController           0x0008 /**< Physical drive unsupported by the controller; @outputof SA_GetPhysicalDriveUnsupportedReasons. */
   #define kPhysicalDriveUnsupportedReasonsSoftwareRAIDNVME     0x0010 /**< NVMe drive not supported by software RAID controller; @outputof SA_GetPhysicalDriveUnsupportedReasons. */
   /** @} */ /* Physical Drive Unsupported Reasons */

/** @} */ /* storc_properties_pd_unsupported_reasons */

/** Determine if drive number is a physical drive on the controller.
 * Should return @ref kTrue for any pd_number returned by @ref SA_EnumerateControllerPhysicalDrives.
 * @param[in] cda Controller data area.
 * @param[in] pd_number Physical drive number to test.
 * @return @ref kTrue iff the test `pd_number` is a present physical drive, @ref kFalse otherwise.
 */
SA_BOOL SA_IsPhysicalDrivePresent(PCDA cda, SA_WORD pd_number);

/** Determine if a device number is present on the controller.
 *
 * @param[in] cda        Controller CDA pointer
 * @param[in] dev_number Any physical device (disk, sep, etc...) number
 * @return               @ref kTrue iff device is present on the controller, @ref kFalse otherwise.
 */
SA_BOOL SA_IsPhysicalDevicePresent(PCDA cda, SA_WORD dev_number);

/** @} */ /* storc_status_pd_status */

/** @} */ /* storc_status_pd */

/******************************************//**
 * @defgroup storc_status_se Storage Enclosure
 * @brief
 * @details
 * @{
 *********************************************/

/** @defgroup storc_status_se_status Storage Enclosure Status
 * @{ */
/** Obtain status information for the storage enclosure.
 * @pre Storage enclosure `se_number` exists.
 * @param[in] cda        Controller data area.
 * @param[in] se_number  Valid storage enclosure number.
 * @return Bitmap of status; see [Storage Enclosure Status Levels](@ref storc_status_se) & [Storage Enclosure Statuses](@ref storc_status_se).
 * @return Use [STORAGE_ENCLOSURE_TEMPERATURE_STATUS](@ref storc_status_se) to decode storage enclosure temperature status.
 * @return Use [STORAGE_ENCLOSURE_FAN_STATUS](@ref storc_status_se) to decode storage enclosure fan status.
 * @return Use [STORAGE_ENCLOSURE_POWER_STATUS](@ref storc_status_se) to decode storage enclosure power supply status.
 * @return Use [STORAGE_ENCLOSURE_PATH_STATUS](@ref storc_status_se) to decode storage enclosure multi-domain path status.
 * @return Use [STORAGE_ENCLOSURE_STATUS_LEVEL](@ref storc_status_se) to decode overall status level.
 *
 * __Example__
 * @code
 * SA_DWORD se_status = SA_GetStorageEnclosureStatusInfo(cda, se_number);
 * printf("PD %u Status Level %s\n", se_number,
 *             STORAGE_ENCLOSURE_STATUS_LEVEL(se_status) == kStorageEnclosureStatusLevelOK ? "OK"
 *             : STORAGE_ENCLOSURE_STATUS_LEVEL(se_status) == kStorageEnclosureStatusLevelInformational ? "Info"
 *             : STORAGE_ENCLOSURE_STATUS_LEVEL(se_status) == kStorageEnclosureStatusLevelWarning ? "Warning"
 *             : STORAGE_ENCLOSURE_STATUS_LEVEL(se_status) == kStorageEnclosureStatusLevelFailed ? "Failed"
 *             : "UNKNOWN");
 * printf("Fan Status: %s",
 *             STORAGE_ENCLOSURE_FAN_STATUS(se_status) ? kStorageEnclosureStatusOK ? "OK"
 *             : STORAGE_ENCLOSURE_FAN_STATUS(se_status) ? kStorageEnclosureStatusFanDegraded ? "Degraded"
 *             : STORAGE_ENCLOSURE_FAN_STATUS(se_status) ? kStorageEnclosureStatusFanFailed ? "Failed"
 *             : "UNKNOWN");
 * @endcode
 */
SA_DWORD SA_GetStorageEnclosureStatusInfo(PCDA cda, SA_WORD se_number);
   /** @name Storage Enclosure Status Levels
    * @outputof SA_GetStorageEnclosureStatusInfo.
    * @{ */
   #define kStorageEnclosureStatusLevelOK                     0x0    /**< Storage enclosure status level OK; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusLevelInformational          0x1    /**< Storage enclosure status level informational; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusLevelWarning                0x2    /**< Storage enclosure status level warning; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusLevelFailed                 0x3    /**< Storage enclosure status level failed; @outputof SA_GetStorageEnclosureStatusInfo. */
   /** Use to mask output of @ref SA_GetStorageEnclosureStatusInfo for the status level of a storage enclosure.
    * @see STORAGE_ENCLOSURE_STATUS_LEVEL.
    */
   #define kStorageEnclosureStatusLevelMask                   0x3
   /** Use to decode storage enclosure's status level output of @ref SA_GetStorageEnclosureStatusInfo.
    * __Example__
    * @code
    * SA_DWORD se_status = SA_GetStorageEnclosureStatusInfo(cda, array_number);
    * printf("PD %u Status Level %s", se_status,
    *             STORAGE_ENCLOSURE_STATUS_LEVEL(se_status)   == kStorageEnclosureStatusLevelOK ?            "OK: "
    *             : STORAGE_ENCLOSURE_STATUS_LEVEL(se_status) == kStorageEnclosureStatusLevelInformational ? "Info: "
    *             : STORAGE_ENCLOSURE_STATUS_LEVEL(se_status) == kStorageEnclosureStatusLevelWarning ?       "Warning: "
    *             : STORAGE_ENCLOSURE_STATUS_LEVEL(se_status) == kStorageEnclosureStatusLevelFailed ?        "Failed: "
    *             : "UNKNOWN");
    * @endcode
    */
   #define STORAGE_ENCLOSURE_STATUS_LEVEL(status_info) ((SA_BYTE)((status_info & kStorageEnclosureStatusLevelMask)))
   /** @} */ /* Storage Enclosure Status Levels */

   /** @name Storage Enclosure Temperature Status
    * @outputof SA_GetStorageEnclosureStatusInfo.
    * @{ */
   #define kStorageEnclosureStatusTempStatusBitmap              0x0000000C
   #define kStorageEnclosureStatusTempStatusBitshift            2
   #define STORAGE_ENCLOSURE_TEMPERATURE_STATUS(status_info) ((status_info & kStorageEnclosureStatusTempStatusBitmap) >> kStorageEnclosureStatusTempStatusBitshift)
   #define kStorageEnclosureStatusTempOK                        0  /**< Storage enclosure temperature status is ok; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusTempWarning                   1  /**< Storage enclosure temperature is at a warning level; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusTempCritical                  2  /**< Storage enclosure temperature is at a critical level; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusTempUnknown                   3  /**< Storage enclosure temperature is unknown; @outputof SA_GetStorageEnclosureStatusInfo. */
   /** @} */ /* Storage Enclosure Temperature Status  */

   /** @name Storage Enclosure Fan Status
    * @outputof SA_GetStorageEnclosureStatusInfo.
    * @{ */
   #define kStorageEnclosureStatusFanStatusBitmap               0x00000030
   #define kStorageEnclosureStatusFanStatusBitshift             4
   #define STORAGE_ENCLOSURE_FAN_STATUS(status_info) ((status_info & kStorageEnclosureStatusFanStatusBitmap) >> kStorageEnclosureStatusFanStatusBitshift)
   #define kStorageEnclosureStatusFanOK                         0  /**< Storage enclosure fan status is ok; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusFanDegraded                   1  /**< Storage enclosure has a degraded fan; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusFanFailed                     2  /**< Storage enclosure has a failed fan; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusFanUnknown                    3  /**< Storage enclosure fan status is unknown; @outputof SA_GetStorageEnclosureStatusInfo. */
   /** @} */ /* Storage Enclosure Fan Status  */

   /** @name Storage Enclosure Power Supply Status
    * @outputof SA_GetStorageEnclosureStatusInfo.
    * @{ */
   #define kStorageEnclosureStatusPowerSupplyStatusBitmap       0x000000C0
   #define kStorageEnclosureStatusPowerSupplyStatusBitshift     6
   #define STORAGE_ENCLOSURE_POWER_STATUS(status_info) ((status_info & kStorageEnclosureStatusPowerSupplyStatusBitmap) >> kStorageEnclosureStatusPowerSupplyStatusBitshift)
   #define kStorageEnclosureStatusPowerSupplyRedundant          0  /**< Storage enclosure power supply redundant and OK; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusPowerSupplyNotRedundant       1  /**< Storage enclosure power supply not redundant; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusPowerSupplyRedundantDegraded  2  /**< Storage enclosure power supply redundant but degraded; @outputof SA_GetStorageEnclosureStatusInfo. */
   /** @} */ /* Storage Enclosure Power Supply  Status  */

   /** @name Storage Enclosure Multi-Domain Path Status
    * @outputof SA_GetStorageEnclosureStatusInfo.
    * @{ */
   #define kStorageEnclosureStatusPathStatusBitmap              0x00000300
   #define kStorageEnclosureStatusPathStatusBitshift            8
   #define STORAGE_ENCLOSURE_PATH_STATUS(status_info) ((status_info & kStorageEnclosureStatusPathStatusBitmap) >> kStorageEnclosureStatusPathStatusBitshift)
   #define kStorageEnclosureStatusPathOK                        0  /**< Storage enclosure power supply not redundant; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusPathInvalidCabling            1  /**< Storage enclosure is cabled in such a way that is not recommended; @outputof SA_GetStorageEnclosureStatusInfo. */
   #define kStorageEnclosureStatusPathFailed                    2  /**< Storage enclosure multi domain has 1 or more path failures; @outputof SA_GetStorageEnclosureStatusInfo. */
   /** @} */ /* Storage Enclosure Multi-Domain Path Status  */

/** @} */ /* storc_status_se_status */

/** @} */ /* storc_status_se */

#if defined(__cplusplus)
}
#endif

/** @} */ /* storc_status */

/** @} */ /* storcore */

#endif /* PSTORC_STATUS_H */

