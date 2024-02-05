/** @file pstorc_features.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore featues header file
   Defines structures and functions for obtaining device feature(s)

*/

#ifndef PSTORC_FEATURES_H
#define PSTORC_FEATURES_H

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/********************************//**
 * @defgroup storc_features Features
 * @brief
 * @details
 * @{
 ***********************************/

#if defined(__cplusplus)
extern "C" {
#endif

/**************************************************//**
 * @defgroup storc_features_ctrl Controller Features
 * @brief
 * @details
 * @{
 *****************************************************/

/** @defgroup storc_features_ctrl_mode Controller Mode Support
 * @brief Get the supported controller operating modes of the controller.
 * @{ */
/** Does controller support HBA mode?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports HBA mode, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsSmartHBAMode(PCDA cda);
/** Does controller support mixed mode?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports mixed mode, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsMixedMode(PCDA cda);
/** @} */ /* storc_features_ctrl_encryption */

/** @defgroup storc_features_ctrl_queue_depth_support Controller Performance Settings Support
 * @{ */
/** Test for queue depth support.
 * @param[in] cda     Controller data area.
 * @return @ref kTrue iff controller supports queue depth, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsQueueDepth(PCDA cda);
/** @} */ /* storc_features_ctrl_performance_settings_support */

/** @defgroup storc_features_ctrl_opo_support Controller Over Provisioning Support
 * @{ */
/** Test for SSD over provisioning support.
 * @param[in] cda     Controller data area.
 * @return @ref kTrue iff controller supports over provisioning.
 */
SA_BOOL SA_ControllerSupportsOPO(PCDA cda);
/** @} */ /* storc_features_ctrl_opo_support */

/** @defgroup storc_features_ctrl_irp_support Controller Inconsistency Repair Policy Support
 * @{ */
/** Test for RAID 6/60 inconsistency repair policy support.
 * @param[in] cda     Controller data area.
 * @return @ref kTrue iff controller supports raid 6/60 inconsistency repair policy.
 */
SA_BOOL SA_ControllerSupportsIRP(PCDA cda);
/** @} */ /* storc_features_ctrl_irp_support */

/** @defgroup storc_features_inconsistency_event_notification_support Controller Surface Analysis Inconsistency Event Notification Support
 * @{ */
/** Get the controller inconsistency event notification support.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if inconsistency event notification is supported.
 * @return @ref kFalse if inconsistency event notification is not supported.
 */
SA_BOOL SA_ControllerSupportsInconsistencyEventNotification(PCDA cda);
/** @} */ /* storc_features_inconsistency_event_notification_support */

/** @defgroup storc_features_ctrl_dpo_support Controller Degraded Performance Optimization Support
 * @{ */
/** Test for degraded performance optimization support.
 * @param[in] cda     Controller data area.
 * @return @ref kTrue iff controller supports degraded performance optimization.
 */
SA_BOOL SA_ControllerSupportsDPO(PCDA cda);
/** @} */ /* storc_features_ctrl_dpo_support */

/** @defgroup storc_features_ctrl_parity_raid_support Controller Parity RAID Support
 * @{ */
/** Test for parity RAID support.
 * To get support for a specific RAID level, use @ref SA_ControllerSupportsRAIDLevel.
 * @see SA_ControllerSupportsRAIDLevel
 * @param[in] cda     Controller data area.
 * @return @ref kTrue iff controller supports parity RAID, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsParityRAID(PCDA cda);
/** @} */ /* storc_features_ctrl_parity_raid_support */

/** @defgroup storc_features_ctrl_nway_raid_support Controller N-RAID Support
 * @{ */
/** Test for N-RAID support.
 * To get support for a specific RAID level, use @ref SA_ControllerSupportsRAIDLevel.
 * @see SA_ControllerSupportsRAIDLevel
 * @param[in] cda     Controller data area.
 * @return @ref kTrue iff controller supports N-RAID such as @ref kRAID1Triple, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsNWayRAID(PCDA cda);
/** @} */ /* storc_features_ctrl_nway_raid_support */

/** @defgroup storc_features_ctrl_raid_support Controller RAID Support
 * @{ */
/** @name RAID Levels Supported
 * @{ */
/** Test for RAID level support.
 *
 * @param[in] cda     Controller data area.
 * @param raid_level  Specified RAID level (see [RAID Levels Supported](@ref storc_features_ctrl_raid_support)).
 * @return @ref kTrue if controller supports specified RAID level, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsRAIDLevel(PCDA cda, SA_BYTE raid_level);
   #define kRAID0        0x01  /**< RAID 0: stripe set or striped volume. */
   #define kRAID1        0x02  /**< RAID 1: mirror volume. */
   #define kRAID10       0x03  /**< RAID 1+0: Nested, stripe set from series of mirrored drives. */
   #define kRAID5        0x04  /**< RAID 5: Block level striping with parity. */
   #define kRAID50       0x05  /**< RAID 5+0: Nested, stripe set from block level striping with parity. */
   #define kRAID6        0x06  /**< RAID 6: Block level striping with extra parity block. */
   #define kRAID60       0x07  /**< RAID 6+0: Nested, stripe set from block level striping with extra parity block. */
   #define kRAID1Triple  0x08  /**< RAID 1Triple: 3 mirrored volume. */
   #define kRAID1ADM     kRAID1Triple /**< @deprecated Use @ref kRAID1Triple instead. */
   #define kRAID10Triple 0x09  /**< RAID 1+0Triple: Nested, stripe set from series of 3 mirrored volume. */
   #define kRAID10ADM    kRAID10Triple /**< @deprecated Use @ref kRAID10Triple instead. */

   /** @return @ref kTrue iff `raid_level` uses parity. */
   #define IS_PARITY_RAID(raid_level) \
      (raid_level == kRAID5 || raid_level == kRAID50 || raid_level == kRAID6 || raid_level == kRAID60)
/** @} */ /* Supported RAID Levels */

/** @name Min/Max Physical Drives Supported for RAID Levels
 * @{ */
/** Get the max and min physical drive for a raid level
 *
 * @param[in] cda         Controller data area.
 * @param[in] raid_level  Specified RAID level Raid level constant.
 * @return min and max number of physical drive for a raid level.
 */
SA_WORD SA_GetMinMaxPDForRAIDLevel(PCDA cda, SA_BYTE raid_level);
   #define kRAIDMaxDriveMask 0x00FF    /**< Mask to use on output of @ref SA_GetMinMaxPDForRAIDLevel to get max number of physical drives for a RAID. */
   #define kRAIDMinDriveMask 0xFF00    /**< Mask to use on output of @ref SA_GetMinMaxPDForRAIDLevel to get min number of  physical drive for a RAID. */
   #define kRAIDMinDriveShift 8  /**< Bitshift to use on output of @ref SA_GetMinMaxPDForRAIDLevel to get min number of physical drive for a RAID. */
   /** Use to decode output of @ref SA_GetMinMaxPDForRAIDLevel to get max number of physical drive for a RAID. */
   #define RAID_MAX_PD_NUMBER(in_value) (in_value & kRAIDMaxDriveMask)
   /** Use to decode output of @ref SA_GetMinMaxPDForRAIDLevel to get min number of physical drive for a RAID. */
   #define RAID_MIN_PD_NUMBER(in_value) ((in_value & kRAIDMinDriveMask) >> kRAIDMinDriveShift )
/** @} */ /* Min/Max Physical Drives Supported for RAID Levels */

/** @name Additional RAID/Configuration Support
 * @{ */
/** Additional RAID/Configuration Support
 *
 * @param[in] cda  Controller data area.
 * @return See [Additional RAID/Configuration Support](@ref storc_features_ctrl_raid_support).
 */
SA_DWORD SA_ControllerSupportsAdditionalRAIDInfo(PCDA cda);
   /** Indicates the controller supports configuring 4kn physical drives; @outputof SA_ControllerSupportsAdditionalRAIDInfo */
   #define kControllerSupports4knRAID   0x00000001
   /** Indicates the controller supports configuring NVMe physical drives; @outputof SA_ControllerSupportsAdditionalRAIDInfo */
   #define kControllerSupportsNVMeRAID  0x00000002
/** @} */ /* Additional RAID/Configuration Support */

/** @} */ /* storc_features_ctrl_raid_support */

/** @defgroup storc_features_ctrl_nvme Controller NVMe Support
 * @{ */
/** @name NVMe Physical Drive Support
 * @{ */
/** Test if controller supports NVMe physical drives.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue iff the controller supports NVMe physical drives, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsNVMe(PCDA cda);
/** @} */ /* NVMe Physical Drive Support */
/** @} */ /* storc_features_ctrl_nvme */

/** @defgroup storc_features_ctrl_rapid_rebuild Controller Rapid Rebuild Priority Support
 * @{ */
/** Test if rapid rebuild is supported.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports rapid rebuild priority, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsRapidRebuild(PCDA cda);
/** @} */ /* storc_features_ctrl_rapid_rebuild */

/** @defgroup storc_features_ctrl_rapid_init Controller Rapid Parity Initialization Support
 * @{ */
/** Test if rapid parity initialization is supported.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports rapid parity initialization, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsRapidInitialization(PCDA cda);
/** @} */ /* storc_features_ctrl_rapid_init */

/** @defgroup storc_features_ctrl_pd_erase Controller Physical Drives Erase Support
 * @brief Get the controller's support for physical drive erase support.
 * @{ */
/** Test if controller based erasing on physical drives is supported.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports erase on physical drives, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsPhysicalDriveErase(PCDA cda);
/** @} */ /* storc_features_ctrl_pd_erase */

/** @defgroup storc_feature_ctrl_pd_sanitize Controller Physical Drives Sanitize Support
 * @brief Get the controller's support for ATA sanitize erase support.
 * @{ */
/** Test if sanitize on physical drives is supported.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports sanitize on physical drives, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsPhysicalDriveSanitize(PCDA cda);
/** @} */ /* storc_features_ctrl_pd_sanitize */

/** @defgroup storc_features_ctrl_pd_interface_support Controller PD Interface Support
 * @{ */
/** Test for PD interface support.
 *
 * @param[in] cda             Controller data area.
 * @param[in] interface_type  Specified physical drive interface (see [Physical Drive Interface Constants](@ref storc_features_ctrl_pd_interface_support)).
 * @return @ref kTrue if controller supports specified Physical Drive Interface, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsPDInterface(PCDA cda, SA_BYTE interface_type);
   /** @name Physical Drive Interface Constants
    * Input of @ref SA_ControllerSupportsPDInterface.
    * @{ */
   #define kPhysicalDriveInterfaceSAS3G        0x01
   #define kPhysicalDriveInterfaceSATA1_5G     0x02
   #define kPhysicalDriveInterfaceSATA3G       0x03
   #define kPhysicalDriveInterfaceSAS6G        0x04
   #define kPhysicalDriveInterfaceSAS12G       0x05
   #define kPhysicalDriveInterfaceSATA6G       0x06
   #define kPhysicalDriveInterfaceMixSAS       0x11
   #define kPhysicalDriveInterfaceMixSATA      0x12
   #define kPhysicalDriveMixEnclosure          0x13   /* Mix drives from different enclosures */
   #define kPhysicalDriveMixSAS_SATA           0x14   /* Mix sas and sata drives */
   #define kPhysicalDriveMixSSD_SAS_SATA       0x15   /* Mix SSD sas and sata drives */
   #define kPhysicalDriveMixSSD_HDD            0x16   /* Mix SSD and HDD drives */
   /** @} */
/** @} */ /* storc_features_ctrl_pd_interface_support */

/** @defgroup storc_features_ctrl_config_support Controller Configuration Support
 * @{ */
/** Get the requested controller strip and stripe size info.
 *
 * @param[in] cda        Controller data area.
 * @param[in] size_type  Specified type of data to return (see [Strip Size Type](@ref storc_features_ctrl_config_support)).
 * @returns The Controller's strip size information (depends on param `size_type`).
 */
SA_DWORD SA_GetControllerStripSizeSupport(PCDA cda, SA_BYTE size_type);
   /** @name Strip Size Type
    * Input for @ref SA_GetControllerStripSizeSupport.
    * @{ */
   /** Report Controller's full stripe size.
    * Input for @ref SA_GetControllerStripSizeSupport.
    */
   #define kControllerStripSizeMaxFullStripeSizeBytes         0x01
   #define kControllerStripSizeMaxStripSizeBytes              0x02
   #define kControllerStripSizeMaxRAID01StripSizeBytes        0x03
   #define kControllerStripSizeMaxRAID5StripSizeBytes         0x04
   #define kControllerStripSizeMaxRAID6StripSizeBytes         0x05
   #define kControllerStripSizeMinStripSizeBytes              0x06
   #define kControllerStripSizeMinNonParityStripSizeBytes     0x07
   #define kControllerStripSizeMinParityStripSizeBytes        0x08
   #define kControllerStripSizeMaxNVMeStripSizeBytes          0x09
   #define kControllerStripSizeLUCacheMaxFullStripeSizeBytes  0x0A /**< @deprecated Use @ref storc_features_ctrl_lu_cache instead. */
   #define kControllerStripSizeLUCacheMaxStripSizeBytes       0x0B
   #define kControllerStripSizeLUCacheMinFullStripeSizeBytes  0x0C /**< @deprecated Use @ref storc_features_ctrl_lu_cache instead. */
   #define kControllerStripSizeLUCacheMinStripSizeBytes       0x0D
   /** @} */

/** Return max number of logical drives the controller can support.
 * @param[in] cda  Controller data area.
 * @return The maximum number of logical drives that can be created on the controller.
 */
SA_WORD SA_ControllerSupportsMaxLogicalDriveCount(PCDA cda);

/** Return max number of Arrays the controller can support.
 * @param[in] cda  Controller data area.
 * @return The maximum number of arrays that can be created on the controller.
 */
SA_WORD SA_ControllerSupportsMaxArrayCount(PCDA cda);

/** Return max number of physical devices the controller can support.
 * @param[in] cda  Controller data area.
 * @return The maximum number of physical devices that can be connected to the controller.
 */
SA_WORD SA_ControllerSupportsMaxPhysicalDriveCount(PCDA cda);

/** Return maximum number of physical drives that can be assigned to an array/logical drive.
 * @param[in] cda  Controller data area.
 * @return The maximum number of physical drives that can be assigned to an array/logical drive.
 */
SA_WORD SA_ControllerSupportsMaxPhysicalDrivesPerLogicalDriveCount(PCDA cda);

/** Return maximum number of dedicated (shared) spare drives the controller can support.
 * @param[in] cda  Controller data area.
 * @return The maximum number of dedicated (shared) spare drives the controller can support.
 */
SA_WORD SA_ControllerSupportsMaxSpareDriveCount(PCDA cda);
/** @} */ /* storc_features_ctrl_config_support */

/** @defgroup storc_features_ctrl_power_mode Controller Power Mode Support
 * @brief Get what power modes are supported by the controller.
 * @{ */
/** Return @ref kTrue/@ref kFalse if Controller supports (or does not support) any power modes.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_ControllerSupportsPowerModeNone(PCDA cda);
/** Return @ref kTrue/@ref kFalse if Controller supports (or does not support) power mode _min_.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_ControllerSupportsPowerModeMin(PCDA cda);
/** Return @ref kTrue/@ref kFalse if Controller supports (or does not support) power mode _balanced_.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_ControllerSupportsPowerModeBalanced(PCDA cda);
/** Return @ref kTrue/@ref kFalse if Controller supports (or does not support) power mode _max_.
 *  @deprecated Use @ref SA_GetPowerModeInformation. */
SA_BOOL SA_ControllerSupportsPowerModeMax(PCDA cda);
/** @} */ /* storc_features_ctrl_power_mode */

/** @defgroup storc_features_ctrl_survival_mode Controller Survival Power Mode Support
 * @brief Determine if the controller supports survival power mode.
 * @{ */
/** Return @ref kTrue/@ref kFalse if Controller supports (or does not support) survival mode.
 * @attention The controller may or may not be capable of configuring survival mode.
 */
SA_BOOL SA_ControllerSupportsSurvivalPowerMode(PCDA cda);
/** @} */ /* storc_features_ctrl_survival_mode */

/** @defgroup storc_features_ctrl_cache_bypass Controller Write Cache Bypass Threshold Support
 * @{ */
/** Return @ref kTrue/@ref kFalse if Controller supports configuring write cache bypass threshold
 */
SA_BOOL SA_ControllerSupportsWCBT(PCDA cda);
/** @} */ /* storc_features_ctrl_cache_bypass */

/** @defgroup storc_features_ctrl_parallel_surface_scan_support Controller Parallel Surface Scan Support
 * @{ */
/** Get the controller parallel surface scan support.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if parallel surface scan is supported.
 * @return @ref kFalse if parallel surface scan is not supported.
 */
SA_BOOL SA_ControllerSupportsPSS(PCDA cda);

/** Get the maximum number of disks that the controller supports scanning in parallel.
 *
 * @param[in] cda  Controller data area.
 * @return The maximum number of disks the controller supports scanning in parallel.
 */
SA_BYTE SA_ControllerSupportsMaxPSSCount(PCDA cda);
/** @} */ /* storc_features_ctrl_parallel_surface_scan_support */

/** @defgroup storc_features_ctrl_nbwc_supported Controller No Battery Write Cache Support
 * @{ */
/** Get the controller no battery write cache support.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if no battery write cache is supported.
 * @return @ref kFalse if no battery write cache is not supported.
 */
SA_BOOL SA_ControllerSupportsNBWC(PCDA cda);
/** @} */ /* storc_features_ctrl_nbwc_supported */

/** @defgroup storc_features_ctrl_wait_for_cache_room_supported Controller Wait For Cache Room Support
 * @{ */
/** Get the controller wait for cache room support.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if wait for cache room is supported.
 * @return @ref kFalse if wait for cache room is not supported.
 */
SA_BOOL SA_ControllerSupportsWaitForCacheRoom(PCDA cda);
/** @} */ /* storc_features_ctrl_wait_for_cache_room_supported */

/** @defgroup storc_features_ctrl_backup_power_source_info_supported Controller Backup Power Source Info Support
 * @{ */
/** Get the controller backup power source info support.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if backup power source info is supported.
 * @return @ref kFalse if backup power source info is not supported.
 */
SA_BOOL SA_ControllerSupportsBackupPowerSourceInfo(PCDA cda);
/** @} */ /* storc_features_ctrl_backup_power_source_info_supported */

/** @defgroup storc_features_ctrl_green_backup_info_supported Controller Green Backup  Info Support
 * @{ */
/** Get the controller green backup info support.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if green backup info is supported.
 * @return @ref kFalse if green backup info is not supported.
 */
SA_BOOL SA_ControllerSupportsGreenBackupInfo(PCDA cda);
/** @} */ /* storc_features_ctrl_green_backup__info_supported */

/** @defgroup storc_features_ctrl_fls_supported Controller Flexible Latency Scheduler Support
 * @{ */
/** Get the controller flexible latency scheduler support.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if flexible latency scheduler is supported.
 * @return @ref kFalse if flexible latency scheduler is not supported.
 */
SA_BOOL SA_ControllerSupportsFLS(PCDA cda);
/** @} */ /* storc_features_ctrl_fls_supported */

/** @defgroup storc_features_ctrl_expander_minimum_scan_duration Controller Expander Minimum Scan Duration Support
 * @{ */
/** Get the controller expander minimum scan duration support.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if setting the expander minimum scan duration is supported.
 * @return @ref kFalse if setting the expander minimum scan duration is not supported.
 */
SA_BOOL SA_ControllerSupportsExpanderMinimumScanDuration(PCDA cda);
/** @} */ /* storc_features_ctrl_expander_minimum_scan_duration */

/** @defgroup storc_features_ctrl_encryption_information Controller Encryption Support
 * @brief Reports if the storage enclosure supports encryption.
 * @{ */
/** Get the controller encryption support.
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if encryption is supported.
 * @return @ref kFalse if encryption is not supported.
 */
SA_BOOL SA_ControllerSupportsEncryption(PCDA cda);

/** Return a list of enabled/configurable encryption features of the controller.
 * @param[in] cda  Controller handler.
 * @return List of enabled [Encryption Features](@ref storc_features_ctrl_encryption_information) or 0 if encryption is not supported.
 */
SA_QWORD SA_GetControllerEncryptionEnabledFeatures(PCDA cda);
   /** @name Encryption Features
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    * @{
    */
   /** Controller support encryption user roles.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureUserRoles                0x0000000000000001ULL
   /** Controller supports encryption password recovery.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeaturePasswordRecovery         0x0000000000000002ULL
   /** Controller supports encryption user (and setting encryption user password).
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureSetUserPassword          0x0000000000000004ULL
   /** Controller supports controller firmware lock.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureFirmwareLock             0x0000000000000008ULL
   /** Controller supports disabling encryption.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureDisableEncryption        0x0000000000000010ULL
   /** Controller supports creating future plaintext logical drives.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureAllowPlaintextVolumes    0x0000000000000020ULL
   /** Controller supports encryption using a remote key manager.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureRemoteKeyManager         0x0000000000000040ULL
   /** Controller supports encryption using a local key manager.
    * Means that @ref kControllerEncryptionRemote is not supported.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureLocalKeyManager          0x0000000000000080ULL
   /** Controller supports local express encryption.
    * Means that @ref kControllerEncryptionLocal is not supported.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureExpressLocalEncryption   0x0000000000000100ULL
   /** Controller supports converting a logical drive to ciphertext without preserving data.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureConvertToEncrypted       0x0000000000000200ULL
   /** Controller supports encoding a plaintext volume to ciphertext while preserving data.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureEncodeVolume             0x0000000000000400ULL
   /** Controller supports encryption controller/drive rekey
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureDriveRekey               0x0000000000000800ULL
   /** Controller supports encryption volume/data rekey.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureDataRekey                0x0000000000001000ULL
   /** Controller supports modifying the master key.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureMasterKeyRekey           0x0000000000002000ULL
   /** Controller supports encryption volatile keys.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureVolatileKeys             0x0000000000004000ULL
   /** Controller supports encryption key cache.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureKeyCache                 0x0000000000008000ULL
   /** Controller supports encryption boot/controller password.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureBootPassword             0x0000000000010000ULL
   /** Controller supports encryption persistent key cache (remote key auth).
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureBootTimeRemoteAuth       0x0000000000020000ULL
   /** Controller supports encryption crypto drive erase.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureCryptoErase              0x0000000000040000ULL
   /** Controller supports importing encrypted logical drives.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureImportForeignKeys        0x0000000000080000ULL
   /** Controller supports pausing/resuming the boot/controller password.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeaturePauseResumeBootPassword  0x0000000000100000ULL
   /** Controller supports changing the key manager between local/remote.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureKeyManagerConversion     0x0000000000200000ULL
   /** Controller supports clearing encryption.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureClearEncryption          0x0000000000400000ULL
   /** Controller supports converting a logical drive to plaintext without preserving data.
    * Means that @ref kControllerEncryptionNone is not supported when using @ref storc_configuration_edit_encryption_set.
    * @outputof SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kEncryptionFeatureConvertToPlaintext       0x0000000000800000ULL
   /** @} */ /* Encryption Features */

/** @} */ /* storc_features_ctrl_encryption_information */

/** @defgroup storc_features_ctrl_carrier_authentication_support Controller Carrier Authentication Support
 * @{ */
/** Get the controller carrier authentication support.
 *
 * @param[in] cda    Controller CDA pointer
 * @return           @ref kTrue if the controller supports carrier authentication
 * @return           @ref kFalse if the controller does not support carrier authentication
 */
SA_BOOL SA_ControllerSupportsCarrierAuthentication(PCDA cda);
/** @} */ /* storc_features_ctrl_carrier_authentication_support */

/** @defgroup storc_features_ctrl_sys_prop Controller System Property
 * @{ */
/** Get the size (in bytes) of a controller system property
 *
 * @param[in] cda Controller data area.
 * @param[in] property_id Property to be read (see psysc_info.h for definition)
 * @return The number of bytes required to hold the property's value. 0 if the property is not defined.
 */
SA_WORD SA_GetControllerSystemPropertySize(PCDA cda, SA_BYTE property_id);

/** Get a system property for the controller
 *
 * @param[in] cda Controller data area.
 * @param[in] property_id Property to be read (see psysc_info.h for definition)
 * @param[in] property_buffer Memory area where the value of the property is stored
 * @param[in] property_buffer_size Size of memory area (in bytes)
 * @return @ref kGetDevicePropertyOK if property is read successfully
 * @return @ref kGetDevicePropertyNoValue if the property is not present for this controller
 * @return @ref kGetDevicePropertyInvalidPropertySize if the buffer size provided is too small
 */
SA_BYTE SA_GetControllerSystemProperty(PCDA cda, SA_BYTE property_id, void *property_buffer, SA_DWORD property_buffer_size);
/** @} */ /* storc_features_ctrl_sys_prop */

/** @defgroup storc_features_ctrl_cache_operations Controller Cache Operations Support
 * @brief Get controller support for cache operations.
 * @{ */
/** Does the controller support cache configuration operations?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports cache operations or @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsCacheOperations(PCDA cda);
/** @} */ /* storc_features_ctrl_cache_operations */

/** @defgroup storc_features_ctrl_drive_write_cache Controller Physical Drive Write Cache Support
 * @brief Get controller support for drive write cache.
 * @{ */
/** Does controller support physical drive write cache?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports physical drive write cache, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsDriveWriteCache(PCDA cda);

/** Does controller support physical drive write cache for all drive/usage types?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports physical drive write cache for all
 * drive/usage types, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsDriveWriteCacheForAllDrives(PCDA cda);
/** @} */ /* storc_features_ctrl_drive_write_cache */

/** @defgroup storc_features_ctrl_pcie Controller PCI Properties
 * @{ */
/** Test for PCI bus type support.
 *
 * @param[in] cda       Controller data area.
 * @param[in] bus_type  Specified PCI bus type (see [PCI Bus Type Constants](@ref storc_features_ctrl_pcie)).
 * @return @ref kTrue if controller supports specified PCI bus type, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsPCIBusType(PCDA cda, SA_BYTE bus_type);
   /** @name PCI Bus Type Constants
    * Input of @ref SA_ControllerSupportsPCIBusType.
    * @{ */
   #define kPCIe  0x01
   #define kPCIX  0x02
   /** @} */
/** @} */ /* storc_features_ctrl_pcie */

/** @defgroup storc_features_ctrl_lk Controller License Keys
 * @{ */
/** Does controller support installing/removing license keys?
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports adding/removing license keys, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsLicenseKeys(PCDA cda);
/** @} */ /* storc_features_ctrl_lk */

/** @defgroup storc_features_ctrl_clear_config Controller Clear Configuration
 * @{ */
/** Does controller support the clear configuration BMIC command?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports the clear configuration BMIC, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsClearConfigurationBMIC(PCDA cda);
/** @} */ /* storc_features_ctrl_clear_config */

/** @defgroup storc_features_ctrl_auto_replace_spares Controller Auto Replace Spares
 * @{ */
/** Does controller support auto-replace spares?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports auto-replace spares, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsAutoReplaceSpares(PCDA cda);
/** @} */ /* storc_features_ctrl_auto_replace_spares */

/** @defgroup storc_features_ctrl_predictive_spare_rebuild Controller Predictive Spare Rebuild Support
 * @brief Get controller support for predictive spare activation.
 * @{ */
/** Does controller support predictive spare rebuild?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports predictive spare rebuild, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsPredictiveSpareRebuild(PCDA cda);
/** @} */ /* storc_features_ctrl_predictive_spare_rebuild */

/** @defgroup storc_features_ctrl_split_mirror_backup Controller Split Mirror Backup
 * @{ */
/** Does controller support split mirror backup?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports split mirror backup, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsSplitMirrorBackup(PCDA cda);
/** @} */ /* storc_features_ctrl_split_mirror_backup */

/** @defgroup storc_features_ctrl_resync_rollback Controller Resync and Rollback Split Mirror Backup
 * @{ */
/** Does controller support resyncing and rolling back a split mirror backup?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports resyncing and rolling back a split mirror
 *         backup, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsResyncAndRollback(PCDA cda);
/** @} */ /* storc_features_ctrl_resync_rollback */

/** @defgroup storc_feature_software_raid Controller Software RAID
 * @{ */
/** Is this a software RAID controller?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if the controller is software RAID, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerIsSoftwareRAID(PCDA cda);
/** @} */ /* storc_feature_software_raid */

/** @defgroup storc_features_ctrl_heal_array Controller Heal Array
 * @{ */
/** Does controller support Heal Array?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports Heal Array, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsHealArray(PCDA cda);
/** @} */ /* storc_features_ctrl_heal_array */


/** @defgroup storc_feature_transformation Controller Transformation
 * @{ */
/** Does the controller support transformations?
 *
 * @param[in] cda  Controller data area.
 * @return See [Supported Transformation Operations](@ref storc_feature_transformation).
 */
SA_DWORD SA_ControllerSupportsTransformations(PCDA cda);
/** @name Supported Transformation Operations
 * @outputof SA_ControllerSupportsTransformations.
 * @{ */
#define kControllerSupportsArrayExpansion            0x00000001 /**< Controller supports Array expansion; @outputof SA_ControllerSupportsTransformations. */
#define kControllerSupportsStripeMigration           0x00000002 /**< Controller supports Logical Drive stripe migration; @outputof SA_ControllerSupportsTransformations. */
#define kControllerSupportsRAIDMigration             0x00000004 /**< Controller supports Logical Drive RAID migration; @outputof SA_ControllerSupportsTransformations. */
#define kControllerSupportsLogicalDriveExtension     0x00000008 /**< Controller supports Logical Drive extension; @outputof SA_ControllerSupportsTransformations. */
#define kControllerSupportsPowerlessTransformations  0x00000020 /**< Controller supports powerless transformations; @outputof SA_ControllerSupportsTransformations. */
#define kControllerSupportsLogicalDriveMove          0x80000000 /**< Controller supports Logical Drive move; @outputof SA_ControllerSupportsTransformations. */
#define kControllerSupportsArrayDriveSwap            0x40000000 /**< Controller supports Array data drive swapping (Array Move/Heal); @outputof SA_ControllerSupportsTransformations. */
/** @} */ /* Supported Transformation Operations */

/** @} */ /* storc_feature_transformations */

/** @defgroup storc_features_ctrl_support_phy_rates Controller Support Phy Rates
* @{ */
/** Does the controller support change phy rates?
*
* @param[in] cda  Controller data area.
* @return default and max phy rates if supported, 0 otherwise. @see [Default and Max Phy Rates Returns](@ref storc_features_ctrl_support_phy_rates)
*/
SA_WORD SA_ControllerSupportsPhyRates(PCDA cda);
   /** Default and Max Phy Rates Returns
   * @outputof SA_ControllerSupportsPhyRates.
   * @{ */
   #define kDefaultPhyRateMask 0x00FF  /**< Mask to use on output of @ref SA_ControllerSupportsPhyRates to get default phy rate. */
   /* @return One of [Connector Phy Rates](@ref storc_properties_ctrl_port_mode). */
   #define DEFAULT_PHY_RATE(in_value) (in_value & kDefaultPhyRateMask)

   #define kSupportedMaxPhyRate6G   0x04
   #define kSupportedMaxPhyRate12G  0x08
   #define kSupportedMaxPhyRate24G  0x10
   #define kSupportedMaxPhyRateMask 0xFF00      /**< Mask to use on output of @ref SA_ControllerSupportsPhyRates to get max phy rate. */
   #define kSupportedMaxPhyRateShift 8          /**< Bitshift to use on output of @ref SA_ControllerSupportsPhyRates to get max phy rate. */
   #define SUPPORTED_MAX_PHY_RATES(in_value) ((in_value & kSupportedMaxPhyRateMask) >> kSupportedMaxPhyRateShift )
   /** @} */
/** @} */ /* storc_features_ctrl_support_phy_rates */

/** @defgroup storc_features_ctrl_lu_cache LU Cache Support
 * @brief Get controller support for LU cache.
* @{ */

/** @name LU Cache Line Sizes
 @{ */
#define kControllerLUCacheLineSize64KB  0x01 /**< Cache line size 64KiB */
#define kControllerLUCacheLineSize128KB 0x02 /**< Cache line size 128KiB */
#define kControllerLUCacheLineSize256KB 0x04 /**< Cache line size 256KiB */
/** @} */

/** @name LU Cache Support Flags
 @{ */
/** Return a list of flags showing generic controller supported for LU cache.
 * @param[in] cda Controller handle.
 * @return Bitmap of supported LU cache capabilities of the controller (see flags at [Controller LU Cache Support Flags](@ref storc_features_ctrl_lu_cache)).
 * @return Bitmap of supported cache line sizes using @ref LU_CACHE_SUPPORTED_LINE_SIZES (see flags at [LU Cache Line Sizes](@ref storc_features_ctrl_lu_cache)).
 */
SA_WORD SA_ControllerSupportsLUCache(PCDA cda);
   /** The controller supports LU cache; @outputof SA_ControllerSupportsLUCache. */
   #define kControllerSupportsLUCacheSupported             0x0001
   /** The controller has any required licenses installed to enable LU cache; @outputof SA_ControllerSupportsLUCache. */
   #define kControllerSupportsLUCacheLicensed              0x0002
   /** The controller supports write thru LU cache; @outputof SA_ControllerSupportsLUCache. */
   #define kControllerSupportsWriteThru                    0x0004
   /** The controller supports write back LU cache; @outputof SA_ControllerSupportsLUCache. */
   #define kControllerSupportsWriteBack                    0x0008
#ifndef DOXYGEN_SHOULD_SKIP_THIS
   #define kControllerLUCacheLineSizeSupportMask           0x0700
   #define kControllerLUCacheLineSizeSupportBitshift       8
#endif
   /** Decode supported cache line sizes (from [LU Cache Line Sizes](@ref storc_features_ctrl_lu_cache)). */
   #define LU_CACHE_SUPPORTED_LINE_SIZES(info)             ((info & kControllerLUCacheLineSizeSupportMask) >> kControllerLUCacheLineSizeSupportBitshift)
/** @} */ /* LU Cache Support Flags */

/** @name LU Cache RAID/Write Policy Support
 @{ */
/** Return @ref kTrue/@ref kFalse if the RAID/Write Policy combination is supported by the controller for LU cache logical drives.
 * @param[in] cda           Controller handle.
 * @param[in] raid_level    Requested RAID level support check.
 * @param[in] write_policy  Requested LU cache write policy.
 * @return @ref kTrue if the `raid_level`/`write_policy` combination is supported by the controller, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsLUCacheRAIDPolicy(PCDA cda, SA_BYTE raid_level, SA_BYTE write_policy);
/** @} */ /* LU Cache RAID/Write Policy Support */

/** @} */ /* storc_features_ctrl_lu_cache */

/** @defgroup storc_features_ctrl_support_sanitize_lock Controller Sanitize Lock Support
 * @brief Get the controller's support for ATA Freeze/Anti-Freeze Sanitize Lock.
 * @{ */
/** Does the controller support sending ATA Freeze/Anti-Freeze command to physical drives?
 *
 * @param[in] cda  Controller data area.
 * @return [Map showing supported sanitize locks](@ref storc_features_ctrl_support_sanitize_lock).
 */
SA_BYTE SA_ControllerSupportsSanitizeLock(PCDA cda);
/** Decode output of @ref SA_ControllerSupportsSanitizeLock to determine if the controller supports sanitize freeze. */
#define kSanitizeLockFreezeSupported      0x01
/** Decode output of @ref SA_ControllerSupportsSanitizeLock to determine if the controller supports sanitize anti-freeze. */
#define kSanitizeLockAntiFreezeSupported  0x02
/** @} */ /* storc_features_ctrl_support_sanitize_lock */

/** @defgroup storc_features_ctrl_support_notify_any_broadcast Controller Notify On Any Broadcast Support
 * @brief Get controller support for "notify on any broadcast" configuration.
 * @{ */
/** Does the controller support enable/disable notify on any broadcast?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports toggling notify on any broadcast, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsNotifyOnAnyBroadcast(PCDA cda);
/** @} */ /* storc_features_ctrl_support_notify_any_broadcast */

/** @defgroup storc_feature_ctrl_support_pcie_relaxed_ordering Controller PCIe Relaxed Ordering Support
 * @brief Get controller support for PCIe relaxed ordering. If not supported, PCIe ordering defaults to strict.
 * @{ */
/** Does the controller support enable/disable PCIe relaxed ordering?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports toggling PCIe relaxed ordering, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsPCIeRelaxedOrdering(PCDA cda);
/** @} */ /* storc_feature_ctrl_support_pcie_relaxed_ordering */

/** @defgroup storc_features_ctrl_support_encryption_key_cache Controller Encryption Key Cache Support
 * @brief Get information on the controller's encryption key cache support.
 * @{ */

/** @name Controller Encryption Key Cache Support
 * @{ */
/** Obtain encryption key cache support info for the Controller.
 * @param[in] cda Controller data area
 * @return see [Encryption Key Cache Support Returns](@ref storc_features_ctrl_support_encryption_key_cache)
 * @return Decode with @ref ENCRYPTION_KEY_CACHE_SUPPORT_MIN_ATTEMPTS to get the controller's key cache minimum retry attempts.
 * @return Decode with @ref ENCRYPTION_KEY_CACHE_SUPPORT_MAX_ATTEMPTS to get the controller's key cache maximum retry attempts.
 * @return Decode with @ref ENCRYPTION_KEY_CACHE_SUPPORT_MIN_INTERVAL to get the controller's key cache minimum retry interval.
 * @return Decode with @ref ENCRYPTION_KEY_CACHE_SUPPORT_MAX_INTERVAL to get the controller's key cache maximum retry interval.
 *
 * __Example__
 * @code
 * SA_QWORD key_cache_support =  SA_ControllerEncryptionKeyCacheSupport(cda);
 * SA_WORD key_cache_min_attempts = ENCRYPTION_KEY_CACHE_SUPPORT_MIN_ATTEMPTS(key_cache_support);
 * SA_WORD key_cache_max_attempts = ENCRYPTION_KEY_CACHE_SUPPORT_MAX_ATTEMPTS(key_cache_support);
 * SA_WORD key_cache_min_interval = ENCRYPTION_KEY_CACHE_SUPPORT_MIN_INTERVAL(key_cache_support);
 * SA_WORD key_cache_max_interval = ENCRYPTION_KEY_CACHE_SUPPORT_MAX_INTERVAL(key_cache_support);
 * printf("Minimum retry attempts: %u\n", key_cache_min_attempts);
 * printf("Maximum retry attempts: %u\n", key_cache_max_attempts);
 * printf("Minimum retry interval: %u\n", key_cache_min_interval);
 * printf("Maximum retry interval: %u\n", key_cache_max_interval);
 * @endcode
 */
SA_QWORD SA_ControllerEncryptionKeyCacheSupport(PCDA cda);
   /** Controller encryption key cache minimum retry attempt mask; @outputof SA_ControllerEncryptionKeyCacheSupport. */
   #define kControllerEncryptionKeyCacheSupportMinRetryAttemptsMask      0x000000000000FFFFULL
   /** Bitshift to use with output of @ref SA_ControllerEncryptionKeyCacheSupport.
    * (after masking with @ref kControllerEncryptionKeyCacheSupportMinRetryAttemptsMask) to get the controller's key cache minimum retry attempts.
   */
   #define kControllerEncryptionKeyCacheSupportMinRetryAttemptsBitShift  0
   /** Decode @outputof SA_ControllerEncryptionKeyCacheSupport to determine the controller's key cache minimum retry attempts. */
   #define ENCRYPTION_KEY_CACHE_SUPPORT_MIN_ATTEMPTS(info) ((SA_WORD)((info & kControllerEncryptionKeyCacheSupportMinRetryAttemptsMask) >> kControllerEncryptionKeyCacheSupportMinRetryAttemptsBitShift))

   /** Controller encryption key cache maximum retry attempt mask; @outputof SA_ControllerEncryptionKeyCacheSupport. */
   #define kControllerEncryptionKeyCacheSupportMaxRetryAttemptsMask      0x00000000FFFF0000ULL
   /** Bitshift to use with output of @ref SA_ControllerEncryptionKeyCacheSupport.
    * (after masking with @ref kControllerEncryptionKeyCacheSupportMaxRetryAttemptsMask) to get the controller's key cache maximum retry attempts.
   */
   #define kControllerEncryptionKeyCacheSupportMaxRetryAttemptsBitShift  16
   /** Decode @outputof SA_ControllerEncryptionKeyCacheSupport to determine the controller's key cache maximum retry attempts. */
   #define ENCRYPTION_KEY_CACHE_SUPPORT_MAX_ATTEMPTS(info) ((SA_WORD)((info & kControllerEncryptionKeyCacheSupportMaxRetryAttemptsMask) >> kControllerEncryptionKeyCacheSupportMaxRetryAttemptsBitShift))

   /** Decode @outputof SA_ControllerEncryptionKeyCacheSupport to determine if the controller supports retry attempts .*/
   #define ENCRYPTION_KEY_CACHE_MAX_ATTEMPTS_SUPPORTED(info) (ENCRYPTION_KEY_CACHE_SUPPORT_MAX_ATTEMPTS(info) > 0 ? kTrue : kFalse)

   /** Controller encryption key cache minimum retry interval mask; @outputof SA_ControllerEncryptionKeyCacheSupport. */
   #define kControllerEncryptionKeyCacheSupportMinRetryIntervalMask      0x0000FFFF00000000ULL
   /** Bitshift to use with output of @ref SA_ControllerEncryptionKeyCacheSupport.
    * (after masking with @ref kControllerEncryptionKeyCacheSupportMinRetryIntervalMask) to get the controller's key cache minimum retry interval.
   */
   #define kControllerEncryptionKeyCacheSupportMinRetryIntervalBitShift  32
   /** Decode @outputof SA_ControllerEncryptionKeyCacheSupport to determine the controller's key cache minimum retry interval. */
   #define ENCRYPTION_KEY_CACHE_SUPPORT_MIN_INTERVAL(info) ((SA_WORD)((info & kControllerEncryptionKeyCacheSupportMinRetryIntervalMask) >> kControllerEncryptionKeyCacheSupportMinRetryIntervalBitShift))

   /** Controller encryption key cache maximum retry interval mask; @outputof SA_ControllerEncryptionKeyCacheSupport. */
   #define kControllerEncryptionKeyCacheSupportMaxRetryIntervalMask      0xFFFF000000000000ULL
   /** Bitshift to use with output of @ref SA_ControllerEncryptionKeyCacheSupport.
    * (after masking with @ref kControllerEncryptionKeyCacheSupportMaxRetryIntervalMask) to get the controller's key cache maximum retry interval.
   */
   #define kControllerEncryptionKeyCacheSupportMaxRetryIntervalBitShift  48
   /** Decode @outputof SA_ControllerEncryptionKeyCacheSupport to determine the controller's key cache maximum retry interval. */
   #define ENCRYPTION_KEY_CACHE_SUPPORT_MAX_INTERVAL(info) ((SA_WORD)((info & kControllerEncryptionKeyCacheSupportMaxRetryIntervalMask) >> kControllerEncryptionKeyCacheSupportMaxRetryIntervalBitShift))

   /** Decode @outputof SA_ControllerEncryptionKeyCacheSupport to determine if the controller supports retry intervals. */
   #define ENCRYPTION_KEY_CACHE_INTERVAL_SUPPORTED(info) (ENCRYPTION_KEY_CACHE_SUPPORT_MAX_INTERVAL(info) > 0 ? kTrue : kFalse)
/** @} */ /* Controller Encryption Key Cache Support */

/** @} */ /* storc_features_ctrl_support_encryption_key_cache */

/** @defgroup storc_features_ctrl_support_encryption_persistent_key_cache Controller Encryption Persistent Key Cache Support
 * @brief Return if the controller supports encryption persistent key cache.
 * @{ */
/** Does the controller support encryption persistent key cache?
 *
 * @param[in] cda  Controller data area.
 * @return @ref kTrue if controller supports encryption persistent key cache, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsPersistentKeyCache(PCDA cda);
/** @} */ /* storc_features_ctrl_support_encryption_persistent_key_cache */

/** @defgroup storc_features_ctrl_support_boot_connectors Controller Boot Connectors Support
 * @brief Return if the controller supports boot connectors
 * @{ */
/** Get the controller boot connector support.
 * @param[in] cda  Controller handler.
 * @return @ref kTrue if boot connectors are supported, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsBootConnectors(PCDA cda);
/** @} */ /* storc_features_ctrl_support_boot_connectors */

/** @defgroup storc_features_ctrl_support_online_firmware_activation Controller Online Firmware Activation
 * @{ */
/** Does the controller support online controller firmware activation?
 *
 * @param[in] cda Controller data area.
 * @return A bitmap of values that describe the controller's online firmware activation support.
 * @return Bit described using [Controller Online Firmware Activation Support](@ref storc_features_ctrl_support_online_firmware_activation).
 */
SA_UINT32 SA_ControllerSupportsOnlineFirmwareActivation(PCDA cda);
   /** @name Controller Online Firmware Activation Support
    * @{ */
   /** Set if the controller supports online firmware activation.
    * @outputof SA_ControllerSupportsOnlineFirmwareActivation
    */
   #define kControllerOnlineFirmwareActivationSupported  0x00000001
   /** Set if the required online firmware activation capable driver is loaded.
    * @outputof SA_ControllerSupportsOnlineFirmwareActivation
    */
   #define kControllerOnlineFirmwareActivationEnabled    0x00000002
   /** @} */ /* Controller Online Firmware Activation Support */
/** @} */ /* storc_features_ctrl_support_online_firmware_activation */

/** @defgroup storc_features_ctrl_supports_ofa_soft_reset Controller OFA Soft Reset Support
 * @brief Return if the controller supports OFA Soft Reset
 * @{ */
/** Get the controller OFA Soft Reset support.
 * @param[in] cda Controller handle.
 * @return @ref kTrue if Soft OFA Reset is supported, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsOFASoftReset(PCDA cda);
/** @} */ /* storc_features_ctrl_supports_ofa_soft_reset */

/** @defgroup storc_features_ctrl_support_sed Controller SED Support
 * @{ */
/** Does the controller support self encrypting drives?
 *
 * @param[in] cda Controller data area.
 * @return A bitmap of values that describe the controller's support of SED (self-encrypting-drives).
 * @return Bit described using [Controller SED Support](@ref storc_features_ctrl_support_sed).
 * @see storc_encryption_status_info.
 */
SA_UINT32 SA_ControllerSupportsSED(PCDA cda);
   /** @name Controller SED Support
    * @{ */
   /** Set if the controller supports reporting basic information of SED and security status.
    * Also see [Physical Drive Encryption Info](@ref storc_encryption_status_info).
    * @outputof SA_ControllerSupportsSED
    */
   #define kControllerSEDInfoSupported 0x00000001
   /** @} */ /* Controller SED Support */
/** @} */ /* storc_features_ctrl_support_sed */

/** @defgroup storc_features_ctrl_support_connector_discovery_configuration Controller Connector Discovery Configuration Support
 * @{ */
/** Controller connector backplane discovery configuration support.
 *
 * @param[in] cda Controller data area.
 * @return A bitmap of values that describe the controller's connector discovery configuration support.
 * @return Bit described using [Controller Connector Discovery Configuration Support](@ref storc_features_ctrl_support_connector_discovery_configuration).
 */
SA_BYTE SA_GetControllerConnectorDiscoveryConfigurationSupport(PCDA cda);
   /** @name Controller Connector Discovery Configuration Support
    * @{ */
   /** Command to get the active controller backplane connector discovery configuration is supported.
    * @outputof SA_GetControllerConnectorDiscoveryConfigurationSupport.
    */
   #define kControllerGetActiveConnectorDiscoveryConfigurationSupported    0x01
   /** Command to get the pending controller backplane connector discovery configuration is supported.
    * @outputof SA_GetControllerConnectorDiscoveryConfigurationSupport.
    */
   #define kControllerGetPendingConnectorDiscoveryConfigurationSupported   0x02
   /** Command to change the pending controller backplane connector discovery configuration is supported.
    * @outputof SA_GetControllerConnectorDiscoveryConfigurationSupport.
    */
   #define kControllerChangeConnectorDiscoveryConfigurationSupported       0x04
   /** Command to get the default controller backplane connector discovery configuration is supported.
    * @outputof SA_GetControllerConnectorDiscoveryConfigurationSupport.
    */
   #define kControllerGetDefaultConnectorDiscoveryConfigurationSupported   0x08
   /** @} */ /* Controller Connector Discovery Configuration Support */
/** @} */ /* storc_features_ctrl_support_connector_discovery_configuration */

/** @defgroup storc_features_ctrl_supports_sed_security_refresh Controller Supports SED Security Refresh
 * @brief Return if the controller supports refreshing the security status of self encrypting drives
 * @{ */
/** Get the controller SED security refresh support
 * @param[in] cda Controller handle.
 * @return @ref kTrue if SED security refresh is supported, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsSEDSecurityRefresh(PCDA cda);
/** @} */ /* storc_features_ctrl_supports_sed_security_refresh */

/** @defgroup storc_features_ctrl_support_debug_log_commands Controller Debug and Logging Command Support
 * @{ */
/** Controller persistent event log support.
 *
 * @post If supported, user may call @ref SA_GetControllerPersistentEventLog to retrieve the persistent
 * event log of the controller.
 *
 * @param[in] cda Controller data area.
 * @return @ref kTrue if the controller supports collecting the persistent event log, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsPersistentEventLog(PCDA cda);
/** @} */ /* storc_features_ctrl_support_debug_log_commands */

/** @defgroup storc_features_ctrl_support_connector_active_phy_map Connector Active Phy Map Support
 * @{ */
/** Controller connector active phy map support.
 *
 * @post If supported, any subsequent calls to @ref SA_GetControllerConnectorInfo will populate the
 * active_phy_map field of the struct pointed to by the connector_info parameter which was passed in the call.
 *
 * @param[in] cda Controller data area.
 * @return @ref kTrue if the controller connector active phy map is supported, @ref kFalse otherwise.
 */
SA_BOOL SA_ControllerSupportsConnectorActivePhyMap(PCDA cda);
/** @} */ /* storc_features_ctrl_support_connector_active_phy_map */

/** @} */ /* storc_features_ctrl */

/**************************************************//**
 * @defgroup storc_features_pd Physical Drive Features
 * @brief
 * @details
 * @{
 *****************************************************/

/** @defgroup storc_features_pd_erase_patterns_supported Physical Drive Erase Patterns Supported
 * @brief Get physical drive erase patterns support by the controller.
 * @{ */
/** Get the erase patterns that selected physical drives supports.
 *
 * @param[in] cda       Controller data area.
 * @param[in] pd_number Target physical drive number.
 * @return A bitmap of supported erase patterns (see [Erase Pattern Values](@ref storc_features_pd_erase_patterns_supported)).
 */
SA_WORD SA_PhysicalDriveSupportsErasePattern(PCDA cda, SA_WORD pd_number);
   /** @name Erase Pattern Values
    * @{ */
   #define kErasePattern1Pass                      0x0001  /**< 1Pass: Controller-based write zeroes. */
   #define kErasePattern2Pass                      0x0002  /**< 2Pass: Controller-based write random-zeroes. */
   #define kErasePattern3Pass                      0x0004  /**< 3Pass: Controller-based write random-random-zeroes. */
   #define kErasePatternOverwriteRestricted        0x0008  /**< Restricted Overwrite: Drive-based sanitize restricted overwrite, only available on supported HDDs. */
   #define kErasePatternOverwriteUnrestricted      0x0010  /**< Unrestricted Overwrite: Drive-based sanitize unrestricted overwrite, only available on supported HDDs. */
   #define kErasePatternBlockEraseRestricted       0x0020  /**< Restricted Block Erase: Drive-based sanitize restricted block erase, only available on supported SSDs. */
   #define kErasePatternBlockEraseUnrestricted     0x0040  /**< Unrestricted Block Erase: Drive-based sanitize unrestricted block erase, only available on supported SSDs. */
   #define kErasePatternCryptoScrambleRestricted   0x0080  /**< Restricted Crypto Scramble: Drive-based sanitize restricted crypto scramble, only available on supported drives. */
   #define kErasePatternCryptoScrambleUnrestricted 0x0100  /**< Unrestricted Crypto Scramble: Drive-based sanitize unrestricted crypto scramble, only available on supported drives. */
   #define kErasePatternSanitizeUnknown            0x8000  /**< Unknown Sanitize: Drive supports sanitize, but pattern information is unavailable because drive is currently sanitizing. */
   /** @} */
/** @} */ /* storc_features_pd_erase_patterns_supported */

/** @defgroup storc_features_pd_support_sanitize_lock Physical Drive Support Freeze/AntiFreeze Sanitize Lock
* @{ */
/** Does the physical drive support ATA Freeze/Anti-Freeze?
 *
 * @pre Only valid if @ref SA_ControllerSupportsSanitizeLock returns non-zero.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Physical drive number.
 * @return [Map showing supported sanitize locks](@ref storc_features_ctrl_support_sanitize_lock).
 */
SA_BYTE SA_PhysicalDriveSupportsSanitizeLock(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_features_pd_support_sanitize_lock */

/** defgroup storc_features_pd_oem_extended_data Physical Drive OEM Extended Data Support
 * @brief Get the physical drive OEM extended data support.
 * @{ */
/** Does the physical drive support reporting OEM extended data?
 *
 * @param[in] cda       Controller data area.
 * @param[in] pd_number Target physical drive number.
 * @return @ref kTrue if OEM extended data is supported, @ref kFalse otherwise.
 * @post User can call @ref SA_GetPhysicalDriveOEMExtendedData.
 */
SA_BOOL SA_PhysicalDriveSupportsOEMExtendedDriveData(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_features_pd_oem_extended_data */

/** @} */ /* storc_features_pd */

#if defined(__cplusplus)
}
#endif

/** @} */ /* storc_features */

/** @} */ /* storcore */

#endif /* PSTORC_FEATURES_H */

