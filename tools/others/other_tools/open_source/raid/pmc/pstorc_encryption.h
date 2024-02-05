/** @file pstorc_encryption.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore encryption header file
   Defines functions for encryption related procedures.

*/

#ifndef STORC_ENCRYPTION_H
#define STORC_ENCRYPTION_H

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/************************************//**
 * @defgroup storc_encryption Encryption
 * @brief Encryption status and direct configuration APIs.
 * @details
 * For virtual encryption setup/configuration, see @ref storc_configuration_edit_encryption.
 * @{
 * @addtogroup storc_configuration_edit_encryption
 * @defgroup storc_encryption_support Encryption Support
 * @{
 * @addtogroup storc_features_ctrl_encryption_information
 * @addtogroup storc_features_ctrl_support_encryption_key_cache
 * @addtogroup storc_features_ctrl_support_encryption_persistent_key_cache
 * @}
 ***************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/** @name Encryption Configurations
 * @{ */
#define kControllerEncryptionNone                0x00 /**< No encryption configuration */
#define kControllerEncryptionExpressLocal        0x01 /**< Local express key manager */
#define kControllerEncryptionLocal               0x02 /**< Local controller key manager */
#define kControllerEncryptionRemote              0x03 /**< ESKM remote key manager */
#define kControllerEncryptionInvalid             0xFF /**< Invalid/Unknown encryption key manager */
/** @} */ /* Encryption Configurations */

/** @name Encryption Users
 * @{ */
/** Denotes the encryption administrator user which has the highest encryption privileges. */
#define kControllerEncryptionCryptoOfficer 0x00
/** Denotes the encryption user which has restricted privileges. */
#define kControllerEncryptionCryptoUser    0x01
/** @} */ /* Encryption Users */

/** @defgroup storc_encryption_status_info Encryption Status
 * @brief Return information on the status/state of the controller's encryption configuration.
 * @ingroup storc_status
 * @{
 */
/** @name Controller Encryption Status
 * @{ */
/** Obtain encryption status info for a Controller.
 * @param[in] cda  Controller data area.
 * @return Map of statuses
 *
 * __Example__
 * @code
 * SA_DWORD ctl_enc_status = SA_GetControllerEncryptionStatus(cda);
 * printf("Controller Encryption Statuses\n");
 * if ((ctl_enc_status & kControllerEncryptionEnabled))
 *     printf(" - Enabled\n");
 * if ((ctl_enc_status & kControllerEncryptionLocalMode))
 *     printf(" - Local Mode\n");
 * if ((ctl_enc_status & kControllerEncryptionCryptoOfficerRecoverySet))
 *     printf(" - Crypto-Officer Recovery Set\n");
 * if ((ctl_enc_status & kControllerEncryptionCryptoOfficerPasswordSet))
 *     printf(" - Crypto-Officer Password Set\n");
 * if ((ctl_enc_status & kControllerEncryptionExpressModeOn))
 *     printf(" - Express Mode\n");
 * if ((ctl_enc_status & kControllerEncryptionControllerLocked))
 *     printf(" - Controller Locked\n");
 *  //...
 * @endcode
 */
SA_QWORD SA_GetControllerEncryptionStatus(PCDA cda);
   #define kControllerEncryptionEnabled                     0x0000000000000001ULL  /**< Controller encryption status enabled; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionLocalMode                   0x0000000000000002ULL  /**< Controller encryption status in local mode; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionCryptoOfficerRecoverySet    0x0000000000000004ULL  /**< Controller encryption status crypto officer recovery set; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionCryptoOfficerPasswordSet    0x0000000000000008ULL  /**< Controller encryption status crypto officer password set; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionExpressModeOn               0x0000000000000010ULL  /**< Controller encryption status in express mode; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionControllerLocked            0x0000000000000020ULL  /**< Controller encryption status locked; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionKeyCacheEnabled             0x0000000000000040ULL  /**< Controller encryption status key cache enabled; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionUserPasswordSet             0x0000000000000080ULL  /**< Controller encryption status user password set; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionFwLocked                    0x0000000000000100ULL  /**< Controller encryption status firmware is locked and flashing is disabled; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionKekSet                      0x0000000000000200ULL  /**< Controller encryption status master key set; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionMissingBootPassword         0x0000000000000400ULL  /**< Controller encryption status drives are locked due to missing boot password; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionMixedVolumesEnabled         0x0000000000000800ULL  /**< Controller encryption status mixed volumes enabled; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionBootPasswordSet             0x0000000000001000ULL  /**< Controller encryption status boot password set; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionBootPasswordSuspended       0x0000000000002000ULL  /**< Controller encryption status boot password is suspended; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionRemoteKekSet                0x0000000000004000ULL  /**< Controller encryption status remote kek is set; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionSkipBootPassword            0x0000000000008000ULL  /**< Controller encryption status skip controller boot password; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionMasterKeyResetInProgress    0x0000000000010000ULL  /**< Controller encryption status master key reset in progress; @outputof SA_GetControllerEncryptionStatus. */

   /* *******************/
   /* Encryption Errors */
   /* *******************/
   #define kControllerEncryptionSelfTestFailed              0x0000000000020000ULL  /**< Controller encryption status self test failed; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionCSPNVRAMDetectFailed        0x0000000000040000ULL  /**< Controller encryption status CSP NVRAM was not detected; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionRTCSRAMDetectFailed         0x0000000000080000ULL  /**< Controller encryption status RTC SRAM was not detected; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionRTFCSPNVRAMCorrupt          0x0000000000100000ULL  /**< Controller encryption was returned to factory settings because: CSP NVRAM is corrupt; @outputof SA_GetControllerEncryptionStatus. */

   /* *****************************/
   /* Remote Key Retrieval Status */
   /* *****************************/
   #define kControllerEncryptionKeyRetrievalNotNeeded       0x0000000000200000ULL  /**< Remote key retrieval not needed; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionKeyRetrievalNeeded          0x0000000000400000ULL  /**< Remote key retrieval needed; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionKeyRetrievalRequested       0x0000000000800000ULL  /**< Remote key retrieval requested; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionKeyRetrievalValid           0x0000000001000000ULL  /**< Remote key retrieval valid; @outputof SA_GetControllerEncryptionStatus. */

   /* ******************************/
   /* Remote Key Connection Status */
   /* ******************************/

   /** Remote key connection not configured.
    * If set, remote key manager connection is not configured.
    * @outputof SA_GetControllerEncryptionStatus.
    * @pre @ref kControllerEncryptionKeyRetrievalValid is set.
    */
   #define kControllerEncryptionKeyConnectionNotConfigured           0x0000000002000000ULL
   /** Remote key connection is configured for redundancy.
    * This bit says nothing of the status of the connection.
    * @outputof SA_GetControllerEncryptionStatus.
    * @pre @ref kControllerEncryptionKeyRetrievalValid is set.
    */
   #define kControllerEncryptionKeyConnectionConfiguredForRedundancy 0x0000000004000000ULL
   /** Remote key connection is currently redundant.
    * @verbatim
    * If not set,
    *   and kControllerEncryptionKeyConnectionConfiguredForRedundancy is set,
    *   and kControllerEncryptionKeyConnectionNoConnection is not set,
    *   then there is only one connection to the remote key manager.
    * @endverbatim
    * @outputof SA_GetControllerEncryptionStatus.
    * @pre @ref kControllerEncryptionKeyRetrievalValid is set.
    */
   #define kControllerEncryptionKeyConnectionIsRedundant             0x0000000008000000ULL
   /** Remote key connection is configured and non-redundant.
    * If set, remote key manager connection is configured and non-redundant with no issues.
    * @outputof SA_GetControllerEncryptionStatus.
    * @pre @ref kControllerEncryptionKeyRetrievalValid is set.
    */
   #define kControllerEncryptionKeyConnectionNotRedundant            0x0000000010000000ULL
   /** Remote key connection is currently not connected.
    * @outputof SA_GetControllerEncryptionStatus.
    * @pre @ref kControllerEncryptionKeyRetrievalValid is set.
    */
   #define kControllerEncryptionKeyConnectionNoConnection            0x0000000020000000ULL

   /* ************************/
   /* Encryption Boot Status */
   /* ************************/

   /** Boot not complete, boot status not valid; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionBootNotComplete                      0x0000000040000000ULL
   /** Boot Complete, boot status valid; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionBootComplete                         0x0000000080000000ULL
   /** Encrypted logical drives are disabled because of a key retrieval failure due to connectivity issues; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionBootStatusRemoteKeyRetrievalFailed   0x0000000100000000ULL
   /** Encrypted logical drives are disabled because a Master Key is missing or incorrect on the Remote Key Manager; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionBootStatusMissingRemoteMasterKey     0x0000000200000000ULL
   /** Encrypted logical drives are disabled because of missing Drive Key(s) on the Remote Key Manager; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionBootStatusMissingRemoteDriveKeys     0x0000000400000000ULL
   /** Encrypted logical drives are disabled because Volume Key(s) failed to decrypt properly; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionBootStatusFailedToDecryptVolumeKey   0x0000000800000000ULL
   /** Encrypted logical drives are disabled because a Local Master Key is missing on an encrypted logical drive import; @outputof SA_GetControllerEncryptionStatus. */
   #define kControllerEncryptionBootStatusNeedKeyImport              0x0000001000000000ULL

   /** Decode @outputof SA_GetControllerEncryptionStatus to determine whether or not encryption is configured. */
   #define CTRL_ENCRYPTION_IS_CONFIGURED(info) (info & kControllerEncryptionCryptoOfficerPasswordSet)
   /** Decode @outputof SA_GetControllerEncryptionStatus to determine if encryption is in express local mode. */
   #define CTRL_ENCRYPTION_IS_EXPRESS_LOCAL(info) (info & kControllerEncryptionExpressModeOn)
   /** Decode @outputof SA_GetControllerEncryptionStatus to determine if encryption is in local key management mode. */
   #define CTRL_ENCRYPTION_IS_LOCAL(info) (!CTRL_ENCRYPTION_IS_EXPRESS_LOCAL(info) && (info & kControllerEncryptionLocalMode))
   /** Decode @outputof SA_GetControllerEncryptionStatus to determine if encryption is in remote key management mode. */
   #define CTRL_ENCRYPTION_IS_REMOTE(info) (!CTRL_ENCRYPTION_IS_EXPRESS_LOCAL(info) && !CTRL_ENCRYPTION_IS_LOCAL(info) && (info & kControllerEncryptionCryptoOfficerPasswordSet))

   /* ************************/
   /* Encryption IKEK Status */
   /* ************************/

   /** Status for this key is unknown or an error occurred; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusUnknown                    0xFF
   /** Status for this key is not applicable; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusNotApplicable              0
   /** The key is needed but has not been requested from the remote key manager; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusNeededButNotRequested      1
   /** The key has been requested from the remote key manager; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusRequested                  2
   /** The key was obtained from the remote key manager; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusObtained                   3
   /** The remote key manager indicated that the key was not found; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusNotFound                   4
   /** The key was not obtained from the remote key manager because of a connection error; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusConnectionError            5
   /** Key that was obtained from the key manager was not able to unlock a volume that required this key; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusKeyMismatch                6
   /** The key was obtained but needs to be verified before being set; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusKeyNeedsVerify             7
   /** The key was obtained and it is currently getting the required information to be verified; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusKeyVerifyInProgress        8
   /** The key that was obtained failed to be verified; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusKeyVerifyFailed            9
   /** The key was imported into a key manager and needs to check if it replicated in another key manager if configured for redundancy; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusKeyNeedsCheckReplication  10
   /** The key is currently being checked to see if it is imported in all key managers if configured for redundancy; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusCheckReplication          11
   /** The key failed to be replicated in all key managers when configured for redundancy; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusKeyFailedReplication      12
   /** The key is currently being imported into a key manager; @outputof SA_GetControllerEncryptionStatus.
    * Value of @ref CTRL_ENCRYPTION_IKEK_STATUS, @ref CTRL_ENCRYPTION_IKEK_RESET_STATUS, and @ref CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS.
    */
   #define kControllerEncryptionIKEKStatusBeingImported             13

#ifndef DOXYGEN_SHOULD_SKIP_THIS
   #define kControllerEncryptionIKEKStatusMask                      0x00000FF000000000ULL
   #define kControllerEncryptionIKEKStatusBitShift                  36
   #define kControllerEncryptionIKEKResetStatusMask                 0x000FF00000000000ULL
   #define kControllerEncryptionIKEKResetStatusBitShift             44
   #define kControllerEncryptionIKEKControllerKeyStatusMask         0x0FF0000000000000ULL
   #define kControllerEncryptionIKEKControllerKeyStatusBitShift     52
#endif
   /** Decode @outputof SA_GetControllerEncryptionStatus for status information regarding the retrieval of ikek. */
   #define CTRL_ENCRYPTION_IKEK_STATUS(info) ((SA_BYTE)((info & kControllerEncryptionIKEKStatusMask) >> kControllerEncryptionIKEKStatusBitShift))
   /** Decode @outputof SA_GetControllerEncryptionStatus for status information regarding the ikek reset. */
   #define CTRL_ENCRYPTION_IKEK_RESET_STATUS(info) ((SA_BYTE)((info & kControllerEncryptionIKEKResetStatusMask) >> kControllerEncryptionIKEKResetStatusBitShift))
   /** Decode @outputof SA_GetControllerEncryptionStatus for status information regarding the ikek controller keys. */
   #define CTRL_ENCRYPTION_IKEK_CTRL_KEY_STATUS(info) ((SA_BYTE)((info & kControllerEncryptionIKEKControllerKeyStatusMask) >> kControllerEncryptionIKEKControllerKeyStatusBitShift))

   /** Decode @outputof SA_GetControllerEncryptionStatus to return which encryption mode (key manager)
    * is configured on the controller.
    */
   #define CTRL_ENCRYPTION_MODE(info) \
      ( CTRL_ENCRYPTION_IS_EXPRESS_LOCAL(info) ? kControllerEncryptionExpressLocal \
      : CTRL_ENCRYPTION_IS_LOCAL(info)         ? kControllerEncryptionLocal \
      : CTRL_ENCRYPTION_IS_REMOTE(info)        ? kControllerEncryptionRemote \
                                               : kControllerEncryptionNone )
/** @} */ /* Controller Encryption Status */

/** @name Controller Encryption Drive Count
 * @{ */
/** Get the number of encrypted physical drives.
 * @param[in] cda  Controller data area.
 * @return the number of encrypted physical drives.
 */
SA_DWORD SA_GetControllerEncryptionPhysicalDriveCount(PCDA cda);
/** @} */

/** @name Encryption Password Status
 * @{ */
/** Get password (login attempt) status for a given user.
 * @pre Encryption is currently configured on the controller.
 * @param[in] cda      Controller handle.
 * @param[in] user     Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @return Use @ref VERIFY_PASSWORD_ATTEMPTS_REMAINING to get number of remaining attempts allowed before lockout occurs.
 * @return Use @ref VERIFY_PASSWORD_LOCKOUT_SECONDS_REMAINING to get seconds remaining until lockout expires.
 */
SA_QWORD SA_GetEncryptionPasswordStatus(PCDA cda, SA_BYTE user);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kVerifyPasswordAttemptsRemainingMask            0x00000000FFFFFFFFULL
#define kVerifyPasswordAttemptsRemainingBitShift        0ULL
#endif
/** Decode number of remaining attempts allowed before lockout occurs; @outputof SA_VerifyEncryptionPassword.; @outputof SA_VerifyEncryptionPasswordSafe. */
#define VERIFY_PASSWORD_ATTEMPTS_REMAINING(info)        ((info & kVerifyPasswordAttemptsRemainingMask) >> kVerifyPasswordAttemptsRemainingBitShift)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kVerifyPasswordLockoutSecondsRemainingMask      0xFFFFFFFF00000000ULL
#define kVerifyPasswordLockoutSecondsRemainingBitShift  32ULL
#endif
/** seconds remaining until lockout expires; @outputof SA_VerifyEncryptionPassword.; @outputof SA_VerifyEncryptionPasswordSafe. */
#define VERIFY_PASSWORD_LOCKOUT_SECONDS_REMAINING(info) ((info & kVerifyPasswordLockoutSecondsRemainingMask) >> kVerifyPasswordLockoutSecondsRemainingBitShift)
/** @} */ /* Encryption Password Status */

/** @name Logical Drive Encryption Status
 * @{ */
/** Get the volatile key information of an encrypted logical drive.
 *
 * @pre `ld_number` is a valid logical drive number.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @return @ref LOGICAL_DRIVE_ENCRYPTION_STATUS
 * @return @ref VOLATILE_KEY_ENABLED
 * @return @ref VOLATILE_KEY_BACKUP_ENABLED
 * @return @ref VOLATILE_KEY_STATUS
 *
 * __Example__
 * @code{.c}
 * SA_DWORD ld_status = SA_GetLogicalDriveEncryptionInfo(cda, ld_number);
 * printf("Logical Drive %u Encryption Status\n", ld_number);
 * if (LOGICAL_DRIVE_ENCRYPTION_STATUS(ld_status) & kLogicalDriveEncryptionEnabled)
 *     printf(" - Enabled\n");
 * if (LOGICAL_DRIVE_ENCRYPTION_STATUS(ld_status) & kLogicalDriveEncryptionKeyWait)
 *     printf(" - Encryption Key Wait\n");
 * //...
 * printf("Logical Drive %u Volatile Key %s\n", ld_number,
 *     VOLATILE_KEY_ENABLED(ld_status) ? "Enabled" : "Disabled");
 * // ...
 * printf("Logical Drive %u Volatile Key Backup %s\n", ld_number,
 *     VOLATILE_KEY_BACKUP_ENABLED(ld_status) ? "Enabled" : "Disabled");
 * //...
 * printf("Logical Drive %u Encryption Volatile Key Status: %s\n", ld_number,
 *       VOLATILE_KEY_STATUS(ld_status) == kLogicalDriveEncryptionVolatileKeyStatusBackupInProgress ? "Backup In Progress"
 *     : VOLATILE_KEY_STATUS(ld_status) == kLogicalDriveEncryptionVolatileKeyStatusBackupAndSetInProgress ? "Backup And Set In Progress"
 *     ...);
 * @endcode
 *
 */
SA_DWORD SA_GetLogicalDriveEncryptionInfo(PCDA cda, SA_WORD ld_number);
   #define kLogicalDriveEncryptionEnabled        0x01  /**< Logical Drive encryption status enabled; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveEncryptionKeyWait        0x02  /**< Logical Drive encryption status waiting for key; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveEncryptionReKeyWait      0x04  /**< Logical Drive encryption status waiting for re-key; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveEncryptionReKeyRunning   0x08  /**< Logical Drive encryption status re-keying; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveEncryptionEncodeWait     0x10  /**< Logical Drive encryption status waiting for encoding; @outputof SA_GetLogicalDriveStatusInfo. */
   #define kLogicalDriveEncryptionEncodeRunning  0x20  /**< Logical Drive encryption status encoding; @outputof SA_GetLogicalDriveStatusInfo. */
   /** Use to mask output of @ref SA_GetLogicalDriveStatusInfo for the encryption status of a logical drive.
    * @see LOGICAL_DRIVE_ENCRYPTION_STATUS.
    */
   #define kLogicalDriveEncryptionStatusMask     0x000000FF
   /** Use to decode logical drive's encryption status output of @ref SA_GetLogicalDriveStatusInfo.
    *
    * __Example__
    * @code{.c}
    * SA_DWORD ld_status = SA_GetLogicalDriveEncryptionInfo(cda, ld_number);
    * printf("Logical Drive %u Encryption Statuses\n", ld_number);
    * if (LOGICAL_DRIVE_ENCRYPTION_STATUS(ld_status) & kLogicalDriveEncryptionEnabled)
    *     printf(" - Enabled\n");
    * if (LOGICAL_DRIVE_ENCRYPTION_STATUS(ld_status) & kLogicalDriveEncryptionKeyWait)
    *     printf(" - Encryption Key Wait\n");
    * // ...
    * @endcode
    */
   #define LOGICAL_DRIVE_ENCRYPTION_STATUS(info_value) ((SA_BYTE)(info_value & kLogicalDriveEncryptionStatusMask))

   #define kLogicalDriveEncryptionVolatileKeyEnabled                       0x01 /**< Volatile keys are enabled for the logical drive; @outputof SA_GetLogicalDriveEncryptionInfo. */
   #define kLogicalDriveEncryptionVolatileKeyDisabled                      0x02 /**< Volatile keys are disabled for the logical drive; @outputof SA_GetLogicalDriveEncryptionInfo. */
   /** Mask output of @ref SA_GetLogicalDriveEncryptionInfo to determine if volatile key is enabled/disabled. */
   #define kLogicalDriveEncryptionVolatileKeyMask                          0x00000300
   /** Bitshift to use with output of @ref SA_GetLogicalDriveEncryptionInfo (after masking with @ref kLogicalDriveEncryptionVolatileKeyMask) to get the encryption volatile key toggle. */
   #define kLogicalDriveEncryptionVolatileKeyBitShift                      8
   /** Use to decode logical drive's encryption volatile status output of @ref SA_GetLogicalDriveStatusInfo.
    *
    * __Example__
    * @code{.c}
    * SA_DWORD ld_status = SA_GetLogicalDriveEncryptionInfo(cda, ld_number);
    * printf("Logical Drive %u Volatile Key %s\n", ld_number,
    *     VOLATILE_KEY_ENABLED(ld_status) ? "Enabled" : "Disabled");
    * @endcode
    */
   #define VOLATILE_KEY_ENABLED(info_value) ((((info_value & kLogicalDriveEncryptionVolatileKeyMask) >> kLogicalDriveEncryptionVolatileKeyBitShift) == kLogicalDriveEncryptionVolatileKeyEnabled) ? kTrue : kFalse)

   #define kLogicalDriveEncryptionVolatileKeyBackupEnabled                 0x04 /**< Volatile key backup is enabled for the logical drive; @outputof SA_GetLogicalDriveEncryptionInfo. */
   #define kLogicalDriveEncryptionVolatileKeyBackupDisabled                0x08 /**< Volatile key backup is disabled for the logical drive; @outputof SA_GetLogicalDriveEncryptionInfo. */
   /** Mask output of @ref SA_GetLogicalDriveEncryptionInfo to determine if volatile key backup is enabled/disabled. */
   #define kLogicalDriveEncryptionVolatileKeyBackupMask                    0x00000C00
   /** Bitshift to use with output of @ref SA_GetLogicalDriveEncryptionInfo (after masking with @ref kLogicalDriveEncryptionVolatileKeyBackupMask) to get the encryption volatile key backup toggle. */
   #define kLogicalDriveEncryptionVolatileKeyBackupBitShift                8
   /** Decode output of @ref SA_GetLogicalDriveEncryptionInfo and return kTrue if logical drive volatile key backup is enabled. */
   /** Use to decode logical drive's encryption volatile key backup status output of @ref SA_GetLogicalDriveStatusInfo.
    *
    * __Example__
    * @code{.c}
    * SA_DWORD ld_status = SA_GetLogicalDriveStatusInfo(cda, ld_number);
    * printf("Logical Drive %u Volatile Key Backup %s\n", ld_number,
    *     VOLATILE_KEY_BACKUP_ENABLED(ld_status) ? "Enabled" : "Disabled");
    * @endcode
    */
   #define VOLATILE_KEY_BACKUP_ENABLED(info_value) ((((info_value & kLogicalDriveEncryptionVolatileKeyBackupMask) >> kLogicalDriveEncryptionVolatileKeyBackupBitShift) == kLogicalDriveEncryptionVolatileKeyBackupEnabled) ? kTrue : kFalse)

   #define kLogicalDriveEncryptionVolatileKeyStatusNoOp                    0x01 /**< Volatile key status: No Op; @outputof SA_GetLogicalDriveEncryptionInfo. */
   #define kLogicalDriveEncryptionVolatileKeyStatusBackupInProgress        0x02 /**< Volatile key status: Backup in progress; @outputof SA_GetLogicalDriveEncryptionInfo. */
   #define kLogicalDriveEncryptionVolatileKeyStatusBackupAndSetInProgress  0x03 /**< Volatile key status: Backup & set in progress; @outputof SA_GetLogicalDriveEncryptionInfo. */
   #define kLogicalDriveEncryptionVolatileKeyStatusRestoreInProgress       0x04 /**< Volatile key status: Restore in progress; @outputof SA_GetLogicalDriveEncryptionInfo. */
   #define kLogicalDriveEncryptionVolatileKeyStatusDeleteInProgress        0x05 /**< Volatile key status: Delete in progress; @outputof SA_GetLogicalDriveEncryptionInfo. */
   /** Mask output of @ref SA_GetLogicalDriveEncryptionInfo to determine if volatile key status. */
   #define kLogicalDriveEncryptionVolatileKeyStatusMask                    0x0000F000
   /** Bitshift to use with output of @ref SA_GetLogicalDriveEncryptionInfo (after masking with @ref kLogicalDriveEncryptionVolatileKeyStatusMask) to get the encryption volatile key status. */
   #define kLogicalDriveEncryptionVolatileKeyStatusBitShift                12
   /** Use to decode logical drive's encryption volatile key status output of @ref SA_GetLogicalDriveStatusInfo.
    *
    * __Example__
    * @code{.c}
    * SA_DWORD ld_status = SA_GetLogicalDriveStatusInfo(cda, ld_number);
    * printf("Logical Drive %u Encryption Volatile Key Status: %s\n", ld_number,
    *       VOLATILE_KEY_STATUS(ld_status) == kLogicalDriveEncryptionVolatileKeyStatusBackupInProgress ? "Backup In Progress"
    *     : VOLATILE_KEY_STATUS(ld_status) == kLogicalDriveEncryptionVolatileKeyStatusBackupAndSetInProgress ? "Backup And Set In Progress"
    *     ...);
    * @endcode
    */
   #define VOLATILE_KEY_STATUS(info_value) ((SA_BYTE)((info_value & kLogicalDriveEncryptionVolatileKeyStatusMask) >> kLogicalDriveEncryptionVolatileKeyStatusBitShift))
/** @} */

/** @name Physical Drive Encryption Info
 * @{ */
/** Get the current encryption info of the physical drive.
 *
 * @pre [SA_ControllerSupportsSED](@ref storc_features_ctrl_support_sed) reports that basic sed info is valid using @ref kControllerSEDInfoSupported.
 * @param[in] cda         Controller Data Area
 * @param[in]  pd_number  Physical drive number.
 * @return [Physical Drive Encryption Info Returns](@ref storc_encryption_status_info)
 * @see storc_features_ctrl_support_sed.
 * @remark f the return has @ref kPhysicalDriveEncryptionSEDQualificationInProgress set, this physical drive is undergoing SED qualification to verify that the drive
 * is usable. Please wait for SED qualification to complete and check the drive again before attempting to use this physical drive.
 * @remark If the return has @ref kPhysicalDriveEncryptionSEDQualificationFailedLockingEnabled set, the firmware has determined that this physical drive may have LockingEnabled
 * set. Please disable locking before attempting to use this physical drive.
 * @remark If the return has @ref kPhysicalDriveEncryptionSEDQualificationFailedRangeLengthSet set, the firmware has determined that this physical drive may have ReadLockEnabled,
 * WriteLockEnabled, or RangeLength set. Please ensure that all security features are disabled before attempting to use this physical drive.
 */
SA_UINT64 SA_GetPhysicalDriveEncryptionInfo(PCDA cda, SA_WORD pd_number);
/** The physical drive is a SED drive.
 * @outputof SA_GetPhysicalDriveEncryptionInfo.
 */
#define kPhysicalDriveEncryptionSEDDrive                                0x0000000000000001ull
/** The physical drive contains at least 1 encrypted logical drive.
 * @outputof SA_GetPhysicalDriveEncryptionInfo.
 */
#define kPhysicalDriveEncryptionContainsEncryptedLogicalDrives          0x0000000000000002ull
/** SED qualification is in progress.
 * @outputof SA_GetPhysicalDriveEncryptionInfo.
 */
#define kPhysicalDriveEncryptionSEDQualificationInProgress              0x0000000000000004ull
/** SED qualification failed due to LockingEnabled is set.
 * @outputof SA_GetPhysicalDriveEncryptionInfo.
 */
#define kPhysicalDriveEncryptionSEDQualificationFailedLockingEnabled    0x0000000000000008ull
/** SED qualification failed due to ReadLockEnabled, WriteLockEnabled or RangeLength is set.
 * @outputof SA_GetPhysicalDriveEncryptionInfo.
 */
#define kPhysicalDriveEncryptionSEDQualificationFailedRangeLengthSet    0x0000000000000010ull


/** Drive is not a SED or is a SED and ownership has not been taken (security has not been enabled).
 * @deprecated No longer used.
 * @outputof SA_GetPhysicalDriveEncryptionInfo.
 */
#define kPhysicalDriveEncryptionSEDSecurityStatusNotEnabled       0x00
/** Ownership is taken for the SED but is unlocked.
 * @deprecated No longer used.
 * @outputof SA_GetPhysicalDriveEncryptionInfo.
 */
#define kPhysicalDriveEncryptionSEDSecurityStatusOwnedAndUnlocked 0x01
/** SED owned and locked (data is not accessible).
 * @deprecated No longer used.
 * @outputof SA_GetPhysicalDriveEncryptionInfo.
 */
#define kPhysicalDriveEncryptionSEDSecurityStatusOwnedAndLocked   0x02

/** Drive is not a SED or the SED security status is unknown.
 * @outputof SA_GetPhysicalDriveEncryptionInfo.
 */
#define kPhysicalDriveEncryptionSEDSecurityStatusUnknown          0x00
/** SED is locked (data is not accessible).
 * @outputof SA_GetPhysicalDriveEncryptionInfo.
 */
#define kPhysicalDriveEncryptionSEDSecurityStatusLocked           0x04

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kPhysicalDriveEncryptionSEDSecurityStatusMask             0x000000000000000E0ull
#define kPhysicalDriveEncryptionSEDSecurityStatusBitShift         5
#endif
/** Use to decode the security status of a SED drive from [SA_GetPhysicalDriveEncryptionInfo](@ref storc_encryption_status_info)
 *
 * __Example__
 * @code{.c}
 * SA_UINT64 pd_status = SA_GetPhysicalDriveEncryptionInfo(cda, pd_number);
 * printf("Physical Drive %u SED Status: %s\n", pd_number,
 *       PHYSICAL_DRIVE_SED_SECURITY_STATUS(pd_status) == kPhysicalDriveEncryptionSEDSecurityStatusUnknown ? "Unknown"
 *     : PHYSICAL_DRIVE_SED_SECURITY_STATUS(pd_status) == kPhysicalDriveEncryptionSEDSecurityStatusLocked ? "Locked"
 *     ...);
 * @endcode
 */
#define PHYSICAL_DRIVE_SED_SECURITY_STATUS(info_value) ((SA_BYTE)((info_value & kPhysicalDriveEncryptionSEDSecurityStatusMask) >> kPhysicalDriveEncryptionSEDSecurityStatusBitShift))

/** @} */ /* Physical Drive Encryption Info */

/** @} */ /* storc_encryption_status_info */

/** @defgroup storc_encryption_clear Encryption Clear Configuration
 * @brief Clear the controller's encryption configuration.
 * @details
 * @{ */

/** @name Can Clear Encryption Configuration
 * @{ */
/** Checks if the encryption configuration can be cleared the controller.
 * @deprecated Use @ref SA_CanClearEncryptionConfigurationSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this
 *          function (@ref SA_CanClearEncryptionConfigurationSafe).
 * @allows SA_ClearEncryptionConfiguration.
 * @param[in] cda         Controller data area.
 * @param[in] master_key  Null-terminated encryption master key string. If not NULL, will
 * be validated and forcibly clear encryption even if encrypted logical drives are present.
 * If no encrypted logical drives exists, `master_key` must be NULL.
 * @return Map of reasons the encryption configuration can/cannot be cleared.
 * @return See [Can Clear Encryption Configuration Returns](@ref storc_encryption_clear)
 *
 * __Example__
 * @code
 * if (SA_CanClearEncryptionConfiguration(cda, NULL) == kCanClearEncryptionConfigurationAllowed)
 * {
 *    if (SA_ClearEncryptionConfiguration(cda, NULL) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to clear the encryption configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanClearEncryptionConfiguration(PCDA cda, const char *master_key);
/** @} */ /* Can Clear Encryption Configuration */

/** @name Can Clear Encryption Configuration (Safe)
 * @{ */
/** Checks if the encryption configuration can be cleared the controller.
 * @allows SA_ClearEncryptionConfigurationSafe.
 * @param[in] cda         Controller data area.
 * @param[in] master_key  Null-terminated encryption master key string. If not NULL, will
 * be validated and forcibly clear encryption even if encrypted logical drives are present.
 * If no encrypted logical drives exists, `master_key` must be NULL.
 * @param[in] master_key_size  Size of the master key C-string (number of bytes). If master_key
 * is NULL master_key_size must be 0.
 * @return Map of reasons the encryption configuration can/cannot be cleared.
 * @return See [Can Clear Encryption Configuration Returns](@ref storc_encryption_clear)
 *
 * __Example__
 * @code
 * if (SA_CanClearEncryptionConfigurationSafe(cda, NULL, 0) == kCanClearEncryptionConfigurationAllowed)
 * {
 *    if (SA_ClearEncryptionConfigurationSafe(cda, NULL, 0) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to clear the encryption configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanClearEncryptionConfigurationSafe(PCDA cda, const char* master_key, size_t master_key_size);
   /** @name Can Clear Encryption Configuration Returns
    * @outputof SA_CanClearEncryptionConfiguration.
    * @outputof SA_CanClearEncryptionConfigurationSafe.
    * @{ */
   /** Clearing the encryption configuration is allowed.
    * @outputof SA_CanClearEncryptionConfiguration.
    * @outputof SA_CanClearEncryptionConfigurationSafe.
    * @allows SA_ClearEncryptionConfiguration.
    * @allows SA_ClearEncryptionConfigurationSafe.
    */
   #define kCanClearEncryptionConfigurationAllowed                  1
   /** Encryption is not supported by the controller.
    * @outputof SA_CanClearEncryptionConfiguration.
    * @outputof SA_CanClearEncryptionConfigurationSafe.
    * @disallows SA_ClearEncryptionConfiguration.
    * @disallows SA_ClearEncryptionConfigurationSafe.
    */
   #define kCanClearEncryptionConfigurationNotSupported             2
   /** Encryption is not configured on the controller.
    * @outputof SA_CanClearEncryptionConfiguration.
    * @outputof SA_CanClearEncryptionConfigurationSafe.
    * @disallows SA_ClearEncryptionConfiguration.
    * @disallows SA_ClearEncryptionConfigurationSafe.
    */
   #define kCanClearEncryptionConfigurationNotConfigured            3
   /** Logical drives exist on the controller.
    * Ignored if parameter `master_key` is given.
    * @outputof SA_CanClearEncryptionConfiguration.
    * @outputof SA_CanClearEncryptionConfigurationSafe.
    * @disallows SA_ClearEncryptionConfiguration.
    * @disallows SA_ClearEncryptionConfigurationSafe.
    */
   #define kCanClearEncryptionConfigurationLogicalDrivesExist       4
   /** An encryption logical drive is undergoing a rekey or encode.
    * @outputof SA_CanClearEncryptionConfiguration.
    * @outputof SA_CanClearEncryptionConfigurationSafe.
    * @disallows SA_ClearEncryptionConfiguration.
    * @disallows SA_ClearEncryptionConfigurationSafe.
    */
   #define kCanClearEncryptionConfigurationEncodeOrRekeyInProgress  5
   /** The given non-null master key is an invalid encryption master key or there are not encrypted logical drives
    * and the master key is not required.
    * @outputof SA_CanClearEncryptionConfiguration.
    * @outputof SA_CanClearEncryptionConfigurationSafe.
    * @disallows SA_ClearEncryptionConfiguration.
    * @disallows SA_ClearEncryptionConfigurationSafe.
    */
   #define kCanClearEncryptionConfigurationInvalidKey               6
   /** The controller currently has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanClearEncryptionConfiguration.
    * @outputof SA_CanClearEncryptionConfigurationSafe.
    * @disallows SA_ClearEncryptionConfiguration.
    * @disallows SA_ClearEncryptionConfigurationSafe.
    */
   #define kCanClearEncryptionConfigurationControllerOFAActive      7
   /** The given master key size is invalid for the given non-null master key.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanClearEncryptionConfigurationSafe.
    * @disallows SA_ClearEncryptionConfigurationSafe.
    */
   #define kCanClearEncryptionConfigurationInvalidKeySize           8
   /** @} */ /* Can Clear Encryption Configuration Returns */
/** @} */ /* Can Clear Encryption Configuration (Safe) */

/** @name Clear Encryption Configuration
 * @{ */
/** Clear the encryption configuration
 * @deprecated Use @ref SA_ClearEncryptionConfigurationSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this
 *          function (@ref SA_ClearEncryptionConfigurationSafe).
 * @pre The user has used @ref SA_CanClearEncryptionConfiguration to ensure the operation can be performed.
 * @param[in] cda  Controller data area.
 * @param[in] master_key  Null-terminated encryption master key string. If not NULL, will
 * be validated and forcibly clear encryption even if encrypted logical drives are present.
 * @return @ref kTrue if clearing the encryption configuration succeeded, @ref kFalse if failed,
 * either due to an command failure or an invalid master key.
 *
 * __Example__
 * @code
 * if (SA_CanClearEncryptionConfiguration(cda, NULL) == kCanClearEncryptionConfigurationAllowed)
 * {
 *    if (SA_ClearEncryptionConfiguration(cda, NULL) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to clear the encryption configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ClearEncryptionConfiguration(PCDA cda, const char *master_key);
/** @} */ /* Clear Encryption Configuration */

/** @name Clear Encryption Configuration (Safe)
 * @{ */
/** Clear the encryption configuration
 * @pre The user has used @ref SA_CanClearEncryptionConfigurationSafe to ensure the operation can be performed.
 * @param[in] cda  Controller data area.
 * @param[in] master_key  Null - terminated encryption master key string.If not NULL, will
 * be validatedand forcibly clear encryption even if encrypted logical drives are present.
 * @param[in] master_key_size  Size of the master key C - string(number of bytes).If master_key
 * is NULL master_key_size must be 0.
 * @return @ref kTrue if clearing the encryption configuration succeeded, @ref kFalse if failed,
 * either due to an command failure or an invalid master key.
 *
 *__Example__
 * @code
 * if (SA_CanClearEncryptionConfigurationSafe(cda, NULL, 0) == kCanClearEncryptionConfigurationAllowed)
 * {
 *    if (SA_ClearEncryptionConfigurationSafe(cda, NULL, 0) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to clear the encryption configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ClearEncryptionConfigurationSafe(PCDA cda, const char* master_key, size_t master_key_size);
/** @} */ /* Clear Encryption Configuration (Safe) */
/** @} */ /* storc_encryption_clear */

/** @defgroup storc_encryption_rekey_ctrl Encryption Rekey Controller
 * @brief Generate new drive keys for all encrypted physical drives attached to the controller.
 * @details
 * While in remote encryption key management mode, the controller can send a request to the key manager to
 * generate new drive keys for all encrypted physical drives that are attached to the controller. This is an
 * instantaneous operation that requires encryption to be enabled, in remote key manamgement mode, and a user
 * and password to login to modify encryption settings.
 * @{
 */

/** @name Can Rekey Controller
 * @{ */
/** Checks if the controller can be rekeyed.
 *
 * @deprecated Use @ref SA_CanRekeyControllerSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CanRekeyControllerSafe).
 *
 * @allows SA_RekeyController.
 * @param[in] cda       Controller data area.
 * @param[in] user      Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password  C-string of user password. If NULL, password check is skipped.
 * @return See [Can Rekey Controller Returns](@ref storc_encryption_rekey_ctrl)
 *
 * @post If return is @ref kCanRekeyControllerLoginFailed, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 *
 * __Example__
 * @code
 * if (SA_CanRekeyController(cda, kControllerEncryptionCryptoOfficer, "P@ssword12345") == kCanRekeyControllerAllowed)
 * {
 *    if (SA_RekeyController(cda, kControllerEncryptionCryptoOfficer, "P@ssword12345") == kFalse)
 *    {
 *       fprintf(stderr, "Failed to rekey the controller\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRekeyController(PCDA cda, SA_BYTE user, const char *password);
/** @} */ /* Can Rekey Controller */

/** @name Can Rekey Controller (Safe)
 * @{ */
 /** Checks if the controller can be rekeyed.
 * @allows SA_RekeyControllerSafe.
 * @param[in] cda             Controller data area.
 * @param[in] user            Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password        C-string of user password. If NULL, password check is skipped.
 * @param[in] password_size   Size of user password.
 * @return See [Can Rekey Controller Returns](@ref storc_encryption_rekey_ctrl)
 *
 * @post If return is @ref kCanRekeyControllerLoginFailed, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 *
 * __Example__
 * @code
 * if (SA_CanRekeyControllerSafe(cda, kControllerEncryptionCryptoOfficer, "P@ssword12345", sizeof("P@ssword12345")) == kCanRekeyControllerAllowed)
 * {
 *    if (SA_RekeyControllerSafe(cda, kControllerEncryptionCryptoOfficer, "P@ssword12345", sizeof("P@ssword12345")) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to rekey the controller\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRekeyControllerSafe(PCDA cda, SA_BYTE user, const char *password, size_t password_size);
   /** Rekeying the controller is allowed.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @allows SA_RekeyController.
    * @allows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerAllowed                                      1
   /** Cannot rekey the controller because encryption is not configured.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerEncryptionNotConfigured                      2
   /** Cannot rekey the controller because the given user does not have the required privileges.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerPermissionDenied                             3
   /** Cannot rekey the controller because encryption is not enabled.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerEncryptionNotEnabled                         4
   /** Cannot rekey the controller because encryption is not in remote key manager mode.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerNotRemoteKeyManager                          5
   /** Cannot rekey the controller because there are no encrypted logical drives.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerNoEncryptedLogicalDrives                     6
   /** Cannot rekey the controller because the volumes are locked due to a missing boot password.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerLockout                                      7
   /** Cannot rekey the controller because one or more encrypted logical drives have failed.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe..
    */
   #define kCanRekeyControllerEncryptedLogicalDriveFailed                  8
   /** Cannot rekey the controller because an encrypted volume is part of a split mirror pair.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerEncryptedLogicalDriveIsSplitMirror           9
   /** Cannot rekey the controller because an encrypted logical drive has failed caching volume.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerEncryptedLogicalDriveHasFailedCachingVolume  10
   /** Cannot rekey the controller because an encrypted volume is initializing.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerEncryptedLogicalDriveIsInitializing          11
   /** Cannot rekey the controller because an encrypted volume is using volatile keys.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerEncryptedLogicalDriveHasVolatileKeys         12
   /** Cannot rekey the controller because the password is incorrect.
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerLoginFailed                                  13
   /** Cannot rekey the controller because the feature is not enabled.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    *
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerNotEnabled                                   14
   /** Cannot rekey the controller because an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation.
    *
    * @outputof SA_CanRekeyController.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyController.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerOFAActive                                    15
   /** Cannot rekey the controller because of invalid password string length.
    * @outputof SA_CanRekeyControllerSafe.
    * @disallows SA_RekeyControllerSafe.
    */
   #define kCanRekeyControllerInvalidPasswordSize                          16
/** @} */ /* Can Rekey Controller (Safe) */

/** @name Rekey Controller
 * @{ */
/** Rekey the controller.
 *
 * @deprecated Use @ref SA_RekeyControllerSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_RekeyControllerSafe).
 *
 * @pre The user has used @ref SA_CanRekeyController to ensure the operation can be performed.
 * @param[in] cda      Controller data area.
 * @param[in] user     Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password Null-terminated string password to verify.
 * @return See [Can Rekey Controller Returns](@ref storc_encryption_rekey_ctrl)
 *
 * __Example__
 * @code
 * if (SA_CanRekeyController(cda, kControllerEncryptionCryptoOfficer, "P@ssword12345") == kCanRekeyControllerAllowed)
 * {
 *    if (SA_RekeyController(cda, kControllerEncryptionCryptoOfficer, "P@ssword12345") == kFalse)
 *    {
 *       fprintf(stderr, "Failed to rekey the controller\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_RekeyController(PCDA cda, SA_BYTE user, const char * password);
/** @} */ /* Rekey Controller */

/** @name Rekey Controller (Safe)
 * @{ */
/** Rekey the controller.
 * @pre The user has used @ref SA_CanRekeyControllerSafe to ensure the operation can be performed.
 * @param[in] cda      Controller data area.
 * @param[in] user     Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password Null-terminated string password to verify.
 * @param[in] password_size Size of password string.
 * @return See [Can Rekey Controller Returns](@ref storc_encryption_rekey_ctrl)
 *
 * __Example__
 * @code
 * if (SA_CanRekeyControllerSafe(cda, kControllerEncryptionCryptoOfficer, "P@ssword12345", sizeof("P@ssword12345")) == kCanRekeyControllerAllowed)
 * {
 *    if (SA_RekeyControllerSafe(cda, kControllerEncryptionCryptoOfficer, "P@ssword12345", sizeof("P@ssword12345")) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to rekey the controller\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_RekeyControllerSafe(PCDA cda, SA_BYTE user, const char * password, size_t password_size);
   /** Rekeying the controller failed.
    * @outputof SA_RekeyController.
    * @outputof SA_RekeyControllerSafe.
    */
   #define kRekeyControllerFailed       0x00
   /** Controller rekeyed successfully.
    * @outputof SA_RekeyController.
    * @outputof SA_RekeyControllerSafe.
    */
   #define kRekeyControllerOK           0x01
/** @} */ /* Rekey Controller (Safe) */

/** @} */ /* storc_encryption_rekey_ctrl */

/** @defgroup storc_encryption_import_foreign_key Encryption Import Foreign Key
 * @brief Import foreign encryption key to unlock drives keyed by a different controller.
 * @details
 * Drives that were keyed using a foreign master key that is not the same as the current controller, the
 * foreign key can be imported and used to unlock the imported drives attached the current controller. This
 * is an instantaneous operation that requires encryption to be enabled.
 * @{
 */

/** @name Can Import Foreign Encryption Key Returns
 * @outputof SA_CanImportForeignEncryptionKey.
 * @{ */
/** Checks if the foreign encryption key can be imported.
 * @deprecated Use @ref SA_CanImportForeignEncryptionKeySafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this
 *          function (@ref SA_CanImportForeignEncryptionKeySafe).
 * @allows SA_ImportForeignEncryptionKey.
 * @param[in] cda         Controller data area.
 * @param[in] foreign_key Foreign encryption key to be imported.
 * @param[in] user        Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password    C-string for encryption user (validated with @ref SA_CheckEncryptionPassword).
 *                        If NULL, password check (@ref kCanImportForeignKeyLoginFailed) is skipped.
 * @return See [Can Import Foreign Encryption Key Returns](@ref storc_encryption_import_foreign_key)
 *
 * __Example__
 * @code
 * if (SA_CanImportForeignEncryptionKey(cda, foreign_key, user, password) == kCanImportForeignKeyAllowed)
 * {
 *    if (SA_ImportForeignEncryptionKey(cda, foreign_key, user, password) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to import the foreign encryption key\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanImportForeignEncryptionKey(PCDA cda, const char * foreign_key, SA_BYTE user, const char * password);
/** @} */ /*  Can Import Foreign Encryption Key Returns */

/** @name Can Import Foreign Encryption Key Returns (Safe)
 * @outputof SA_CanImportForeignEncryptionKeySafe.
 * @{ */
/** Checks if the foreign encryption key can be imported.
 * @allows SA_ImportForeignEncryptionKeySafe.
 * @param[in] cda              Controller data area.
 * @param[in] foreign_key      Foreign encryption key to be imported.
 * @param[in] foreign_key_size Size of the foreign_key C-string (number of bytes).
 * @param[in] user             Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password         C-string for encryption user (validated with @ref SA_CheckEncryptionPassword).
 *                             If NULL, password check (@ref kCanImportForeignKeyLoginFailed) is skipped.
 * @param[in] password_size    Size of the password C-string (number of bytes).
 * @return See [Can Import Foreign Encryption Key Returns](@ref storc_encryption_import_foreign_key)
 *
 * __Example__
 * @code
 * if (SA_CanImportForeignEncryptionKeySafe(cda, foreign_key, sizeof(foreign_key), user, password, sizeof(password)) == kCanImportForeignKeyAllowed)
 * {
 *    if (SA_ImportForeignEncryptionKeySafe(cda, foreign_key, sizeof(foreign_key, user, password, sizeof(password)) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to import the foreign encryption key\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanImportForeignEncryptionKeySafe(PCDA cda, const char * foreign_key, size_t foreign_key_size, SA_BYTE user, const char * password, size_t password_size);

   /** Importing the foreign encryption key is allowed.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe
    * @allows SA_ImportForeignEncryptionKey.
    * @allows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyAllowed                   1
   /** Cannot import the foreign encryption key because the given key is invalid.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyInvalidKey                2
   /** Cannot import the foreign encryption key because encryption is not configured.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyEncryptionNotConfigured   3
   /** Cannot import the foreign encryption key because encryption is not enabled.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyEncryptionNotEnabled      4
   /** Cannot import the foreign encryption key because encryption is not in local key manager mode.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyNotLocalKeyManager        5
   /** Cannot import the foreign encryption key because no logical drives are offline due to missing encryption Key.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyNoLogicalDrivesOffline    6
   /** Cannot import the foreign encryption key because controller lockout (missing boot password).
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyControllerLockout         7
   /** Cannot import the foreign encryption key because controller fails self-test.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyControllerFailedSelfTest  8
   /** Cannot import the foreign encryption key because the given user is invalid.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyInvalidUser               9
   /** Cannot import the foreign encryption key because the given user does not have the required privileges.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyPermissionDenied          10
   /** Cannot import the foreign encryption key because the given password for the user is non-null and incorrect.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyLoginFailed               11
   /** Cannot import the foreign encryption key because the feature is not enabled.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyNotEnabled                12
   /** Cannot import the foreign encryption key because the controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanImportForeignEncryptionKey.
    * @disallows SA_ImportForeignEncryptionKey.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyControllerOFAActive       13
   /** Cannot import the foreign encryption key because the foreign key size is invalid.
    * @outputof SA_CanImportForeignEncryptionKeySafe.
    * @disallows SA_ImportForeignEncryptionKeySafe.
    */
   #define kCanImportForeignKeyInvalidForeignKeySize     14
    /** Cannot import the foreign encryption key because the password size is invalid.
     * @outputof SA_CanImportForeignEncryptionKeySafe.
     * @disallows SA_ImportForeignEncryptionKeySafe.
     */
   #define kCanImportForeignKeyInvalidPasswordSize       15
/** @} */ /* Can Import Foreign Encryption Key Returns (Safe) */

/** @name Import Foreign Encryption Key
 * @{ */
/** Import Foreign Encryption Key.
 * @deprecated Use @ref SA_ImportForeignEncryptionKeySafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this
 *          function (@ref SA_ImportForeignEncryptionKeySafe).
 * @pre The user has used @ref SA_CanImportForeignEncryptionKey to ensure the operation can be performed.
 * @param[in] cda           Controller data area.
 * @param[in] foreign_key   C - string master key.
 * @param[in] user          Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password      Null-terminated string password to verify.
 * @return See [Import Foreign Encryption Key Returns](@ref storc_encryption_import_foreign_key)
 */
SA_BYTE SA_ImportForeignEncryptionKey(PCDA cda, const char * foreign_key, SA_BYTE user, const char * password);
/** @} */ /* Import Foreign Encryption Key */

/** @name Import Foreign Encryption Key (Safe)
 * @{ */
/** Import Foreign Encryption Key.
 * @pre The user has used @ref SA_CanImportForeignEncryptionKeySafe to ensure the operation can be performed.
 * @param[in] cda              Controller data area.
 * @param[in] foreign_key      C - string master key.
 * @param[in] foreign_key_size Size of the foreign keyC-string (number of bytes)
 * @param[in] user             Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password         Null-terminated string password to verify.
 * @param[in] password_size    Size of the password C-string (number of bytes).
 * @return See [Import Foreign Encryption Key Returns](@ref storc_encryption_import_foreign_key)
 */
SA_BYTE SA_ImportForeignEncryptionKeySafe(PCDA cda, const char * foreign_key, size_t foreign_key_size, SA_BYTE user, const char * password, size_t password_size);
   /** Import failed when attempting to import the foreign key.
    * @outputof SA_ImportForeignEncryptionKey.
    * @outputof SA_ImportForeignEncryptionKeySafe.
    */
   #define kImportForeignEncryptionKeyFailed                0x00
   /** Import of Foreign key succeeded.
    * @outputof SA_ImportForeignEncryptionKey.
    * @outputof SA_ImportForeignEncryptionKeySafe.
    */
   #define kImportForeignEncryptionKeyOK                    0x01
   /** Login failed when attempting to import the foreign key.
    * @outputof SA_ImportForeignEncryptionKey.
    * @outputof SA_ImportForeignEncryptionKeySafe.
    */
   #define kImportForeignEncryptionKeyFailedNullPassword    0x02
/** @} */ /* Import Foreign Encryption Key (Safe) */
/** @} */ /* storc_encryption_import_foreign_key */

/** @defgroup storc_encryption_verify_password Encryption Verify Password
 * @brief API used to attempt an encryption login.
 * @{
 */
/** @name Verify User Encryption Password (Login)
 * @{ */
/** Verify a user's encryption password.
 * @deprecated Use @ref SA_VerifyEncryptionPasswordSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this
 *          function (@ref SA_VerifyEncryptionPasswordSafe).
 * @see SA_GetEncryptionPasswordStatus.
 * @pre Encryption is currently configured on the controller.
 * @param[in] cda      Controller handle.
 * @param[in] user     Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password Null-terminated string password to verify.
 * @return @ref kTrue iff password for given user was verified on the controller, @ref kFalse otherwise.
 * @post If return is @ref kFalse, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 */
SA_BOOL SA_VerifyEncryptionPassword(PCDA cda, SA_BYTE user, const char *password);
/** Verify a user's encryption password (Safe).
 * @see SA_GetEncryptionPasswordStatus.
 * @pre Encryption is currently configured on the controller.
 * @param[in] cda           Controller handle.
 * @param[in] user          Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password      Null-terminated string password to verify.
 * @param[in] password_size Size of the password C-string (number of bytes)..
 * @return @ref kTrue iff password for given user was verified on the controller, @ref kFalse otherwise.
 * @post If return is @ref kFalse, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 */
SA_BOOL SA_VerifyEncryptionPasswordSafe(PCDA cda, SA_BYTE user, const char *password, size_t password_size);
/** @} */ /* Verify User Encryption Password (Login) */
/** @} */ /* storc_encryption_verify_password */

/** @defgroup storc_encryption_user_capabilities Encryption User Capabilities
 * @brief Return the encryption actions allowed for a given encryption user.
 * @{
 */
/** @name Encryption User Capabilities
 * @{ */
/** Get the encryption capabilities of a given user.
 * @pre Encryption is currently configured on the controller.
 * @param[in] cda      Controller handle.
 * @param[in] user     Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @return Map of capabilities for the given user.
 */
SA_DWORD SA_GetEncryptionUserCapabilities(PCDA cda, SA_BYTE user);
/** Set Time; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetTime                    0x00000001
/** Set Master Key name; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetMasterKey               0x00000002
/** Set Server Name; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetServerName              0x00000004
/** Set Password; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetPassword                0x00000008
/** Set Control Encryption; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetControlEncryption       0x00000010
/** Set KEK Cache; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetKekCache                0x00000020
/** Rekey; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilityRekey                      0x00000040
/** Set Control Lock FW; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetControllerFWLock        0x00000080
/** Set Control Return to Factory; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetControlReturnToFactory  0x00000100
/** Set Control Local Key Params; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetControlLocalKeyParams   0x00000200
/** Verify Password; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilityVerifyPassword             0x00000400
/** Set Password Recovery Parameters; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetRecoveryParams          0x00000800
/** Check Password Recovery Answer; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilityCheckRecoveryAnswer        0x00001000
/** Set/Change Boot Password; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetBootPassword            0x00002000
/** Suspend/Resume Boot Password; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySuspendResumeBootPassword  0x00004000
/** Foreign Local KEK Import; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilityForeignLocalKekImport      0x00008000
/** Remove Boot Password; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilityRemoveBootPassword         0x00010000
/** Get Password Status; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilityGetPasswordStatus          0x00020000
/** Delegate Services; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilityDelegateServices           0x00040000
/** Change Crypto Password; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilityChangeCryptoPassword       0x00080000
/** Change User Password; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilityChangeUserPassword         0x00100000
/** Restore Default Permissions; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilityRestoreDefaultPermissions  0x00200000
/** Set/reset Local Master Key Name; @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetLocalMasterKey          0x00400000
/** Set DEK storage and allow backup/restores (volatile keys); @outputof SA_GetEncryptionUserCapabilities. */
#define kEncryptionUserCapabilitySetDEKStorage              0x00800000
/** @} */ /* Encryption User Capabilities */
/** @} */ /* storc_encryption_user_capabilities */

/** @defgroup storc_encryption_rekey_volume Encryption Rekey Volume
 * @brief Perform a controller encryption volume rekey.
 * @{
 */
/** @name Can Rekey Volume
 * @{ */
/** Checks if the volume can be rekeyed.
 *
 * @deprecated Use @ref SA_CanRekeyEncryptionVolumeKeySafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CanRekeyEncryptionVolumeKeySafe).
 *
 * @allows SA_RekeyEncryptionVolumeKey.
 * @param[in] cda            Controller data area.
 * @param[in] ld_number      Logical drive number.
 * @param[in] preserve_data  Set to @ref kTrue if data on the logical drive should be preserved, @ref kFalse otherwise.
 * @param[in] user           Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password       Null-terminated string password to verify.
 * @return See [Can Rekey Volume Returns](@ref storc_encryption_rekey_volume)
 *
 * @post If return is @ref kCanRekeyEncryptionVolumeKeyLoginFailed, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 *
 * __Example__
 * @code
 * if (SA_CanRekeyEncryptionVolumeKey(cda, ld_number, user, kTrue, password) == kCanRekeyEncryptionVolumeKeyAllowed)
 * {
 *    if (SA_RekeyEncryptionVolumeKey(cda, ld_number, user, kTrue, password) != kRekeyEncryptionVolumeKeyOK)
 *    {
 *       fprintf(stderr, "Failed to rekey the volume\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRekeyEncryptionVolumeKey(PCDA cda, SA_WORD ld_number, SA_BOOL preserve_data, SA_BYTE user, const char *password);
/** @} */ /* Can Rekey Volume */

/** @name Can Rekey Volume (Safe)
 * @{ */
/** Checks if the volume can be rekeyed.
 * @allows SA_RekeyEncryptionVolumeKeySafe.
 * @param[in] cda            Controller data area.
 * @param[in] ld_number      Logical drive number.
 * @param[in] preserve_data  Set to @ref kTrue if data on the logical drive should be preserved, @ref kFalse otherwise.
 * @param[in] user           Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password       Null-terminated string password to verify.
 * @param[in] password_size  Size of the password string.
 * @return See [Can Rekey Volume Returns](@ref storc_encryption_rekey_volume)
 *
 * @post If return is @ref kCanRekeyEncryptionVolumeKeyLoginFailed, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 *
 * __Example__
 * @code
 * if (SA_CanRekeyEncryptionVolumeKeySafe(cda, ld_number, user, kTrue, password, sizeof(password)) == kCanRekeyEncryptionVolumeKeyAllowed)
 * {
 *    if (SA_RekeyEncryptionVolumeKeySafe(cda, ld_number, user, kTrue, password, sizeof(password)) != kRekeyEncryptionVolumeKeyOK)
 *    {
 *       fprintf(stderr, "Failed to rekey the volume\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRekeyEncryptionVolumeKeySafe(PCDA cda, SA_WORD ld_number, SA_BOOL preserve_data, SA_BYTE user, const char *password, size_t password_size);
   /** Rekeying the volume is allowed.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @allows SA_RekeyEncryptionVolumeKey.
    * @allows SA_RekeyEncryptionVolumeKeySafe.
    */
   #define kCanRekeyEncryptionVolumeKeyAllowed                               1
   /** Cannot rekey the volume the logical drive number is invalid or does not exist.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    */
   #define kCanRekeyEncryptionVolumeKeyEncryptionInvalidLogicalDrive         2
   /** Cannot rekey the volume because encryption is not configured.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanRekeyEncryptionVolumeKeyEncryptionNotConfigured               3
   /** Cannot rekey the volume because encryption is not enabled.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanRekeyEncryptionVolumeKeyEncryptionNotEnabled                  4
   /** Cannot rekey the volume because encryption mode is local express.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanRekeyEncryptionVolumeKeyLocalExpressEncryption                5
   /** Cannot rekey the volume because the logical drive is not encryped.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetLogicalDriveEncryptionInfo.
    */
   #define kCanRekeyEncryptionVolumeKeyLogicalDriveNotEncrypted              6
   /** Cannot rekey the volume because volatile keys are enabled for the logical drive.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetLogicalDriveStatusInfo.
    */
   #define kCanRekeyEncryptionVolumeKeyVolatileKeysEnabled                   7
   /** Cannot rekey the volume because the volume is not a data array.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetLogicalDriveBasicInfo.
    */
   #define kCanRekeyEncryptionVolumeKeyNotDataArray                          8
   /** Cannot rekey the volume because the logical drive status is not OK.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    */
   #define kCanRekeyEncryptionVolumeKeyLUCacheLogicalDriveStatusNotOK        9
   /** Cannot rekey the volume because the cache logical drive status is not OK.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetLogicalDriveStatusInfo.
    */
   #define kCanRekeyEncryptionVolumeKeyLogicalDriveStatusNotOK              10
   /** Cannot rekey the volume because the controller cache status is not OK.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanRekeyEncryptionVolumeKeyControllerCacheStatusNotOK           11
   /** Cannot rekey the volume because the controller status is not OK.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanRekeyEncryptionVolumeKeyControllerStatusNotOK                12
   /** Cannot rekey the volume because the controller is locked (missing boot password).
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanRekeyEncryptionVolumeKeyControllerLocked                     13
   /** Cannot rekey the volume because the user is invalid.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanRekeyEncryptionVolumeKeyInvalidUser                          14
   /** Cannot rekey the volume because permission is denied for the user.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetEncryptionUserCapabilities.
    */
   #define kCanRekeyEncryptionVolumeKeyPermissionDenied                     15
   /** Cannot rekey the volume because the encryption password failed.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_VerifyEncryptionPasswordSafe.
    */
   #define kCanRekeyEncryptionVolumeKeyLoginFailed                          16
   /** Cannot rekey the volume because the encryption feature is not enabled.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanRekeyEncryptionVolumeKeyNotEnabled                           17
   /** Cannot rekey the volume because it has a registered split mirror pair.
    * @deprecated Uses @ref kCanRekeyEncryptionVolumeKeyNotDataArray instead.
    * The logical drive's split mirror pair may have been delete, become offline or unavailable
    * without being properly separated from the logical drive using resync, rollback, or activate.
    *
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKey.
    */
   #define kCanRekeyEncryptionVolumeKeyHasSplitMirrorPair                   18
   /** Cannot rekey the volume because the controller as an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation.
    *
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    */
   #define kCanRekeyEncryptionVolumeKeyControllerOFAActive                  19
   /** Cannot rekey the volume because there is no transform memory and powerless transformations are not supported.
    * @outputof SA_CanRekeyEncryptionVolumeKey.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKey.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    */
   #define kCanRekeyEncryptionVolumeKeyPowerlessTransformationNotSupported  20
   /** Cannot rekey the volume because the password size is not valid.
    * @outputof SA_CanRekeyEncryptionVolumeKeySafe.
    * @disallows SA_RekeyEncryptionVolumeKeySafe.
    */
   #define kCanRekeyEncryptionVolumeKeyInvalidPasswordSize                  21
/** @} */ /* Can Rekey Volume (Safe) */

/** @name Encryption Rekey Volume
 * @{ */
/** Rekey the volume.
 *
 * @deprecated Use @ref SA_RekeyEncryptionVolumeKeySafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_RekeyEncryptionVolumeKeySafe).
 *
 * @pre The user has used @ref SA_CanRekeyEncryptionVolumeKey to ensure the operation can be performed.
 * @param[in] cda           Controller data area.
 * @param[in] ld_number     Logical drive number to rekey.
 * @param[in] preserve_data Set to @ref kTrue if data on the logical drive should be preserved, @ref kFalse otherwise.
 * @param[in] user          Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password      Null-terminated string password to verify.
 * @return @ref kRekeyEncryptionVolumeKeyFailed when volume rekey fails.
 * @return @ref kRekeyEncryptionVolumeKeyOK when volume rekey is successful.
 *
 * @post If return is @ref kRekeyEncryptionVolumeKeyFailed, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 *
 * __Example__
 * @code
 * if (SA_CanRekeyEncryptionVolumeKey(cda, ld_number, kTrue, user, password) == kCanRekeyEncryptionVolumeKeyAllowed)
 * {
 *    if (SA_RekeyEncryptionVolumeKey(cda, ld_number, kTrue, user, password) != kRekeyEncryptionVolumeKeyOK)
 *    {
 *       fprintf(stderr, "Failed to rekey the volume\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_RekeyEncryptionVolumeKey(PCDA cda, SA_WORD ld_number, SA_BOOL preserve_data, SA_BYTE user, const char *password);
/** @} */ /* Encryption Rekey Volume */

/** @name Encryption Rekey Volume (Safe)
 * @{ */
/** Rekey the volume.
 * @pre The user has used @ref SA_CanRekeyEncryptionVolumeKeySafe to ensure the operation can be performed.
 * @param[in] cda           Controller data area.
 * @param[in] ld_number     Logical drive number to rekey.
 * @param[in] preserve_data Set to @ref kTrue if data on the logical drive should be preserved, @ref kFalse otherwise.
 * @param[in] user          Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password      Null-terminated string password to verify.
 * @param[in] password_size Size of the string password.
 * @return @ref kRekeyEncryptionVolumeKeyFailed when volume rekey fails.
 * @return @ref kRekeyEncryptionVolumeKeyOK when volume rekey is successful.
 *
 * @post If return is @ref kRekeyEncryptionVolumeKeyFailed, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 *
 * __Example__
 * @code
 * if (SA_CanRekeyEncryptionVolumeKeySafe(cda, ld_number, kTrue, user, password, sizeof(password)) == kCanRekeyEncryptionVolumeKeyAllowed)
 * {
 *    if (SA_RekeyEncryptionVolumeKeySafe(cda, ld_number, kTrue, user, password, sizeof(password)) != kRekeyEncryptionVolumeKeyOK)
 *    {
 *       fprintf(stderr, "Failed to rekey the volume\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_RekeyEncryptionVolumeKeySafe(PCDA cda, SA_WORD ld_number, SA_BOOL preserve_data, SA_BYTE user, const char *password, size_t password_size);
   /** Rekeying the volume failed.
    * @outputof SA_RekeyEncryptionVolumeKey.
    * @outputof SA_RekeyEncryptionVolumeKeySafe.
    */
   #define kRekeyEncryptionVolumeKeyFailed       0x00
   /** Volume rekeyed successfully.
    * @outputof SA_RekeyEncryptionVolumeKey.
    * @outputof SA_RekeyEncryptionVolumeKeySafe.
    */
   #define kRekeyEncryptionVolumeKeyOK           0x01
/** @} */ /* Encryption Rekey Volume (Safe) */

/** @} */ /* storc_encryption_rekey_volume */

/** @defgroup storc_encryption_rekey_array Encryption Rekey Array
 * @brief Perform volume rekey on all encrypted logical drives of an Array
 * @{
 */
/** @name Can Rekey All Encrypted Logical Drives of an Array
 * @{ */
/** Checks if the array can be rekeyed.
 *
 * @deprecated Use @ref SA_CanRekeyArrayEncryptionVolumeKeySafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CanRekeyArrayEncryptionVolumeKeySafe).
 *
 * @allows SA_RekeyArrayEncryptionVolumeKey.
 * @param[in] cda            Controller data area.
 * @param[in] array_number   Array number.
 * @param[in] preserve_data  Set to @ref kTrue if data on the logical drive should be preserved, @ref kFalse otherwise.
 * @param[in] user           Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password       Null-terminated string password to verify.
 * @return See [Can Rekey Array Volume Returns](@ref storc_encryption_rekey_array)
 *
 * @post If return is @ref kCanRekeyArrayEncryptionVolumeKeyLoginFailed, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 *
 *
 * __Example__
 * @code
 * if (SA_CanRekeyArrayEncryptionVolumeKey(cda, array_number, kTrue, user, password) == kCanRekeyArrayEncryptionVolumeKeyAllowed)
 * {
 *    if (SA_RekeyArrayEncryptionVolumeKey(cda, array_number, kTrue, user, password) != kRekeyArrayEncryptionVolumeKeyOK)
 *    {
 *       fprintf(stderr, "Failed to rekey the array\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRekeyArrayEncryptionVolumeKey(PCDA cda, SA_WORD array_number, SA_BOOL preserve_data, SA_BYTE user, const char *password);
/** @} */ /* Can Rekey All Encrypted Logical Drives of an Array */

/** @name Can Rekey All Encrypted Logical Drives of an Array (Safe)
 * @{ */
 /** Checks if the array can be rekeyed.
 * @allows SA_RekeyArrayEncryptionVolumeKeySafe.
 * @param[in] cda            Controller data area.
 * @param[in] array_number   Array number.
 * @param[in] preserve_data  Set to @ref kTrue if data on the logical drive should be preserved, @ref kFalse otherwise.
 * @param[in] user           Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password       Null-terminated string password to verify.
 * @param[in] password_size  Size of password string.
 * @return See [Can Rekey Array Volume Returns](@ref storc_encryption_rekey_array)
 *
 * @post If return is @ref kCanRekeyArrayEncryptionVolumeKeyLoginFailed, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 *
 *
 * __Example__
 * @code
 * if (SA_CanRekeyArrayEncryptionVolumeKeySafe(cda, array_number, kTrue, user, password, sizeof(password)) == kCanRekeyArrayEncryptionVolumeKeyAllowed)
 * {
 *    if (SA_RekeyArrayEncryptionVolumeKeySafe(cda, array_number, kTrue, user, password, sizeof(password)) != kRekeyArrayEncryptionVolumeKeyOK)
 *    {
 *       fprintf(stderr, "Failed to rekey the array\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRekeyArrayEncryptionVolumeKeySafe(PCDA cda, SA_WORD array_number, SA_BOOL preserve_data, SA_BYTE user, const char *password, size_t password_size);
   /** Rekeying the Array is allowed.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @allows SA_RekeyArrayEncryptionVolumeKey.
    * @allows SA_RekeyArrayEncryptionVolumeKeySafe.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyAllowed                           kCanRekeyEncryptionVolumeKeyAllowed
   /** Cannot rekey the Array because the Array number is invalid or does not exist.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyInvalidArray                      kCanRekeyEncryptionVolumeKeyEncryptionInvalidLogicalDrive
   /** Cannot rekey the Array because encryption is not configured.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyEncryptionNotConfigured           kCanRekeyEncryptionVolumeKeyEncryptionNotConfigured
   /** Cannot rekey the Array because encryption is not enabled.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyEncryptionNotEnabled              kCanRekeyEncryptionVolumeKeyEncryptionNotEnabled
   /** Cannot rekey the Array because encryption mode is local express.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyLocalExpressEncryption            kCanRekeyEncryptionVolumeKeyLocalExpressEncryption
   /** Cannot rekey the Array because the logical drive is not encrypted.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetLogicalDriveEncryptionInfo.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyNoEncryptedLogicalDrives          kCanRekeyEncryptionVolumeKeyLogicalDriveNotEncrypted
   /** Cannot rekey the Array because volatile keys are enabled for the logical drive.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetLogicalDriveStatusInfo.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyVolatileKeysEnabled               kCanRekeyEncryptionVolumeKeyVolatileKeysEnabled
   /** Cannot rekey the Array because the volume is not a data array.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetLogicalDriveBasicInfo.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyNotDataArray                      kCanRekeyEncryptionVolumeKeyNotDataArray
   /** Cannot rekey the Array because the logical drive status is not OK.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyLUCacheLogicalDriveStatusNotOK    kCanRekeyEncryptionVolumeKeyLUCacheLogicalDriveStatusNotOK
   /** Cannot rekey the Array because the cache logical drive status is not OK.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetLogicalDriveStatusInfo.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyLogicalDriveStatusNotOK           kCanRekeyEncryptionVolumeKeyLogicalDriveStatusNotOK
   /** Cannot rekey the Array because the controller cache status is not OK.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyControllerCacheStatusNotOK        kCanRekeyEncryptionVolumeKeyControllerCacheStatusNotOK
   /** Cannot rekey the Array because the controller status is not OK.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyControllerStatusNotOK             kCanRekeyEncryptionVolumeKeyControllerStatusNotOK
   /** Cannot rekey the Array because the controller is locked (missing boot password).
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyControllerLocked                  kCanRekeyEncryptionVolumeKeyControllerLocked
   /** Cannot rekey the Array because the user is invalid.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyInvalidUser                       kCanRekeyEncryptionVolumeKeyInvalidUser
   /** Cannot rekey the Array because permission is denied for the user.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetEncryptionUserCapabilities.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyPermissionDenied                  kCanRekeyEncryptionVolumeKeyPermissionDenied
   /** Cannot rekey the Array because the encryption password failed.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_VerifyEncryptionPasswordSafe.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyLoginFailed                       kCanRekeyEncryptionVolumeKeyLoginFailed
   /** Cannot rekey the Array because the encryption feature is not enabled.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyNotEnabled                        kCanRekeyEncryptionVolumeKeyNotEnabled
   /** Cannot rekey the Array because it has a registered split mirror pair.
    * The logical drive's split mirror pair may have been delete, become offline or unavailable
    * without being properly separated from the logical drive using resync, rollback, or activate.
    *
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyHasSplitMirrorPair                kCanRekeyEncryptionVolumeKeyHasSplitMirrorPair
   /** Cannot rekey the Array because the controller as an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    *
    * @outputof SA_CanRekeyArrayEncryptionVolumeKey.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKey.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyControllerOFAActive               kCanRekeyEncryptionVolumeKeyControllerOFAActive
   /** Cannot rekey the Array because the password size is invalid.
    * @outputof SA_CanRekeyArrayEncryptionVolumeKeySafe.
    * @disallows SA_RekeyArrayEncryptionVolumeKeySafe.
    */
   #define kCanRekeyArrayEncryptionVolumeKeyInvalidPasswordSize               kCanRekeyEncryptionVolumeKeyInvalidPasswordSize
/** @} */ /* Can Rekey All Encrypted Logical Drives of an Array (Safe) */

/** @name Encryption Rekey Array
 * @{ */
/** Rekeys all encrypted logical drives of an Array.
 *
 * @deprecated Use @ref SA_RekeyArrayEncryptionVolumeKeySafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_RekeyArrayEncryptionVolumeKeySafe).
 *
 * @pre The user has used @ref SA_CanRekeyArrayEncryptionVolumeKey to ensure the operation can be performed.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Array number.
 * @param[in] preserve_data Set to @ref kTrue if data on the logical drive should be preserved, @ref kFalse otherwise.
 * @param[in] user          Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password      Null-terminated string password to verify.
 * @return @ref kRekeyArrayEncryptionVolumeKeyFailed when volume rekey fails.
 * @return @ref kRekeyArrayEncryptionVolumeKeyOK when volume rekey is successful.
 *
 * @post If return is @ref kRekeyArrayEncryptionVolumeKeyFailed, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 *
 * __Example__
 * @code
 * if (SA_CanRekeyArrayEncryptionVolumeKey(cda, array_number, kTrue, user, password) == kCanRekeyArrayEncryptionVolumeKeyAllowed)
 * {
 *    if (SA_RekeyArrayEncryptionVolumeKey(cda, array_number, kTrue, user, password) != kRekeyArrayEncryptionVolumeKeyOK)
 *    {
 *       fprintf(stderr, "Failed to rekey the array\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_RekeyArrayEncryptionVolumeKey(PCDA cda, SA_WORD array_number, SA_BOOL preserve_data, SA_BYTE user, const char *password);
/** @} */ /* Encryption Rekey Array */

/** @name Encryption Rekey Array (Safe)
 * @{ */
/** Rekeys all encrypted logical drives of an Array.
 * @pre The user has used @ref SA_CanRekeyArrayEncryptionVolumeKeySafe to ensure the operation can be performed.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Array number.
 * @param[in] preserve_data Set to @ref kTrue if data on the logical drive should be preserved, @ref kFalse otherwise.
 * @param[in] user          Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password      Null-terminated string password to verify.
 * @param[in] password_size Size of string password.
 * @return @ref kRekeyArrayEncryptionVolumeKeyFailed when volume rekey fails.
 * @return @ref kRekeyArrayEncryptionVolumeKeyOK when volume rekey is successful.
 *
 * @post If return is @ref kRekeyArrayEncryptionVolumeKeyFailed, user should use @ref SA_GetEncryptionPasswordStatus
 * to check password lockout for the given user.
 *
 * __Example__
 * @code
 * if (SA_CanRekeyArrayEncryptionVolumeKeySafe(cda, array_number, kTrue, user, password, sizeof(password)) == kCanRekeyArrayEncryptionVolumeKeyAllowed)
 * {
 *    if (SA_RekeyArrayEncryptionVolumeKeySafe(cda, array_number, kTrue, user, password, sizeof(password)) != kRekeyArrayEncryptionVolumeKeyOK)
 *    {
 *       fprintf(stderr, "Failed to rekey the array\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_RekeyArrayEncryptionVolumeKeySafe(PCDA cda, SA_WORD array_number, SA_BOOL preserve_data, SA_BYTE user, const char *password, size_t password_size);
   /** Rekeying the volume failed.
    * @outputof SA_RekeyArrayEncryptionVolumeKey.
    * @outputof SA_RekeyArrayEncryptionVolumeKeySafe.
    */
   #define kRekeyArrayEncryptionVolumeKeyFailed       0x00
   /** Volume rekeyed successfully.
    * @outputof SA_RekeyArrayEncryptionVolumeKey.
    * @outputof SA_RekeyArrayEncryptionVolumeKeySafe.
    */
   #define kRekeyArrayEncryptionVolumeKeyOK           0x01
/** @} */ /* Encryption Rekey Array (Safe) */
/** @} */ /* storc_encryption_rekey_array */

/** @defgroup storc_encryption_recover_password Encryption Password Recovery
 * @brief Use the configuration encryption recovery question/answer to reset the crypto officer password.
 * @{
 */
/** @name Controller Encryption Recovery Question
 * @{ */
/** Get the encryption recovery question.
 * @pre `buffer_size` must be at least 257 bytes to contain the recovery question and terminating NULL.
 *
 * @param[in] cda              Controller data area.
 * @param[in] question_buffer  Question buffer.
 * @param[in] buffer_size      Size (in bytes) of `question_buffer`.
 * @return NULL terminating string of encryption recovery question, same as `question_buffer`.
 * @return Empty string if given buffer was not large enough to store the encryption recovery question.
 *
 * __Example__
 * @code{.c}
 * char question_buffer[257];
 * memset(question_buffer, 0, sizeof(question_buffer));
 * SA_GetEncryptionRecoveryQuestion(cda, question_buffer, kEncryptionRecoveryQuestionLength);
 * printf("The Recovery Question is: \n %s\n", question_buffer);
 * @endcode
 */
const char * SA_GetEncryptionRecoveryQuestion(PCDA cda, char * question_buffer, size_t buffer_size);
/** @} */ /* Controller Encryption Recovery Question */

/** @name Can Recover Encryption Password
 * @{ */
/** Can the encryption crypto officer password be recovered/reset.
 *
 * @deprecated Use @ref SA_CanResetEncryptionPasswordSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CanResetEncryptionPasswordSafe).
 *
 * @post If allowed, user may call @ref SA_ResetEncryptionPassword.
 *
 * @param[in] cda           Controller data area.
 * @param[in] answer        C-string recovery answer buffer (to question from @ref SA_GetEncryptionRecoveryQuestion).
                            If NULL, answer check (@ref kCanResetEncryptionPasswordInvalidAnswer) is skipped.
 * @param[in] new_password  C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
                            If NULL, password check (@ref kCanResetEncryptionPasswordInvalidPassword) is skipped.
 * @return See returns for [Can Recover Encryption Password](@ref storc_encryption_recover_password).
 *
 * __Example__
 * @code
 * if (SA_CanResetEncryptionPassword(cda, "the answer is fourty two", "MyNewP@ssword12345") == kCanResetEncryptionPasswordAllowed)
 * {
 *    if (SA_ResetEncryptionPassword(cda, "the answer is fourty two", "MyNewP@ssword12345") == kFalse)
 *    {
 *        fprintf(stderr, "Failed to reset encryption password\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanResetEncryptionPassword(PCDA cda, const char * answer, const char *new_password);
/** @} */ /* Can Recover Encryption Password */

/** @name Can Recover Encryption Password (Safe)
 * @{ */
/** Can the encryption crypto officer password be recovered/reset.
 *
 * @post If allowed, user may call @ref SA_ResetEncryptionPasswordSafe.
 *
 * @param[in] cda                Controller data area.
 * @param[in] answer             C-string recovery answer buffer (to question from @ref SA_GetEncryptionRecoveryQuestion).
 *                               If NULL, answer check (@ref kCanResetEncryptionPasswordInvalidAnswer) is skipped.
 * @param[in] answer_size        Size of C-string recovery answer buffer.
 * @param[in] new_password       C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 *                               If NULL, password check (@ref kCanResetEncryptionPasswordInvalidPassword) is skipped.
 * @param[in] new_password_size  Size of C-string new password for crypto officer.
 * @return See returns for [Can Recover Encryption Password](@ref storc_encryption_recover_password).
 *
 * __Example__
 * @code
 * if (SA_CanResetEncryptionPasswordSafe(cda, "the answer is fourty two", sizeof("the answer is fourty two"), "MyNewP@ssword12345", sizeof("MyNewP@ssword12345")) == kCanResetEncryptionPasswordAllowed)
 * {
 *    if (SA_ResetEncryptionPasswordSafe(cda, "the answer is fourty two", sizeof("the answer is fourty two"), "MyNewP@ssword12345", sizeof("MyNewP@ssword12345")) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to reset encryption password\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanResetEncryptionPasswordSafe(PCDA cda, const char * answer, size_t answer_size, const char *new_password, size_t new_password_size);
   /** Allowed.
    * @outputof SA_CanResetEncryptionPassword.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @allows SA_ResetEncryptionPassword.
    * @allows SA_ResetEncryptionPasswordSafe.
    */
   #define kCanResetEncryptionPasswordAllowed                      1
   /** Encryption password cannot be reset because encryption is not supported.
    * @outputof SA_CanResetEncryptionPassword.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @disallows SA_ResetEncryptionPassword.
    * @disallows SA_ResetEncryptionPasswordSafe.
    * @see SA_ControllerSupportsEncryption.
    */
   #define kCanResetEncryptionPasswordEncryptionNotSupported       2
   /** Encryption password cannot be reset because encryption is not configured.
    * @outputof SA_CanResetEncryptionPassword.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @disallows SA_ResetEncryptionPassword.
    * @disallows SA_ResetEncryptionPasswordSafe.
    * @see storc_configuration_edit_encryption.
    */
   #define kCanResetEncryptionPasswordEncryptionNotConfigured      3
   /** Encryption question/answer recovery is not set.
    * @outputof SA_CanResetEncryptionPassword.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @disallows SA_ResetEncryptionPassword.
    * @disallows SA_ResetEncryptionPasswordSafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanResetEncryptionPasswordEncryptionRecoveryNotSet     4
   /** Encryption password cannot be reset for express local encryption.
    * @outputof SA_CanResetEncryptionPassword.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @disallows SA_ResetEncryptionPassword.
    * @disallows SA_ResetEncryptionPasswordSafe.
    */
   #define kCanResetEncryptionPasswordExpressEncryption            5
   /** Encryption password cannot be reset because the answer parameter is invalid.
    * @outputof SA_CanResetEncryptionPassword.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @disallows SA_ResetEncryptionPassword.
    * @disallows SA_ResetEncryptionPasswordSafe.
    */
   #define kCanResetEncryptionPasswordInvalidAnswer                6
   /** Encryption password cannot be reset because the new password is invalid.
    * @outputof SA_CanResetEncryptionPassword.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @disallows SA_ResetEncryptionPassword.
    * @disallows SA_ResetEncryptionPasswordSafe.
    * @see SA_CheckEncryptionPassword.
    */
   #define kCanResetEncryptionPasswordInvalidPassword              7
   /** Encryption password cannot be reset because the feature is not enabled.
    * @outputof SA_CanResetEncryptionPassword.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @disallows SA_ResetEncryptionPassword.
    * @disallows SA_ResetEncryptionPasswordSafe.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanResetEncryptionPasswordNotEnabled                   8
   /** Encryption password cannot be reset because of an active or pending online firmware activation operation.
    * @outputof SA_CanResetEncryptionPassword.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @disallows SA_ResetEncryptionPassword.
    * @disallows SA_ResetEncryptionPasswordSafe.
    * @see storc_status_ctrl_online_firmware_activation
    */
   #define kCanResetEncryptionPasswordControllerOFAActive          9
   /** Encryption password cannot be reset because answer size is invalid.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @disallows SA_ResetEncryptionPasswordSafe.
    */
   #define kCanResetEncryptionPasswordInvalidAnswerSize            10
   /** Encryption password cannot be reset because password size is invalid.
    * @outputof SA_CanResetEncryptionPasswordSafe.
    * @disallows SA_ResetEncryptionPasswordSafe.
    */
   #define kCanResetEncryptionPasswordInvalidPasswordSize          11
/** @} */ /* Can Recover Encryption Password (Safe) */

/** @name Recover Encryption Password
 * @{ */
/** Reset the encryption crypto officer password using a pre-configured question/answer.
 *
 * @deprecated Use @ref SA_ResetEncryptionPasswordSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_ResetEncryptionPasswordSafe).
 *
 * @pre The user has called SA_CanResetEncryptionPassword to confirm the operation is allowed.
 *
 * @param[in] cda           Controller data area.
 * @param[in] answer        C-string recovery answer buffer (to question from @ref SA_GetEncryptionRecoveryQuestion).
 * @param[in] new_password  C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 * @return @ref kTrue if password was reset successfully.
 * @return @ref kFalse if operation failed or answer was incorrect.
 *
 * @post If return is @ref kFalse, user should use @ref SA_GetEncryptionPasswordStatus to check password lockout for the
 * @ref kControllerEncryptionCryptoOfficer user.
 *
 * __Example__
 * @code
 * if (SA_CanResetEncryptionPassword(cda, "the answer is fourty two", "MyNewP@ssword12345") == kCanResetEncryptionPasswordAllowed)
 * {
 *    if (SA_ResetEncryptionPassword(cda, "the answer is fourty two", "MyNewP@ssword12345") == kFalse)
 *    {
 *        fprintf(stderr, "Failed to reset encryption password\n");
 *        SA_QWORD password_status = SA_GetEncryptionPasswordStatus(cda, kControllerEncryptionCryptoOfficer);
 *        fprintf(stderr, "\n"
 *           "  Login Attempts Remaining: %u\n"
 *           "  Lockout Time Remaining: %u:%02u\n",
 *           (SA_DWORD)VERIFY_PASSWORD_ATTEMPTS_REMAINING(password_status),
 *           (SA_DWORD)VERIFY_PASSWORD_LOCKOUT_SECONDS_REMAINING(password_status) / 60,
 *           (SA_DWORD)VERIFY_PASSWORD_LOCKOUT_SECONDS_REMAINING(password_status) % 60
 *        );
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ResetEncryptionPassword(PCDA cda, const char * answer, const char *new_password);
/** @} */ /* Recover Encryption Password */

/** @name Recover Encryption Password (Safe)
 * @{ */
/** Reset the encryption crypto officer password using a pre-configured question/answer.
 *
 * @pre The user has called SA_CanResetEncryptionPasswordSafe to confirm the operation is allowed.
 *
 * @param[in] cda                Controller data area.
 * @param[in] answer             C-string recovery answer buffer (to question from @ref SA_GetEncryptionRecoveryQuestion).
 * @param[in] answer_size        Size of C-string recovery answer buffer.
 * @param[in] new_password       C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 * @param[in] new_password_size  Size of C-string new password for crypto officer.
 * @return @ref kTrue if password was reset successfully.
 * @return @ref kFalse if operation failed or answer was incorrect.
 *
 * @post If return is @ref kFalse, user should use @ref SA_GetEncryptionPasswordStatus to check password lockout for the
 * @ref kControllerEncryptionCryptoOfficer user.
 *
 * __Example__
 * @code
 * if (SA_CanResetEncryptionPasswordSafe(cda, "the answer is fourty two", sizeof("the answer is fourty two"), "MyNewP@ssword12345", sizeof("MyNewP@ssword12345")) == kCanResetEncryptionPasswordAllowed)
 * {
 *    if (SA_ResetEncryptionPasswordSafe(cda, "the answer is fourty two", sizeof("the answer is fourty two"), "MyNewP@ssword12345", sizeof("MyNewP@ssword12345")) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to reset encryption password\n");
 *        SA_QWORD password_status = SA_GetEncryptionPasswordStatus(cda, kControllerEncryptionCryptoOfficer);
 *        fprintf(stderr, "\n"
 *           "  Login Attempts Remaining: %u\n"
 *           "  Lockout Time Remaining: %u:%02u\n",
 *           (SA_DWORD)VERIFY_PASSWORD_ATTEMPTS_REMAINING(password_status),
 *           (SA_DWORD)VERIFY_PASSWORD_LOCKOUT_SECONDS_REMAINING(password_status) / 60,
 *           (SA_DWORD)VERIFY_PASSWORD_LOCKOUT_SECONDS_REMAINING(password_status) % 60
 *        );
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ResetEncryptionPasswordSafe(PCDA cda, const char * answer, size_t answer_size, const char *new_password, size_t new_password_size);
/** @} */ /* Recover Encryption Password (Safe) */

/** @} */ /* storc_encryption_recover_password */

/** @defgroup storc_encryption_restore_volatile_key_backup Encryption Restore Volatile Key
 * @brief Restore encrypted logical drive volatile key from backup.
 * @{
 */
/** @name Can Restore Volatile Key Backup
 @{ */
/** Can the logical drive's volatile key be recovered from the remote backup.
 *
 * @deprecated Use @ref SA_CanRestoreLogicalDriveVolatileKeySafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CanRestoreLogicalDriveVolatileKeySafe).
 *
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @param[in] user       Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password   C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 *                         If NULL, password check (@ref kCanResetEncryptionPasswordInvalidPassword) is skipped.
 * @return See returns for [Can Restore Volatile Key Backup](@ref storc_encryption_restore_volatile_key_backup).
 *
 * @post If allowed, user may call @ref SA_RestoreLogicalDriveVolatileKey.
 * @post If returns @ref kCanRestoreLogicalDriveVolatileKeyLoginFailed, user should check password
 * lockout using @ref SA_GetEncryptionPasswordStatus.
 *
 * __Example__
 * @code
 * if (SA_CanRestoreLogicalDriveVolatileKey(cda, ld_number, kControllerEncryptionCryptoOfficer, "MyP@ssword12345") == kCanRestoreLogicalDriveVolatileKeyAllowed)
 * {
 *    if (SA_RestoreLogicalDriveVolatileKey(cda, ld_number, kControllerEncryptionCryptoOfficer, "MyP@ssword12345") == kFalse)
 *    {
 *        fprintf(stderr, "Failed to recover backup for logical drive %u volatile key\n", ld_number);
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRestoreLogicalDriveVolatileKey(PCDA cda, SA_WORD ld_number, SA_BYTE user, const char *password);
/** @} */ /* Can Restore Volatile Key Backup */

/** @name Can Restore Volatile Key Backup (Safe)
 @{ */
/** Can the logical drive's volatile key be recovered from the remote backup.
 *
 * @param[in] cda             Controller data area.
 * @param[in] ld_number       Logical drive number.
 * @param[in] user            Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password        C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 *                             If NULL, password check (@ref kCanResetEncryptionPasswordInvalidPassword) is skipped.
 * @param[in] password_size   Size of C-string new password for crypto officer.
 * @return See returns for [Can Restore Volatile Key Backup](@ref storc_encryption_restore_volatile_key_backup).
 *
 * @post If allowed, user may call @ref SA_RestoreLogicalDriveVolatileKeySafe.
 * @post If returns @ref kCanRestoreLogicalDriveVolatileKeyLoginFailed, user should check password
 * lockout using @ref SA_GetEncryptionPasswordStatus.
 *
 * __Example__
 * @code
 * if (SA_CanRestoreLogicalDriveVolatileKeySafe(cda, ld_number, kControllerEncryptionCryptoOfficer, "MyP@ssword12345", sizeof("MyP@ssword12345")) == kCanRestoreLogicalDriveVolatileKeyAllowed)
 * {
 *    if (SA_RestoreLogicalDriveVolatileKeySafe(cda, ld_number, kControllerEncryptionCryptoOfficer, "MyP@ssword12345", sizeof("MyP@ssword12345")) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to recover backup for logical drive %u volatile key\n", ld_number);
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRestoreLogicalDriveVolatileKeySafe(PCDA cda, SA_WORD ld_number, SA_BYTE user, const char *password, size_t password_size);
   /** Allowed.
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @allows SA_RestoreLogicalDriveVolatileKey.
    * @allows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyAllowed                   1
   /** The volatile key cannot be restored because the target logical drive is invalid or does not exist.
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyInvalidLogicalDrive       2
   /** The volatile key cannot be restored because the user is invalid.
    * see [Encryption Users](@ref storc_encryption)
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyInvalidUser               3
   /** The volatile key cannot be restored because the user does not have permission to perform the action.
    * @see SA_GetEncryptionUserCapabilities.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyPermissionDenied          4
   /** The volatile key cannot be restored because the given password for the user is non-null and incorrect.
    * @see SA_VerifyEncryptionPasswordSafe.
    *
    * @warning User should check password lockout using @ref SA_GetEncryptionPasswordStatus.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyLoginFailed               5
   /** The volatile key cannot be restored because the logical drive is not a data logical drive.
    * The logical drive may be an LU cache volume or a split mirror volume.
    * @see storc_properties_ld_basic_info.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyNotData                   6
   /** The volatile key cannot be restored because encryption is not configured.
    * @see SA_GetControllerEncryptionStatus.
    * @see CTRL_ENCRYPTION_IS_CONFIGURED.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyEncryptionNotConfigured   7
   /** The volatile key cannot be restored because encryption is not enabled.
    * @see SA_GetControllerEncryptionStatus.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyEncryptionNotEnabled      8
   /** The volatile key cannot be restored because the current key manager is not remote
    * which means there is no backup to restore.
    * @see SA_GetControllerEncryptionStatus.
    * @see CTRL_ENCRYPTION_IS_REMOTE.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyNoRemoteKeyManager        9
   /** The volatile key cannot be restored because the logical drive is not encrypted.
    * @see SA_GetLogicalDriveEncryptionInfo.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyLogicalDriveNotEncrypted  10
   /** The volatile key cannot be restored because volatile keys are not enabled for the logical drive.
    * @see SA_GetLogicalDriveEncryptionInfo.
    * @see VOLATILE_KEY_ENABLED.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyNotConfigured             11
   /** The volatile key cannot be restored because no backup exists for the logical drive's volatile key.
    * @see SA_GetLogicalDriveEncryptionInfo.
    * @see VOLATILE_KEY_BACKUP_ENABLED.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyMissingBackup             12
   /** The volatile key cannot be restored because the logical drive does not require restoring the volatile key.
    * @see storc_status_ld.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyRestoreNotRequired        13
   /** The volatile key cannot be restored because a current volatile key operation is currently in progress.
    * @see SA_GetLogicalDriveEncryptionInfo.
    * @see VOLATILE_KEY_STATUS.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyOpInProgress              14
   /** The volatile key cannot be restored because volatile keys are not supported/enabled.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyNotSupported              15
   /** The volatile key cannot be restored because of an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation.
    *
    * @outputof SA_CanRestoreLogicalDriveVolatileKey.
    * @outputof SA_CanRestoreLogicalDriveVolatileKeySafe.
    * @disallows SA_RestoreLogicalDriveVolatileKey.
    * @disallows SA_RestoreLogicalDriveVolatileKeySafe.
    */
   #define kCanRestoreLogicalDriveVolatileKeyControllerOFAActive       16
/** @} */ /* Can Restore Volatile Key Backup (Safe) */

/** @name Restore Volatile Key Backup
 @{ */
/** Restore the logical drive's volatile from a remote backup.
 *
 * @deprecated Use @ref SA_RestoreLogicalDriveVolatileKeySafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_RestoreLogicalDriveVolatileKeySafe).
 *
 * @pre The user has called SA_CanRestoreLogicalDriveVolatileKey to confirm the operation is allowed.
 *
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @param[in] user       Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password   C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 *                         If NULL, password check (@ref kCanResetEncryptionPasswordInvalidPassword) is skipped.
 * @return @ref kTrue if the volatile key is successfully restoring from the remote backup, @ref kFalse otherwise.
 *
 * @post If returns @ref kFalse, user should check password lockout using @ref SA_GetEncryptionPasswordStatus.
 * @post If returns @ref kTrue, the user can call @ref SA_GetLogicalDriveEncryptionInfo to get volatile key status.
 *
 * __Example__
 * @code
 * if (SA_CanRestoreLogicalDriveVolatileKey(cda, ld_number, kControllerEncryptionCryptoOfficer, "MyP@ssword12345") == kCanRestoreLogicalDriveVolatileKeyAllowed)
 * {
 *    if (SA_RestoreLogicalDriveVolatileKey(cda, ld_number, kControllerEncryptionCryptoOfficer, "MyP@ssword12345") == kFalse)
 *    {
 *        fprintf(stderr, "Failed to recover backup for logical drive %u volatile key\n", ld_number);
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_RestoreLogicalDriveVolatileKey(PCDA cda, SA_WORD ld_number, SA_BYTE user, const char *password);
/** @} */ /* Can Restore Volatile Key Backup */

/** @name Restore Volatile Key Backup (Safe)
 @{ */
/** Restore the logical drive's volatile from a remote backup.
 *
 * @pre The user has called SA_CanRestoreLogicalDriveVolatileKeySafe to confirm the operation is allowed.
 *
 * @param[in] cda             Controller data area.
 * @param[in] ld_number       Logical drive number.
 * @param[in] user            Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password        C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 *                             If NULL, password check (@ref kCanResetEncryptionPasswordInvalidPassword) is skipped.
 * @param[in] password_size   Size of C-string new password for crypto officer.
 * @return @ref kTrue if the volatile key is successfully restoring from the remote backup, @ref kFalse otherwise.
 *
 * @post If returns @ref kFalse, user should check password lockout using @ref SA_GetEncryptionPasswordStatus.
 * @post If returns @ref kTrue, the user can call @ref SA_GetLogicalDriveEncryptionInfo to get volatile key status.
 *
 * __Example__
 * @code
 * if (SA_CanRestoreLogicalDriveVolatileKeySafe(cda, ld_number, kControllerEncryptionCryptoOfficer, "MyP@ssword12345", sizeof("MyP@ssword12345")) == kCanRestoreLogicalDriveVolatileKeyAllowed)
 * {
 *    if (SA_RestoreLogicalDriveVolatileKeySafe(cda, ld_number, kControllerEncryptionCryptoOfficer, "MyP@ssword12345", sizeof("MyP@ssword12345")) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to recover backup for logical drive %u volatile key\n", ld_number);
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_RestoreLogicalDriveVolatileKeySafe(PCDA cda, SA_WORD ld_number, SA_BYTE user, const char *password, size_t password_size);
/** @} */ /* Can Restore Volatile Key Backup (Safe) */

/** @} */ /* storc_encryption_restore_volatile_key_backup */

/** @defgroup storc_encryption_key_cache Encryption Key Cache
 * @brief Get current status/state of the controller's encryption key cache.
 * @ingroup storc_status
 * @{
 */
/** @name Controller Encryption Key Cache Status
 * @{ */
/** Obtain encryption key cache status info for a Controller.
 * @param[in] cda  Controller data area.
 * @return See [Encryption Key Cache Status Returns](@ref storc_encryption_key_cache)
 * @return Decode with @ref ENCRYPTION_KEY_CACHE_ATTEMPTS to get the controller's encryption key cache configured maximum attempts.
 * @return Decode with @ref ENCRYPTION_KEY_CACHE_INTERVAL to get the controller's encryption key cache retry interval in minutes.
 * @return Decode with @ref ENCRYPTION_KEY_CACHE_ENABLED to determine if the controller's encryption key cache is enabled.
 * @return Decode with @ref ENCRYPTION_PERSISTENT_KEY_CACHE_ENABLED to determine if the controller's encryption persistent key cache is enabled.
 *
 * __Example__
 * @code
 * SA_QWORD key_cache_status  = SA_GetControllerEncryptionKeyCacheStatus(cda);
 * SA_WORD persistent_key_cache_enabled = ENCRYPTION_PERSISTENT_KEY_CACHE_ENABLED(key_cache_status);
 * SA_BOOL key_cache_enabled            = ENCRYPTION_KEY_CACHE_ENABLED(key_cache_status);
 * SA_WORD key_cache_attempts           = ENCRYPTION_KEY_CACHE_ATTEMPTS(key_cache_status);
 * SA_WORD key_cache_interval           = ENCRYPTION_KEY_CACHE_INTERVAL(key_cache_status);
 * printf("Persistent Key Cache: %s\n" persistent_key_cache_enabled ? "Enabled" : "Disabled");
 * printf("Key Cache: %s\n", key_cache_enabled ? "Enabled" : "Disabled");
 * printf("Current retry attempts: %u\n", key_cache_attempts);
 * printf("Current retry interval (minutes): %u\n", key_cache_interval);
 * @endcode
 */
SA_QWORD SA_GetControllerEncryptionKeyCacheStatus(PCDA cda);
   /** Controller encryption key cache status retry attempts mask; @outputof SA_GetControllerEncryptionKeyCacheStatus. */
   #define kControllerEncryptionKeyCacheStatusRetryAttemptsMask       0x000000000000FFFFULL
   /** Bitshift to use with output of @ref SA_GetControllerEncryptionKeyCacheStatus
    * (after masking with @ref kControllerEncryptionKeyCacheStatusRetryAttemptsMask) to get the controller's key cache retry attempts.
    */
   #define kControllerEncryptionKeyCacheStatusRetryAttemptsBitShift   0
    /** Decode @outputof SA_GetControllerEncryptionKeyCacheStatus to determine the controller's key cache configured maximum attempts. */
   #define ENCRYPTION_KEY_CACHE_ATTEMPTS(info) ((SA_WORD)((info & kControllerEncryptionKeyCacheStatusRetryAttemptsMask) >> kControllerEncryptionKeyCacheStatusRetryAttemptsBitShift))

   /** Controller encryption key cache status retry interval mask; @outputof SA_GetControllerEncryptionKeyCacheStatus. */
   #define kControllerEncryptionKeyCacheStatusRetryIntervalMask       0x00000000FFFF0000ULL
   /** Bitshift to use with output of @ref SA_GetControllerEncryptionKeyCacheStatus
    * (after masking with @ref kControllerEncryptionKeyCacheStatusRetryIntervalMask) to get the controller's key cache retry interval in minutes.
    */
   #define kControllerEncryptionKeyCacheStatusRetryIntervalBitShift   16
   /** Decode @outputof SA_GetControllerEncryptionKeyCacheStatus to determine the controller's key cache retry interval in minutes. */
   #define ENCRYPTION_KEY_CACHE_INTERVAL(info) ((SA_WORD)((info & kControllerEncryptionKeyCacheStatusRetryIntervalMask) >> kControllerEncryptionKeyCacheStatusRetryIntervalBitShift))

   /** Controller encryption persistent key cache enabled mask; @outputof SA_GetControllerEncryptionKeyCacheStatus. */
   #define kControllerEncryptionPersistentKeyCacheEnabledMask         0x4000000000000000ULL
   /** Bitshift to use with output of @ref SA_GetControllerEncryptionKeyCacheStatus
   * (after masking with @ref kControllerEncryptionPersistentKeyCacheEnabledMask) to determine if persistent key cache is enabled on the controller.
   */
   #define kControllerEncryptionPersistentKeyCacheEnabledBitShift     62
   /** Decode @outputof SA_GetControllerEncryptionKeyCacheStatus to determine whether or not encryption persistent key cache is enabled. */
   #define ENCRYPTION_PERSISTENT_KEY_CACHE_ENABLED(info) ((SA_WORD)(((info & kControllerEncryptionPersistentKeyCacheEnabledMask) >> kControllerEncryptionPersistentKeyCacheEnabledBitShift) ? kTrue : kFalse))

   /** Controller encryption key cache status enabled mask; @outputof SA_GetControllerEncryptionKeyCacheStatus. */
   #define kControllerEncryptionKeyCacheStatusEnabledMask             0x8000000000000000ULL
   /** Bitshift to use with output of @ref SA_GetControllerEncryptionKeyCacheStatus
    * (after masking with @ref kControllerEncryptionKeyCacheStatusEnabledMask) to determine if key cache is enabled on the controller.
    */
   #define kControllerEncryptionKeyCacheStatusEnabledBitShift         63
   /** Decode @outputof SA_GetControllerEncryptionKeyCacheStatus to determine whether or not encryption key cache is enabled. */
   #define ENCRYPTION_KEY_CACHE_ENABLED(info) ((SA_WORD)((info & kControllerEncryptionKeyCacheStatusEnabledMask) ? kTrue : kFalse))
/** @} */ /* Controller Encryption Key Cache Status */

/** @} */ /* storc_encryption_key_cache */

/** @defgroup storc_encryption_rescan_remote_keys Encryption Rescan Remote Keys
 * @brief Perform an encryption rescan of remote keys.
 * @{
 */
/** @name Can Rescan Remote Keys
 * @{ */
/** Can the remote keys be rescanned?
 *
 * @param[in] cda        Controller data area.
 * @return See returns for [Can Rescan Remote Keys](@ref storc_encryption_rescan_remote_keys).
 *
 * @post If allowed, user may call @ref SA_RescanEncryptionRemoteKeys.
 *
 * __Example__
 * @code
 * if (SA_CanRescanEncryptionRemoteKeys(cda) == kCanRescanEncryptionRemoteKeysAllowed)
 * {
 *    if (SA_RescanEncryptionRemoteKeys(cda) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to rescan remote keys\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRescanEncryptionRemoteKeys(PCDA cda);
   /** Allowed.
    * @outputof SA_CanRescanEncryptionRemoteKeys.
    * @allows SA_RescanEncryptionRemoteKeys.
    */
   #define kCanRescanEncryptionRemoteKeysAllowed                   1
   /** Remote keys cannot be rescanned because encryption is not configured.
    * @outputof SA_CanRescanEncryptionRemoteKeys.
    * @disallows SA_RescanEncryptionRemoteKeys.
    */
   #define kCanRescanEncryptionRemoteKeysEncryptionNotConfigured   2
   /** Remote keys cannot be rescanned because encryption is not configured to use a remote key manager.
    * @see SA_GetControllerEncryptionStatus.
    * @outputof SA_CanRescanEncryptionRemoteKeys.
    * @disallows SA_RescanEncryptionRemoteKeys.
    */
   #define kCanRescanEncryptionRemoteKeysRemoteKeysNotConfigured   3
   /** Remote keys cannot be rescanned because encryption is disabled.
    * @see SA_GetControllerEncryptionStatus.
    * @outputof SA_CanRescanEncryptionRemoteKeys.
    * @disallows SA_RescanEncryptionRemoteKeys.
    */
   #define kCanRescanEncryptionRemoteKeysEncryptionNotEnabled      4
   /** Remote keys cannot be rescanned because the controller is locked due to a missing boot password.
    * @see SA_GetControllerEncryptionStatus.
    * @outputof SA_CanRescanEncryptionRemoteKeys.
    * @disallows SA_RescanEncryptionRemoteKeys.
    */
   #define kCanRescanEncryptionRemoteKeysControllerLocked          5
   /** Remote keys cannot be rescanned because the controller has failed an encryption self test.
    * @see SA_GetControllerEncryptionStatus.
    * @outputof SA_CanRescanEncryptionRemoteKeys.
    * @disallows SA_RescanEncryptionRemoteKeys.
    */
   #define kCanRescanEncryptionRemoteKeysControllerFailedSelfTest  6
   /** Remote keys cannot be rescanned because the controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanRescanEncryptionRemoteKeys.
    * @disallows SA_RescanEncryptionRemoteKeys.
    */
   #define kCanRescanEncryptionRemoteKeysControllerOFAActive       7
/** @} */ /* Can Rescan Remote Keys */

/** @name Rescan Remote Keys
 * @{
 */
/** Rescan remote keys.
 *
 * @pre The user has called SA_CanRescanEncryptionRemoteKeys to confirm the operation is allowed.
 *
 * @param[in] cda        Controller data area.
 * @return @ref kTrue if remote keys were rescanned successfully, @ref kFalse otherwise.
 *
 *
 * __Example__
 * @code
 * if (SA_CanRescanEncryptionRemoteKeys(cda) == kCanRescanEncryptionRemoteKeysAllowed)
 * {
 *    if (SA_RescanEncryptionRemoteKeys(cda) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to rescan remote keys\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_RescanEncryptionRemoteKeys(PCDA cda);
/** @} */ /* Rescan Remote Keys */

/** @} */ /* storc_encryption_rescan_remote_keys */

/** @defgroup storc_encryption_crypto_erase Encryption Crypto Erase Logical Drive
 * @brief Erase logical drive content by cryptographic erasure.
 * @details
 * @{ */

/** @name Can Crypto Erase Logical Drive.
 * @{ */
/** Checks if the crypto erasure of a logical drive can be done.
 *
 * @deprecated Use @ref SA_CanCryptoEraseLogicalDriveSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CanCryptoEraseLogicalDriveSafe).
 *
 * @post If allowed, user may call @ref SA_CryptoEraseLogicalDrive.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @param[in] user       Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password   C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 *                       If NULL, password check (@ref kCanResetEncryptionPasswordInvalidPassword) is skipped.
 * @return See [Can Crypto Erase Logical Drive Returns](@ref storc_encryption_crypto_erase)
 * @post If return is @ref kCanCryptoEraseLogicalDriveInvalidPassword, user should use
 * @ref SA_GetEncryptionPasswordStatus to check password lockout for the user.
 * @post If return is @ref kCanCryptoEraseLogicalDriveAllowed user may call @ref SA_CryptoEraseLogicalDrive.
 *
 * __Example__
 * @code
 * if (SA_CanCryptoEraseLogicalDrive(cda, ld_number, user, password) == kCanCryptoEraseLogicalDriveAllowed)
 * {
 *    if (SA_CryptoEraseLogicalDrive(cda, ld_number, user, password) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to crypto erase the logical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanCryptoEraseLogicalDrive(PCDA cda, SA_WORD ld_number, SA_BYTE user, const char * password);
/** @} */ /* Can Crypto Erase Logical Drive Returns */

/** @name Can Crypto Erase Logical Drive (Safe).
 * @{ */
 /** Checks if the crypto erasure of a logical drive can be done.
 * @post If allowed, user may call @ref SA_CryptoEraseLogicalDriveSafe.
 * @param[in] cda             Controller data area.
 * @param[in] ld_number       Logical drive number.
 * @param[in] user            Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password        C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 *                            If NULL, password check (@ref kCanResetEncryptionPasswordInvalidPassword) is skipped.
 * @param[in] password_size   Size of new password for crypto officer.
 * @return See [Can Crypto Erase Logical Drive Returns](@ref storc_encryption_crypto_erase)
 * @post If return is @ref kCanCryptoEraseLogicalDriveInvalidPassword, user should use
 * @ref SA_GetEncryptionPasswordStatus to check password lockout for the user.
 * @post If return is @ref kCanCryptoEraseLogicalDriveAllowed user may call @ref SA_CryptoEraseLogicalDrive.
 *
 * __Example__
 * @code
 * if (SA_CanCryptoEraseLogicalDriveSafe(cda, ld_number, user, password, sizeof(password)) == kCanCryptoEraseLogicalDriveAllowed)
 * {
 *    if (SA_CryptoEraseLogicalDriveSafe(cda, ld_number, user, password, sizeof(password)) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to crypto erase the logical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanCryptoEraseLogicalDriveSafe(PCDA cda, SA_WORD ld_number, SA_BYTE user, const char * password, size_t password_size);
   /** Allowed.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @allows SA_CryptoEraseLogicalDrive.
    * @allows SA_CryptoEraseLogicalDriveSafe.
    */
   #define kCanCryptoEraseLogicalDriveAllowed                               1
   /** Crypto erase cannot occur because the logical drive is invalid.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    */
   #define kCanCryptoEraseLogicalDriveInvalidLogicalDrive                   2
   /** Crypto erase cannot occur because the user is invalid.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see IS_LOGICAL_DRIVE_CONFIGURED
    */
   #define kCanCryptoEraseLogicalDriveInvalidUser                           3
   /** Crypto erase cannot occur because encryption is not configured for the controller.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetControllerEncryptionStatus.
    * @see CTRL_ENCRYPTION_IS_CONFIGURED.
    */
   #define kCanCryptoEraseLogicalDriveEncryptionNotConfigured               4
   /** Crypto erase cannot occur because encryption is not enabled.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanCryptoEraseLogicalDriveEncryptionNotEnabled                  5
    /** Crypto erase cannot occur because the encryption mode is express local.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetControllerEncryptionStatus.
    * @see CTRL_ENCRYPTION_IS_EXPRESS_LOCAL.
    */
   #define kCanCryptoEraseLogicalDriveInExpressLocalMode                    6
    /** Crypto erase cannot occur because the logical drive is offline.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetLogicalDriveEncryptionInfo.
    * @see LOGICAL_DRIVE_ENCRYPTION_STATUS.
    */
   #define kCanCryptoEraseLogicalDriveLogicalDriveIsOffline                 7
   /** Crypto erase cannot occur because the logical drive is not encoded.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetLogicalDriveEncryptionInfo.
    */
   #define kCanCryptoEraseLogicalDriveLogicalDriveNotEncrypted              8
   /** Crypto erase cannot occur because the logical drive status is not OK.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanCryptoEraseLogicalDriveControllerLockout                     9
   /** Crypto erase cannot occur because the controller has a lockout.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetLogicalDriveStatusInfo.
    */
   #define kCanCryptoEraseLogicalDriveLogicalDriveStatusNotOK              10
   /** Crypto erase cannot occur because the array status is not OK.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetArrayStatusInfo.
    */
   #define kCanCryptoEraseLogicalDriveArrayStatusNotOK                     11
   /** Crypto erase cannot occur because volatile keys are enabled.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetLogicalDriveStatusInfo.
    * @see VOLATILE_KEY_ENABLED.
    */
   #define kCanCryptoEraseLogicalDriveVolatileKeysEnabled                  12
   /** Crypto erase cannot occur because the logical drive is not a data logical drive.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetLogicalDriveBasicInfo.
    */
   #define kCanCryptoEraseLogicalDriveNotDataLogicalDrive                  13
   /** Crypto erase cannot occur because the user permission is denied.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetEncryptionUserCapabilities.
    */
   #define kCanCryptoEraseLogicalDriveUserPermissionDenied                 14
   /** Crypto erase cannot occur because the password is invalid.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_VerifyEncryptionPasswordSafe.
    */
   #define kCanCryptoEraseLogicalDriveInvalidPassword                      15
   /** Crypto erase cannot occur because the feature is not supported/enabled.
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanCryptoEraseLogicalDriveNotEnabled                           16
   /** Crypto erase cannot occur because the logical drive has a registered split mirror pair.
    * @deprecated Uses @ref kCanCryptoEraseLogicalDriveNotDataLogicalDrive instead.
    * The logical drive's split mirror pair may have been delete, become offline or unavailable
    * without being properly separated from the logical drive using resync, rollback, or activate.
    *
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDrive.
    */
   #define kCanCryptoEraseLogicalDriveHasSplitMirrorPair                   17
   /** Crypto erase cannot occur because the controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    *
    * @outputof SA_CanCryptoEraseLogicalDrive.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDrive.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    */
   #define kCanCryptoEraseLogicalDriveControllerOFAActive                  18
   /** Crypto erase cannot occur because the password size is invalid.
    * @outputof SA_CanCryptoEraseLogicalDriveSafe.
    * @disallows SA_CryptoEraseLogicalDriveSafe.
    */
   #define kCanCryptoEraseLogicalDriveInvalidPasswordSize                  19
/** @} */ /* Can Crypto Erase Logical Drive (Safe). */

/** @name Crypto Erase Logical Drive.
 * @{ */
/** Performs a crypto erasure of a logical drive.
 *
 * @deprecated Use @ref SA_CryptoEraseLogicalDriveSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CryptoEraseLogicalDriveSafe).
 *
 * @pre The user has used @ref SA_CanCryptoEraseLogicalDrive to ensure the operation can be performed.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Logical drive number.
 * @param[in] user       Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password   C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 * @return @ref kTrue if erase completed successfully, @ref kFalse otherwise.
 * @post The logical drive should be ready for use when the function call returns.
 * @post If return is @ref kCanCryptoEraseLogicalDriveInvalidPassword, user should use
 * @ref SA_GetEncryptionPasswordStatus to check password lockout for the user.
 *
 * __Example__
 * @code
 * if (SA_CanCryptoEraseLogicalDrive(cda, ld_number, user, password) == kCanCryptoEraseLogicalDriveAllowed)
 * {
 *    if (SA_CryptoEraseLogicalDrive(cda, ld_number, user, password) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to crypto erase the logical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_CryptoEraseLogicalDrive(PCDA cda, SA_WORD ld_number, SA_BYTE user, const char * password);
/** @} */ /* Crypto Erase Logical Drive */

/** @name Crypto Erase Logical Drive (Safe).
 * @{ */
/** Performs a crypto erasure of a logical drive.
 * @pre The user has used @ref SA_CanCryptoEraseLogicalDriveSafe to ensure the operation can be performed.
 * @param[in] cda             Controller data area.
 * @param[in] ld_number       Logical drive number.
 * @param[in] user            Encryption user (see [Encryption Users](@ref storc_encryption)).
 * @param[in] password        C-string new password for crypto officer (validated with @ref SA_CheckEncryptionPassword).
 * @param[in] password_size   Size of new password for crypto officer.
 * @return @ref kTrue if erase completed successfully, @ref kFalse otherwise.
 * @post The logical drive should be ready for use when the function call returns.
 * @post If return is @ref kCanCryptoEraseLogicalDriveInvalidPassword, user should use
 * @ref SA_GetEncryptionPasswordStatus to check password lockout for the user.
 *
 * __Example__
 * @code
 * if (SA_CanCryptoEraseLogicalDriveSafe(cda, ld_number, user, password, sizeof(password)) == kCanCryptoEraseLogicalDriveAllowed)
 * {
 *    if (SA_CryptoEraseLogicalDriveSafe(cda, ld_number, user, password, sizeof(password)) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to crypto erase the logical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_CryptoEraseLogicalDriveSafe(PCDA cda, SA_WORD ld_number, SA_BYTE user, const char * password, size_t password_size);
/** @} */ /* Crypto Erase Logical Drive (Safe). */

/** @} */ /* storc_encryption_crypto_erase Crypto Erase Logical Drive */


/** @defgroup storc_encryption_unlock_contoller Encryption Unlock Controller
 * @brief Unlock the controller locked by controller boot password.
 * @details
 * The encrypted volumes will be locked when the controller password is escaped or not entered correctly
 * during boot time. The boot password can be re-entered to unlock the controller and the locked volumes. This
 * is an instantaneous operation that requires encryption to be enabled.
 * @{
 */

/** @name Can Unlock Controller
 * @{ */
/** Checks if the controller can be unlocked.
 * @allows SA_DoUnlockController.
 * @param[in] cda                Controller data area.
 * @param[in] boot_password      Boot password to be entered.
 * @param[in] boot_password_size Size of the boot password C-string (number of bytes).
 * @return See [Can Unlock Controller Returns](@ref storc_encryption_unlock_contoller)
 *
 * __Example__
 * @code
 * if (SA_CanUnlockController(cda, boot_password, boot_password_size) == kCanUnlockControllerAllowed)
 * {
 *    if (SA_DoUnlockController(cda, boot_password, boot_password_size) == kDoUnlockControllerOK)
 *    {
 *       fprintf(stderr, "Succeeded to unlock controller.\n");
 *    }
 *    else
 *    {
 *       fprintf(stderr, "Failed to unlock controller.\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanUnlockController(PCDA cda, const char * boot_password, size_t boot_password_size);
   /** @name Can Unlock Controller Returns
    * @outputof SA_CanUnlockController.
    * @{ */
   /** Unlock controller is allowed.
    * @outputof SA_CanUnlockController.
    * @allows SA_DoUnlockController.
    */
   #define kCanUnlockControllerAllowed                           1
   /** Cannot unlock controller because the controller doesn't support encryption.
    * @outputof SA_CanUnlockController.
    * @disallows SA_DoUnlockController.
    */
   #define kCanUnlockControllerEncryptionNotSupported            2
   /** Cannot unlock controller because the controller doesn't support boot password.
    * @outputof SA_CanUnlockController.
    * @disallows SA_DoUnlockController.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanUnlockControllerBootPasswordNotSupported          3
   /** Cannot unlock controller because encryption is not enabled.
    * @outputof SA_CanUnlockController.
    * @disallows SA_DoUnlockController.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanUnlockControllerEncryptionNotEnabled              4
   /** Cannot unlock controller because the controller has a pending or running online firmware activation operation.
    * @outputof SA_CanUnlockController.
    * @disallows SA_DoUnlockController.
    * @see SA_IS_CONTROLLER_OFA_RUNNING
    */
   #define kCanUnlockControllerOFAActive                          5
   /** Cannot unlock controller because the controller status is not OK.
    * @outputof SA_CanUnlockController.
    * @disallows SA_DoUnlockController.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanUnlockControllerControllerStatusNotOK             6
   /** Cannot unlock controller because encryption is not configured.
    * @outputof SA_CanUnlockController.
    * @disallows SA_DoUnlockController.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanUnlockControllerEncryptionNotConfigured           7
   /** Cannot unlock controller because controller boot password is not set.
    * @outputof SA_CanUnlockController.
    * @disallows SA_DoUnlockController.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanUnlockControllerBootPasswordNotSet                 8
   /** Cannot unlock controller because the controller is not locked.
    * @outputof SA_CanUnlockController.
    * @disallows SA_DoUnlockController.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanUnlockControllerControllerNotLocked               9
   /** Cannot unlock controller because there are no more unlock attempts remaining.
    * @outputof SA_CanUnlockController.
    * @disallows SA_DoUnlockController.
    * @see VERIFY_PASSWORD_ATTEMPTS_REMAINING.
    */
   #define kCanUnlockControllerNoUnlockAttemptsRemaining         10
   /** Cannot unlock controller because the input boot password is invalid.
    * @outputof SA_CanUnlockController.
    * @disallows SA_DoUnlockController.
    * @see SA_CheckEncryptionBootPassword.
    */
   #define kCanUnlockControllerInvalidBootPassword               11
   /** @} */ /* Can Unlock Controller Returns */
/** @} */ /* Can Unlock Controller */

/** @name Do Unlock Controller
 * @{ */
/** Perform Unlock Controller With Boot Password.
 * @pre The user has used @ref SA_CanUnlockController to ensure the operation can be performed.
 * @param[in] cda                Controller data area.
 * @param[in] boot_password      Boot password to be entered.
 * @param[in] boot_password_size Size of the boot password C-string (number of bytes).
 * @return See [Do Unlock Controller Returns](@ref storc_encryption_unlock_contoller)
 *
 * __Example__
 * @code
 * if (SA_CanUnlockController(cda, boot_password, boot_password_size) == kCanUnlockControllerAllowed)
 * {
 *    if (SA_DoUnlockController(cda, boot_password, boot_password_size) == kDoUnlockControllerOK)
 *    {
 *       fprintf(stderr, "Succeeded to unlock controller.\n");
 *    }
 *    else
 *    {
 *       fprintf(stderr, "Failed to unlock controller.\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_DoUnlockController(PCDA cda, const char* boot_password, size_t boot_password_size);
   /** @name Do Unlock Controller Returns
    * @outputof SA_CanUnlockController.
    * @{ */
   /** Unlock controller failed.
    * @outputof SA_DoUnlockController.
    */
   #define kDoUnlockControllerFailed                0x00
   /** Unlock controller succeeded.
    * @outputof SA_DoUnlockController.
    */
   #define kDoUnlockControllerOK                    0x01
   /** @} */ /* Do Unlock Controller Returns */
/** @} */ /* Do Unlock Controller */
/** @} */ /* storc_encryption_unlock_contoller */

/** @} */ /* storc_encryption */

/** @} */ /* storcore */


#if defined(__cplusplus)
}
#endif

#endif /* STORC_ENCRYPTION_H */

