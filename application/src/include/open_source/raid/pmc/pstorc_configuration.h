/** @file pstorc_configuration.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore configuration header file
   Defines structures and functions for configuring devices

*/

#ifndef PSTORC_CONFIGURATION_H
#define PSTORC_CONFIGURATION_H

/************************//**
* @addtogroup storcore
* @{
***************************/

/******************************************//**
 * @defgroup storc_configuration Configuration
 * @brief
 * @details
 * There are 3 types of configuration changes:
 *  - @ref storc_configuration_direct
 *  - @ref storc_configuration_virtual
 *  - @ref storc_configuration_ctrl_port_discovery
 * @{
 *********************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*********************************************//**
 * @defgroup storc_configuration_virtual Virtual Configuration/Modification
 * @brief Configuration changes that take place in a virtual state and are then committed.
 * @details
 * A virtual change to a configuration applies to a instance of a virtual CDA.
 * Multiple changes can be queued to a CDA allowing a caller to determine if the
 * final set of changes can co-exist before making any actual changes to your
 * configuration.
 *
 * Functions under the [Configuration](@ref storc_configuration) module that do
 * include *Editable* in the name are functions that execute virtual changes.
 *
 * In order to start working with an [editable configuration](@ref storc_configuration_editable),
 * the user must first initialize a CDA for virtual changes by creating an editable
 * configuration (which is stored within a CDA).
 *
 * Once a user has made all desired changes, the virtual configuration can be
 * applied to actual hardware by committing the editable configuration. Otherwise,
 * a user can simply delete an editable configuration without committing, leaving
 * the hardware state unchanged.
 *
 * @see storc_configuration_editable
 *
 * A user can edit only 1 editable configuration per CDA at a time.
 *
 * Virtual Transformations
 * =======================
 * See examples at @ref storc_config_transform_examples.
 *
 * Many transformations are performed using the editable configuration. This allows
 * clients to virtually organize a transformation before committing their changes.
 *
 * To ensure integrity of user data, transformations have different restrictions & rules
 * that can change on each modification to the editable configuration.
 *
 * In general, as multiple transformations are queued on a single Array, the number of available
 * operations (including transformations) and/or the number available parameters for an
 * allowed transformations will become more limited.
 *
 * Committed a set of transformations, waiting for them to complete, and starting a new
 * editable configuration gives you the most available options on new transformations.
 *
 * For example, say you have 2 logical drives on the same Array (LD 0, and LD 1).
 * If you start a logical drive extension on LD 0, you will not be able to migrate
 * the RAID of LD 1 unless it's migrating to a higher fault tolerant RAID. Just as if
 * you were to queue a RAID migration of LD 1 to a lower fault tolerant RAID, you
 * would not be able to extend LD 0. In order to extend LD 0 and migrate LD 1 to a
 * lower fault tolerant RAID, you would have to commit one of the transformations,
 * wait for the transformation to complete, start a new editable configuration and
 * queue the other operation.
 *
 * In general, a single editable configuration can queue multiple transformations of
 * the same type on one or multiple Arrays without restrictions (i.e. Array 1 with multiple
 * ld extend operations, Array 2 with multiple migrate operations, Array 3 performing
 * an Array expand, ....). The only exception is logical drive move: once LD move is queued
 * from Array X to Array Y, Array X will continue to accept other transformations while
 * Array Y can now only be modified by moving more/other logical drives to Array Y.
 *
 * Logical Drive Extension
 * ------------------------------------
 * Example: @ref storc_config_transform_example_extend
 *
 * Module: @ref storc_configuration_edit_ld_set_size
 *
 * Logical Drive Move
 * -------------------------------
 * Example: @ref storc_config_transform_example_ld_move.
 *
 * Module: @ref storc_configuration_edit_ld_set_array
 *
 * Array Data Drive Swap
 * ----------------------------------
 * Example: @ref storc_config_transform_example_array_swap
 *
 * Module: @ref storc_configuration_edit_array_move
 *
 * Logical Drive RAID/SS Migration
 * --------------------------------------------
 * Example: @ref storc_config_transform_example_ld_migrate
 *
 * Module: @ref storc_configuration_edit_ld_set_raid
 * and @ref storc_configuration_edit_ld_set_stripsize
 *
 * Array Expand
 * -------------------------
 * Example: @ref storc_config_transform_example_expand
 *
 * Module: @ref storc_configuration_edit_array_add_data_drive
 *
 * Array Shrink
 * -------------------------
 * Example: @ref storc_config_transform_example_shrink
 *
 * Module: @ref storc_configuration_edit_array_remove_data_drive
 * @{
 ************************************************/

/*****************************************************************//**
 * @defgroup storc_configuration_editable The Editable Configuration
 * @brief Functions used to create, commit, & delete an editable configuration to apply virtual changes.
 * @details
 * To apply virtual changes to a configuration.
 * the user uses the following calls:
 *   1. Create editable configuration (see @ref SA_CanCreateEditableConfiguration/@ref SA_CreateEditableConfiguration)
 *   2. Use *Editable* functions to make changes to a configuration.
 *   3. If the configuration is valid and the user wants to:
 *     * Apply the changes:
 *       1. Commit the editable configuration (see @ref SA_CanCommitEditableConfiguration/@ref SA_CommitEditableConfiguration)
 *       2. Clean/delete the editable configuration (see @ref SA_DestroyEditableConfiguration)
 *     * Revert the editable changes & start over:
 *       1. Clean/delete the editable configuration (see @ref SA_DestroyEditableConfiguration)
 *       2. Re-create editable configuration (see @ref SA_CanCreateEditableConfiguration/@ref SA_CreateEditableConfiguration)
 *     * Abandon the editable changes:
 *       1. Clean/delete the editable configuration (see @ref SA_DestroyEditableConfiguration) *
 *
 * @ref storc_config_examples
 * @see storc_configuration_virtual.
 * @{
 ********************************************************************/

/** @name Can Create Editable Configuration
 * @{ */
/** Can an editable config object be created?
 * @post If valid, user may call @ref SA_CreateEditableConfiguration.
 * @param[in] cda  Controller data area.
 * @return See [Can Create Editable Configuration Returns](@ref storc_configuration_editable).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableConfiguration(cda) == kCanCreateEditableConfigOK)
 * {
 *    if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 *    {
 *        fprintf(stderr, "Failed to create editable configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanCreateEditableConfiguration(PCDA cda);
   /** Editable configuration can be created.
    * @allows SA_CreateEditableConfiguration.
    */
   #define kCanCreateEditableConfigOK                       0x01
   /** Cannot create the editable configuration because it already exists.
    * @outputof SA_CanCreateEditableConfiguration.
    * @disallows SA_CreateEditableConfiguration.
    */
   #define kCanCreateEditableConfigEditableConfigExists     0x02
   /** Cannot create the editable configuration because the Controller has inconsistent port settings.
    * @outputof SA_CanCreateEditableConfiguration.
    * @disallows SA_CreateEditableConfiguration.
    */
   #define kCanCreateEditableConfigInconsistentPortSettings 0x03
   /** Cannot create the editable configuration because the Controller's
    * status is not OK (see @ref SA_GetControllerStatusInfo), the encryption
    * configuration for the Controller is locked, or the Controller's encryption
    * self test has failed (see @ref SA_GetControllerEncryptionStatus).
    * @outputof SA_CanCreateEditableConfiguration.
    * @disallows SA_CreateEditableConfiguration.
    */
   #define kCanCreateEditableConfigControllerStatusNotOK    0x04
   /** Cannot create the editable configuration because there is a pending or active controller
    * online firmware activation operation in progress.
    * @see [SA_GetControllerOnlineFirmwareActivationStatus](@ref storc_status_ctrl_online_firmware_activation).
    * @outputof SA_CanCreateEditableConfiguration.
    * @disallows SA_CreateEditableConfiguration.
    */
   #define kCanCreateEditableConfigControllerOFAActive      0x05
/** @} */ /* Can Create Editable Configuration */

/** @name Create Editable Configuration
 * @{ */
/** Create the editable configuration.
 * @pre The user has used @ref SA_CanCreateEditableConfiguration to ensure the operation can be performed.
 * @param[in] cda                           Controller data area.
 * @return See [Create Editable Configuration Returns](@ref storc_configuration_editable).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableConfiguration(cda) == kCanCreateEditableConfigOK)
 * {
 *    if (SA_CreateEditableConfiguration(cda) != kCreateEditableConfigOK)
 *    {
 *        fprintf(stderr, "Failed to create editable configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CreateEditableConfiguration(PCDA cda);
   /** Failed to create the editable configuration.
    * @outputof SA_CreateEditableConfiguration.
    */
   #define kCreateEditableConfigFailedUnknown 0x00
   /** Successfully created the editable configuration.
    * @outputof SA_CreateEditableConfiguration.
    */
   #define kCreateEditableConfigOK 0x01
/** @} */ /* Create Editable Configuration */

/** @name Can Commit Editable Configuration
 * @{ */
/** Can the editable configuration be committed?
 * @post If valid, user may call @ref SA_CommitEditableConfiguration.
 * @param[in] cda Controller data area.
 * @return See [Can Commit Editable Configuration](@ref storc_configuration_editable).
 *
 * __Example__
 * @code
 * if (SA_CanCommitEditableConfiguration(cda) == kCanCommitEditableConfigurationOK)
 * {
 *    if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *    {
 *        fprintf(stderr, "Failed to commit editable configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanCommitEditableConfiguration(PCDA cda);
   /** The configuration is valid and can be committed.
    * @allows SA_CommitEditableConfiguration.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationOK                            1
   /** The configuration cannot be committed because it has not been created yet.
    * @disallows SA_CommitEditableConfiguration.
    * @see SA_CreateEditableConfiguration.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationNoEditableConfigCreated       2
   /** The configuration cannot be committed because it has no changes to commit.
    * @disallows SA_CommitEditableConfiguration.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationNoChangesToCommit             3
   /** The configuration cannot be committed because some/all of its Logical Drive(s) have not been finalized & locked.
    * @disallows SA_CommitEditableConfiguration.
    * @see SA_LockEditableLogicalDrive.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationUnlockedLogicalDrivesExist    4
   /** Changes have been made on the hardware/configuration outside of the editable configuration since it was created.
    * @deprecated This is now an special error that will occur on @ref SA_CommitEditableConfiguration.
    * The client must now delete/recreate a new editable configuration to apply their changes.
    * @disallows SA_CommitEditableConfiguration.
    * @see SA_LockEditableLogicalDrive.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationOutOfSync                     5
   /** The configuration cannot be committed because the encryption password has not been set.
    * @disallows SA_CommitEditableConfiguration.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationEncryptionPasswordNotSet      6
   /** The configuration cannot be committed because the encryption key has not been set.
    * @disallows SA_CommitEditableConfiguration.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationEncryptionKeyNotSet           7
   /** The configuration cannot be committed because it requires a login/encryption password.
    * @see SA_CanCommitEncryptedEditableConfigurationSafe.
    * @disallows SA_CommitEditableConfiguration.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationPasswordRequired              8
   /** The configuration cannot be committed because the spare configuration is in conflict with the spare activation mode settings.
    * This usually occurs when the configuration will result in spare drive(s) assigned to an Array of all RAID 0 logical drives
    * while the spare activation mode will be 'failure'.
    * Must be resolved by either:
    *   -# Finding and removing the spare from the Array(s) in conflict.
    *   -# Add/transform a non-RAID 0 logical drive to the Array(s) in conflict.
    *   -# Change the spare activation mode to 'predictive'.
    * @see SA_GetEditableControllerSAM.
    * @disallows SA_CommitEditableConfiguration.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationInvalidSpareActivationMode    9
   /** The configuration cannot be committed because the configuration contains LU caching logical drives not assigned to any data drive.
    * Delete or assign the orphan logical drive(s).
    * @see storc_configuration_edit_ld_delete
    * @see SA_GetEditableLogicalDriveLUCacheNumber
    * @disallows SA_CommitEditableConfiguration.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationHasOrphanLUCacheLogicalDrive  10
   /** The configuration cannot be committed because the controller has a pending or running online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @disallows SA_CommitEditableConfiguration.
    * @outputof SA_CanCommitEditableConfiguration.
    */
   #define kCanCommitEditableConfigurationControllerOFAActive           11
/** @} */ /* Can Commit Editable Configuration */

/** @name Commit Editable Configuration
 * @{ */
/** Commit the editable configuration to the controller.
 * @attention Will delete the editable configuration.
 * @pre The user has used @ref SA_CanCommitEditableConfiguration to ensure the operation can be performed.
 * @post The editable configuration can no longer be modified/committed.
 * @param[in] cda  Controller data area.
 * @return See [Commit Editable Configuration](@ref storc_configuration_editable).
 *
 * __Example__
 * @code
 * if (SA_CanCommitEditableConfiguration(cda) == kCanCommitEditableConfigurationOK)
 * {
 *    if (SA_CommitEditableConfiguration(cda) != kCommitEditableConfigurationOK)
 *    {
 *        fprintf(stderr, "Failed to commit editable configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CommitEditableConfiguration(PCDA cda);
   /** Failed to commit the editable configuration.
    * @deprecated No longer used.
    * @outputof SA_CommitEditableConfiguration
    */
   #define kCommitEditableConfigurationFailedUnknown            0x00
   /** The editable configuration was successfully committed.
    * @outputof SA_CommitEditableConfiguration
    */
   #define kCommitEditableConfigurationOK                       0x01
   /** Failed to commit the editable configuration due to
    * @ref SA_CanCommitEditableConfiguration failure.
    * @outputof SA_CommitEditableConfiguration
    */
   #define kCommitEditableConfigurationCanCommitFailed          0x02
   /** Failed to commit the editable configuration due to one or more errors.
    * Use @ref SA_GetEditableConfigurationCommitErrors to get additional
    * information about errors found during SA_CommitEditableConfiguration.
    * @outputof SA_CommitEditableConfiguration
    */
   #define kCommitEditableConfigurationFailedAdditionalErrors   0x03
   /**< Commit was canceled due to changes that were made on the hardware/configuration
    * outside of the editable configuration since it was created.
    *
    * No changes were made and the editable configuration is still open.
    */
   #define kCommitEditableConfigurationOutOfSync                0x04
/** @} */ /* Commit Editable Configuration */

/** @name Can Commit Encrypted Editable Configuration
 * @deprecated Use @ref SA_CanCommitEncryptedEditableConfigurationSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this
 *          function (@ref SA_CanCommitEncryptedEditableConfigurationSafe).
 * @{ */
/** Can the encrypted editable configuration be committed?
 * This is used instead of SA_CanCommitEditableConfiguration when the changes require
 * encryption changes (@ref SA_CanCommitEditableConfiguration returns @ref kCanCommitEditableConfigurationPasswordRequired).
 * @see SA_VerifyEncryptionPasswordSafe.
 * @post If valid, user may call @ref SA_CommitEncryptedEditableConfiguration.
 * @param[in] cda       Controller data area.
 * @param[in] user      Encryption user for login (ignored if changes do not require encryption changes).
 * @param[in] password  Null-terminated encryption user password string for login (ignored if changes do not require encryption changes).
 * @return See returns for @ref SA_CanCommitEditableConfiguration.
 * @return Additionally, See [Can Commit Encrypted Editable Configuration](@ref storc_configuration_editable).
 *
 * __Example__
 * @code
 * if (SA_CanCommitEncryptedEditableConfiguration(cda, kControllerEncryptionCryptoOfficer, "MyPassword") == kCanCommitEditableConfigurationOK)
 * {
 *    if (SA_CommitEncryptedEditableConfiguration(cda, kControllerEncryptionCryptoOfficer, "MyPassword") != kCommitEditableConfigurationOK)
 *    {
 *        fprintf(stderr, "Failed to commit editable configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanCommitEncryptedEditableConfiguration(PCDA cda, SA_BYTE user, const char *password);
/** @} */ /* Can Commit Encrypted Editable Configuration */

/** @name Can Commit Encrypted Editable Configuration (Safe)
 * @{ */
/** Can the encrypted editable configuration be committed?
 * This is used instead of SA_CanCommitEditableConfiguration when the changes require
 * encryption changes (@ref SA_CanCommitEditableConfiguration returns @ref kCanCommitEditableConfigurationPasswordRequired).
 * @see SA_VerifyEncryptionPasswordSafe.
 * @post If valid, user may call @ref SA_CommitEncryptedEditableConfigurationSafe.
 * @param[in] cda             Controller data area.
 * @param[in] user            Encryption user for login (ignored if changes do not require encryption changes).
 * @param[in] password        Null-terminated encryption user password string for login (ignored if changes do not require encryption changes).
 * @param[in] password_size   Size of the password string buffer (number of bytes).
 * @return See returns for @ref SA_CanCommitEditableConfiguration.
 * @return Additionally, See [Can Commit Encrypted Editable Configuration](@ref storc_configuration_editable).
 *
 * __Example__
 * @code
 * if (SA_CanCommitEncryptedEditableConfigurationSafe(cda, kControllerEncryptionCryptoOfficer, "MyPassword", sizeof("MyPassword")) == kCanCommitEditableConfigurationOK)
 * {
 *    if (SA_CommitEncryptedEditableConfigurationSafe(cda, kControllerEncryptionCryptoOfficer, "MyPassword", sizeof("MyPassword")) != kCommitEditableConfigurationOK)
 *    {
 *        fprintf(stderr, "Failed to commit editable configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanCommitEncryptedEditableConfigurationSafe(PCDA cda, SA_BYTE user, const char *password, size_t password_size);
   /** The configuration cannot be committed because the given user does not have the required privileges.
    * @see SA_GetEncryptionUserCapabilities.
    * @disallows SA_CommitEncryptedEditableConfiguration.
    * @disallows SA_CommitEncryptedEditableConfigurationSafe.
    * @outputof SA_CanCommitEncryptedEditableConfiguration.
    * @outputof SA_CanCommitEncryptedEditableConfigurationSafe.
    */
   #define kCanCommitEncryptedEditableConfigurationPermissionDenied  0x11
   /** The configuration cannot be committed because the given user/password is invalid.
    * @see SA_VerifyEncryptionPasswordSafe.
    * @disallows SA_CommitEncryptedEditableConfiguration.
    * @disallows SA_CommitEncryptedEditableConfigurationSafe.
    * @outputof SA_CanCommitEncryptedEditableConfiguration.
    * @outputof SA_CanCommitEncryptedEditableConfigurationSafe.
    */
   #define kCanCommitEncryptedEditableConfigurationLoginFailed       0x12
/** @} */ /* Can Commit Encrypted Editable Configuration (Safe) */

/** @name Commit Encrypted Editable Configuration
 * @deprecated Use @ref SA_CommitEncryptedEditableConfigurationSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this
 *          function (@ref SA_CommitEncryptedEditableConfigurationSafe).
 * @{ */
/** Commit the encrypted editable configuration to the controller.
 * This is used instead of SA_CommitEditableConfiguration when the changes require
 * encryption changes (@ref SA_CanCommitEditableConfiguration returns @ref kCanCommitEditableConfigurationPasswordRequired).
 * @attention Will delete the editable configuration.
 * @pre The user has used @ref SA_CanCommitEncryptedEditableConfiguration to ensure the operation can be performed.
 * @post The editable configuration can no longer be modified/committed.
 * @param[in] cda       Controller data area.
 * @param[in] user      [Encryption user](@ref storc_encryption) for login (ignored if changes do not require encryption changes).
 * @param[in] password  Null-terminated encryption user password string for login (ignored if changes do not require encryption changes).
 * @return See returns for @ref SA_CommitEditableConfiguration.
 *
 * __Example__
 * @code
 * if (SA_CanCommitEncryptedEditableConfiguration(cda, kControllerEncryptionCryptoOfficer, "MyPassword") == kCanCommitEditableConfigurationOK)
 * {
 *    if (SA_CommitEncryptedEditableConfiguration(cda, kControllerEncryptionCryptoOfficer, "MyPassword") != kCommitEditableConfigurationOK)
 *    {
 *        fprintf(stderr, "Failed to commit editable configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CommitEncryptedEditableConfiguration(PCDA cda, SA_BYTE user, const char *password);
/** @} */ /* Commit Encrypted Editable Configuration */

/** @name Commit Encrypted Editable Configuration (Safe)
 * @{ */
/** Commit the encrypted editable configuration to the controller.
 * This is used instead of SA_CommitEditableConfiguration when the changes require
 * encryption changes (@ref SA_CanCommitEditableConfiguration returns @ref kCanCommitEditableConfigurationPasswordRequired).
 * @attention Will delete the editable configuration.
 * @pre The user has used @ref SA_CanCommitEncryptedEditableConfigurationSafe to ensure the operation can be performed.
 * @post The editable configuration can no longer be modified/committed.
 * @param[in] cda             Controller data area.
 * @param[in] user            [Encryption user](@ref storc_encryption) for login (ignored if changes do not require encryption changes).
 * @param[in] password        Null-terminated encryption user password string for login (ignored if changes do not require encryption changes).
 * @param[in] password_size   Size of the password string buffer (number of bytes).
 * @return See returns for @ref SA_CommitEditableConfiguration.
 *
 * __Example__
 * @code
 * if (SA_CanCommitEncryptedEditableConfigurationSafe(cda, kControllerEncryptionCryptoOfficer, "MyPassword", sizeof("MyPassword")) == kCanCommitEditableConfigurationOK)
 * {
 *    if (SA_CommitEncryptedEditableConfigurationSafe(cda, kControllerEncryptionCryptoOfficer, "MyPassword", sizeof("MyPassword")) != kCommitEditableConfigurationOK)
 *    {
 *        fprintf(stderr, "Failed to commit editable configuration\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CommitEncryptedEditableConfigurationSafe(PCDA cda, SA_BYTE user, const char *password, size_t password_size);
/** @} */ /* Commit Encrypted Editable Configuration (Safe) */

/** @name Destroy Editable Configuration
 * @{ */
/** Destroy the editable configuration, leaving the controller in the
 * same state as when the editable config was created.
 * @attention Will delete the editable configuration.
 * @post The editable configuration can no longer be modified/committed.
 * @param[in] cda  Controller data area.
 */
void SA_DestroyEditableConfiguration(PCDA cda);
/** @} */ /* Destroy Editable Configuration */

/** @name Editable Configuration Commit Errors
 * @{ */
#define kEditableConfigurationCommitErrorDeleteLogicalDrive               0x0000000000000001ULL /**< Failed to delete one or more logical drives. */
#define kEditableConfigurationCommitErrorSettingControllerParams          0x0000000000000002ULL /**< Failed to set one or more controller parameters. */
#define kEditableConfigurationCommitErrorSpareDriveChanges                0x0000000000000004ULL /**< Failed to set spare drive changes. */
#define kEditableConfigurationCommitErrorLogicalDriveSettings             0x0000000000000008ULL /**< Failed to set logical drive settings. */
#define kEditableConfigurationCommitErrorClearEncryption                  0x0000000000000010ULL /**< Failed to clear existing local/remote encryption. */
#define kEditableConfigurationCommitErrorEncryptionSettings               0x0000000000000020ULL /**< Failed to set one or more encryption settings. */
#define kEditableConfigurationCommitErrorCreatePlainTextLogicalDrives     0x0000000000000040ULL /**< Failed to set encryption key. */
#define kEditableConfigurationCommitErrorRekeyToPlainText                 0x0000000000000080ULL /**< Failed to rekey one or more logical drives as plain text. */
#define kEditableConfigurationCommitErrorCreateEncryptedLogicalDrives     0x0000000000000100ULL /**< Failed to create one or more encrypted logical drives. */
#define kEditableConfigurationCommitErrorEncodeLogicalDrives              0x0000000000000200ULL /**< Failed to encode one or more existing logical drives. */
#define kEditableConfigurationCommitErrorVolatileKeys                     0x0000000000000400ULL /**< Failed to set volatile keys for one or more existing logical drives. */
#define kEditableConfigurationCommitErrorEncryptionDisable                0x0000000000000800ULL /**< Failed to disable encryption. */
#define kEditableConfigurationCommitErrorLogicalDriveLabels               0x0000000000001000ULL /**< Failed to set label on one or more logical drives. */
#define kEditableConfigurationCommitErrorControllerSurvivalMode           0x0000000000002000ULL /**< Failed to set controller survival mode. */
#define kEditableConfigurationCommitErrorControllerSurfaceScan            0x0000000000004000ULL /**< Failed to set controller surface scan. */
#define kEditableConfigurationCommitErrorControllerCacheConfig            0x0000000000008000ULL /**< Failed to set controller cache configuration. */
#define kEditableConfigurationCommitErrorLogicalDriveTransformation       0x0000000000010000ULL /**< Failed to transform one or more logical drives. */
#define kEditableConfigurationCommitErrorLUCacheLogicalDriveChanges       0x0000000000020000ULL /**< Failed to configure LU cache logical drive changes. */
#define kEditableConfigurationCommitErrorTransformEncodeLogicalDrives     0x0000000000040000ULL /**< Failed to encrypt one or more logical drives. */
#define kEditableConfigurationCommitErrorDestructiveEncodeLogicalDrives   0x0000000000080000ULL /**< Failed to encrypt one or more logical drives without preserving data. */
#define kEditableConfigurationCommitErrorSpareDriveSEDQualFailed          0x0000000000100000ULL /**< Failed to set spare drive due to SED qualification failure. */

/** Obtain additional errors found during committing an editable configuration.
 * @attention Editable configuration commit errors will be cleared when
 * @ref SA_CreateEditableConfiguration is invoked.
 * @param[in] cda        Controller data area.
 * @return Editable configuration commit errors.
 */
SA_UINT64 SA_GetEditableConfigurationCommitErrors(PCDA cda);
/** @} */ /* Editable Configuration Commit Errors */

/** @} */ /* storc_configuration_editable */

/***********************************************************//**
* @defgroup storc_configuration_edit_ctrl Editable Controller
* @brief Modify controller settings.
* @details
* @see storc_properties_ctrl
* @{
**************************************************************/

/** @defgroup storc_configuration_edit_ctrl_boot_vol Controller Boot Volume Configuration
 * @brief Modify the controller's legacy BIOS boot logical and physical volumes.
 * @details
 * @see storc_properties_ctrl_boot_volume
 * @{ */

/** @name Can Clear Editable Boot Volume
 * @{ */
/** Can the [editable] controller's boot volume be cleared?
 *
 * @post If valid, user may call @ref SA_ClearEditableControllerBootVolume.
 * @param[in] cda                  Controller data area.
 * @param[in] precedence           Specifies the primary or secondary boot volume ID to set (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @return See [Can Clear Editable Boot Volume Returns](@ref storc_configuration_edit_ctrl_boot_vol)
 *
 * __Example__
 * @code
 * if (SA_CanClearEditableControllerBootVolume(cda, kBootableVolumePrecedencePrimary) == kCanClearEditableControllerBootVolumeAllowed)
 * {
 *    if (SA_ClearEditableControllerBootVolume(cda, kBootableVolumePrecedencePrimary) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to clear primary boot drive\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanClearEditableControllerBootVolume(PCDA cda, SA_BYTE precedence);
   /** Clear controller boot volume is allowed.
    * @outputof SA_CanClearEditableControllerBootVolume.
    * @allows SA_ClearEditableControllerBootVolume.
    */
   #define kCanClearEditableControllerBootVolumeAllowed                  1
   /** No editable configuration created.
    * @outputof SA_CanClearEditableControllerBootVolume.
    * @disallows SA_ClearEditableControllerBootVolume.
    */
   #define kCanClearEditableControllerBootVolumeNoEditableConfigCreated  2
   /** Invalid boot volume precedence.
    * @outputof SA_CanClearEditableControllerBootVolume.
    * @disallows SA_ClearEditableControllerBootVolume.
    */
   #define kCanClearEditableControllerBootVolumeInvalidPrecedence        3
   /** Requested boot volume not set.
    * @outputof SA_CanClearEditableControllerBootVolume.
    * @disallows SA_ClearEditableControllerBootVolume.
    */
   #define kCanClearEditableControllerBootVolumeNotSet                   4
   /** The controller does not support setting/clearing the requested boot volume.
    * @outputof SA_CanClearEditableControllerBootVolume.
    * @disallows SA_ClearEditableControllerBootVolume.
    */
   #define kCanClearEditableControllerBootVolumeOperationUnsupported     5
   /** Clearing boot volume not allowed at current stage of configuration.
    * @outputof SA_CanClearEditableControllerBootVolume.
    * @disallows SA_ClearEditableControllerBootVolume.
    */
   #define kCanClearEditableControllerBootVolumeNotAllowed               6
/** @} */ /* Can Change Editable Boot Volume */

/** @name Clear Editable Boot Volume
 * @{ */
/** Clear the [editable] controller's boot volume.
 *
 * @pre The user has used @ref SA_CanClearEditableControllerBootVolume to ensure the operation can be performed.
 * @param[in] cda                  Controller data area.
 * @param[in] precedence           Specifies the primary or secondary boot volume ID to set (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @return @ref kTrue if boot volume cleared, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanClearEditableControllerBootVolume(cda, kBootableVolumePrecedencePrimary) == kCanClearEditableControllerBootVolumeAllowed)
 * {
 *    if (SA_ClearEditableControllerBootVolume(cda, kBootableVolumePrecedencePrimary) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to clear primary boot drive\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ClearEditableControllerBootVolume(PCDA cda, SA_BYTE precedence);
/** @} */ /* Clear Editable Boot Volume */

/** @name Get Editable Boot Volume
 * @{ */
/** Return current value of editable controller's bootable volume.
 * @param[in] cda                  Controller data area.
 * @param[in] precedence           Specifies the primary or secondary boot volume ID to get (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @param[in] volume_type          Type to specify if target drive is a logical or physical drive (see [Bootable Volume Drive Type](@ref storc_properties_ctrl_boot_volume)).
 * @return Device number of requested boot volume or @ref kInvalidLDNumber/@ref kInvalidPDNumber.
 */
SA_WORD SA_GetEditableControllerBootVolume(PCDA cda, SA_BYTE precedence, SA_BYTE volume_type);
/** @} */ /* Get Editable Boot Volume */

/** @name Can Change Editable Boot Volume
 * @{ */
/** Can the [editable] controller's boot volume be changed?
 *
 * @post If valid, user may call @ref SA_ChangeEditableControllerBootVolume.
 * @param[in] cda                  Controller data area.
 * @param[in] precedence           Specifies the primary or secondary boot volume ID to set (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @param[in] volume_type          Type to specify if target drive is a logical or physical drive (see [Bootable Volume Drive Type](@ref storc_properties_ctrl_boot_volume)).
 * @param[in] editable_dev_number  Target drive number of LD or PD.
 * @return See [Can Change Editable Boot Volume Returns](@ref storc_configuration_edit_ctrl_boot_vol)
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerBootVolume(cda, kBootableVolumePrecedencePrimary, kBootableVolumeDriveTypeLogical, 1) == kCanChangeEditableControllerBootVolumeAllowed)
 * {
 *    if (SA_ChangeEditableControllerBootVolume(cda, kBootableVolumePrecedencePrimary, kBootableVolumeDriveTypeLogical, 1) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set primary bootable logical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerBootVolume(PCDA cda, SA_BYTE precedence, SA_BYTE volume_type, SA_WORD editable_dev_number);
   /** Change controller boot volume is allowed.
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @allows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeAllowed                    1
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @disallows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeNoEditableConfigCreated    2
   /** Invalid boot volume precedence.
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @disallows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeInvalidPrecedence          3
   /** Invalid boot volume type.
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @disallows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeInvalidBootVolumeType      4
   /** Invalid boot volume number.
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @disallows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeInvalidBootVolumeNumber    5
   /** The controller does not support setting the requested boot volume.
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @disallows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeOperationUnsupported       6
   /** The requested physical drive is not available as a boot volume
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @disallows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeInvalidPhysicalBootVolume  7
   /** The requested logical drive is not available as a boot volume
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @disallows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeInvalidLogicalBootVolume   8
   /** The requested remote volume is not available as a boot volume
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @disallows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeInvalidRemoteBootVolume    9
   /** The requested bootable volume is already set.
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @disallows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeAlreadySet                 10
   /** Modifying boot volume not allowed at current stage of configuration.
    * @outputof SA_CanChangeEditableControllerBootVolume.
    * @disallows SA_ChangeEditableControllerBootVolume.
    */
   #define kCanChangeEditableControllerBootVolumeNotAllowed                 11
/** @} */ /* Can Change Editable Boot Volume */

/** @name Change Editable Boot Volume
 * @{ */
/** Change the [editable] controller's boot volume.
 *
 * @pre The user has used @ref SA_CanChangeEditableControllerBootVolume to ensure the operation can be performed.
 * @param[in] cda                  Controller data area.
 * @param[in] precedence           Specifies the primary or secondary boot volume ID to set (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @param[in] volume_type          Type to specify if target drive is a logical or physical drive (see [Bootable Volume Drive Type](@ref storc_properties_ctrl_boot_volume)).
 * @param[in] editable_dev_number  Target drive number of LD or PD.
 * @return @ref kTrue if boot volume set, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerBootVolume(cda, kBootableVolumePrecedencePrimary, kBootableVolumeDriveTypeLogical, 1) == kCanChangeEditableControllerBootVolumeAllowed)
 * {
 *    if (SA_ChangeEditableControllerBootVolume(cda, kBootableVolumePrecedencePrimary, kBootableVolumeDriveTypeLogical, 1) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set primary bootable logical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerBootVolume(PCDA cda, SA_BYTE precedence, SA_BYTE volume_type, SA_WORD editable_dev_number);
/** @} */ /* Change Editable Boot Volume */

/** @} */ /* storc_configuration_edit_ctrl_boot_vol */

/** @defgroup storc_configuration_edit_ctrl_sam Controller Spare Activation Mode Configuration
 * @brief Modify the controller's spare activation mode.
 * @details
 * @see storc_features_ctrl_predictive_spare_rebuild
 * @see storc_properties_spare_activation_mode
 * @{ */

/** @name Editable Spare Activation Mode
 * @{ */
/** Return current value of editable controller's spare activation mode.
 * @return [Spare Activation Modes](@ref storc_properties_spare_activation_mode)
 */
SA_BYTE SA_GetEditableControllerSAM(PCDA cda);
/** @} */ /* Editable Spare Activation Mode */

/** @name Can Change Editable Spare Activation Mode
 * @{ */
/** Can the [editable] controller's Spare Activation mode be changed?
 *
 * @post If valid, user may call @ref SA_ChangeEditableControllerSAM.
 * @param[in] cda                             Controller data area.
 * @param[in] intended_spare_activation_mode  Desired Controller Spare Activation mode (see [Spare Activation Modes](@ref storc_properties_spare_activation_mode)).
 * @return Bitmap showing if the Spare Activation mode can be changed and reason(s) (see [Can Change Spare Activation Mode Returns](@ref storc_configuration_edit_ctrl_sam)).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerSAM(cda, kSpareActivationModeFailure) == kCanChangeEditableControllerSAMOperationAllowed)
 * {
 *    if (SA_ChangeEditableControllerSAM(cda, kSpareActivationModeFailure) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set controller spare activation mode to Failure Spare Activation\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerSAM(PCDA cda, SA_BYTE intended_spare_activation_mode);
   /** Change spare activation mode is allowed.
    * @outputof SA_CanChangeEditableControllerSAM.
    * @allows SA_ChangeEditableControllerSAM.
    */
   #define kCanChangeEditableControllerSAMOperationAllowed                          0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerSAM.
    * @disallows SA_ChangeEditableControllerSAM.
    */
   #define kCanChangeEditableControllerSAMNoEditableConfigCreated                   0x02
   /** The intended mode is not supported by this controller.
    * @outputof SA_CanChangeEditableControllerSAM.
    * @disallows SA_ChangeEditableControllerSAM.
    */
   #define kCanChangeEditableControllerSAMUnsupportedMode                           0x03
   /** Array is transforming. Change activation mode is not allowed at this time.
    * @outputof SA_CanChangeEditableControllerSAM.
    * @disallows SA_ChangeEditableControllerSAM.
    */
   #define kCanChangeEditableControllerSAMArrayIsTransforming                       0x04
   /** Predictive mode and an array has active spare with Raid 0. The change will result data loss.
    * @outputof SA_CanChangeEditableControllerSAM.
    * @disallows SA_ChangeEditableControllerSAM.
    */
   #define kCanChangeEditableControllerSAMArrayHasActiveSpare                       0x05
   /** Cannot set desired spare activation mode mode while a RAID 0 logical drive has an auto-replace spare.
    * @outputof SA_CanChangeEditableControllerSAM.
    * @disallows SA_ChangeEditableControllerSAM.
    */
   #define kCanChangeEditableControllerSAMArrayHasAutoReplaceSpare                  0x06
   /** Controller is already in the selected spare activation mode.
    * @outputof SA_CanChangeEditableControllerSAM.
    * @disallows SA_ChangeEditableControllerSAM.
    */
    #define kCanChangeEditableControllerSAMSelectedModeActive                       0x07
   /** Controller has no configured editable logical drives.
    * @outputof SA_CanChangeEditableControllerSAM.
    * @disallows SA_ChangeEditableControllerSAM.
    */
    #define kCanChangeEditableControllerSAMNoLogicalDrives                          0x08
   /** Predictive mode and an editable array contains a predictive failure drive.
    * @outputof SA_CanChangeEditableControllerSAM.
    * @disallows SA_ChangeEditableControllerSAM.
    */
    #define kCanChangeEditableControllerSAMEditableArrayHasPredictiveFailureDrive   0x09
   /** An editable logical drive is offline due to a missing key.
    * @see SA_GetLogicalDriveStatusInfo.
    * @outputof SA_CanChangeEditableControllerSAM.
    * @disallows SA_ChangeEditableControllerSAM.
    */
    #define kCanChangeEditableControllerSAMLogicalDriveMissingKey                   0x0A

/** @} */ /* Can Change Editable Spare Activation Mode */

/** @name Change Editable Spare Activation Mode
 * @{ */
/** Change the [editable] controller's spare activation mode to the requested value.
 *
 * @pre The user has used @ref SA_CanChangeEditableControllerSAM to ensure the operation can be performed.
 * @param[in] cda                             Controller data area.
 * @param[in] intended_spare_activation_mode  Desired Spare Activation mode (see [Spare Activation Modes](@ref storc_properties_spare_activation_mode)).
 * @return @ref kTrue if spare activation mode was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerSAM(cda, kSpareActivationModeFailure) == kCanChangeEditableControllerSAMOperationAllowed)
 * {
 *    if (SA_ChangeEditableControllerSAM(cda, kSpareActivationModeFailure) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set spare activation mode to Failure Spare Activation\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerSAM(PCDA cda, SA_BYTE intended_spare_activation_mode);
/** @} */ /* Change Editable Spare Activation Mode */

/** @} */ /* storc_configuration_edit_ctrl_sam */

/** @defgroup storc_configuration_edit_ctrl_drive_write_cache Controller Drive Write Cache Configuration
 * @brief Modify the controller's drive write cache setting.
 * @details
 * @see storc_features_ctrl_drive_write_cache
 * @see storc_properties_ctrl_drive_write_cache
 * @{ */

/** Use to set the drive write cache for any/all drive types.
 * Input of @ref SA_CanChangeEditableControllerDriveWriteCachePolicy
 * Input of @ref SA_ChangeEditableControllerDriveWriteCachePolicy
 */
#define kDriveWriteCacheDriveTypeAll 0xFF
/** Use to set the drive write cache for any/all supported drive usage types.
 * Input of @ref SA_CanChangeEditableControllerDriveWriteCachePolicy
 * Input of @ref SA_ChangeEditableControllerDriveWriteCachePolicy
 */
#define kDriveWriteCacheUsageTypeAll 0xFF

/** @name Get Editable Drive Write Cache Policy
 * @{ */
/** Return current value of editable controller's drive write cache mode.
 * @deprecated Use @ref SA_GetEditableControllerDriveWriteCachePolicy instead.
 * @param[in] cda  Controller data area.
 * @return [Drive Write Cache Policies](@ref storc_properties_ctrl_drive_write_cache)
 */
SA_BYTE SA_GetEditableControllerDriveWriteCacheMode(PCDA cda);

/** Return current value of editable controller's drive write cache policy.
 * @param[in] cda  Controller data area.
 * @param[in] drive_type [Drive write cache drive type](@ref storc_properties_ctrl_drive_write_cache).
 * @param[in] usage_type [Drive write cache usage type](@ref storc_properties_ctrl_drive_write_cache).
 * @return [Drive Write Cache Policies](@ref storc_properties_ctrl_drive_write_cache)
 */
SA_BYTE SA_GetEditableControllerDriveWriteCachePolicy(PCDA cda, SA_BYTE drive_type, SA_BYTE usage_type);
/** @} */ /* Get Editable Drive Write Cache Policy */

/** @name Can Change Editable Drive Write Cache Policy
 * @{ */
/** Check [editable] controller's support for modifying the physical drive write cache.
 * @deprecated Use @ref SA_CanChangeEditableControllerDriveWriteCachePolicy instead.
 * @param[in] cda                  Controller data area.
 * @param[in] pd_write_cache_mode  Desired physical drive write cache mode (see [Physical Drive Write Cache Modes](@ref storc_properties_ctrl_drive_write_cache).
 * @return See [Can Change Editable Drive Write Cache Mode Returns](@ref storc_configuration_edit_ctrl_drive_write_cache).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerDriveWriteCacheMode(cda, kControllerDriveWriteCacheEnabled) == kCanChangeEditableControllerDriveWriteCacheModeAllowed)
 * {
 *    if (SA_ChangeEditableControllerDriveWriteCacheMode(cda, kControllerDriveWriteCacheEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to enable Drive Write Cache\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerDriveWriteCacheMode(PCDA cda, SA_BYTE pd_write_cache_mode);
   /** Change drive write cache mode allowed.
    * @outputof SA_CanChangeEditableControllerDriveWriteCacheMode.
    * @allows SA_ChangeEditableControllerDriveWriteCacheMode.
    */
   #define kCanChangeEditableControllerDriveWriteCacheModeAllowed                 0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerDriveWriteCacheMode.
    * @disallows SA_ChangeEditableControllerDriveWriteCacheMode.
    */
   #define kCanChangeEditableControllerDriveWriteCacheModeNoEditableConfigCreated 0x02
   /** Change drive write cache mode not supported by the controller.
    * @outputof SA_CanChangeEditableControllerDriveWriteCacheMode.
    * @disallows SA_ChangeEditableControllerDriveWriteCacheMode.
    */
   #define kCanChangeEditableControllerDriveWriteCacheModeNotSupported            0x03
   /** Drive write cache already in desired mode.
    * @outputof SA_CanChangeEditableControllerDriveWriteCacheMode.
    * @disallows SA_ChangeEditableControllerDriveWriteCacheMode.
    */
   #define kCanChangeEditableControllerDriveWriteCacheModeNoChange                0x04
   /** Invalid mode parameter.
    * @outputof SA_CanChangeEditableControllerDriveWriteCacheMode.
    * @disallows SA_ChangeEditableControllerDriveWriteCacheMode.
    */
   #define kCanChangeEditableControllerDriveWriteCacheModeInvalidMode             0x05
   /** Controller has no configured editable logical drives.
    * @outputof SA_CanChangeEditableControllerDriveWriteCacheMode.
    * @disallows SA_ChangeEditableControllerDriveWriteCacheMode.
    */
   #define kCanChangeEditableControllerDriveWriteCacheModeNoLogicalDrives         0x06

/** Check [editable] controller's support for modifying the physical drive write cache.
 * @attention Currently, drive_type can only be @ref kDriveWriteCacheDriveTypeAll.
 * @param[in] cda        Controller data area.
 * @param[in] drive_type [Drive write cache drive type](@ref storc_properties_ctrl_drive_write_cache) or @ref kDriveWriteCacheDriveTypeAll.
 * @param[in] usage_type [Drive write cache usage type](@ref storc_properties_ctrl_drive_write_cache) or @ref kDriveWriteCacheUsageTypeAll.
 * @param[in] drive_write_cache_policy  Desired physical drive write cache mode (see [Physical Drive Write Cache Policies](@ref storc_properties_ctrl_drive_write_cache).
 * @post If valid, user may call @ref SA_ChangeEditableControllerDriveWriteCachePolicy.
 * @return See [Can Set Editable Drive Write Cache Policy Returns](@ref storc_configuration_edit_ctrl_drive_write_cache).
 */
SA_BYTE SA_CanChangeEditableControllerDriveWriteCachePolicy(PCDA cda, SA_BYTE drive_type, SA_BYTE usage_type, SA_BYTE drive_write_cache_policy);
   /** Change drive write cache mode allowed.
    * @outputof SA_CanChangeEditableControllerDriveWriteCachePolicy.
    * @allows SA_ChangeEditableControllerDriveWriteCachePolicy.
    */
   #define kCanChangeEditableControllerDriveWriteCachePolicyAllowed                  1
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerDriveWriteCachePolicy.
    * @disallows SA_ChangeEditableControllerDriveWriteCachePolicy.
    */
   #define kCanChangeEditableControllerDriveWriteCachePolicyNoEditableConfigCreated  2
   /** Change drive write cache mode not supported by the controller.
    * @outputof SA_CanChangeEditableControllerDriveWriteCachePolicy.
    * @disallows SA_ChangeEditableControllerDriveWriteCachePolicy.
    */
   #define kCanChangeEditableControllerDriveWriteCachePolicyNotSupported             3
   /** Drive write cache already in desired mode.
    * @deprecated No longer used.
    * @outputof SA_CanChangeEditableControllerDriveWriteCachePolicy.
    * @disallows SA_ChangeEditableControllerDriveWriteCachePolicy.
    */
   #define kCanChangeEditableControllerDriveWriteCachePolicyNoChange                 4
   /** Invalid mode parameter.
    * @outputof SA_CanChangeEditableControllerDriveWriteCachePolicy.
    * @disallows SA_ChangeEditableControllerDriveWriteCachePolicy.
    */
   #define kCanChangeEditableControllerDriveWriteCachePolicyInvalidPolicy            5
   /** Controller has no configured editable logical drives.
    * @outputof SA_CanChangeEditableControllerDriveWriteCachePolicy.
    * @disallows SA_ChangeEditableControllerDriveWriteCachePolicy.
    */
   #define kCanChangeEditableControllerDriveWriteCachePolicyNoLogicalDrives          6
   /** Invalid/unknown value for drive_type parameter.
    * @outputof SA_CanChangeEditableControllerDriveWriteCachePolicy.
    * @disallows SA_ChangeEditableControllerDriveWriteCachePolicy.
    */
   #define kCanChangeEditableControllerDriveWriteCachePolicyInvalidDriveType         7
   /** Invalid/unknown value for usage_type parameter.
    * @outputof SA_CanChangeEditableControllerDriveWriteCachePolicy.
    * @disallows SA_ChangeEditableControllerDriveWriteCachePolicy.
    */
   #define kCanChangeEditableControllerDriveWriteCachePolicyInvalidUsageType         8
   /** The drive type is not supported by the controller.
    * @deprecated No longer used.
    * @outputof SA_CanChangeEditableControllerDriveWriteCachePolicy.
    * @disallows SA_ChangeEditableControllerDriveWriteCachePolicy.
    */
   #define kCanChangeEditableControllerDriveWriteCachePolicyUnsupportedDriveType     9
   /** The usage type is not supported by the controller.
    * @outputof SA_CanChangeEditableControllerDriveWriteCachePolicy.
    * @disallows SA_ChangeEditableControllerDriveWriteCachePolicy.
    */
   #define kCanChangeEditableControllerDriveWriteCachePolicyUnsupportedUsageType     10
/** @} */ /* Can Change Editable Drive Write Cache Policy */

/** @name Change Editable Drive Write Cache Mode
 * @{ */
/** Change the [editable] controller's physical drive write cache mode.
 * @deprecated Use @ref SA_ChangeEditableControllerDriveWriteCachePolicy instead.
 * @param[in] cda                  Controller data area.
 * @param[in] pd_write_cache_mode  Desired physical drive write cache mode.
 * @return @ref kTrue if drive write cache mode was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerDriveWriteCacheMode(cda, kControllerDriveWriteCacheEnabled) == kCanChangeEditableControllerDriveWriteCacheModeAllowed)
 * {
 *    if (SA_ChangeEditableControllerDriveWriteCacheMode(cda, kControllerDriveWriteCacheEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to enable Drive Write Cache\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerDriveWriteCacheMode(PCDA cda, SA_BYTE pd_write_cache_mode);

/** Change the [editable] controller's physical drive write cache policy.
 * @pre The user has used @ref SA_CanChangeEditableControllerDriveWriteCachePolicy to ensure the operation can be performed.
 * @param[in] cda        Controller data area.
 * @param[in] drive_type [Drive write cache drive type](@ref storc_properties_ctrl_drive_write_cache) or @ref kDriveWriteCacheDriveTypeAll.
 * @param[in] usage_type [Drive write cache usage type](@ref storc_properties_ctrl_drive_write_cache) or @ref kDriveWriteCacheUsageTypeAll.
 * @param[in] drive_write_cache_policy  Desired physical drive write cache mode (see [Physical Drive Write Cache Policies](@ref storc_properties_ctrl_drive_write_cache).
 * @return @ref kTrue if drive write cache policy was changed, @ref kFalse otherwise.
 */
SA_BOOL SA_ChangeEditableControllerDriveWriteCachePolicy(PCDA cda, SA_BYTE drive_type, SA_BYTE usage_type, SA_BYTE drive_write_cache_policy);
/** @} */ /* Change Editable Drive Write Cache Policy */

/** @} */ /* storc_configuration_edit_ctrl_drive_write_cache */

/** @defgroup storc_configuration_edit_ctrl_rebuild_priority Controller Rebuild Priority Configuration
 * @brief Modify the controller's rebuild priority.
 * @details
 * @see storc_properties_ctrl_rebuild_priority
 * @{ */

/** @name Get Editable Rebuild Priority
 * @{ */
/** Return current value of editable controller's rebuild priority.
 * @return [Controller Rebuild Priorities](@ref storc_properties_ctrl_rebuild_priority)
 */
SA_BYTE SA_GetEditableControllerRebuildPriority(PCDA cda);
/** @} */ /* Get Editable Rebuild Priority */

/** @name Can Change Editable Rebuild Priority
 * @{ */
/** Check [editable] controller's support for modifying the rebuild priority.
 * @param[in] cda               Controller data area.
 * @param[in] rebuild_priority  Desired rebuild priority (see [Controller Rebuild Priorities](@ref storc_properties_ctrl_rebuild_priority).
 * @return See [Can Change Editable Rebuild Priority Returns](@ref storc_configuration_edit_ctrl_rebuild_priority).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerRebuildPriority(cda, kRebuildPriorityMedium) == kCanChangeEditableControllerRebuildPriorityAllowed)
 * {
 *    if (SA_ChangeEditableControllerRebuildPriority(cda, kRebuildPriorityMedium) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set medium rebuild priority\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerRebuildPriority(PCDA cda, SA_BYTE rebuild_priority);
   /** Change rebuild priority modification allowed.
    * @outputof SA_CanChangeEditableControllerRebuildPriority.
    * @allows SA_ChangeEditableControllerRebuildPriority.
    */
   #define kCanChangeEditableControllerRebuildPriorityAllowed                 0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerRebuildPriority.
    * @disallows SA_ChangeEditableControllerRebuildPriority.
    */
   #define kCanChangeEditableControllerRebuildPriorityNoEditableConfigCreated 0x02
   /** Invalid rebuild priority.
    * @outputof SA_CanChangeEditableControllerRebuildPriority.
    * @disallows SA_ChangeEditableControllerRebuildPriority.
    */
   #define kCanChangeEditableControllerRebuildPriorityInvalidValue            0x03
   /** Controller does not support rapid rebuilding.
    * @outputof SA_CanChangeEditableControllerRebuildPriority.
    * @disallows SA_ChangeEditableControllerRebuildPriority.
    */
   #define kCanChangeEditableControllerRebuildPriorityRapidUnsupported        0x04
   /** Requested rebuild priority already set.
    * @outputof SA_CanChangeEditableControllerRebuildPriority.
    * @disallows SA_ChangeEditableControllerRebuildPriority.
    */
   #define kCanChangeEditableControllerRebuildPriorityNoChange                0x05
   /** Controller has no configured editable logical drives.
    * @outputof SA_CanChangeEditableControllerRebuildPriority.
    * @disallows SA_ChangeEditableControllerRebuildPriority.
    */
   #define kCanChangeEditableControllerRebuildPriorityNoLogicalDrives         0x06
/** @} */ /* Can Change Editable Rebuild Priority */

/** @name Change Editable Rebuild Priority
 * @{ */
/** Change the [editable] controller's rebuild priority.
 * @param[in] cda               Controller data area.
 * @param[in] rebuild_priority  Desired rebuild priority (see [Controller Rebuild Priorities](@ref storc_properties_ctrl_rebuild_priority).
 * @return @ref kTrue if rebuild priority was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerRebuildPriority(cda, kRebuildPriorityMedium) == kCanChangeEditableControllerRebuildPriorityAllowed)
 * {
 *    if (SA_ChangeEditableControllerRebuildPriority(cda, kRebuildPriorityMedium) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set medium rebuild priority\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerRebuildPriority(PCDA cda, SA_BYTE rebuild_priority);
/** @} */ /* Change Editable Rebuild Priority */

/** @} */ /* storc_configuration_edit_ctrl_rebuild_priority */

/** @defgroup storc_configuration_edit_ctrl_expand_priority Controller Expand Priority Configuration
 * @brief Modify the controller's expand priority.
 * @details
 * @see storc_properties_ctrl_expand_priority
 * @{ */

/** @name Get Editable Expand Priority
 * @{ */
/** Return current value of editable controller's expand priority.
 * @return [Controller Expand Priorities](@ref storc_properties_ctrl_expand_priority)
 */
SA_BYTE SA_GetEditableControllerExpandPriority(PCDA cda);
/** @} */ /* Get Editable Expand Priority */

/** @name Can Change Editable Expand Priority
 * @{ */
/** Check [editable] controller's support for modifying the expand priority.
 * @param[in] cda               Controller data area.
 * @param[in] expand_priority  Desired expand priority (see [Controller Expand Priorities](@ref storc_properties_ctrl_expand_priority).
 * @return See [Can Change Editable Expand Priority Returns](@ref storc_configuration_edit_ctrl_expand_priority).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerExpandPriority(cda, kExpandPriorityMedium) == kCanChangeEditableControllerExpandPriorityAllowed)
 * {
 *    if (SA_ChangeEditableControllerExpandPriority(cda, kExpandPriorityMedium) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set medium expand priority\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerExpandPriority(PCDA cda, SA_BYTE expand_priority);
   /** Change drive write cache mode allowed.
    * @outputof SA_CanChangeEditableControllerExpandPriority.
    * @allows SA_ChangeEditableControllerExpandPriority.
    */
   #define kCanChangeEditableControllerExpandPriorityAllowed                  1
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerExpandPriority.
    * @disallows SA_ChangeEditableControllerExpandPriority.
    */
   #define kCanChangeEditableControllerExpandPriorityNoEditableConfigCreated  2
   /** Invalid expand priority.
    * @outputof SA_CanChangeEditableControllerExpandPriority.
    * @disallows SA_ChangeEditableControllerExpandPriority.
    */
   #define kCanChangeEditableControllerExpandPriorityInvalidValue             3
   /** Controller has no configured editable logical drives.
    * @outputof SA_CanChangeEditableControllerExpandPriority.
    * @disallows SA_ChangeEditableControllerExpandPriority.
    */
   #define kCanChangeEditableControllerExpandPriorityNoLogicalDrives          4
   /** Requested expand priority already set.
    * @outputof SA_CanChangeEditableControllerExpandPriority.
    * @disallows SA_ChangeEditableControllerExpandPriority.
    */
   #define kCanChangeEditableControllerExpandPriorityNoChange                 5
   /** Expand priority is not supported on the device.
    * @outputof SA_CanChangeEditableControllerExpandPriority.
    * @disallows SA_ChangeEditableControllerExpandPriority.
    */
   #define kCanChangeEditableControllerExpandPriorityUnsupported              6
/** @} */ /* Can Change Editable Expand Priority */

/** @name Change Editable Expand Priority
 * @{ */
/** Change the [editable] controller's expand priority.
 * @param[in] cda              Controller data area.
 * @param[in] expand_priority  Desired expand priority (see [Controller Expand Priorities](@ref storc_properties_ctrl_expand_priority).
 * @return @ref kTrue if expand priority was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerExpandPriority(cda, kExpandPriorityMedium) == kCanChangeEditableControllerExpandPriorityAllowed)
 * {
 *    if (SA_ChangeEditableControllerExpandPriority(cda, kExpandPriorityMedium) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set medium expand priority\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerExpandPriority(PCDA cda, SA_BYTE expand_priority);
/** @} */ /* Change Editable Expand Priority */

/** @} */ /* storc_configuration_edit_ctrl_expand_priority */

/** @defgroup storc_configuration_edit_ctrl_ssap Controller Surface Scan Analysis Priority Configuration
 * @brief Modify the controller's surface analysis priority.
 * @details
 * @see storc_configuration_edit_ctrl_pssc
 * @see storc_properties_ctrl_surface_analysis_priority
 * @{ */

/** @name Get Editable Surface Scan Analysis Priority
 * @{ */
/** Return current value of editable controller's surface scan analysis priority.
 * @return [Helper Functions for Surface Analysis Priority](@ref storc_properties_ctrl_surface_analysis_priority)
 */
SA_WORD SA_GetEditableControllerSSAPriority(PCDA cda);
/** @} */ /* Get Editable Surface Scan Analysis Priority */

/** @name Can Change Editable Surface Scan Analysis Priority
 * @{ */
/** Check [editable] controller's support for modifying the surface scan analysis priority.
 * @param[in] cda                             Controller data area.
 * @param[in] surface_scan_analysis_priority  Desired surface scan analysis priority (see [Controller Surface Analysis Priority](@ref storc_properties_ctrl_surface_analysis_priority).
 * @return See [Can Change Editable Surface Scan Analysis Priority Returns](@ref storc_configuration_edit_ctrl_ssap).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerSSAPriority(cda, 0) == kCanChangeEditableSurfaceScanPriorityAllowed)
 * {
 *    if (SA_ChangeEditableControllerSSAPriority(cda, 0) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Surface Scan Analysis Priority\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerSSAPriority(PCDA cda, SA_WORD surface_scan_analysis_priority);
   /** Change surface scan priority analysis allowed.
    * @outputof SA_CanChangeEditableControllerSSAPriority.
    * @allows SA_ChangeEditableControllerSSAPriority.
    */
   #define kCanChangeEditableControllerSSAPriorityAllowed                 0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerExpandPriority.
    * @disallows SA_ChangeEditableControllerExpandPriority.
    */
   #define kCanChangeEditableControllerSSAPriorityNoEditableConfigCreated 0x02
   /** Invalid priority value.
    * @outputof SA_CanChangeEditableControllerExpandPriority.
    * @disallows SA_ChangeEditableControllerExpandPriority.
    */
   #define kCanChangeEditableControllerSSAPriorityInvalidValue            0x03
   /** No editable logical drives configured.
    * @outputof SA_CanChangeEditableControllerExpandPriority.
    * @disallows SA_ChangeEditableControllerExpandPriority.
    */
   #define kCanChangeEditableControllerSSAPriorityNoLogicalDrives         0x04
   /** Requested surface scan analysis priority already set.
    * @outputof SA_CanChangeEditableControllerExpandPriority.
    * @disallows SA_ChangeEditableControllerExpandPriority.
    */
   #define kCanChangeEditableControllerSSAPriorityNoChange                0x05
/** @} */ /* Can Change Editable Surface Scan Analysis Priority */

/** @name Change Editable Surface Scan Analysis Priority
 * @{ */
/** Change the [editable] controller's surface scan analysis priority.
 * @param[in] cda                             Controller data area.
 * @param[in] surface_scan_analysis_priority  Desired physical surface scan priority.
 * @return @ref kTrue if surface scan analysis priority was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerSSAPriority(cda, 0) == kCanChangeEditableSurfaceScanPriorityAllowed)
 * {
 *    if (SA_ChangeEditableControllerSSAPriority(cda, 0) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Surface Scan Analysis Priority\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerSSAPriority(PCDA cda, SA_WORD surface_scan_analysis_priority);
/** @} */ /* Change Editable Surface Scan Analysis Priority */

/** @} */ /* storc_configuration_edit_ctrl_ssap */

/** @defgroup storc_configuration_edit_ctrl_pssc Controller Parallel Surface Scan Count Configuration
 * @brief Modify the controllers parallel surface analysis count.
 * @details
 * @see storc_features_ctrl_parallel_surface_scan_support
 * @see storc_configuration_edit_ctrl_ssap
 * @see storc_properties_ctrl_parallel_surface_scan_current_count
 * @{ */

/** @name Get Editable Parallel Surface Scan Count
 * @{ */
/** Return current value of editable controller's parallel surface scan count.
 * @return [Controller Parallel Surface Scan Counts](@ref storc_properties_ctrl_parallel_surface_scan_current_count)
 */
SA_BYTE SA_GetEditableControllerPSSCount(PCDA cda);
/** @} */ /* Get Editable Parallel Surface Scan Count */

/** @name Can Change Editable Parallel Surface Scan Count
 * @{ */
/** Check [editable] controller's support for changing the parallel surface scan count.
 * @param[in] cda                          Controller data area.
 * @param[in] parallel_surface_scan_count  Desired parallel surface scan count.
 * @return See [Can Change Editable Parallel Surface Scan Count Returns](@ref storc_configuration_edit_ctrl_pssc).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerPSSCount(cda, 0) == kCanChangeEditableControllerPSSCountAllowed)
 * {
 *    if (SA_ChangeEditableControllerPSSCount(cda, 0) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Parallel Surface Scan Count\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerPSSCount(PCDA cda, SA_BYTE parallel_surface_scan_count);
   /** Change parallel surface scan count allowed.
    * @outputof SA_CanChangeEditableControllerPSSCount.
    * @allows SA_ChangeEditableControllerPSSCount.
    */
   #define kCanChangeEditableControllerPSSCountAllowed                 0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerPSSCount.
    * @disallows SA_ChangeEditableControllerPSSCount.
    */
   #define kCanChangeEditableControllerPSSCountNoEditableConfigCreated 0x02
   /** Value below minimum or above maximum allowed value.
    * @outputof SA_CanChangeEditableControllerPSSCount.
    * @disallows SA_ChangeEditableControllerPSSCount.
    */
   #define kCanChangeEditableControllerPSSCountOutOfRange              0x03
   /** Controller does not support operation.
    * @outputof SA_CanChangeEditableControllerPSSCount.
    * @disallows SA_ChangeEditableControllerPSSCount.
    */
   #define kCanChangeEditableControllerPSSCountOperationUnsupported    0x04
   /** No configured editable logical drive.
    * @outputof SA_CanChangeEditableControllerPSSCount.
    * @disallows SA_ChangeEditableControllerPSSCount.
    */
   #define kCanChangeEditableControllerPSSCountNoLogicalDrives         0x05
   /** Requested parallel surface scan count already set.
    * @outputof SA_CanChangeEditableControllerPSSCount.
    * @disallows SA_ChangeEditableControllerPSSCount.
    */
   #define kCanChangeEditableControllerPSSCountNoChange                0x06
/** @} */ /* Can Change Editable Parallel Surface Scan Count */

/** @name Change Editable Parallel Surface Scan Count
 * @{ */
/** Change the [editable] controller's parallel surface scan count.
 * @param[in] cda                          Controller data area.
 * @param[in] parallel_surface_scan_count  Desired parallel surface scan count.
 * @return @ref kTrue if parallel surface scan count was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerPSSCount(cda, 0) == kCanChangeEditableControllerPSSCountAllowed)
 * {
 *    if (SA_ChangeEditableControllerPSSCount(cda, 0) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Parallel Surface Scan Count\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerPSSCount(PCDA cda, SA_BYTE parallel_surface_scan_count);
/** @} */ /* Change Editable Parallel Surface Scan Count */

/** @} */ /* storc_configuration_edit_ctrl_pssc */

/** @defgroup storc_configuration_edit_ctrl_dpo Controller Degraded Performance Optimization Configuration
 * @brief Modify the controller's degraded performance optimization setting.
 * @details
 * @see storc_features_ctrl_dpo_support
 * @see storc_properties_ctrl_dpo
 * @{ */

/** @name Get Editable Degraded Performance Optimization
 * @{ */
/** Return current value of editable controller's degraded performance optimization setting.
 * @return [Degraded Performance Optimization Settings](@ref storc_properties_ctrl_dpo)
 */
SA_BYTE SA_GetEditableControllerDPO(PCDA cda);
/** @} */ /* Get Editable Degraded Performance Optimization */

/** @name Can Change Editable Degraded Performance Optimization
* @{ */
/** Check [editable] controller's support for changing the degraded performance optimization.
 * @param[in] cda                                Controller data area.
 * @param[in] degraded_performance_optimization  Desired degraded performance optimization (see [Degraded Performance Optimization Settings](@ref storc_properties_ctrl_dpo).
 * @return See [Can Change Editable Degraded Performance Optimization Returns](@ref storc_configuration_edit_ctrl_dpo).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerDPO(cda, kControllerDPOEnabled) == kCanChangeEditableControllerDPOAllowed)
 * {
 *    if (SA_ChangeEditableControllerDPO(cda, kControllerDPOEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Degraded Performance Optimization\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerDPO(PCDA cda, SA_BYTE degraded_performance_optimization);
   /** Change degraded performance optimization allowed.
    * @outputof SA_CanChangeEditableControllerDPO.
    * @allows SA_ChangeEditableControllerDPO.
    */
   #define kCanChangeEditableControllerDPOAllowed                 0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerDPO.
    * @disallows SA_ChangeEditableControllerDPO.
    */
   #define kCanChangeEditableControllerDPONoEditableConfigCreated 0x02
   /** Invalid degraded performance optimization setting.
    * @outputof SA_CanChangeEditableControllerDPO.
    * @disallows SA_ChangeEditableControllerDPO.
    */
   #define kCanChangeEditableControllerDPOInvalidValue            0x03
   /** No editable logical drives configured.
    * @outputof SA_CanChangeEditableControllerDPO.
    * @disallows SA_ChangeEditableControllerDPO.
    */
   #define kCanChangeEditableControllerDPONoLogicalDrives         0x04
   /** Requested degraded performance optimization already set.
    * @outputof SA_CanChangeEditableControllerDPO.
    * @disallows SA_ChangeEditableControllerDPO.
    */
   #define kCanChangeEditableControllerDPONoChange                0x05
   /** DPO not supported by the controller.
    * @outputof SA_CanChangeEditableControllerDPO.
    * @disallows SA_ChangeEditableControllerDPO.
    */
   #define kCanChangeEditableControllerDPONotSupported            0x06
/** @} */ /* Can Change Editable Degraded Performance Optimization */

/** @name Change Editable Degraded Performance Optimization
* @{ */
/** Change the [editable] controller's degraded performance optimization.
 * @param[in] cda                                Controller data area.
 * @param[in] degraded_performance_optimization  Desired degraded performance optimization.
 * @return @ref kTrue if degraded performance optimization was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerDPO(cda, kControllerDPOEnabled) == kCanChangeEditableControllerDPOAllowed)
 * {
 *    if (SA_ChangeEditableControllerDPO(cda, kControllerDPOEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Degraded Performance Optimization\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerDPO(PCDA cda, SA_BYTE degraded_performance_optimization);
/** @} */ /* Change Editable Degraded Performance Optimization */

/** @} */ /* storc_configuration_edit_ctrl_dpo */

/** @defgroup storc_configuration_edit_ctrl_irp Controller Inconsistency Repair Policy Configuration
 * @brief Modify the controller's inconsistency repair policy.
 * @details
 * @see storc_features_ctrl_irp_support
 * @see storc_properties_ctrl_irp
 * @{ */

/** @name Get Editable Inconsistency Repair Policy
 * @{ */
/** Return current value of editable controller's inconsistency repair policy.
 * @return [Controller Inconsistency Repair Policy Settings](@ref storc_properties_ctrl_irp)
 */
SA_BYTE SA_GetEditableControllerIRP(PCDA cda);
/** @} */ /* Get Editable Inconsistency Repair Policy */

/** @name Can Change Editable Inconsistency Repair Policy
 * @{ */
/** Check [editable] controller's support for changing the inconsistency repair policy.
 * @param[in] cda                          Controller data area.
 * @param[in] inconsistency_repair_policy  Desired inconsistency repair policy (see [Controller Inconsistency Repair Policy Settings](@ref storc_properties_ctrl_irp).
 * @return See [Can Change Editable Inconsistency Repair Policy Returns](@ref storc_configuration_edit_ctrl_irp).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerIRP(cda, kControllerIRPEnabled) == kCanChangeEditableControllerIRPAllowed)
 * {
 *    if (SA_ChangeEditableControllerIRP(cda, kControllerIRPEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Inconsistency Repair Policy\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerIRP(PCDA cda, SA_BYTE inconsistency_repair_policy);
   /** Change inconsistency repair policy allowed.
    * @outputof SA_CanChangeEditableControllerIRP.
    * @allows SA_ChangeEditableControllerIRP.
    */
   #define kCanChangeEditableControllerIRPAllowed                 0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerIRP.
    * @disallows SA_ChangeEditableControllerIRP.
    */
   #define kCanChangeEditableControllerIRPNoEditableConfigCreated 0x02
   /** Invalid inconsistency repair policy setting.
    * @outputof SA_CanChangeEditableControllerIRP.
    * @disallows SA_ChangeEditableControllerIRP.
    */
   #define kCanChangeEditableControllerIRPInvalidValue            0x03
   /** Inconsistency repair policy is unsupported for this controller.
    * @outputof SA_CanChangeEditableControllerIRP.
    * @disallows SA_ChangeEditableControllerIRP.
    */
   #define kCanChangeEditableControllerIRPUnsupported             0x04
   /** No editable logical drives configured.
    * @outputof SA_CanChangeEditableControllerIRP.
    * @disallows SA_ChangeEditableControllerIRP.
    */
   #define kCanChangeEditableControllerIRPNoLogicalDrives         0x05
   /** Requested inconsistency repair policy already set.
    * @outputof SA_CanChangeEditableControllerIRP.
    * @disallows SA_ChangeEditableControllerIRP.
    */
   #define kCanChangeEditableControllerIRPNoChange                0x06
/** @} */ /* Can Change Editable Inconsistency Repair Policy */

/** @name Change Editable Inconsistency Repair Policy
 * @{ */
/** Change the [editable] controller's inconsistency repair policy.
 * @param[in] cda                          Controller data area.
 * @param[in] inconsistency_repair_policy  Desired inconsistency repair policy.
 * @return @ref kTrue if inconsistency repair policy was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerIRP(cda, kControllerIRPEnabled) == kCanChangeEditableControllerIRPAllowed)
 * {
 *    if (SA_ChangeEditableControllerIRP(cda, kControllerIRPEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Inconsistency Repair Policy\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerIRP(PCDA cda, SA_BYTE inconsistency_repair_policy);
/** @} */ /* Change Editable Inconsistency Repair Policy */

/** @} */ /* storc_configuration_edit_ctrl_irp */

/** @defgroup storc_configuration_edit_inconsistency_event_notification Controller Inconsistency Event Notification Configuration
 * @brief Modify the controller's inconsistency event notification.
 * @details
 * @see storc_features_inconsistency_event_notification_support
 * @see storc_properties_ctrl_inconsistency_event_notification_setting
 * @{ */

/** @name Get Editable Inconsistency Event Notification
 * @{ */
/** Return current value of editable controller's inconsistency event notification.
 * @return [Controller Inconsistency Event Notification Settings](@ref storc_properties_ctrl_inconsistency_event_notification_setting)
 */
SA_BYTE SA_GetEditableControllerInconsistencyEventNotification(PCDA cda);
/** @} */ /* Get Editable Inconsistency Event Notification */

/** @name Can Change Editable Inconsistency Event Notification
 * @{ */
/** Check [editable] controller's support for changing the inconsistency event notification.
 * @param[in] cda                                 Controller data area.
 * @param[in] inconsistency_event_notification    Desired inconsistency event notification (see [Controller Inconsistency Event Notification Settings](@ref storc_properties_ctrl_inconsistency_event_notification_setting).
 * @return See [Can Change Editable Inconsistency Event Notification Returns](@ref storc_configuration_edit_inconsistency_event_notification).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerInconsistencyEventNotification(cda, kControllerInconsistencyEventNotificationEnabled) == kCanChangeEditableControllerInconsistencyEventNotificationAllowed)
 * {
 *    if (SA_ChangeEditableControllerInconsistencyEventNotification(cda, kControllerInconsistencyEventNotificationEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Inconsistency Event Notification\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerInconsistencyEventNotification(PCDA cda, SA_BYTE inconsistency_event_notification);
   /** Change inconsistency event notification allowed.
    * @outputof SA_CanChangeEditableControllerInconsistencyEventNotification.
    * @allows SA_ChangeEditableControllerInconsistencyEventNotification.
    */
   #define kCanChangeEditableControllerInconsistencyEventNotificationAllowed                 0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerInconsistencyEventNotification.
    * @disallows SA_ChangeEditableControllerInconsistencyEventNotification.
    */
   #define kCanChangeEditableControllerInconsistencyEventNotificationNoEditableConfigCreated 0x02
   /** Invalid inconsistency event notification setting.
    * @outputof SA_CanChangeEditableControllerInconsistencyEventNotification.
    * @disallows SA_ChangeEditableControllerInconsistencyEventNotification.
    */
   #define kCanChangeEditableControllerInconsistencyEventNotificationInvalidValue            0x03
   /** Inconsistency event notification is unsupported for this controller.
    * @outputof SA_CanChangeEditableControllerInconsistencyEventNotification.
    * @disallows SA_ChangeEditableControllerInconsistencyEventNotification.
    */
   #define kCanChangeEditableControllerInconsistencyEventNotificationUnsupported             0x04
   /** no editable logical drives configured.
    * @outputof SA_CanChangeEditableControllerInconsistencyEventNotification.
    * @disallows SA_ChangeEditableControllerInconsistencyEventNotification.
    */
   #define kCanChangeEditableControllerInconsistencyEventNotificationNoLogicalDrives         0x05
   /** Requested inconsistency event notification already set.
    * @outputof SA_CanChangeEditableControllerInconsistencyEventNotification.
    * @disallows SA_ChangeEditableControllerInconsistencyEventNotification.
    */
   #define kCanChangeEditableControllerInconsistencyEventNotificationNoChange                0x06
/** @} */ /* Can Change Editable Inconsistency Event Notification */

/** @name Change Editable Inconsistency Event Notification
 * @{ */
/** Change the [editable] controller's inconsistency event notification.
 * @pre The user has used @ref SA_CanChangeEditableControllerInconsistencyEventNotification to ensure the operation can be performed.
 * @param[in] cda                               Controller data area.
 * @param[in] inconsistency_event_notification  Desired inconsistency event notification.
 * @return @ref kTrue if inconsistency event notification was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerInconsistencyEventNotification(cda, kControllerInconsistencyEventNotificationEnabled) == kCanChangeEditableControllerInconsistencyEventNotificationAllowed)
 * {
 *    if (SA_ChangeEditableControllerInconsistencyEventNotification(cda, kControllerInconsistencyEventNotificationEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Inconsistency Event Notification\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerInconsistencyEventNotification(PCDA cda, SA_BYTE inconsistency_event_notification);
/** @} */ /* Change Editable Inconsistency Event Notification */

/** @} */ /* storc_configuration_edit_inconsistency_event_notification */

/** @defgroup storc_configuration_edit_ctrl_elevator_sort Controller Elevator Sort Configuration
 * @brief Modify a controller's elevator sort toggle.
 * @details
 * @see storc_properties_ctrl_elevator_sort
 * @{ */

/** @name Get Editable Elevator Sort
 * @{ */
/** Return current value of editable controller's elevator sort.
 * @return [Controller Elevator Sort Settings](@ref storc_properties_ctrl_elevator_sort)
 */
SA_BYTE SA_GetEditableControllerElevatorSort(PCDA cda);
/** @} */ /* Get Editable Elevator Sort */

/** @name Can Change Editable Elevator Sort
 * @{ */
/** Check [editable] controller's support for changing the elevator sort.
 * @param[in] cda            Controller data area.
 * @param[in] elevator_sort  Desired elevator sort (see [Controller Elevator Sort Settings](@ref storc_properties_ctrl_elevator_sort).
 * @return See [Can Change Editable Elevator Sort Returns](@ref storc_configuration_edit_ctrl_elevator_sort).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerElevatorSort(cda, kControllerElevatorSortEnabled) == kCanChangeEditableControllerElevatorSortAllowed)
 * {
 *    if (SA_ChangeEditableControllerElevatorSort(cda, kControllerElevatorSortEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Elevator Sort\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerElevatorSort(PCDA cda, SA_BYTE elevator_sort);
   /** Change elevator sort allowed.
    * @outputof SA_CanChangeEditableControllerElevatorSort.
    * @allows SA_ChangeEditableControllerElevatorSort.
    */
   #define kCanChangeEditableControllerElevatorSortAllowed                 0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerElevatorSort.
    * @disallows SA_ChangeEditableControllerElevatorSort.
    */
   #define kCanChangeEditableControllerElevatorSortNoEditableConfigCreated 0x02
   /** Invalid elevator sort setting.
    * @outputof SA_CanChangeEditableControllerElevatorSort.
    * @disallows SA_ChangeEditableControllerElevatorSort.
    */
   #define kCanChangeEditableControllerElevatorSortInvalidValue            0x03
   /** no editable logical drives configured.
    * @outputof SA_CanChangeEditableControllerElevatorSort.
    * @disallows SA_ChangeEditableControllerElevatorSort.
    */
   #define kCanChangeEditableControllerElevatorSortNoLogicalDrives         0x04
   /** Requested elevator sort already set.
    * @outputof SA_CanChangeEditableControllerElevatorSort.
    * @disallows SA_ChangeEditableControllerElevatorSort.
    */
   #define kCanChangeEditableControllerElevatorSortNoChange                0x05
/** @} */ /* Can Change Editable Elevator Sort */

/** @name Change Editable Elevator Sort
 * @{ */
/** Change the [editable] controller's elevator sort.
 * @param[in] cda            Controller data area.
 * @param[in] elevator_sort  Desired elevator sort.
 * @return @ref kTrue if elevator sort was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerElevatorSort(cda, kControllerElevatorSortEnabled) == kCanChangeEditableControllerElevatorSortAllowed)
 * {
 *    if (SA_ChangeEditableControllerElevatorSort(cda, kControllerElevatorSortEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Elevator Sort\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerElevatorSort(PCDA cda, SA_BYTE elevator_sort);
/** @} */ /* Change Editable Elevator Sort */

/** @} */ /* storc_configuration_edit_ctrl_elevator_sort */

/** @defgroup storc_configuration_edit_queue_depth Controller Queue Depth Configuration
 * @brief Modify a controller's queue depth setting.
 * @details
 * @see storc_features_ctrl_queue_depth_support
 * @see storc_properties_ctrl_queue_depth
 * @{ */

/** @name Get Editable Queue Depth
 * @{ */
/** Return current value of editable controller's queue depth.
 * @return [Controller Queue Depths](@ref storc_properties_ctrl_queue_depth)
 */
SA_BYTE SA_GetEditableControllerQueueDepth(PCDA cda);
/** @} */ /* Get Editable Queue Depth */

/** @name Can Change Editable Queue Depth
 * @{ */
/** Check [editable] controller's support for changing the queue depth.
 * @param[in] cda          Controller data area.
 * @param[in] queue_depth  Desired queue depth (see [Controller Queue Depths](@ref storc_properties_ctrl_queue_depth).
 * @return See [Can Change Editable Queue Depth Returns](@ref storc_configuration_edit_queue_depth).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerQueueDepth(cda, kControllerQueueDepthAutomatic) == kCanChangeEditableControllerQueueDepthAllowed)
 * {
 *    if (SA_ChangeEditableControllerQueueDepth(cda, kControllerQueueDepthAutomatic) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Queue Depth\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerQueueDepth(PCDA cda, SA_BYTE queue_depth);
   /** Change queue depth allowed.
    * @outputof SA_CanChangeEditableControllerQueueDepth.
    * @allows SA_ChangeEditableControllerQueueDepth.
    */
   #define kCanChangeEditableControllerQueueDepthAllowed                  1
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerQueueDepth.
    * @disallows SA_ChangeEditableControllerQueueDepth.
    */
   #define kCanChangeEditableControllerQueueDepthNoEditableConfigCreated  2
   /** Invalid queue depth setting.
    * @outputof SA_CanChangeEditableControllerQueueDepth.
    * @disallows SA_ChangeEditableControllerQueueDepth.
    */
   #define kCanChangeEditableControllerQueueDepthInvalidValue             3
   /** No editable logical drives configured.
    * @outputof SA_CanChangeEditableControllerQueueDepth.
    * @disallows SA_ChangeEditableControllerQueueDepth.
    */
   #define kCanChangeEditableControllerQueueDepthNoLogicalDrives          4
   /** Requested queue depth already set.
    * @outputof SA_CanChangeEditableControllerQueueDepth.
    * @disallows SA_ChangeEditableControllerQueueDepth.
    */
   #define kCanChangeEditableControllerQueueDepthNoChange                 5
   /** The controller does not support queue depth, or modifying the
    * queue depth is not supported.
    * @ref SA_ControllerSupportsQueueDepth.
    * @outputof SA_CanChangeEditableControllerQueueDepth.
    * @disallows SA_ChangeEditableControllerQueueDepth.
    */
   #define kCanChangeEditableControllerQueueDepthNotSupported             6
/** @} */ /* Can Change Editable Queue Depth */

/** @name Change Editable Queue Depth
 * @{ */
/** Change the [editable] controller's queue depth.
 * @param[in] cda          Controller data area.
 * @param[in] queue_depth  Desired queue depth.
 * @return @ref kTrue if queue depth was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerQueueDepth(cda, kControllerQueueDepthAutomatic) == kCanChangeEditableControllerQueueDepthAllowed)
 * {
 *    if (SA_ChangeEditableControllerQueueDepth(cda, kControllerQueueDepthAutomatic) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Queue Depth\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerQueueDepth(PCDA cda, SA_BYTE queue_depth);
/** @} */ /* Change Editable Queue Depth */

/** @} */ /* storc_configuration_edit_queue_depth */

/** @defgroup storc_configuration_edit_fls Controller Flexible Latency Scheduler Configuration
 * @brief Modify a controller's flexible latency scheduler setting.
 * @details
 * @see storc_features_ctrl_fls_supported
 * @see storc_properties_ctrl_fls_setting
 * @{ */

/** @name Get Editable Flexible Latency Scheduler
 * @{ */
/** Return current value of editable controller's flexible latency scheduler.
 * @return [Controller FLS Settings](@ref storc_properties_ctrl_fls_setting)
 */
SA_BYTE SA_GetEditableControllerFLS(PCDA cda);
/** @} */ /* Get Editable Flexible Latency Scheduler */

/** @name Can Change Editable Flexible Latency Scheduler
 * @{ */
/** Check [editable] controller's support for changing the flexible latency scheduler.
 * @param[in] cda                         Controller data area.
 * @param[in] flexible_latency_scheduler  Desired flexible latency scheduler (see [Controller FLS Settings](@ref storc_properties_ctrl_fls_setting).
 * @return See [Can Change Editable Flexible Latency Scheduler Returns](@ref storc_configuration_edit_fls).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableFlexibleLatencySchedulre(cda, kControllerFLSDefault) == kCanChangeEditableControllerFLSAllowed)
 * {
 *    if (SA_ChangeEditableControllerFLS(cda, kControllerFLSDefault) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Flexible Latency Scheduler\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerFLS(PCDA cda, SA_BYTE flexible_latency_scheduler);
   /** Change flexible latency scheduler allowed.
    * @outputof SA_CanChangeEditableControllerFLS.
    * @allows SA_ChangeEditableControllerFLS.
    */
   #define kCanChangeEditableControllerFLSAllowed                  0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerFLS.
    * @disallows SA_ChangeEditableControllerFLS.
    */
   #define kCanChangeEditableControllerFLSNoEditableConfigCreated  0x02
   /** Invalid flexible latency scheduler setting.
    * @outputof SA_CanChangeEditableControllerFLS.
    * @disallows SA_ChangeEditableControllerFLS.
    */
   #define kCanChangeEditableControllerFLSInvalidValue             0x03
   /** no editable logical drives configured.
    * @outputof SA_CanChangeEditableControllerFLS.
    * @disallows SA_ChangeEditableControllerFLS.
    */
   #define kCanChangeEditableControllerFLSNoLogicalDrives          0x04
   /** Requested flexible latency scheduler already set.
    * @outputof SA_CanChangeEditableControllerFLS.
    * @disallows SA_ChangeEditableControllerFLS.
    */
   #define kCanChangeEditableControllerFLSNoChange                 0x05
/** @} */ /* Can Change Editable Flexible Latency Scheduler */

/** @name Change Editable Flexible Latency Scheduler
 * @{ */
/** Change the [editable] controller's flexible latency scheduler.
 * @param[in] cda          Controller data area.
 * @param[in] flexible_latency_scheduler  Desired flexible latency scheduler.
 * @return @ref kTrue if flexible latency scheduler was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableFlexibleLatencySchedulre(cda, kControllerFLSDefault) == kCanChangeEditableControllerFLSAllowed)
 * {
 *    if (SA_ChangeEditableControllerFLS(cda, kControllerFLSDefault) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Flexible Latency Scheduler\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerFLS(PCDA cda, SA_BYTE flexible_latency_scheduler);
/** @} */ /* Change Editable Flexible Latency Scheduler */

/** @} */ /* storc_configuration_edit_fls */

/** @defgroup storc_configuration_edit_nbwc Controller No Battery Write Cache Configuration
 * @brief Modify a controller's no battery write cache setting.
 * @details
 * @see storc_features_ctrl_nbwc_supported
 * @see storc_properties_ctrl_nbwc_setting
 * @{ */

/** Get Editable No Battery Write Cache
 * @{ */
/** Return current value of editable controller's no battery write cache.
 * @return [Controller NBWC Settings](@ref storc_properties_ctrl_nbwc_setting)
 */
SA_BYTE SA_GetEditableControllerNBWC(PCDA cda);
/** @} */ /* Get Editable No Battery Write Cache */

/** Can Change Editable No Battery Write Cache
 * @{ */
/** Check [editable] controller's support for changing the no battery write cache.
 * @param[in] cda          Controller data area.
 * @param[in] nbwc_setting Desired no battery write cache setting (see [Controller NBWC Settings](@ref storc_properties_ctrl_nbwc_setting).
 * @return See [Can Change Editable No Battery Write Cache Returns](@ref storc_configuration_edit_nbwc).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerNBWC(cda, kControllerNBWCEnabled) == kCanChangeEditableControllerNBWCAllowed)
 * {
 *    if (SA_ChangeEditableControllerNBWC(cda, kControllerNBWCEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change No Battery Write Cache\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerNBWC(PCDA cda, SA_BYTE nbwc_setting);
   /** Change NBWC allowed.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @allows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCAllowed                               0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCNoEditableConfigCreated               0x02
   /** Controller does not support operation.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCOperationUnsupported                  0x03
   /** Invalid value for no battery write cache.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCInvalidValue                          0x04
   /** No configured editable logical drive.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCNoLogicalDrives                       0x05
   /** Requested no battery write cache setting already set.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCNoChange                              0x06
   /** Operation is unavailable because current or pending HBA mode.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCInHBAMode                             0x07
   /** No cache board present.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCNoCachePresent                        0x08
   /** Controller cache is inactive.
    * @deprecated No longer used.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCControllerCacheInactive               0x09
   /** Controller cache status is not OK.
    * @see SA_GetControllerStatusInfo.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCCacheStatusNotOK                      0x0A
   /** Cannot change NBWC because all logical drives have accelerator set
    * to @ref kLogicalDriveAcceleratorIOBypass.
    * @deprecated Use @ref kCanChangeEditableControllerNBWCNoLogicalDriveAllowsNBWC instead.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCAllLogicalDriveAcceleratorIOBypass    0x0B
   /** Cannot change NBWC because editable read cache percent in not in range.
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    * @see SA_GetEditableControllerReadCachePercent.
    * @see SA_GetControllerNonBatteryBackedReadCachePercentLimits.
    */
   #define kCanChangeEditableControllerNBWCReadCachePercentNotInRange            0x0C
   /** Cannot change NBWC because all existing logical drives have other accelerator methods set and
    * does not allow no battery write cache. The other accelerator methods can be @ref kLogicalDriveAcceleratorIOBypass
    * or @ref kLogicalDriveAcceleratorLUCache. Or the exiting logical drives are @ref kLogicalDriveTypeSplitMirrorSetPrimary,
    * @ref kLogicalDriveTypeSplitMirrorSetBackup, @ref kLogicalDriveTypeSplitMirrorSetBackupOrphan or @ref kLogicalDriveTypeCache.
    * Use this output to replace deprecated @ref kCanChangeEditableControllerNBWCCacheStatusNotOK
    * @outputof SA_CanChangeEditableControllerNBWC.
    * @disallows SA_ChangeEditableControllerNBWC.
    */
   #define kCanChangeEditableControllerNBWCNoLogicalDriveAllowsNBWC              0x0D
/** @} */ /* Can Change Editable No Battery Write Cache */

/** Change Editable No Battery Write Cache
 * @{ */
/** Change the [editable] controller's no battery write cache.
 * @param[in] cda          Controller data area.
 * @param[in] nbwc_setting Desired no battery write cache setting (see [Controller NBWC Settings](@ref storc_properties_ctrl_nbwc_setting).
 * @return @ref kTrue if no battery write cache was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerNBWC(cda, kControllerNBWCEnabled) == kCanChangeEditableControllerNBWCAllowed)
 * {
 *    if (SA_ChangeEditableControllerNBWC(cda, kControllerNBWCEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change No Battery Write Cache\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerNBWC(PCDA cda, SA_BYTE nbwc_setting);
/** @} */ /* Change Editable No Battery Write Cache */

/** @} */ /* storc_configuration_edit_nbwc */

/** @defgroup storc_configuration_edit_wait_for_cache_room Controller Wait For Cache Room Configuration
 * @brief Modify a controller's wait for cache room setting.
 * @details
 * @see storc_features_ctrl_wait_for_cache_room_supported
 * @see storc_properties_ctrl_wait_for_cache_room
 * @{ */

/** @name Get Editable Wait For Cache Room
 * @{ */
/** Return current value of editable controller's wait for cache room setting.
 * @return [Controller Wait For Cache Room Setting](@ref storc_properties_ctrl_wait_for_cache_room)
 */
SA_BYTE SA_GetEditableControllerWaitForCacheRoom(PCDA cda);
/** @} */ /* Get Editable Wait For Cache Room */

/** @name Can Change Editable Wait For Cache Room
 * @{ */
/** Check [editable] controller's support for changing the wait for cache room.
 * @param[in] cda                         Controller data area.
 * @param[in] wait_for_cache_room_setting Desired wait for cache room setting (see [Controller Wait For Cache Room Setting](@ref storc_properties_ctrl_wait_for_cache_room).
 * @return See returns for [Can Change Editable Wait For Cache Room](@ref storc_configuration_edit_wait_for_cache_room).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerWaitForCacheRoom(cda, kControllerWaitForCacheRoomEnabled) == kCanChangeEditableControllerWaitForCacheRoomAllowed)
 * {
 *    if (SA_ChangeEditableControllerWaitForCacheRoom(cda, kControllerWaitForCacheRoomEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Wait For Cache Room\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerWaitForCacheRoom(PCDA cda, SA_BYTE wait_for_cache_room_setting);
   /** Change Wait For Cache Room allowed.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @allows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomAllowed                               0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @disallows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomNoEditableConfigCreated               0x02
   /** Controller does not support operation.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @disallows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomOperationUnsupported                  0x03
   /** Invalid value for wait for cache room.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @disallows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomInvalidValue                          0x04
   /** No configured editable logical drive.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @disallows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomNoLogicalDrives                       0x05
   /** Requested wait for cache room setting already set.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @disallows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomNoChange                              0x06
   /** Operation is unavailable because current or pending HBA mode.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @disallows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomInHBAMode                             0x07
   /** No cache board present.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @disallows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomNoCachePresent                        0x08
   /** Controller cache is inactive.
    * @deprecated No longer used.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @disallows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomControllerCacheInactive               0x09
   /** Controller cache status is not OK.
    * @see SA_GetControllerStatusInfo.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @disallows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomCacheStatusNotOK                      0x0A
   /** Cannot change WaitForCacheRoom because all existing logical drives have other accelerator methods set and
    * does not allow no battery write cache. The other accelerator methods can be @ref kLogicalDriveAcceleratorIOBypass
    * or @ref kLogicalDriveAcceleratorLUCache. Or the exiting logical drives are @ref kLogicalDriveTypeSplitMirrorSetPrimary,
    * @ref kLogicalDriveTypeSplitMirrorSetBackup, @ref kLogicalDriveTypeSplitMirrorSetBackupOrphan or @ref kLogicalDriveTypeCache.
    * @outputof SA_CanChangeEditableControllerWaitForCacheRoom.
    * @disallows SA_ChangeEditableControllerWaitForCacheRoom.
    */
   #define kCanChangeEditableControllerWaitForCacheRoomNoLogicalDriveAllowsWaitForCacheRoom  0x0B
/** @} */ /* Can Change Editable Wait For Cache Room */

/** @name Change Editable Wait For Cache Room
 * @{ */
/** Change the [editable] controller's wait for cache room.
 * @param[in] cda                          Controller data area.
 * @param[in] wait_for_cache_room_setting  Desired wait for cache room setting (see [Controller Wait For Cache Room Setting](@ref storc_properties_ctrl_wait_for_cache_room).
 * @return @ref kTrue if wait for cache room was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerWaitForCacheRoom(cda, kControllerWaitForCacheRoomEnabled) == kCanChangeEditableControllerWaitForCacheRoomAllowed)
 * {
 *    if (SA_ChangeEditableControllerWaitForCacheRoom(cda, kControllerWaitForCacheRoomEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Wait For Cache Room\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerWaitForCacheRoom(PCDA cda, SA_BYTE wait_for_cache_room_setting);
/** @} */ /* Change Editable Wait For Cache Room */

/** @} */ /* storc_configuration_edit_wait_for_cache_room */

/** @defgroup storc_configuration_edit_read_cache_percent Controller Read Cache Percent Configuration
 * @brief Modify the controller's read/write cache ratio.
 * @details
 * @see storc_properties_ctrl_read_cache_percent
 * @{ */

/** Get Editable Controller Read Cache Percent
 * @{ */
/** Return current value of editable controller's read cache percent.
 * @return [Controller Read Cache Percent](@ref storc_properties_ctrl_read_cache_percent)
 */
SA_BYTE SA_GetEditableControllerReadCachePercent(PCDA cda);
/** @} */ /* Get Editable Controller Read Cache Percent */

/** Get Editable Controller Read Cache Percent Limits
 * @{ */
/** Get editable controller's minimum and maximum read cache percent.
 * @param[in] cda Controller data area.
 * @return Use @ref GET_EDITABLE_CONTROLLER_MIN_READ_CACHE_PERCENT to get editable controller's minimum read cache percent. (@ref storc_configuration_edit_read_cache_percent)
 * @return Use @ref GET_EDITABLE_CONTROLLER_MAX_READ_CACHE_PERCENT to get editable controller's maximum read cache percent. (@ref storc_configuration_edit_read_cache_percent)
 */
SA_WORD SA_GetEditableControllerReadCachePercentLimits(PCDA cda);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kGetEditableControllerReadCachePercentMinLimitMask     0x00FF
#define kGetEditableControllerReadCachePercentMinLimitBitShift 0
#endif

/** Decode editable controller's minimum read cache percent.
 * @outputof SA_GetEditableControllerReadCachePercentLimits.
 */
#define GET_EDITABLE_CONTROLLER_MIN_READ_CACHE_PERCENT(limit) (((limit) & kGetEditableControllerReadCachePercentMinLimitMask) >> kGetEditableControllerReadCachePercentMinLimitBitShift)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define kGetEditableControllerReadCachePercentMaxLimitMask     0xFF00
#define kGetEditableControllerReadCachePercentMaxLimitBitShift 8
#endif

/** Decode editable controller's maximum read cache percent.
 * @outputof SA_GetEditableControllerReadCachePercentLimits.
 */
#define GET_EDITABLE_CONTROLLER_MAX_READ_CACHE_PERCENT(limit) (((limit) & kGetEditableControllerReadCachePercentMaxLimitMask) >> kGetEditableControllerReadCachePercentMaxLimitBitShift)

/** Invalid minimum or maximum read cache percent for editable controller.
 * @outputof GET_EDITABLE_CONTROLLER_MIN_READ_CACHE_PERCENT.
 * @outputof GET_EDITABLE_CONTROLLER_MAX_READ_CACHE_PERCENT.
 */
#define kGetEditableControllerReadCachePercentLimitsInvalid  101
/** @} */ /* Get Editable Controller Read Cache Percent Limits */

/** Can Change Editable Controller Read Cache Percent
 * @{ */
/** Check [editable] controller's support for changing the read cache percent.
 * @param[in] cda                 Controller data area.
 * @param[in] read_cache_percent  Desired read cache percent setting (see [Controller Read Cache Percent](@ref storc_properties_ctrl_read_cache_percent).
 * @return See [Can Change Editable Controller Read Cache Percent Returns](@ref storc_configuration_edit_read_cache_percent).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerReadCachePercent(cda, 50) == kCanChangeEditableControllerReadCachePercentAllowed)
 * {
 *    if (SA_ChangeEditableControllerReadCachePercent(cda, 50) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Read Cache Percent\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerReadCachePercent(PCDA cda, SA_BYTE read_cache_percent);
   /** Change Read Cache Percent allowed.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @allows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentAllowed                   0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentNoEditableConfigCreated   0x02
   /** Invalid value for read cache percent.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentInvalidValue              0x03
   /** No configured editable logical drive.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentNoLogicalDrives           0x04
   /** Requested read cache percent setting already set.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentNoChange                  0x05
   /** Operation is unavailable because current or pending HBA mode.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentInHBAMode                 0x06
   /** Backup power source is not charged or not present.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentBadPowerSource            0x07
   /** No cache board present.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentNoCachePresent            0x08
   /** Controller cache is inactive.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentControllerCacheInactive   0x09
   /** Cannot set read cache while LU cache is enabled.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentLUCacheEnabled            0x0A
   /** Cannot set read cache while cache is not ok.
    * @see SA_GetControllerStatusInfo.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentCacheStatusNotOk          0x0B
   /** Read cache percent value not in range.
    * @see SA_GetEditableControllerReadCachePercentLimits.
    * @outputof SA_CanChangeEditableControllerReadCachePercent.
    * @disallows SA_ChangeEditableControllerReadCachePercent.
    */
   #define kCanChangeEditableControllerReadCachePercentNotInRange                0x0C
/** @} */ /* Can Change Editable Controller Read Cache Percent */

/** Change Editable Controller Read Cache Percent
 * @{ */
/** Change the [editable] controller's read cache percent.
 * @param[in] cda                 Controller data area.
 * @param[in] read_cache_percent  Desired read cache percent setting (see [Controller Read Cache Percent](@ref storc_properties_ctrl_read_cache_percent).
 * @return @ref kTrue if read cache percent was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerReadCachePercent(cda, 50) == kCanChangeEditableControllerReadCachePercentAllowed)
 * {
 *    if (SA_ChangeEditableControllerReadCachePercent(cda, 50) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Read Cache Percent\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerReadCachePercent(PCDA cda, SA_BYTE read_cache_percent);
/** @} */ /* Change Editable Controller Read Cache Percent */

/** @} */ /* storc_configuration_edit_read_cache_percent */

/** @defgroup storc_configuration_edit_mnp_delay Controller MNP Analysis Delay Configuration
 * @brief Modify the controller's monitor and performance analysis delay.
 * @details
 * @see storc_properties_mnp_delay
 * @{ */

/** @name Get Editable MNP Delay
 * @{ */
/** Return current value of editable controller's monitor and performance analysis delay.
 * @return [Controller MNP Delay Settings](@ref storc_properties_mnp_delay)
 */
SA_WORD SA_GetEditableControllerMNPDelay(PCDA cda);
/** @} */ /* Get Editable MNP Delay */

/** @name Can Change Editable MNP Delay
 * @{ */
/** Check [editable] controller's support for changing the mnp analysis delay.
 * @param[in] cda        Controller data area.
 * @param[in] mnp_delay  Desired MNP delay (see [Controller MNP Settings](@ref storc_properties_mnp_delay).
 * @return See [Can Change Editable MNP Delay Returns](@ref storc_configuration_edit_mnp_delay).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerMNPDelay(cda, kControllerMNPDelayDefault) == kCanChangeEditableControllerMNPDelayAllowed)
 * {
 *    if (SA_ChangeEditableControllerMNPDelay(cda, kControllerMNPDelayDefault) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change MNP Analysis Delay\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerMNPDelay(PCDA cda, SA_WORD mnp_delay);
   /** Change MNP delay allowed.
    * @outputof SA_CanChangeEditableControllerMNPDelay.
    * @allows SA_ChangeEditableControllerMNPDelay.
    */
   #define kCanChangeEditableControllerMNPDelayAllowed                  0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerMNPDelay.
    * @disallows SA_ChangeEditableControllerMNPDelay.
    */
   #define kCanChangeEditableControllerMNPDelayNoEditableConfigCreated  0x02
   /** Invalid MNP delay setting.
    * @outputof SA_CanChangeEditableControllerMNPDelay.
    * @disallows SA_ChangeEditableControllerMNPDelay.
    */
   #define kCanChangeEditableControllerMNPDelayInvalidValue             0x03
   /** No editable logical drives configured.
    * @outputof SA_CanChangeEditableControllerMNPDelay.
    * @disallows SA_ChangeEditableControllerMNPDelay.
    */
   #define kCanChangeEditableControllerMNPDelayNoLogicalDrives          0x04
   /** Requested MNP delay already set.
    * @outputof SA_CanChangeEditableControllerMNPDelay.
    * @disallows SA_ChangeEditableControllerMNPDelay.
    */
   #define kCanChangeEditableControllerMNPDelayNoChange                 0x05
/** @} */ /* Can Change Editable MNP Delay */

/** @name Change Editable MNP Delay
 * @{ */
/** Change the [editable] controller's MNP delay.
 * @param[in] cda          Controller data area.
 * @param[in] mnp_delay    Desired MNP delay.
 * @return @ref kTrue if MNP delay was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerMNPDelay(cda, 60) == kCanChangeEditableControllerMNPDelayAllowed)
 * {
 *    if (SA_ChangeEditableControllerMNPDelay(cda, 60) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change MNP Analysis Delay\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerMNPDelay(PCDA cda, SA_WORD mnp_delay);
/** @} */ /* Change Editable MNP Delay */

/** @} */ /* storc_configuration_edit_mnp_delay */

/** @defgroup storc_configuration_edit_survivalmode Controller Survival Power Mode Configuration
 * @brief Configure controller survival power mode.
 * @details
 * @see storc_features_ctrl_survival_mode
 * @see storc_configuration_ctrl_power_mode
 * @see storc_configuration_edit_survivalmode
 * @{ */

/** @name Get Editable Survival Power Mode
 * @{ */
/** Return current value of editable controller's survival mode.
 * @return [Controller SurvivalPowerMode Settings](@ref storc_configuration_edit_survivalmode)
 */
SA_BYTE SA_GetEditableControllerSurvivalPowerMode(PCDA cda);
/** @} */ /* Get Editable Survival Power Mode */

/** @name Can Change Editable Survival Power Mode
 * @{ */
/** Check [editable] controller's support for changing the survival mode.
 * @param[in] cda          Controller data area.
 * @param[in] survivalmode_setting Desired survival mode setting (see [Controller SurvivalPowerMode Settings](@ref storc_configuration_edit_survivalmode).
 * @return See [Can Change Editable Survival Power Mode Returns](@ref storc_configuration_edit_survivalmode).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerSurvivalPowerMode(cda, kControllerSurvivalPowerModeEnabled) == kCanChangeEditableControllerSurvivalPowerModeAllowed)
 * {
 *    if (SA_ChangeEditableControllerSurvivalPowerMode(cda, kControllerSurvivalPowerModeEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Survival Power Mode\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerSurvivalPowerMode(PCDA cda, SA_BYTE survivalmode_setting);
   /** Change SurvivalPowerMode allowed.
    * @outputof SA_CanChangeEditableControllerSurvivalPowerMode.
    * @allows SA_ChangeEditableControllerSurvivalPowerMode.
    */
   #define kCanChangeEditableControllerSurvivalPowerModeAllowed                   0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerSurvivalPowerMode.
    * @disallows SA_ChangeEditableControllerSurvivalPowerMode.
    */
   #define kCanChangeEditableControllerSurvivalPowerModeNoEditableConfigCreated   0x02
   /** Controller does not support the operation.
    * @outputof SA_CanChangeEditableControllerSurvivalPowerMode.
    * @disallows SA_ChangeEditableControllerSurvivalPowerMode.
    */
   #define kCanChangeEditableControllerSurvivalPowerModeOperationUnsupported      0x03
   /** Invalid value for survival mode.
    * @outputof SA_CanChangeEditableControllerSurvivalPowerMode.
    * @disallows SA_ChangeEditableControllerSurvivalPowerMode.
    */
   #define kCanChangeEditableControllerSurvivalPowerModeInvalidValue              0x04
   /** Requested survival mode setting already set.
    * @outputof SA_CanChangeEditableControllerSurvivalPowerMode.
    * @disallows SA_ChangeEditableControllerSurvivalPowerMode.
    */
   #define kCanChangeEditableControllerSurvivalPowerModeNoChange                  0x05
   /** The survival mode is not configurable for this controller.
    * @outputof SA_CanChangeEditableControllerSurvivalPowerMode.
    * @disallows SA_ChangeEditableControllerSurvivalPowerMode.
    */
   #define kCanChangeEditableControllerSurvivalPowerModeNotConfigurable           0x06
/** @} */ /* Can Change Editable Survival Power Mode */

/** @name Change Editable Survival Power Mode
 * @{ */
/** Change the [editable] controller's survival mode.
 * @param[in] cda          Controller data area.
 * @param[in] survivalmode_setting Desired survival mode setting (see [Controller SurvivalPowerMode Settings](@ref storc_configuration_edit_survivalmode).
 * @return @ref kTrue if survival mode was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerSurvivalPowerMode(cda, kControllerSurvivalPowerModeEnabled) == kCanChangeEditableControllerSurvivalPowerModeAllowed)
 * {
 *    if (SA_ChangeEditableControllerSurvivalPowerMode(cda, kControllerSurvivalPowerModeEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change Survival Power Mode\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerSurvivalPowerMode(PCDA cda, SA_BYTE survivalmode_setting);
/** @} */ /* Change Editable Survival Power Mode */

/** @} */ /* storc_configuration_edit_survivalmode */

/** @defgroup storc_configuration_edit_ctrl_write_cache_bypass_threshold Controller Write Cache Bypass Threshold Configuration
 * @brief Modify the controller's write cache bypass threshold.
 * @details
 * @see storc_features_ctrl_cache_bypass
 * @see storc_properties_ctrl_write_cache_bypass_threshold
 * @{ */

/** @name Get Editable Write Cache Bypass Threshold
 * @{ */
/** Return current value of editable controller's write cache bypass threshold (in cache lines).
 * @return Controller write cache bypass threshold [1..65] (in cache lines).
 */
SA_DWORD SA_GetEditableControllerWCBT(PCDA cda);
/** @} */ /* Get Editable Write Cache Bypass Threshold */

/** @name Can Change Editable Write Cache Bypass Threshold
 * @{ */
/** Check [editable] controller's support for modifying the write cache bypass threshold.
 * @param[in] cda               Controller data area.
 * @param[in] write_cache_bypass_threshold  Desired write cache bypass threshold [1..65] (in cache lines).
 * @return See [Can Change Editable Write Cache Bypass Threshold Returns](@ref storc_configuration_edit_ctrl_write_cache_bypass_threshold).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerWCBT(cda, 1) == kCanChangeEditableControllerWCBTAllowed)
 * {
 *    if (SA_ChangeEditableControllerWCBT(cda, 1) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set write cache bypass threshold\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerWCBT(PCDA cda, SA_DWORD write_cache_bypass_threshold);
   /** Change write cache bypass threshold allowed.
    * @outputof SA_CanChangeEditableControllerWCBT.
    * @allows SA_ChangeEditableControllerWCBT.
    */
   #define kCanChangeEditableControllerWCBTAllowed                      0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerWCBT.
    * @disallows SA_ChangeEditableControllerWCBT.
    */
   #define kCanChangeEditableControllerWCBTNoEditableConfigCreated      0x02
   /** Invalid write cache bypass threshold.
    * @outputof SA_CanChangeEditableControllerWCBT.
    * @disallows SA_ChangeEditableControllerWCBT.
    */
   #define kCanChangeEditableControllerWCBTInvalidValue                 0x03
   /** Controller does not support the operation.
    * @outputof SA_CanChangeEditableControllerWCBT.
    * @disallows SA_ChangeEditableControllerWCBT.
    */
   #define kCanChangeEditableControllerWCBTOperationUnsupported         0x04
   /** Controller has no configured editable logical drives.
    * @outputof SA_CanChangeEditableControllerWCBT.
    * @disallows SA_ChangeEditableControllerWCBT.
    */
   #define kCanChangeEditableControllerWCBTNoLogicalDrives              0x05
   /** Requested write cache bypass threshold already set.
    * @outputof SA_CanChangeEditableControllerWCBT.
    * @disallows SA_ChangeEditableControllerWCBT.
    */
   #define kCanChangeEditableControllerWCBTNoChange                     0x06
   /** Controller cache status is not ok.
    * @see SA_GetControllerStatusInfo.
    * @outputof SA_CanChangeEditableControllerWCBT.
    * @disallows SA_ChangeEditableControllerWCBT.
    */
   #define kCanChangeEditableControllerWCBTControllerCacheStatusNotOK   0x07
/** @} */ /* Can Change Editable Write Cache Bypass Threshold */

/** @name Change Editable Write Cache Bypass Threshold
 * @{ */
/** Change the [editable] controller's write cache bypass threshold.
 * @param[in] cda              Controller data area.
 * @param[in] write_cache_bypass_threshold  Desired write cache bypass threshold [1..65] (in cache lines).
 * @return @ref kTrue if write cache bypass threshold was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerWCBT(cda, 1) == kCanChangeEditableControllerWCBTAllowed)
 * {
 *    if (SA_ChangeEditableControllerWCBT(cda, 1) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set write cache bypass threshold\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerWCBT(PCDA cda, SA_DWORD write_cache_bypass_threshold);
/** @} */ /* Change Editable Write Cache Bypass Threshold */

/** @} */ /* storc_configuration_edit_ctrl_write_cache_bypass_threshold */

/** @defgroup storc_configuration_edit_ctrl_sanitize_lock Controller Sanitize Lock Configuration
 * @brief Modify the controller's ATA freeze/anti-freeze sanitize lock configuration.
 * @details
 * @see storc_features_ctrl_support_sanitize_lock
 * @see storc_properties_ctrl_sanitize_lock
 * @{ */

/** @name Get Editable Sanitize Freeze/Anti-Freeze Lock
 * @{ */
/** Return current value of editable controller's sanitize freeze/anti-freeze lock.
 * @return [Controller Controller Sanitize Locks](@ref storc_properties_ctrl_sanitize_lock)
 */
SA_BYTE SA_GetEditableControllerSanitizeLock(PCDA cda);
/** @} */ /* Get Editable Sanitize Freeze/Anti-Freeze Lock */

/** @name Can Change Editable Sanitize Freeze/Anti-Freeze Lock
 * @{ */
/** Check [editable] controller's support for modifying the sanitize freeze/anti-freeze lock.
 * @param[in] cda            Controller data area.
 * @param[in] sanitize_lock  Desired sanitize lock (see [Controller Controller Sanitize Locks](@ref storc_properties_ctrl_sanitize_lock).
 * @return See [Can Change Editable Sanitize Lock Returns](@ref storc_configuration_edit_ctrl_sanitize_lock).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerSanitizeLock(cda, kSanitizeLockFreeze) == kCanChangeEditableControllerSanitizeLockAllowed)
 * {
 *    if (SA_ChangeEditableControllerSanitizeLock(cda, kSanitizeLockFreeze) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set the sanitize lock to freeze\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerSanitizeLock(PCDA cda, SA_BYTE sanitize_lock);
   /** Changing the controller sanitize lock is allowed.
    * @outputof SA_CanChangeEditableControllerSanitizeLock.
    * @allows SA_ChangeEditableControllerSanitizeLock.
    */
   #define kCanChangeEditableControllerSanitizeLockAllowed                 0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerSanitizeLock.
    * @disallows SA_ChangeEditableControllerSanitizeLock.
    */
   #define kCanChangeEditableControllerSanitizeLockNoEditableConfigCreated 0x02
   /** Invalid sanitize lock type.
    * @outputof SA_CanChangeEditableControllerSanitizeLock.
    * @disallows SA_ChangeEditableControllerSanitizeLock.
    */
   #define kCanChangeEditableControllerSanitizeLockInvalidValue            0x03
   /** Controller does not support the given sanitize lock state.
    * @outputof SA_CanChangeEditableControllerSanitizeLock.
    * @disallows SA_ChangeEditableControllerSanitizeLock.
    */
   #define kCanChangeEditableControllerSanitizeLockUnsupported             0x04
   /** Requested sanitize lock already set.
    * @outputof SA_CanChangeEditableControllerSanitizeLock.
    * @disallows SA_ChangeEditableControllerSanitizeLock.
    */
   #define kCanChangeEditableControllerSanitizeLockNoChange                0x05
/** @} */ /* Can Change Editable Sanitize Lock */

/** @name Change Editable Sanitize Lock
 * @{ */
/** Change the [editable] controller's sanitize lock.
 * @param[in] cda            Controller data area.
 * @param[in] sanitize_lock  Desired sanitize lock (see [Controller Controller Sanitize Locks](@ref storc_properties_ctrl_sanitize_lock).
 * @return @ref kTrue if sanitize lock was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerSanitizeLock(cda, kSanitizeLockFreeze) == kCanChangeEditableControllerSanitizeLockAllowed)
 * {
 *    if (SA_ChangeEditableControllerSanitizeLock(cda, kSanitizeLockFreeze) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set the sanitize lock to freeze\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerSanitizeLock(PCDA cda, SA_BYTE sanitize_lock);
/** @} */ /* Change Editable Sanitize Lock */

/** @} */ /* storc_configuration_edit_ctrl_sanitize_lock */

/** @defgroup storc_configuration_edit_ctrl_notify_any_broadcast Controller Notify On Any Broadcast Configuration
 * @brief Modify the controller's "Notify On Any Broadcast" setting.
 * @details
 * @see storc_features_ctrl_support_notify_any_broadcast
 * @see storc_properties_ctrl_notify_on_any_broadcast
 * @{ */

/** @name Get Editable Notify On Any Broadcast
 * @{ */
/** Return current value of editable controller's notify on any broadcast state.
 * @return Same as @ref SA_GetControllerNotifyOnAnyBroadcast.
 * @return See [Controller Notify On Any Broadcast Returns](@ref storc_properties_ctrl_notify_on_any_broadcast).
 */
SA_BYTE SA_GetEditableControllerNotifyOnAnyBroadcast(PCDA cda);
/** @} */ /* Get Editable Notify On Any Broadcast */

/** @name Can Change Editable Notify On Any Broadcast
 * @{ */
/** Check [editable] controller's support for modifying the notify on any broadcast setting
 * @post If valid, user may call @ref SA_ChangeEditableControllerNotifyOnAnyBroadcast.
 * @param[in] cda      Controller data area.
 * @param[in] setting  @ref kControllerNotifyOnAnyBroadcastDisabled to disable or @ref kControllerNotifyOnAnyBroadcastEnabled to enable.
 * @return See [Can Change Editable Notify On Any Broadcast Returns](@ref storc_configuration_edit_ctrl_notify_any_broadcast).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerNotifyOnAnyBroadcast(cda, kControllerNotifyOnAnyBroadcastEnabled) == kCanChangeEditableControllerNotifyOnAnyBroadcastAllowed)
 * {
 *    if (SA_ChangeEditableControllerNotifyOnAnyBroadcast(cda, kControllerNotifyOnAnyBroadcastEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to enable notify on any broadcast\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerNotifyOnAnyBroadcast(PCDA cda, SA_BYTE setting);
   /** Change enable/disable notify on any broadcast allowed.
    * @outputof SA_CanChangeEditableControllerNotifyOnAnyBroadcast.
    * @allows SA_ChangeEditableControllerNotifyOnAnyBroadcast.
    */
   #define kCanChangeEditableControllerNotifyOnAnyBroadcastAllowed                 0x01
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerNotifyOnAnyBroadcast.
    * @disallows SA_ChangeEditableControllerNotifyOnAnyBroadcast.
    */
   #define kCanChangeEditableControllerNotifyOnAnyBroadcastNoEditableConfigCreated 0x02
   /** Invalid input setting.
    * @outputof SA_CanChangeEditableControllerNotifyOnAnyBroadcast.
    * @disallows SA_ChangeEditableControllerNotifyOnAnyBroadcast.
    */
   #define kCanChangeEditableControllerNotifyOnAnyBroadcastInvalidValue            0x03
   /** Controller does not support the operation.
    * @outputof SA_CanChangeEditableControllerNotifyOnAnyBroadcast.
    * @disallows SA_ChangeEditableControllerNotifyOnAnyBroadcast.
    */
   #define kCanChangeEditableControllerNotifyOnAnyBroadcastOperationUnsupported    0x04
   /** Requested write cache bypass threshold already set.
    * @outputof SA_CanChangeEditableControllerNotifyOnAnyBroadcast.
    * @disallows SA_ChangeEditableControllerNotifyOnAnyBroadcast.
    */
   #define kCanChangeEditableControllerNotifyOnAnyBroadcastNoChange                0x05
/** @} */ /* Can Change Editable Notify On Any Broadcast */

/** @name Change Editable Notify On Any Broadcast
 * @{ */
/** Change the [editable] controller's notify on any broadcast setting.
 * @pre The user has used @ref SA_CanChangeEditableControllerNotifyOnAnyBroadcast to ensure the operation can be performed.
 * @param[in] cda      Controller data area.
 * @param[in] setting  @ref kControllerNotifyOnAnyBroadcastDisabled to disable or @ref kControllerNotifyOnAnyBroadcastEnabled to enable.
 * @return @ref kTrue if setting is successfully queued to change on next commit, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerNotifyOnAnyBroadcast(cda, kControllerNotifyOnAnyBroadcastEnabled) == kCanChangeEditableControllerNotifyOnAnyBroadcastAllowed)
 * {
 *    if (SA_ChangeEditableControllerNotifyOnAnyBroadcast(cda, kControllerNotifyOnAnyBroadcastEnabled) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to enable notify on any broadcast\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerNotifyOnAnyBroadcast(PCDA cda, SA_BYTE setting);
/** @} */ /* Change Editable Notify On Any Broadcast */

/** @} */ /* storc_configuration_edit_ctrl_notify_any_broadcast */

/** @} */ /* storc_configuration_edit_ctrl */

/***********************************************************//**
 * @defgroup storc_configuration_edit_encryption Encryption (Virtual Configuration)
 * @brief Setup/Configure the controller's encryption configuration.
 * @details
 * ### Examples
 * - @ref storc_encryption_examples
 * - @ref storc_encryption_setup_example
 * - @ref storc_encryption_modify_example
* @{
**************************************************************/

/** @defgroup storc_configuration_edit_encryption_set Encryption Setup/Enable Key Manager
 * @brief Setup/configure encryption (setup/configure the encryption key manager).
 * @{ */

/** @name Get Editable Encryption
 * @{ */
/** Return current value of editable controller's encryption settings.
 * @return [Controller Encryption Configurations](@ref storc_encryption)
 */
SA_BYTE SA_GetEditableControllerEncryption(PCDA cda);
/** @} */ /* Get Editable Encryption */

/** @name Can Change Editable Encryption
 * @{ */
/** Check [editable] controller's support for changing encryption configuration.
 * @param[in] cda                Controller data area.
 * @param[in] encryption_config  Desired encryption configuration (see [Controller Encryption Configurations](@ref storc_encryption).
 * @return See [Can Change Editable Encryption Returns](@ref storc_configuration_edit_encryption_set).
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerEncryption(cda, kControllerEncryptionExpressLocal) == kCanChangeEditableControllerEncryptionAllowed)
 * {
 *    if (SA_ChangeEditableControllerEncryption(cda, kControllerEncryptionExpressLocal) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to enable express local encryption\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeEditableControllerEncryption(PCDA cda, SA_BYTE encryption_config);
   /** Change encryption configuration allowed.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @allows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionAllowed                      1
   /** No editable configuration created.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionNoEditableConfigCreated      2
   /** Controller does not support operation or the conversion feature is not enabled.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionOperationUnsupported         3
   /** Controller is in HBA mode and does not support encryption in HBA mode.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionInHBAMode                    4
   /** The controller already has volumes that are encrypted.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionHasEncryptedVolumes          5
   /** The controller already has new logical drives not yet committed.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionHasUnsavedLogicalDrives      6
   /** The desired encryption settings is already set.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionNoChange                     7
   /** Invalid/Unknown desired encryption type (see [Controller Encryption Configurations](@ref storc_encryption)).
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionInvalidValue                 8
   /** Encryption configuration not allowed at this time.
    * @deprecated No longer used.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionNotAllowed                   9
   /** Encryption configuration not allowed while there are logical drives locked/offline due to a missing encryption key.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionHasLogicalDriveMissingKey    10
   /** Encryption configuration not allowed because the requested key manager is not an enabled feature.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionManagerNotEnabled            11
   /** Clearing encryption is not an enabled feature.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    * @outputof SA_CanChangeEditableControllerEncryption.
    * @disallows SA_ChangeEditableControllerEncryption.
    */
   #define kCanChangeEditableControllerEncryptionClearEncryptionNotEnabled    12
/** @} */ /* Can Change Editable Encryption */

/** @name Change Editable Encryption
 * @{ */
/** Change the [editable] controller's encryption configuration.
 * @param[in] cda                Controller data area.
 * @param[in] encryption_config  Desired encryption configuration (see [Controller Encryption Configurations](@ref storc_encryption).
 * @return @ref kTrue if MNP delay was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeEditableControllerEncryption(cda, kControllerEncryptionExpressLocal) == kCanChangeEditableControllerEncryptionAllowed)
 * {
 *    if (SA_ChangeEditableControllerEncryption(cda, kControllerEncryptionExpressLocal) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to enable express local encryption\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ChangeEditableControllerEncryption(PCDA cda, SA_BYTE encryption_config);
/** @} */ /* Change Editable Encryption */

/** @} */ /* storc_configuration_edit_encryption_set  */

/** @defgroup storc_configuration_edit_encryption_key Encryption Set/Change Key
 * @brief Setup/configure the encryption master key.
 * @{ */

/** @name Check Encryption Master Key
 * @{ */
/** Used to check if the master key is valid for encryption on a given controller.
 *
 * @warning Key checking will stop on first NULL character or until `buffer_size` characters have been checked.
 * @param[in] cda                Controller handler.
 * @param[in] encryption_config  Desired encryption configuration (see [Controller Encryption Configurations](@ref storc_encryption).
 * @param[in] key_buffer         Key buffer (not required to be null-terminated).
 * @param[in] buffer_size        Number of characters in `key_buffer`
 * @return @ref kCheckEncryptionMasterKeyOk if key is valid or a map indicating what properties of the key are invalid.
 */
SA_BYTE SA_CheckEncryptionMasterKey(PCDA cda, SA_BYTE encryption_config, const char *key_buffer, SA_WORD buffer_size);
   /** Key is valid for encryption on this controller.
    * @outputof SA_CheckEncryptionMasterKey.
    */
   #define kCheckEncryptionMasterKeyOk                       0x00
   /** Key is not valid for encryption on this controller
    * because the key is too short.
    * @outputof SA_CheckEncryptionMasterKey.
    */
   #define kCheckEncryptionMasterKeyTooShort                 0x01
   /** Key is not valid for encryption on this controller
    * because the key is too long.
    * @outputof SA_CheckEncryptionMasterKey.
    */
   #define kCheckEncryptionMasterKeyTooLong                  0x02
   /** Key is not valid for encryption on this controller
    * because the key contains an invalid character.
    * @outputof SA_CheckEncryptionMasterKey.
    */
   #define kCheckEncryptionMasterKeyInvalidChar              0x04
   /** Key is not valid for encryption on this controller because the key contains an invalid character.
    * @outputof SA_CheckEncryptionMasterKey.
    */
   #define kCheckEncryptionMasterKeyInvalidUnknown           0x08
   /** Key is not valid for the invalid/unknown encryption config.
    * @outputof SA_CheckEncryptionMasterKey.
    */
   #define kCheckEncryptionMasterKeyInvalidEncryptionConfig  0x10
/** @} */ /* Check Encryption Master Key */

/** @name Can Set/Change Editable Encryption Key
 * @{ */
/** Check [editable] controller's encryption key be set or modified.
 * @deprecated Use @ref SA_CanSetEditableControllerEncryptionKeySafe instead.
 * @attention This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CanSetEditableControllerEncryptionKeySafe).
 * @post If valid, user may call @ref SA_SetEditableControllerEncryptionKey.
 * @param[in] cda   Controller data area.
 * @param[in] key   C-string (null-terminated) desired key or NULL to 'unset' a key change (Also see @ref SA_CheckEncryptionMasterKey).
 *                  If NULL, key is not checked: This is useful for checking if an
 *                     operation can be performed without user input.
 *                  If "", key changes are discarded: This is useful for canceling a key
 *                     queued for change.
 * @return @ref kCanSetEditableControllerEncryptionKeyOK or a [reason](@ref storc_configuration_edit_encryption_key) the key could not be set/modified.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableControllerEncryptionKey(cda, "MyKey") == kCanSetEditableControllerEncryptionKeyOK)
 * {
 *    if (SA_SetEditableControllerEncryptionKey(cda, "MyKey") != kSetEditableControllerEncryptionKeyOK)
 *    {
 *        fprintf(stderr, "Failed to set encryption key\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableControllerEncryptionKey(PCDA cda, const char * key);
/** @} */ /* Can Set/Change Editable Encryption Key */

/** @name Can Set/Change Editable Encryption Key (Safe)
 * @{ */
 /** Check [editable] controller's encryption key be set or modified.
  * @post If valid, user may call @ref SA_SetEditableControllerEncryptionKeySafe.
  * @param[in] cda        Controller data area.
  * @param[in] key        C-string (null-terminated) desired key or NULL to 'unset' a key change (Also see @ref SA_CheckEncryptionMasterKey).
  *                       If NULL, key is not checked: This is useful for checking if an
  *                          operation can be performed without user input.
  *                       If "", key changes are discarded: This is useful for canceling a key
  *                          queued for change.
  * @param[in] key_size   Size of the key C-string buffer.
  * @return @ref kCanSetEditableControllerEncryptionKeyOK or a [reason](@ref storc_configuration_edit_encryption_key) the key could not be set/modified.
  *
  * __Example__
  * @code
  * if (SA_CanSetEditableControllerEncryptionKeySafe(cda, "MyKey", sizeof("MyKey")) == kCanSetEditableControllerEncryptionKeyOK)
  * {
  *    if (SA_SetEditableControllerEncryptionKeySafe(cda, "MyKey", sizeof("MyKey")) != kSetEditableControllerEncryptionKeyOK)
  *    {
  *        fprintf(stderr, "Failed to set encryption key\n");
  *    }
  * }
  * @endcode
  */
SA_BYTE SA_CanSetEditableControllerEncryptionKeySafe(PCDA cda, const char * key, size_t key_size);
   /** Encryption key can be set.
    * @outputof SA_CanSetEditableControllerEncryptionKey.
    * @outputof SA_CanSetEditableControllerEncryptionKeySafe.
    * @allows SA_SetEditableControllerEncryptionKey.
    * @allows SA_SetEditableControllerEncryptionKeySafe.
    */
   #define kCanSetEditableControllerEncryptionKeyOK                           1
   /** Encryption key cannot be set because there is not editable configuration.
    * @outputof SA_CanSetEditableControllerEncryptionKey.
    * @outputof SA_CanSetEditableControllerEncryptionKeySafe.
    * @disallows SA_SetEditableControllerEncryptionKey.
    * @disallows SA_SetEditableControllerEncryptionKeySafe.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableControllerEncryptionKeyNoEditableConfig             2
   /** Encryption key cannot be set because encryption has not been enabled.
    * @outputof SA_CanSetEditableControllerEncryptionKey.
    * @outputof SA_CanSetEditableControllerEncryptionKeySafe.
    * @disallows SA_SetEditableControllerEncryptionKey.
    * @disallows SA_SetEditableControllerEncryptionKeySafe.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableControllerEncryptionKeyEncryptionUnconfigured       3
   /** Encryption key cannot be set because express local encryption is enabled.
    * @outputof SA_CanSetEditableControllerEncryptionKey.
    * @outputof SA_CanSetEditableControllerEncryptionKeySafe.
    * @disallows SA_SetEditableControllerEncryptionKey.
    * @disallows SA_SetEditableControllerEncryptionKeySafe.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableControllerEncryptionKeyUsingExpressEncryption       4
   /** Encryption key cannot be set because the key is invalid.
    * @outputof SA_CanSetEditableControllerEncryptionKey.
    * @outputof SA_CanSetEditableControllerEncryptionKeySafe.
    * @disallows SA_SetEditableControllerEncryptionKey.
    * @disallows SA_SetEditableControllerEncryptionKeySafe.
    * @see SA_CheckEncryptionMasterKey.
    */
   #define kCanSetEditableControllerEncryptionKeyInvalidKey                   5
   /** Encryption key cannot be set because offline encrypted logical drive exists.
    * @outputof SA_CanSetEditableControllerEncryptionKey.
    * @outputof SA_CanSetEditableControllerEncryptionKeySafe.
    * @disallows SA_SetEditableControllerEncryptionKey.
    * @disallows SA_SetEditableControllerEncryptionKeySafe.
    * @see SA_CheckEncryptionMasterKey.
    */
   #define kCanSetEditableControllerEncryptionKeyOfflineEncryptedDriveExists  6
   /** Encryption key cannot be modified because the feature is not enabled.
    * @outputof SA_CanSetEditableControllerEncryptionKey.
    * @outputof SA_CanSetEditableControllerEncryptionKeySafe.
    * @disallows SA_SetEditableControllerEncryptionKey.
    * @disallows SA_SetEditableControllerEncryptionKeySafe.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanSetEditableControllerEncryptionKeyChangeNotEnabled             7
   /** Encryption key cannot be modified because a master key reset is in progress.
    * @outputof SA_CanSetEditableControllerEncryptionKey.
    * @outputof SA_CanSetEditableControllerEncryptionKeySafe.
    * @disallows SA_SetEditableControllerEncryptionKey.
    * @disallows SA_SetEditableControllerEncryptionKeySafe.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanSetEditableControllerEncryptionKeyMasterKeyResetInProgress     8
   /** Encryption key cannot be set because the key is an invalid key size.
    * @outputof SA_CanSetEditableControllerEncryptionKeySafe.
    * @disallows SA_SetEditableControllerEncryptionKeySafe.
    */
   #define kCanSetEditableControllerEncryptionKeyInvalidKeySize               9
/** @} */ /* Can Set/Change Editable Encryption Key (Safe) */

/** @name Set/Change Editable Encryption Key
 * @{ */
/** Set/modify [editable] controller's encryption key.
 * @deprecated Use @ref SA_SetEditableControllerEncryptionKeySafe instead.
 * @attention This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_SetEditableControllerEncryptionKeySafe).
 * @pre The user has used @ref SA_CanSetEditableControllerEncryptionKey to ensure the operation can be performed.
 * @param[in] cda  Controller data area.
 * @param[in] key  C-string (null-terminated) desired key.
 *                 If "", key changes are discarded: This is useful for canceling a key
 *                   queued for change.
 * @return @ref kSetEditableControllerEncryptionKeyOK or status indicating failure.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableControllerEncryptionKey(cda, "MyKey") == kCanSetEditableControllerEncryptionKeyOK)
 * {
 *    if (SA_SetEditableControllerEncryptionKey(cda, "MyKey") != kSetEditableControllerEncryptionKeyOK)
 *    {
 *        fprintf(stderr, "Failed to set encryption key\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableControllerEncryptionKey(PCDA cda, const char * key);
/** @} */ /* Set/Change Editable Encryption Key */

/** @name Set/Change Editable Encryption Key (Safe)
 * @{ */
 /** Set/modify [editable] controller's encryption key.
  * @pre The user has used @ref SA_CanSetEditableControllerEncryptionKeySafe to ensure the operation can be performed.
  * @param[in] cda        Controller data area.
  * @param[in] key        C-string (null-terminated) desired key.
  *                       If "", key changes are discarded: This is useful for canceling a key
  *                         queued for change.
  * @param[in] key_size   Size of the key C-string buffer.
  * @return @ref kSetEditableControllerEncryptionKeyOK or status indicating failure.
  *
  * __Example__
  * @code
  * if (SA_CanSetEditableControllerEncryptionKeySafe(cda, "MyKey", sizeof("MyKey")) == kCanSetEditableControllerEncryptionKeyOK)
  * {
  *    if (SA_SetEditableControllerEncryptionKeySafe(cda, "MyKey", sizeof("MyKey")) != kSetEditableControllerEncryptionKeyOK)
  *    {
  *        fprintf(stderr, "Failed to set encryption key\n");
  *    }
  * }
  * @endcode
  */
SA_BYTE SA_SetEditableControllerEncryptionKeySafe(PCDA cda, const char * key, size_t key_size);
   /** Failed to set/modify the encryption key.
    * @outputof SA_SetEditableControllerEncryptionKey.
    * @outputof SA_SetEditableControllerEncryptionKeySafe.
    */
   #define kSetEditableControllerEncryptionKeyFailed  0
   /** Successfully set/modified the encryption key.
    * @outputof SA_SetEditableControllerEncryptionKey.
    * @outputof SA_SetEditableControllerEncryptionKeySafe.
    */
   #define kSetEditableControllerEncryptionKeyOK      1
/** @} */ /* Set/Change Editable Encryption Key (Safe) */

/** @} */ /* storc_configuration_edit_encryption_key  */

/** @defgroup storc_configuration_edit_encryption_password Encryption Set/Change Password
 * @brief Setup/configure the encryption user passwords.
 * @{ */

/** @name Can Set/Change Editable Encryption Password
 * @{ */
/** Used to check if a password is valid for encryption on a given controller.
 *
 * @warning Password checking will stop on first NULL character or until `buffer_size` characters have been checked.
 * @param[in] cda              Controller handler.
 * @param[in] password_buffer  Password buffer (not required to be null-terminated).
 * @param[in] buffer_size      Number of characters in `password_buffer`
 * @return @ref kCheckEncryptionPasswordOk if password is valid or a map indicating what properties of the password are invalid.
 */
SA_BYTE SA_CheckEncryptionPassword(PCDA cda, const char *password_buffer, SA_WORD buffer_size);
   /** Password is valid for encryption on this controller.
    * @outputof SA_CheckEncryptionPassword.
    */
   #define kCheckEncryptionPasswordOk                     0x00
   /** Password is not valid for encryption on this controller
    * because the password does not contain a lowercase letter.
    * @outputof SA_CheckEncryptionPassword.
    */
   #define kCheckEncryptionPasswordMissingLowerCaseLetter 0x01
   /** Password is not valid for encryption on this controller
    * because the password does not contain an uppercase letter.
    * @outputof SA_CheckEncryptionPassword.
    */
   #define kCheckEncryptionPasswordMissingUpperCaseLetter 0x02
   /** Password is not valid for encryption on this controller
    * because the password does not contain a number.
    * @outputof SA_CheckEncryptionPassword.
    */
   #define kCheckEncryptionPasswordMissingNumber          0x04
   /** Password is not valid for encryption on this controller
    * because the password does not contain a special character.
    * A special character is any printable non-space/non-alphanumeric character.
    * @outputof SA_CheckEncryptionPassword.
    */
   #define kCheckEncryptionPasswordMissingSpecialChar     0x08
   /** Password is not valid for encryption on this controller
    * because the password is too short.
    * @outputof SA_CheckEncryptionPassword.
    */
   #define kCheckEncryptionPasswordTooShort               0x10
   /** Password is not valid for encryption on this controller
    * because the password is too long.
    * @outputof SA_CheckEncryptionPassword.
    */
   #define kCheckEncryptionPasswordTooLong                0x20
   /** Password is not valid for encryption on this controller
    * because the password contains an invalid character.
    * @outputof SA_CheckEncryptionPassword.
    */
   #define kCheckEncryptionPasswordInvalidChar            0x40
   /** Password is not valid for encryption on this controller
    * but for an unknown reason.
    * @outputof SA_CheckEncryptionPassword.
    */
   #define kCheckEncryptionPasswordInvalidUnknown         0x80
/** @} */ /* Can Set/Change Editable Encryption Password */

/** @name Can Set/Change Editable Encryption Password
 * @{ */
/** Check [editable] controller's encryption password be set or modified.
 * @deprecated Use @ref SA_CanSetEditableControllerEncryptionPasswordSafe instead.
 * @attention This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CanSetEditableControllerEncryptionPasswordSafe).
 * @post If valid, user may call @ref SA_SetEditableControllerEncryptionPassword.
 * @param[in] cda       Controller data area.
 * @param[in] user      Encryption user (one of [Encryption User](@ref storc_configuration_edit_encryption)).
 * @param[in] password  C-string (null-terminated) desired password (also see @ref SA_CheckEncryptionPassword).
 *                      If NULL, password is not checked: This is useful for checking that
 *                          an operation can be performed without user input.
 *                      If "", password changes are discarded: This is useful for canceling
 *                          a change to the password without losing other desired changes.
 * @return @ref kCanSetEditableControllerEncryptionPasswordOK or a [reason](@ref storc_configuration_edit_encryption_password) the password could not be set/modified.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableControllerEncryptionPassword(cda, kControllerEncryptionCryptoOfficer, "MyP@ssword12345") == kCanSetEditableControllerEncryptionPasswordOK)
 * {
 *    if (SA_SetEditableControllerEncryptionPassword(cda, kControllerEncryptionCryptoOfficer, "MyP@ssword12345") != kSetEditableControllerEncryptionPasswordOK)
 *    {
 *        fprintf(stderr, "Failed to set encryption password\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableControllerEncryptionPassword(PCDA cda, SA_BYTE user, const char * password);
/** @} */ /* Can Set/Change Editable Encryption Password */

/** @name Can Set/Change Editable Encryption Password (Safe)
 * @{ */
/** Check [editable] controller's encryption password be set or modified.
 * @post If valid, user may call @ref SA_CanSetEditableControllerEncryptionPasswordSafe.
 * @param[in] cda             Controller data area.
 * @param[in] user            Encryption user (one of [Encryption User](@ref storc_configuration_edit_encryption)).
 * @param[in] password        C-string (null-terminated) desired password (also see @ref SA_CheckEncryptionPassword).
 *                            If NULL, password is not checked: This is useful for checking that
 *                                an operation can be performed without user input.
 *                            If "", password changes are discarded: This is usefule for canceling
 *                                a change to the password without losing other desired changes.
 * @param[in] password_size   Size of password C-string buffer.
 * @return @ref kCanSetEditableControllerEncryptionPasswordOK or a [reason](@ref storc_configuration_edit_encryption_password) the password could not be set/modified.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableControllerEncryptionPasswordSafe(cda, kControllerEncryptionCryptoOfficer, "MyP@ssword12345", sizeof("MyP@ssword12345")) == kCanSetEditableControllerEncryptionPasswordOK)
 * {
 *    if (SA_SetEditableControllerEncryptionPasswordSafe(cda, kControllerEncryptionCryptoOfficer, "MyP@ssword12345", sizeof("MyP@ssword12345")) != kSetEditableControllerEncryptionPasswordOK)
 *    {
 *        fprintf(stderr, "Failed to set encryption password\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableControllerEncryptionPasswordSafe(PCDA cda, SA_BYTE user, const char * password, size_t password_size);
   /** Encryption password can be set.
    * @outputof SA_CanSetEditableControllerEncryptionPassword.
    * @outputof SA_CanSetEditableControllerEncryptionPasswordSafe.
    * @allows SA_SetEditableControllerEncryptionPassword.
    * @allows SA_SetEditableControllerEncryptionPasswordSafe.
    */
   #define kCanSetEditableControllerEncryptionPasswordOK                      1
   /** Encryption password cannot be set because there is not editable configuration.
    * @outputof SA_CanSetEditableControllerEncryptionPassword.
    * @outputof SA_CanSetEditableControllerEncryptionPasswordSafe.
    * @disallows SA_SetEditableControllerEncryptionPassword.
    * @disallows SA_SetEditableControllerEncryptionPasswordSafe.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableControllerEncryptionPasswordNoEditableConfig        2
   /** Encryption password cannot be set because encryption has not been configured.
    * @outputof SA_CanSetEditableControllerEncryptionPassword.
    * @outputof SA_CanSetEditableControllerEncryptionPasswordSafe.
    * @disallows SA_SetEditableControllerEncryptionPassword.
    * @disallows SA_SetEditableControllerEncryptionPasswordSafe.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableControllerEncryptionPasswordEncryptionUnconfigured  3
   /** Encryption password cannot be set because express local encryption is enabled.
    * @outputof SA_CanSetEditableControllerEncryptionPassword.
    * @outputof SA_CanSetEditableControllerEncryptionPasswordSafe.
    * @disallows SA_SetEditableControllerEncryptionPassword.
    * @disallows SA_SetEditableControllerEncryptionPasswordSafe.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableControllerEncryptionPasswordUsingExpressEncryption  4
   /** Encryption password cannot be set because the password is invalid.
    * @outputof SA_CanSetEditableControllerEncryptionPassword.
    * @outputof SA_CanSetEditableControllerEncryptionPasswordSafe.
    * @disallows SA_SetEditableControllerEncryptionPassword.
    * @disallows SA_SetEditableControllerEncryptionPasswordSafe.
    * @see SA_CheckEncryptionPassword.
    */
   #define kCanSetEditableControllerEncryptionPasswordInvalidPassword         5
   /** Encryption password cannot be set because the user is invalid/unknown.
    * @outputof SA_CanSetEditableControllerEncryptionPassword.
    * @outputof SA_CanSetEditableControllerEncryptionPasswordSafe.
    * @disallows SA_SetEditableControllerEncryptionPassword.
    * @disallows SA_SetEditableControllerEncryptionPasswordSafe.
    */
   #define kCanSetEditableControllerEncryptionPasswordInvalidUser             6
   /** Encryption password cannot be set because the user account feature is not enabled.
    * @outputof SA_CanSetEditableControllerEncryptionPassword.
    * @outputof SA_CanSetEditableControllerEncryptionPasswordSafe.
    * @disallows SA_SetEditableControllerEncryptionPassword.
    * @disallows SA_SetEditableControllerEncryptionPasswordSafe.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanSetEditableControllerEncryptionPasswordUserNotEnabled          7
   /** Encryption password cannot be set because the password size is invalid.
    * @outputof SA_CanSetEditableControllerEncryptionPasswordSafe.
    * @disallows SA_SetEditableControllerEncryptionPasswordSafe.
    */
   #define kCanSetEditableControllerEncryptionPasswordInvalidPasswordSize     8
/** @} */ /* Can Set/Change Editable Encryption Password (Safe) */

/** @name Set/Change Editable Encryption Password
 * @{ */
/** Set/modify [editable] controller's encryption password.
 * @deprecated Use @ref SA_SetEditableControllerEncryptionPasswordSafe instead.
 * @attention This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_SetEditableControllerEncryptionPasswordSafe).
 * @pre The user has used @ref SA_CanSetEditableControllerEncryptionPassword to ensure the operation can be performed.
 * @param[in] cda       Controller data area.
 * @param[in] user      Encryption user (one of [Encryption User](@ref storc_configuration_edit_encryption)).
 * @param[in] password  C-string (null-terminated) desired password (also see @ref SA_CheckEncryptionPassword).
 *                      If "", password changes are discarded: This is useful for canceling
 *                          a change to the password without losing other desired changes.
 * @return @ref kSetEditableControllerEncryptionPasswordOK or status indicating failure.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableControllerEncryptionPassword(cda, kControllerEncryptionCryptoOfficer, "MyP@ssword12345") == kCanSetEditableControllerEncryptionPasswordOK)
 * {
 *    if (SA_SetEditableControllerEncryptionPassword(cda, kControllerEncryptionCryptoOfficer, "MyP@ssword12345") != kSetEditableControllerEncryptionPasswordOK)
 *    {
 *        fprintf(stderr, "Failed to set encryption password\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableControllerEncryptionPassword(PCDA cda, SA_BYTE user, const char * password);
/** @} */ /* Set/Change Editable Encryption Password */

/** @name Set/Change Editable Encryption Password (Safe)
 * @{ */
/** Set/modify [editable] controller's encryption password.
 * @pre The user has used @ref SA_CanSetEditableControllerEncryptionPasswordSafe to ensure the operation can be performed.
 * @param[in] cda             Controller data area.
 * @param[in] user            Encryption user (one of [Encryption User](@ref storc_configuration_edit_encryption)).
 * @param[in] password        C-string (null-terminated) desired password (also see @ref SA_CheckEncryptionPassword).
 *                            If "", password changes are discarded: This is usefule for canceling
 *                                a change to the password without losing other desired changes.
 * @param[in] password_size   Size of password C-string buffer.
 * @return @ref kSetEditableControllerEncryptionPasswordOK or status indicating failure.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableControllerEncryptionPasswordSafe(cda, kControllerEncryptionCryptoOfficer, "MyP@ssword12345", sizeof("MyP@ssword12345")) == kCanSetEditableControllerEncryptionPasswordOK)
 * {
 *    if (SA_SetEditableControllerEncryptionPasswordSafe(cda, kControllerEncryptionCryptoOfficer, "MyP@ssword12345", sizeof("MyP@ssword12345")) != kSetEditableControllerEncryptionPasswordOK)
 *    {
 *        fprintf(stderr, "Failed to set encryption password\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableControllerEncryptionPasswordSafe(PCDA cda, SA_BYTE user, const char * password, size_t password_size);
   /** Failed to set/modify the encryption password.
    * @outputof SA_SetEditableControllerEncryptionPassword.
    * @outputof SA_SetEditableControllerEncryptionPasswordSafe.
    */
   #define kSetEditableControllerEncryptionPasswordFailed  0
   /** Successfully set/modified the encryption password.
    * @outputof SA_SetEditableControllerEncryptionPassword.
    * @outputof SA_SetEditableControllerEncryptionPasswordSafe.
    */
   #define kSetEditableControllerEncryptionPasswordOK      1
/** @} */ /* Set/Change Editable Encryption Password (Safe) */

/** @} */ /* storc_configuration_edit_encryption_password  */

/** @defgroup storc_configuration_edit_encryption_allow_plaintext Encryption Allow Future Plaintext
 * @brief Setup/configure the encryption setting to allow/deny future plaintext logical drives.
 * @{ */
/** @name Can Allow Plaintext Volumes
 * @{ */
/** Can the controller encryption allow plaintext volumes to be set on new logical drives?
 * @post If valid, user may call @ref SA_AllowEditableEncryptionPlaintextVolumes.
 * @param[in] cda                   Controller handler.
 * @param[in] allow_plaintext       @ref kTrue allow plaintext volumes when controller encryption is configured for new logical drives,
 * @ref kFalse disallow plaintext volumes when controller encryption is configured for new logical drives
 * @return  [Can Allow Editable Encryption Plaintext Returns](@ref  storc_configuration_edit_encryption_allow_plaintext)
 *
 * __Example__
 * @code
 * if (SA_CanAllowEditableEncryptionPlaintextVolumes(cda, kTrue) == kCanAllowPlaintextOK)
 * {
 *    if (SA_AllowEditableEncryptionPlaintextVolumes(cda, kTrue) != kAllowEditableEncryptionPlaintextSuccess)
 *    {
 *       fprintf(stderr, "Failed to allow plaintext.\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanAllowEditableEncryptionPlaintextVolumes(PCDA cda, SA_BOOL allow_plaintext);
   /** Plaintext is allowed on this controller.
    * @outputof SA_CanAllowEditableEncryptionPlaintextVolumes.
    * @allows SA_AllowEditableEncryptionPlaintextVolumes.
    */
   #define kCanAllowPlaintextOK                                   0x01
   /** Plaintext is not allowed on this controller because there is no editable configuration created.
    * @outputof SA_CanAllowEditableEncryptionPlaintextVolumes.
    * @disallows SA_AllowEditableEncryptionPlaintextVolumes.
    */
   #define kCanAllowPlaintextNoEditableConfig                     0x02
   /** Plaintext is not allowed on this controller because encryption is not enabled.
    * @outputof SA_CanAllowEditableEncryptionPlaintextVolumes.
    * @disallows SA_AllowEditableEncryptionPlaintextVolumes.
    */
   #define kCanAllowPlaintextEncryptionUnconfigured               0x03
    /** Plaintext is not allowed on this controller because encryption is invalid.
    * @outputof SA_CanAllowEditableEncryptionPlaintextVolumes.
    * @disallows SA_AllowEditableEncryptionPlaintextVolumes.
    */
   #define kCanAllowPlaintextEncryptionInvalid                    0x04
   /** Plaintext is not allowed on this controller because encryption is express local.
    * @outputof SA_CanAllowEditableEncryptionPlaintextVolumes.
    * @disallows SA_AllowEditableEncryptionPlaintextVolumes.
    */
   #define kCanAllowPlaintextEncryptionExpressLocal               0x05
   /** Plaintext value trying to be set is already set on this controller.
    * @outputof SA_CanAllowEditableEncryptionPlaintextVolumes.
    * @disallows SA_AllowEditableEncryptionPlaintextVolumes.
    */
   #define kCanAllowPlaintextValueAlreadySet                      0x06
   /** Plaintext is not allowed when there new editable logical drives.
    * @outputof SA_CanAllowEditableEncryptionPlaintextVolumes.
    * @disallows SA_AllowEditableEncryptionPlaintextVolumes.
    */
   #define kCanAllowPlaintextLogicalDriveExists                   0x07
   /** Plaintext is not allowed because the feature is not enabled.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    * @outputof SA_CanAllowEditableEncryptionPlaintextVolumes.
    * @disallows SA_AllowEditableEncryptionPlaintextVolumes.
    */
   #define kCanAllowPlaintextNotSupported                         0x08
/** @} */ /* Can Allow Plaintext Volumes */


/** @name Allow Plaintext
* @{ */
/** Allow the controller encryption to have plaintext volumes.
* @pre The user has used @ref SA_CanAllowEditableEncryptionPlaintextVolumes to ensure the operation is allowed.
* @param[in] cda                   Controller handler.
* @param[in] allow_plaintext       @ref kTrue allow plaintext volumes when controller encryption is configured for new logical drives,
* @ref kFalse disallow plaintext volumes when controller encryption is configured for new logical drives
* @return  [Allow Editable Encryption Plaintext Returns](@ref  storc_configuration_edit_encryption_allow_plaintext)
*
* __Example__
* @code
* if (SA_CanAllowEditableEncryptionPlaintextVolumes(cda, kTrue) == kCanAllowPlaintextOK)
* {
*    if (SA_AllowEditableEncryptionPlaintextVolumes(cda, kTrue) != kAllowPlaintextSuccess)
*    {
*       fprintf(stderr, "Failed to allow plaintext.\n");
*    }
* }
* @endcode
*/
SA_BYTE SA_AllowEditableEncryptionPlaintextVolumes(PCDA cda, SA_BOOL allow_plaintext);
   /** Failed to allow plaintext volumes on the controller.
   * @outputof SA_AllowEditableEncryptionPlaintextVolumes
   */
   #define kAllowPlaintextFailedUnknown            0x00
   /** Successfully allowed plaintext volumes on the controller
   * @outputof SA_AllowEditableEncryptionPlaintextVolumes
   */
   #define kAllowPlaintextSuccess                  0x01
/** @} */ /* Allow Plaintext */


/** @name Controller Plaintext Configurations
 * @{ */
#define kControllerPlaintextDisabled   0x00
#define kControllerPlaintextEnabled    0x01
#define kControllerPlaintextInvalid    0xFF
/** @} */

/** @name Get Editable Encryption Controller Plaintext Allowed
 * @{ */
/** Return current value of editable controller's plaintext settings.
 * @return [Controller Plaintext Configurations](@ref storc_configuration_edit_encryption_allow_plaintext)
 */
SA_BYTE SA_GetEditableControllerEncryptionPlaintextAllowed(PCDA cda);
/** @} */ /* Get Editable Encryption Controller Plaintext Allowed */

/** @} */ /* storc_configuration_edit_encryption_allow_plaintext  */

/** @defgroup storc_configuration_edit_encryption_toggle Encryption Enable/Disable
 * @brief Enable/disable encryption while retaining the configuration.
 * @{ */

/** @name Get Editable Controller Encryption Enable/Disable
 * @{ */
/** Verify the controller's pending encryption state.
 * @param[in] cda      Controller handle.
 * @return @ref kTrue iff controller encryption will be enabled.
 * @return @ref kFalse iff controller encryption will be disabled.
 */
SA_BOOL SA_GetEditableControllerEncryptionToggle(PCDA cda);
/** @} */ /* Get Editable Controller Encryption Enable/Disable */

/** @name Can Enable/Disable Editable Controller Encryption
 * @{ */
/** Can editable controller encryption be enabled/disabled?
 * @post If valid, user may call @ref SA_ToggleEditableControllerEncryption.
 * @param[in] cda       Controller data area.
 * @param[in] toggle    @ref kTrue to enable controller encryption, @ref kFalse to disable controller encryption.
 * @return See [Can Enable/Disable Editable Controller Encryption](@ref storc_configuration_edit_encryption_toggle).
 * __Example__
 * @code
 * if (SA_CanToggleEditableControllerEncryption(cda, kTrue) == kCanToggleEditableControllerEncryptionOK)
 * {
 *    if (SA_ToggleEditableControllerEncryption(cda, kTrue) != kToggleEditableControllerEncryptionOK)
 *    {
 *       fprintf(stderr, "Failed to enable encryption for the controller \n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanToggleEditableControllerEncryption(PCDA cda, SA_BOOL toggle);
   /** Editable controller encryption may be enabled or disabled using @ref SA_ToggleEditableControllerEncryption.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @allows SA_ToggleEditableControllerEncryption.
    */
   #define kCanToggleEditableControllerEncryptionOK                              1
   /** Cannot enable/disable encryption for the controller because the editable configuration has not been created yet.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @disallows SA_ToggleEditableControllerEncryption.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanToggleEditableControllerEncryptionNoEditableConfig                2
   /** Cannot enable/disable encryption for the controller because controller encryption has not been configured.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @disallows SA_ToggleEditableControllerEncryption.
    */
   #define kCanToggleEditableControllerEncryptionNotConfigured                   3
   /** Cannot enable/disable encryption for the controller because the encryption is set to express local.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @disallows SA_ToggleEditableControllerEncryption.
    */
   #define kCanToggleEditableControllerEncryptionExpressEncryption               4
   /** Cannot enable/disable encryption for the controller because this value is already set.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @disallows SA_ToggleEditableControllerEncryption.
    */
   #define kCanToggleEditableControllerEncryptionAlreadySet                      5
   /** Cannot enable/disable encryption for the controller because there are logical drives currently rekeying.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @disallows SA_ToggleEditableControllerEncryption.
    */
   #define kCanToggleEditableControllerEncryptionLogicalDrivesRekeying           6
   /** Cannot enable/disable encryption for the controller because there are new logical drives.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @disallows SA_ToggleEditableControllerEncryption.
    */
   #define kCanToggleEditableControllerEncryptionNewLogicalDrivesExist           7
   /** Cannot disable encryption because an existing encrypted logical drive has a oustanding/pending operation.
    * Such operation might be:
    * * Erase in progress.
    * * Rebuild in progress
    * * Transformation in progress.
    * * Transformation queued.
    * * Initializing (RPI) in progress.
    * @see storc_status_ld.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @disallows SA_ToggleEditableControllerEncryption.
    */
   #define kCanToggleEditableControllerEncryptionPendingOperation                8
   /** Cannot enable/disable encryption for the controller because the feature is not supported/enabled.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @disallows SA_ToggleEditableControllerEncryption.
    */
   #define kCanToggleEditableControllerEncryptionNotSupported                    9
   /** Cannot disable encryption for the controller because encrypted logical drives exist.
    * @see LOGICAL_DRIVE_ENCRYPTION_STATUS.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @disallows SA_ToggleEditableControllerEncryption.
    */
   #define kCanToggleEditableControllerEncryptionEncryptedLogicalDrivesExist     10
   /** Cannot disable encryption for the controller because the controller boot password is set.
    * @see SA_GetControllerEncryptionStatus.
    * @outputof SA_CanToggleEditableControllerEncryption.
    * @disallows SA_ToggleEditableControllerEncryption.
    */
   #define kCanToggleEditableControllerEncryptionBootPasswordSet                 11
/** @} */ /* Can Enable/Disable Editable Controller Encryption */

/** @name Enable/Disable Editable Controller Encryption
 * @{ */
/** Change the editable controller encryption to be enabled/disabled
 * @pre The user has used @ref SA_CanToggleEditableControllerEncryption to ensure the operation can be performed.
 * @param[in]           cda Controller data area.
 * @param[in] toggle    @ref kTrue to enable controller encryption, @ref kFalse to disable controller encryption.
 * @return See [Enable/Disable Editable Controller Encryption](@ref storc_configuration_edit_encryption_toggle).
 * __Example__
 * @code
 * if (SA_CanToggleEditableControllerEncryption(cda, kTrue) == kCanToggleEditableControllerEncryptionOK)
 * {
 *    if (SA_ToggleEditableControllerEncryption(cda, kTrue) != kToggleEditableControllerEncryptionOK)
 *    {
 *       fprintf(stderr, "Failed to enable encryption for the controller \n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_ToggleEditableControllerEncryption(PCDA cda, SA_BOOL toggle);
   /** Failed to toggle editable controller encryption.
    * @outputof SA_ToggleEditableControllerEncryption.
    */
   #define kToggleEditableControllerEncryptionFailedUnknown       0
   /** Succcessfully toggled editable controller encryption.
    * @outputof SA_ToggleEditableControllerEncryption.
    */
   #define kToggleEditableControllerEncryptionOK                  1
/** @} */ /* Enable/Disable Editable Controller Encryption */

/** @} */ /* storc_configuration_edit_encryption_toggle  */

/** @defgroup storc_configuration_edit_encryption_recovery_question_answer Encryption Password Recovery Question/Answer
 * @brief Setup/configure the encryption recovery question and answer.
* @{ */

/** @name Check Encryption Password Recovery Question
 * @{ */
/** Used to check if a password recovery question is valid for encryption on a given controller.
 *
 * @warning Recovery question checking will stop on first NULL character or until `buffer_size` characters have been checked.
 * @param[in] cda              Controller handler.
 * @param[in] recovery_buffer  Recovery question buffer (not required to be null-terminated).
 * @param[in] buffer_size      Number of characters in `recovery_buffer`
 * @return @ref kCheckRecoveryQuestionOk if recovery question is valid.
 */
SA_BYTE SA_CheckEncryptionRecoveryQuestion(PCDA cda, const char *recovery_buffer, SA_WORD buffer_size);
   /** Recovery question is valid for encryption on this controller.
    * @outputof SA_CheckEncryptionRecoveryQuestion.
    */
   #define kCheckRecoveryQuestionOk                     0x00
   /** Recovery question is not valid for encryption on this controller
    * because the recovery question is too short.
    * @outputof SA_CheckEncryptionRecoveryQuestion.
    */
   #define kCheckRecoveryQuestionTooShort               0x01
   /** Recovery question is not valid for encryption on this controller
    * because the recovery question is too long.
    * @outputof SA_CheckEncryptionRecoveryQuestion.
    */
   #define kCheckRecoveryQuestionTooLong                0x02
   /** Recovery question is not valid for encryption on this controller
    * because the recovery question contains an invalid character.
    * @outputof SA_CheckEncryptionRecoveryQuestion.
    */
   #define kCheckRecoveryQuestionInvalidChar            0x04
   /** Recovery question is not valid for encryption on this controller
    * but for an unknown reason.
    * @outputof SA_CheckEncryptionRecoveryQuestion.
    */
   #define kCheckRecoveryQuestionInvalidUnknown         0x08
/** @} */ /*  Check Encryption Password Recovery Question */

/** @name Check Encryption Password Recovery Answer
 * @{ */
/** Used to check if a password recovery answer is valid for encryption on a given controller.
 *
 * @warning Recovery answer checking will stop on first NULL character or until `buffer_size` characters have been checked.
 * @param[in] cda                 Controller handler.
 * @param[in] recovery_buffer     Recovery answer buffer (not required to be null-terminated).
 * @param[in] buffer_size         Number of characters in `recovery_buffer`
 * @return @ref kCheckRecoveryAnswerOk if recovery answer is valid.
 * @see SA_CanSetEditableEncryptionQuestionAndAnswerSafe
 */
SA_BYTE SA_CheckEncryptionRecoveryAnswer(PCDA cda, const char *recovery_buffer, SA_WORD buffer_size);
   /** Recovery answer is valid for encryption on this controller.
    * @outputof SA_CheckEncryptionRecoveryAnswer.
    */
   #define kCheckRecoveryAnswerOk                     0x00
   /** Recovery answer is not valid for encryption on this controller
    * because the recovery answer is too short.
    * @outputof SA_CheckEncryptionRecoveryAnswer.
    */
   #define kCheckRecoveryAnswerTooShort               0x01
   /** Recovery answer is not valid for encryption on this controller
    * because the recovery answer is too long.
    * @outputof SA_CheckEncryptionRecoveryAnswer.
    */
   #define kCheckRecoveryAnswerTooLong                0x02
   /** Recovery answer is not valid for encryption on this controller
    * because the recovery answer contains an invalid character.
    * @outputof SA_CheckEncryptionRecoveryAnswer.
    */
   #define kCheckRecoveryAnswerInvalidChar            0x04
   /** Recovery answer is not valid for encryption on this controller
    * but for an unknown reason.
    * @outputof SA_CheckEncryptionRecoveryAnswer.
    */
   #define kCheckRecoveryAnswerInvalidUnknown         0x08
/** @} */ /*  Check Encryption Password Recovery Answer */

/** @name Get Editable Encryption Recovery Question
* @{ */
/** Used to get a recovery question for encryption on a given editable controller configuration.
* @pre `buffer_size` must be at least 257 bytes to contain the recovery question and terminating NULL.
*
* @param[in] cda          Controller handler.
* @param[in] buffer       Recovery question buffer.
* @param[in] buffer_size  Number of characters in `question_buffer`
* @return @ref kGetEditableEncryptionRecoveryQuestionOK if recovery question in editable configuration is copied to question_buffer.
* @return Empty string if given question_buffer was not large enough to store the encryption recovery question.
* @see SA_SetEditableEncryptionQuestionAndAnswerSafe
*/

SA_BYTE SA_GetEditableEncryptionRecoveryQuestion(PCDA cda, char *buffer, size_t buffer_size);
/** Successfully retrieved the editable configuration encryption recovery question.
* @outputof SA_GetEditableEncryptionRecoveryQuestion.
*/
#define kGetEditableEncryptionRecoveryQuestionOK                           0x01
/** Failed to get the editable configuration encryption recovery question because no editable configuration has been created.
* @outputof SA_GetEditableEncryptionRecoveryQuestion.
*/
#define kGetEditableEncryptionRecoveryQuestionNoEditableConfigCreated      0x02
/** Failed to get the editable configuration encryption recovery question because the output buffer is NULL.
* @outputof SA_GetEditableEncryptionRecoveryQuestion.
*/
#define kGetEditableEncryptionRecoveryQuestionNullBuffer                   0x03
/** Failed to get the editable configuration encryption recovery question because the output buffer is too small.
* @outputof SA_GetEditableEncryptionRecoveryQuestion.
*/
#define kGetEditableEncryptionRecoveryQuestionBufferSizeTooSmall           0x04
/** @} */ /* Get Editable Configuration Encryption Recovery Question */

/** @name Can Set Editable Encryption Password Recovery Question/Answer
 * @{ */
/** Used to check if an [editable] controller's recovery password question/answer can be set or modified.
 *
 * @deprecated Use @ref SA_CanSetEditableEncryptionQuestionAndAnswerSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this
 *          function (@ref SA_CanSetEditableEncryptionQuestionAndAnswerSafe).
 * @post If valid, user may call @ref SA_SetEditableEncryptionQuestionAndAnswer.
 * @param[in] cda       Controller handler.
 * @param[in] question  Recovery question C-string (also see @ref SA_CheckEncryptionRecoveryQuestion).
 *                      If NULL, question is not checked: This is useful for checking if an
 *                        operation can be performed without user input.
 *                      If an empty string (and `answer` is also empty), check if an outstanding
 *                        change to the encryption recovery q&a can be dropped.
 * @param[in] answer    Recovery answer C-string (also see @ref SA_CheckEncryptionRecoveryAnswer).
 *                      If NULL, answer is not checked: This is useful for checking if an
 *                        operation can be performed without user input.
 *                      If an empty string (and `question` is also empty), check if an outstanding
 *                        change to the encryption recovery q&a can be dropped.
 * @return [Can Set Editable Encryption Password Recovery Question/Answer](@ref storc_configuration_edit_encryption_recovery_question_answer).
 * @see SA_CheckEncryptionRecoveryQuestion
 * @see SA_CheckEncryptionRecoveryAnswer
 */
SA_BYTE SA_CanSetEditableEncryptionQuestionAndAnswer(PCDA cda, const char *question, const char *answer);
/** @} */ /*  Can Set Editable Encryption Password Recovery Question/Answer */

/** @name Can Set Editable Encryption Password Recovery Question/Answer (Safe)
 * @{ */
/** Used to check if an [editable] controller's recovery password question/answer can be set or modified.
 *
 * @post If valid, user may call @ref SA_SetEditableEncryptionQuestionAndAnswerSafe.
 * @param[in] cda           Controller handler.
 * @param[in] question      Recovery question C-string (also see @ref SA_CheckEncryptionRecoveryQuestion).
 *                          If NULL, question is not checked: This is useful for checking if an
 *                            operation can be performed without user input.
 *                          If an empty string (and `answer` is also empty), check if an outstanding
 *                            change to the encryption recovery q&a can be dropped.
 * @param[in] question_size Size of the recovery question C-string (number of bytes).
 * @param[in] answer        Recovery answer C-string (also see @ref SA_CheckEncryptionRecoveryAnswer).
 *                          If NULL, answer is not checked: This is useful for checking if an
 *                            operation can be performed without user input.
 *                          If an empty string (and `question` is also empty), check if an outstanding
 *                            change to the encryption recovery q&a can be dropped.
 * @param[in] answer_size   Size of the recovery answer C-string (number of bytes).
 * @return [Can Set Editable Encryption Password Recovery Question/Answer](@ref storc_configuration_edit_encryption_recovery_question_answer).
 * @see SA_CheckEncryptionRecoveryQuestion
 * @see SA_CheckEncryptionRecoveryAnswer
 */
SA_BYTE SA_CanSetEditableEncryptionQuestionAndAnswerSafe(PCDA cda, const char *question, size_t question_size, const char *answer, size_t answer_size);
   /** Recovery question/answer can be set.
    * @outputof SA_CanSetEditableEncryptionQuestionAndAnswer.
    * @outputof SA_CanSetEditableEncryptionQuestionAndAnswerSafe.
    * @allows SA_SetEditableEncryptionQuestionAndAnswer.
    * @allows SA_SetEditableEncryptionQuestionAndAnswerSafe.
    */
   #define kCanSetEditableEncryptionRecoveryQuestionAndAnswerOk                           1
    /** Encryption recovery question/answer cannot be set because there is no editable configuration.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswer.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswerSafe.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswer.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswerSafe.
     * @see SA_CreateEditableConfiguration.
     */
   #define kCanSetEditableEncryptionRecoveryQuestionAndAnswerNoEditableConfig             2
    /** Encryption recovery question/answer cannot be set because encryption is not configured.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswer.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswerSafe.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswer.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswerSafe.
     * @see SA_ChangeEditableControllerEncryption.
     */
   #define kCanSetEditableEncryptionRecoveryQuestionAndAnswerEncryptionNotConfigured      3
    /** Encryption recovery question/answer cannot be set for express local encryption.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswer.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswerSafe.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswer.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswerSafe.
     * @see SA_ChangeEditableControllerEncryption.
     */
   #define kCanSetEditableEncryptionRecoveryQuestionAndAnswerForExpressLocal              4
    /** Encryption recovery question cannot be set failed check encryption recovery question.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswer.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswerSafe.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswer.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswerSafe.
     * @see SA_CheckEncryptionRecoveryQuestion.
     */
   #define kCanSetEditableEncryptionRecoveryQuestionAndAnswerInvalidQuestion              5
    /** Encryption recovery question cannot be set failed check encryption recovery answer.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswer.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswerSafe.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswer.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswerSafe.
     * @see SA_CheckEncryptionRecoveryAnswer.
     */
   #define kCanSetEditableEncryptionRecoveryQuestionAndAnswerInvalidAnswer                6
    /** Encryption recovery question change cannot be unset because no recovery question/answer has been set.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswer.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswerSafe.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswer.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswerSafe.
     */
   #define kCanSetEditableEncryptionRecoveryQuestionAndAnswerNoChange                     7
   /** Encryption recovery question size is invalid.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswerSafe.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswerSafe.
     */
   #define kCanSetEditableEncryptionRecoveryQuestionAndAnswerInvalidQuestionSize          8
   /** Encryption recovery answer size is invalid.
     * @outputof SA_CanSetEditableEncryptionQuestionAndAnswerSafe.
     * @disallows SA_SetEditableEncryptionQuestionAndAnswerSafe.
     */
   #define kCanSetEditableEncryptionRecoveryQuestionAndAnswerInvalidAnswerSize            9
/** @} */ /*  Can Set Editable Encryption Password Recovery Question/Answer (Safe) */

/** @name Set/Change Editable Encryption Recovery Question and Answer
 * @{ */
/** Set/modify [editable] controller's encryption recovery question and answer.
 * @deprecated Use @ref SA_SetEditableEncryptionQuestionAndAnswerSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this
 *          function (@ref SA_SetEditableEncryptionQuestionAndAnswerSafe).
 * @pre The user has used @ref SA_CanSetEditableEncryptionQuestionAndAnswer to ensure the operation can be performed.
 * @param[in] cda       Controller data area.
 * @param[in] question  Recovery question C-string (also see @ref SA_CheckEncryptionRecoveryQuestion).
 *                      If NULL, question is not checked: This is useful for checking if an
 *                        operation can be performed without user input.
 *                      If an empty string (and `answer` is also empty), drop an oustanding
 *                        change to the encryption recovery q&a.
 * @param[in] answer    Recovery answer C-string (also see @ref SA_CheckEncryptionRecoveryAnswer).
 *                      If NULL, answer is not checked: This is useful for checking if an
 *                        operation can be performed without user input.
 *                      If an empty string (and `question` is also empty), drop an oustanding
 *                        change to the encryption recovery q&a.
 * @return @ref kSetEditableEncryptionRecoveryQuestionAndAnswerOk or status indicating failure.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableEncryptionQuestionAndAnswer(cda, "When is USA Independence Day?" "It is July 4th 1776.") == kCanSetEditableEncryptionRecoveryQuestionAndAnswerOk)
 *    if (SA_SetEditableEncryptionQuestionAndAnswer(cda, "When is USA Independence Day?" "It is July 4th 1776.") != kSetEditableEncryptionRecoveryQuestionAndAnswerOk)
 *    {
 *        fprintf(stderr, "Failed to set encryption recovery question and answer.\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableEncryptionQuestionAndAnswer(PCDA cda, const char *question, const char *answer);
/** @} */ /* Set/Change Editable Encryption Recovery Question and Answer */

/** @name Set/Change Editable Encryption Recovery Question and Answer (Safe)
 * @{ */
/** Set/modify [editable] controller's encryption recovery question and answer.
 * @pre The user has used @ref SA_CanSetEditableEncryptionQuestionAndAnswerSafe to ensure the operation can be performed.
 * @param[in] cda           Controller data area.
 * @param[in] question      Recovery question C-string (also see @ref SA_CheckEncryptionRecoveryQuestion).
 *                          If NULL, question is not checked: This is useful for checking if an
 *                            operation can be performed without user input.
 *                          If an empty string (and `answer` is also empty), drop an oustanding
 *                            change to the encryption recovery q&a.
 * @param[in] question_size Size of the recovery question C-string (number of bytes).
 * @param[in] answer        Recovery answer C-string (also see @ref SA_CheckEncryptionRecoveryAnswer).
 *                          If NULL, answer is not checked: This is useful for checking if an
 *                            operation can be performed without user input.
 *                          If an empty string (and `question` is also empty), drop an oustanding
 *                            change to the encryption recovery q&a.
 * @param[in] answer_size   Size of the recovery answer C-string (number of bytes).
 * @return @ref kSetEditableEncryptionRecoveryQuestionAndAnswerOk or status indicating failure.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableEncryptionQuestionAndAnswerSafe(cda, "When is USA Independence Day?", sizeof("When is USA Independence Day?"), "July 4th 1776.", sizeof("July 4th 1776.")) == kCanSetEditableEncryptionRecoveryQuestionAndAnswerOk)
 * {
 *    if (SA_SetEditableEncryptionQuestionAndAnswerSafe(cda, "When is USA Independence Day?", sizeof("When is USA Independence Day?"), "July 4th 1776.", sizeof("July 4th 1776.")) != kSetEditableEncryptionRecoveryQuestionAndAnswerOk)
 *    {
 *        fprintf(stderr, "Failed to set encryption recovery question and answer.\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableEncryptionQuestionAndAnswerSafe(PCDA cda, const char *question, size_t question_size, const char *answer, size_t answer_size);
   /** Failed to set/modify the encryption recovery question and answer.
     * @outputof SA_SetEditableEncryptionQuestionAndAnswer.
     * @outputof SA_SetEditableEncryptionQuestionAndAnswerSafe.
     */
   #define kSetEditableEncryptionRecoveryQuestionAndAnswerFailed  0
   /** Successfully set/modified the encryption recovery question and answer.
     * @outputof SA_SetEditableEncryptionQuestionAndAnswer
     * @outputof SA_SetEditableEncryptionQuestionAndAnswerSafe.
     */
   #define kSetEditableEncryptionRecoveryQuestionAndAnswerOk      1
/** @} */ /* Set/Change Editable Encryption Recovery Question and Answer (Safe) */

/** @} */ /* storc_configuration_edit_encryption_recovery_question_answer */

/** @defgroup storc_configuration_edit_encryption_boot_password Encryption Boot Password
 * @brief Setup/configure/clear the encryption boot (controller) password.
 * @{ */

/** @name Check Encryption Boot Password
 * @{ */
SA_BYTE SA_CheckEncryptionBootPassword(PCDA cda, const char *password_buffer, SA_WORD buffer_size);
   /** Boot Password is valid for encryption on this controller.
    * @outputof SA_CheckEncryptionBootPassword.
    */
   #define kCheckBootPasswordOk                                            0x00
   /** Boot Password is invalid (too short) for encryption on this controller.
    * @outputof SA_CheckEncryptionBootPassword.
    */
   #define kCheckBootPasswordFailedTooShort                                0x01
   /** Boot Password is invalid (too long) for encryption on this controller.
    * @outputof SA_CheckEncryptionBootPassword.
    */
   #define kCheckBootPasswordFailedTooLong                                 0x02
   /** Boot Password is invalid (has invalid char) for encryption on this controller.
    * @outputof SA_CheckEncryptionBootPassword.
    */
   #define kCheckBootPasswordFailedInvalidChar                             0x03
   /** Boot Password is invalid (NULL char) for encryption on this controller.
    * @outputof SA_CheckEncryptionBootPassword.
    */
   #define kCheckBootPasswordFailedNull                                    0x04
/** @} */ /* Check Encryption Boot Password */

/** @name Can Set Encryption Boot Password
 * @{ */
/** Used to check if an [editable] controller's boot password can be set.
 * @deprecated Use @ref SA_CanSetEditableEncryptionBootPasswordSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CanSetEditableEncryptionBootPasswordSafe).
 *
 * @post If valid, user may call @ref SA_SetEditableEncryptionBootPassword.
 * @param[in] cda              Controller handler.
 * @param[in] password_buffer  C-string (null-terminated) desired boot password.
 *                             If NULL, boot password is not checked: This is useful for checking if an
 *                               operation can be performed without user input.
 *                             If "", boot password changes are discarded: This is useful for canceling a boot
 *                               password queued for change.
 * @return [Can Set Editable Encryption Boot Password](@ref storc_configuration_edit_encryption_boot_password).
 * @see SA_CheckEncryptionBootPassword
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableEncryptionBootPassword(cda, "MyPassword") == kCanSetEditableEncryptionBootPasswordOk)
 * {
 *    if (SA_SetEditableEncryptionBootPassword(cda, "MyPassword") != kSetEditableEncryptionBootPasswordOk)
 *    {
 *        fprintf(stderr, "Failed to set encryption boot password\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableEncryptionBootPassword(PCDA cda, const char *password_buffer);
/** @} */ /* Can Set Encryption Boot Password */

/** @name Can Set Encryption Boot Password (Safe)
 * @{ */
/** Used to check if an [editable] controller's boot password can be set.
 *
 * @post If valid, user may call @ref SA_SetEditableEncryptionBootPasswordSafe.
 * @param[in] cda                    Controller handler.
 * @param[in] password_buffer        C-string (null-terminated) desired boot password.
 *                                   If NULL, boot password is not checked: This is useful for checking if an
 *                                      operation can be performed without user input.
 *                                   If "", boot password changes are discarded: This is useful for canceling a boot
 *                                      password queued for change.
 * @param[in] password_buffer_size   Size of password_buffer.
 * @return [Can Set Editable Encryption Boot Password](@ref storc_configuration_edit_encryption_boot_password).
 * @see SA_CheckEncryptionBootPassword
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableEncryptionBootPasswordSafe(cda, "MyPassword", sizeof("MyPassword")) == kCanSetEditableEncryptionBootPasswordOk)
 * {
 *    if (SA_SetEditableEncryptionBootPasswordSafe(cda, "MyPassword", sizeof("MyPassword")) != kSetEditableEncryptionBootPasswordOk)
 *    {
 *        fprintf(stderr, "Failed to set encryption boot password\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableEncryptionBootPasswordSafe(PCDA cda, const char *password_buffer, size_t password_buffer_size);
   /** Encryption boot password can be set.
    * @outputof SA_CanSetEditableEncryptionBootPassword.
    * @outputof SA_CanSetEditableEncryptionBootPasswordSafe.
    * @allows SA_SetEditableEncryptionBootPassword.
    * @allows SA_SetEditableEncryptionBootPasswordSafe.
    * @see SA_ChangeEditableControllerEncryption.
   */
   #define kCanSetEditableEncryptionBootPasswordOk                                  1
   /** Encryption boot password cannot be set because encryption is not configured.
    * @outputof SA_CanSetEditableEncryptionBootPassword.
    * @outputof SA_CanSetEditableEncryptionBootPasswordSafe.
    * @disallows SA_SetEditableEncryptionBootPassword.
    * @disallows SA_SetEditableEncryptionBootPasswordSafe.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionBootPasswordEncryptionNotConfigured             2
   /** Encryption boot password cannot be set because there is no editable configuration.
    * @outputof SA_CanSetEditableEncryptionQuestionAndAnswer.
    * @outputof SA_CanSetEditableEncryptionBootPasswordSafe.
    * @disallows SA_SetEditableEncryptionQuestionAndAnswer.
    * @disallows SA_SetEditableEncryptionBootPasswordSafe.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableEncryptionBootPasswordNoEditableConfig                    3
   /** Encryption boot password cannot be set because the password is invalid.
    * @outputof SA_CanSetEditableEncryptionBootPassword.
    * @outputof SA_CanSetEditableEncryptionBootPasswordSafe.
    * @disallows SA_SetEditableEncryptionBootPassword.
    * @disallows SA_SetEditableEncryptionBootPasswordSafe.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionBootPasswordInvalid                             4
   /** Encryption boot password cannot be set because encryption is disabled.
    * @outputof SA_CanSetEditableEncryptionBootPassword.
    * @outputof SA_CanSetEditableEncryptionBootPasswordSafe.
    * @disallows SA_SetEditableEncryptionBootPassword.
    * @disallows SA_SetEditableEncryptionBootPasswordSafe.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionBootPasswordEncryptionDisabled                  5
   /** Encryption boot password cannot be set because of Local Express mode.
    * @outputof SA_CanSetEditableEncryptionBootPassword.
    * @outputof SA_CanSetEditableEncryptionBootPasswordSafe.
    * @disallows SA_SetEditableEncryptionBootPassword.
    * @disallows SA_SetEditableEncryptionBootPasswordSafe.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionBootPasswordExpressEncryption                   6
   /** Encryption boot password cannot be set because of boot password state is disabled.
    * @outputof SA_CanSetEditableEncryptionBootPassword.
    * @outputof SA_CanSetEditableEncryptionBootPasswordSafe.
    * @disallows SA_SetEditableEncryptionBootPassword.
    * @disallows SA_SetEditableEncryptionBootPasswordSafe.
    * @see SA_SetEditableEncryptionBootPasswordState.
    */
   #define kCanSetEditableEncryptionBootPasswordDisabledState                       7
   /** Encryption boot password cannot be set because the feature is not enabled.
    * @outputof SA_CanSetEditableEncryptionBootPassword.
    * @outputof SA_CanSetEditableEncryptionBootPasswordSafe.
    * @disallows SA_SetEditableEncryptionBootPassword.
    * @disallows SA_SetEditableEncryptionBootPasswordSafe.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanSetEditableEncryptionBootPasswordNotEnabled                          8
   /** Encryption boot password cannot be set because the password is invalid size.
    * @outputof SA_CanSetEditableEncryptionBootPasswordSafe.
    * @disallows SA_SetEditableEncryptionBootPasswordSafe.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionBootPasswordInvalidSize                         9
/** @} */ /* Can Set Encryption Boot Password (Safe) */

/** @name Set/Change Editable Encryption Boot Password
 * @{ */
/** Set/modify [editable] controller's encryption boot password.
 * @deprecated Use @ref SA_SetEditableEncryptionBootPasswordSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_SetEditableEncryptionBootPasswordSafe).
 * @pre The user has used @ref SA_CanSetEditableEncryptionBootPassword to ensure the operation can be performed.
 * @param[in] cda              Controller data area.
 * @param[in] password_buffer  C-string (null-terminated) desired boot password.
 *                             If "", boot password changes are discarded: This is useful for canceling a boot
 *                               password queued for change.
 * @return @ref kSetEditableEncryptionBootPasswordOk or status indicating failure.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableEncryptionBootPassword(cda, "MyPassword") == kCanSetEditableEncryptionBootPasswordOk)
 * {
 *    if (SA_SetEditableEncryptionBootPassword(cda, "MyPassword") != kSetEditableEncryptionBootPasswordOk)
 *    {
 *        fprintf(stderr, "Failed to set encryption boot password\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableEncryptionBootPassword(PCDA cda, const char *password_buffer);
/** @} */ /* Set/Change Editable Encryption Boot Password */

/** @name Set/Change Editable Encryption Boot Password (Safe)
 * @{ */
/** Set/modify [editable] controller's encryption boot password.
 * @pre The user has used @ref SA_CanSetEditableEncryptionBootPasswordSafe to ensure the operation can be performed.
 * @param[in] cda                    Controller data area.
 * @param[in] password_buffer        C-string (null-terminated) desired boot password.
 *                                   If "", boot password changes are discarded: This is useful for canceling a boot
 *                                     password queued for change.
 * @param[in] password_buffer_size   Size of password_buffer.
 * @return @ref kSetEditableEncryptionBootPasswordOk or status indicating failure.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableEncryptionBootPasswordSafe(cda, "MyPassword", sizeof("MyPassword")) == kCanSetEditableEncryptionBootPasswordOk)
 * {
 *    if (SA_SetEditableEncryptionBootPasswordSafe(cda, "MyPassword", sizeof("MyPassword")) != kSetEditableEncryptionBootPasswordOk)
 *    {
 *        fprintf(stderr, "Failed to set encryption boot password\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableEncryptionBootPasswordSafe(PCDA cda, const char *password_buffer, size_t password_buffer_size);
   /** Failed to set/modify the encryption boot password.
   * @outputof SA_SetEditableEncryptionBootPassword.
   * @outputof SA_SetEditableEncryptionBootPasswordSafe.
   */
   #define kSetEditableEncryptionBootPasswordFailed  0
   /** Successfully set/modified the encryption boot password.
   * @outputof SA_SetEditableEncryptionBootPassword.
   * @outputof SA_SetEditableEncryptionBootPasswordSafe.
   */
   #define kSetEditableEncryptionBootPasswordOk      1
/** @} */ /* Set/Change Editable Encryption Boot Password (Safe) */

/** @} */ /* storc_configuration_edit_encryption_boot_password */

/** @defgroup storc_configuration_edit_encryption_boot_password_toggle Encryption Boot Password Suspend/Resume
 * @brief Disable/reenable the encryption boot (controller) password.
 * @{ */

/** @name Encryption Boot Password States
 * @{ */
/** Encryption boot password is not set in editable configuration. */
#define kEncryptionBootPasswordUnset     0
/** Encryption boot password is disabled/suspended in editable configuration. */
#define kEncryptionBootPasswordDisabled  1
/** Encryption boot password is enabled (or will resume) in editable configuration. */
#define kEncryptionBootPasswordEnabled   2
/** @} */ /* Encryption Boot Password States */

/** @name Get Encryption Boot Password State
 * @{ */
SA_BYTE SA_GetEditableEncryptionBootPasswordState(PCDA cda);
/** @} */ /* Get Encryption Boot Password State */

/** @name Can Set Encryption Boot Password State
 * @{ */
/** Used to check if a controller's boot password state can be set.
 *
 * @post If valid, user may call @ref SA_SetEditableEncryptionBootPasswordState.
 * @param[in] cda          Controller handler.
 * @param[in] state        Boot password state to set. Valid inputs are @ref kEncryptionBootPasswordDisabled and @ref kEncryptionBootPasswordEnabled. [See Encryption Boot Password States].
 * @return [Can Set Editable Encryption Boot Password State](@ref storc_configuration_edit_encryption_boot_password).
 */
SA_BYTE SA_CanSetEditableEncryptionBootPasswordState(PCDA cda, SA_BYTE state);
   /** Encryption boot password state can be set.
    * @outputof SA_CanSetEditableEncryptionBootPasswordState.
    * @allows SA_SetEditableEncryptionBootPasswordState.
    */
   #define kCanSetEditableEncryptionBootPasswordStateOk                                   1
   /** Encryption boot password state cannot be set because of Local Express mode.
    * @outputof SA_CanSetEditableEncryptionBootPasswordState.
    * @disallows SA_SetEditableEncryptionBootPasswordState.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionBootPasswordStateExpressEncryption                    2
   /** Encryption boot password state cannot be set because the boot password isn't set when trying to unset it.
    * @outputof SA_CanSetEditableEncryptionBootPasswordState.
    * @disallows SA_SetEditableEncryptionBootPasswordState.
    * @see SA_SetEditableEncryptionBootPassword.
    */
   #define kCanSetEditableEncryptionBootPasswordStateBootPasswordNotSet                   3
   /** Encryption boot password state cannot be set because  the controller is locked
    * @outputof SA_CanSetEditableEncryptionBootPasswordState.
    * @disallows SA_SetEditableEncryptionBootPasswordState.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionBootPasswordStateControllerLockout                    4
   /** Encryption boot password state cannot be set because encryption is not configured.
    * @outputof SA_CanSetEditableEncryptionBootPasswordState.
    * @disallows SA_SetEditableEncryptionBootPasswordState.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionBootPasswordStateEncryptionNotConfigured              5
   /** Encryption boot password state cannot be set because encryption is disabled.
    * @outputof SA_CanSetEditableEncryptionBootPasswordState.
    * @disallows SA_SetEditableEncryptionBootPasswordState.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionBootPasswordStateEncryptionDisabled                   6
   /** Encryption boot password state cannot be set because there is no editable configuration.
    * @outputof SA_CanSetEditableEncryptionBootPasswordState.
    * @disallows SA_SetEditableEncryptionBootPasswordState.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableEncryptionBootPasswordStateNoEditableConfig                     7
   /** Encryption boot password state cannot be set because there is no editable change.
    * @outputof SA_CanSetEditableEncryptionBootPasswordState.
    * @disallows SA_SetEditableEncryptionBootPasswordState.
    */
   #define kCanSetEditableEncryptionBootPasswordStateNoChange                             8
   /** Encryption boot password state cannot be set because the controller's current boot password state is disabled.
    * @outputof SA_CanSetEditableEncryptionBootPasswordState.
    * @disallows SA_SetEditableEncryptionBootPasswordState.
    */
   #define kCanSetEditableEncryptionBootPasswordStateDisabled                             9
   /** Encryption boot password cannot be paused/resumed because the feature is not enabled.
    * @outputof SA_CanSetEditableEncryptionBootPasswordState.
    * @disallows SA_SetEditableEncryptionBootPasswordState.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanSetEditableEncryptionBootPasswordStateNotEnabled                           10
/** @} */ /* Can Set Encryption Boot Password State */

/** @name Set Encryption Boot Password State
 * @{ */
/** Set/modify [editable] controller's encryption boot password state.
 * @pre The user has used @ref SA_CanSetEditableEncryptionBootPasswordState to ensure the operation can be performed.
 * @param[in] cda    Controller data area.
 * @param[in] state  State to set.
 * @return @ref kSetEditableEncryptionBootPasswordStateOk or status indicating failure.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableEncryptionBootPasswordState(cda, kEncryptionBootPasswordEnabled) == kCanSetEditableEncryptionBootPasswordStateOk)
 * {
 *    if (SA_SetEditableEncryptionBootPasswordState(cda, kEncryptionBootPasswordEnabled) != kSetEditableEncryptionBootPasswordStateOk)
 *    {
 *        fprintf(stderr, "Failed to set encryption boot password state\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableEncryptionBootPasswordState(PCDA cda, SA_BYTE state);
   /** Failed to set/modify the encryption boot password state.
    * @outputof SA_SetEditableEncryptionBootPasswordState.
    */
   #define kSetEditableEncryptionBootPasswordStateFailed  0
   /** Successfully set/modified the encryption boot password state.
    * @outputof SA_SetEditableEncryptionBootPasswordState.
    */
   #define kSetEditableEncryptionBootPasswordStateOk      1
/** @} */ /* Set Encryption Boot Password State */

/** @} */ /* storc_configuration_edit_encryption_boot_password_toggle */

/** @defgroup storc_configuration_edit_encryption_ld_set_encryption Encryption Set/Change Logical Drive
 * @brief Convert logical drive to/from plaintext to cyphertext.
 * @{ */

/** @name Set Logical Drive Encryption Data States
 * @{ */
/** Data on the logical drive will be encrypted. */
#define kSetEditableLogicalDriveEncryptionDataEncrypted 0x00
/** Data on the logical drive will be plaintext. */
#define kSetEditableLogicalDriveEncryptionDataPlaintext 0x01
/** @} */ /* Set Logical Drive Encryption Data States */

/** @name Can Set Editable Logical Drive Encryption
 * @{ */
/** Set the data encryption state of the editable logical drive.
 * @param[in]  cda                    Controller data area.
 * @param[in]  editable_ld_number     Target LD number.
 * @param[in]  data_encryption_state  One of the encryption state value of [Set Logical Drive Encryption Data States](@ref storc_configuration_edit_encryption_ld_set_encryption).
 * @param[in]  preserve_data          Set to @ref kTrue if data on the logical drive should be preserved, @ref kFalse otherwise.
 *                                    This parameter is ignored for new logical drives.
 * @return See [Can Set Editable Logical Drive Encryption Returns](@ref storc_configuration_edit_encryption_ld_set_encryption).
 */
SA_BYTE SA_CanSetEditableLogicalDriveEncryption(PCDA cda, SA_WORD editable_ld_number, SA_BYTE data_encryption_state, SA_BOOL preserve_data);
   /** The editable logical drive encryption data state can be changed.
    * @allows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionOK                             1
   /** The logical drive encryption data state cannot be changed because the editable configuration has not been created.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionNoEditableConfigCreated        2
   /** The logical drive encryption data state cannot be changed because the controller encryption setting is invalid.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionEncryptionConfigInvalid        3
   /** The logical drive encryption data state cannot be changed because encryption is not configured.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionEncryptionNotConfigured        4
   /** The logical drive encryption data state cannot be changed because local express encryption is enabled.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionExpressLocalEncryptionEnabled  5
   /** The logical drive cannot be encrypted because it is already set to the desired state.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionAlreadySet                     6
   /** The logical drive cannot be plaintext because plaintext volumes are not allowed.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionPlaintextNotAllowed            7
   /** The logical drive encryption data state cannot be changed because decrypting an encrypted volume is not allowed.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionCannotDecrypt                  8
   /** The logical drive encryption data state cannot be changed because the requested state is not allowed.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionNotAllowed                     9
   /** The logical drive encryption data state cannot be changed because encryption is disabled.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionDisabled                       10
   /** The logical drive encryption data state cannot be changed because the desired encryption state is not valid.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionInvalidEncryptionState         11
   /** The logical drive encryption data state cannot be changed because the array is not a data array.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionNotDataArray                   12
   /** The logical drive cannot be encrypted/encoded because LU cache is enabled
    * @deprecated No longer used.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionLUCachingEnabled               13
   /** The logical drive cannot be encrypted/encoded because the controller currently has
    * queued the max number of transformations.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionMaxTransformationsReached      14
   /** The logical drive cannot be encrypted/encoded because the logical drive status is not OK.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    * @see SA_GetLogicalDriveStatusInfo.
    */
   #define kCanSetEditableLogicalDriveEncryptionLogicalDriveStatusNotOK        15
   /** The logical drive cannot be encrypted/encoded because the Array status is not OK
    * or has a conflicting transformation.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    * @see SA_GetEditableArrayStatusInfo.
    */
   #define kCanSetEditableLogicalDriveEncryptionArrayStatusNotOK               16
   /** The logical drive cannot be encrypted/encoded because the Contoller status is not OK.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanSetEditableLogicalDriveEncryptionControllerStatusNotOK          17
   /** The logical drive cannot be encrypted/encoded because the Contoller cache status is not OK.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanSetEditableLogicalDriveEncryptionControllerCacheStatusNotOK     18
   /** The logical drive cannot be encrypted/encoded because the logical drive stripsize is too large.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionExceedsTransformationMemory    19
   /** The logical drive cannot be encrypted/encoded because there is a pending change to the logical
    * drive's volatile key state.
    * @see storc_configuration_edit_encryption_ld_volatile_key.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionQueuedVolatileKeyChange        20
   /** The logical drive cannot be encrypted/encoded because the feature is not enabled.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionNotEnabled                     21
   /** The logical drive cannot be encrypted/encoded because it has a registered split mirror pair.
    * @deprecated Uses @ref kCanSetEditableLogicalDriveEncryptionNotDataArray.
    * The logical drive's split mirror pair may have been delete, become offline or unavailable
    * without being properly separated from the logical drive using resync, rollback, or activate.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionHasSplitMirrorPair             22
   /** The logical drive cannot be encrypted/encoded because it has a caching LUN that
    * has a bad status.
    * @disallows SA_SetEditableLogicalDriveEncryption.
    * @outputof SA_CanSetEditableLogicalDriveEncryption.
    */
   #define kCanSetEditableLogicalDriveEncryptionLUCacheStatusNotOK             23
/** @} */ /* Can Set Editable Logical Drive Encryption */


/** @name Set Editable Logical Drive Encryption
* @{ */
/** Set the data encryption state of the editable logical drive.
* @param[in]  cda                    Controller data area.
* @param[in]  editable_ld_number     Target LD number.
* @param[in]  data_encryption_state  One of the encryption state value of [Set Logical Drive Encryption Data States](@ref storc_configuration_edit_encryption_ld_set_encryption).
* @param[in]  preserve_data          Set to @ref kTrue if data on the logical drive should be preserved, @ref kFalse otherwise.
* @return See [Set Editable Logical Drive Encryption Returns](@ref storc_configuration_edit_encryption_ld_set_encryption).
*/
SA_BYTE SA_SetEditableLogicalDriveEncryption(PCDA cda, SA_WORD editable_ld_number, SA_BYTE data_encryption_state, SA_BOOL preserve_data);
/** The editable logical drive encryption data state will not be changed.
* @outputof SA_SetEditableLogicalDriveEncryption.
*/
#define kSetEditableLogicalDriveEncryptionFailedUnknown  0x00
/** The editable logical drive encryption data state will be changed.
* @outputof SA_SetEditableLogicalDriveEncryption.
*/
#define kSetEditableLogicalDriveEncryptionOK             0x01
/** @} */ /* Set Editable Logical Drive Encryption */

/** @} */ /* storc_configuration_edit_encryption_ld_set_encryption */

/** @defgroup storc_configuration_edit_encryption_ld_volatile_key Encryption Logical Drive Volatile Key
 * @brief Setup/configure logical drive encryption volatile keys.
 @{ */

/** @name Get Editable Logical Drive Volatile Key
 @{ */
/** Return the pending volatile key state of an editable logical drive.
 * @param[in]  cda                 Controller data area.
 * @param[in]  editable_ld_number  Target editable logical drive.
 * @return If volatile keys will not be enabled, @ref kEditableLogicalDriveVolatileKeyDisabled.
 * @return If volatile keys will be enabled but without a backup, @ref kEditableLogicalDriveVolatileKeyEnabledNoBackup.
 * @return If volatile keys will be enabled and will be backed up, @ref kEditableLogicalDriveVolatileKeyEnabledWithBackup.
 * @return @ref kEditableLogicalDriveVolatileKeyUnknown otherwise.
 */
SA_BYTE SA_GetEditableLogicalDriveVolatileKey(PCDA cda, SA_WORD editable_ld_number);
   /** @outputof SA_GetEditableLogicalDriveVolatileKey */
   #define kEditableLogicalDriveVolatileKeyUnknown            0
   /** Volatile keys are disabled for this logical drive.
    * @outputof SA_GetEditableLogicalDriveVolatileKey.
    */
   #define kEditableLogicalDriveVolatileKeyDisabled           1
   /** Volatile keys are enabled for this logical drive without a remote backup.
    * @outputof SA_GetEditableLogicalDriveVolatileKey.
    */
   #define kEditableLogicalDriveVolatileKeyEnabledNoBackup    2
   /** Volatile keys are enabled for this logical drive with a remote backup.
    * @outputof SA_GetEditableLogicalDriveVolatileKey.
    */
   #define kEditableLogicalDriveVolatileKeyEnabledWithBackup  3
/** @} */ /* Get Editable Logical Drive Volatile Key */

/** @name Can Set Logical Drive Volatile Key
 @{ */
/** Can volatile keys (and backup) be configured for the editable logical drive?
 * @post If valid, user may call @ref SA_SetEditableLogicalDriveVolatileKey.
 * @param[in]  cda                         Controller data area.
 * @param[in]  editable_ld_number          Target editable logical drive.
 * @param[in]  enable_volatile_key         @ref kTrue to enable volatile keys, @ref kFalse to disable.
 * @param[in]  enable_volatile_key_backup  @ref kTrue to enable volatile key backup, @ref kFalse to remove backup.
 * @return See [Can Set Logical Drive Volatile Key](@ref storc_configuration_edit_encryption_ld_volatile_key).
 * __Example__
 * @code
 * if (SA_CanSetEditableLogicalDriveVolatileKey(cda, 2, kTrue, kTrue) == kCanSetEditableLogicalDriveVolatileKeyOK)
 * {
 *    if (SA_SetEditableLogicalDriveVolatileKey(cda, 2, kTrue, kTrue) != kSetEditableLogicalDriveVolatileKeyOK)
 *    {
 *       fprintf(stderr, "Failed to enable volatile keys for logical drive %u\n", 2);
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableLogicalDriveVolatileKey(PCDA cda, SA_WORD editable_ld_number, SA_BOOL enable_volatile_key, SA_BOOL enable_volatile_key_backup);
   /** The desired editable logical drive volatile key state can be changed.
    * @allows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyOK                        1
   /** The logical drive volatile key state cannot be changed because the editable configuration has not been created.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyNoEditableConfigCreated   2
   /** The logical drive volatile key state cannot be changed because the editable logical drive does not exist.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyInvalidLogicalDrive       3
   /** The logical drive volatile key state cannot be disabled with a backup.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyNoKeyToBackup             4
   /** The logical drive volatile key state cannot be changed because encrypted has not be configured.
    * @see storc_configuration_edit_encryption_set.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyEncryptionNotConfigured   5
   /** The logical drive volatile key state cannot be changed because encryption is not enabled.
    * @see storc_configuration_edit_encryption_toggle.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyEncryptionNotEnabled      6
   /** The logical drive volatile key state cannot be changed in local express encryption.
    * @see storc_configuration_edit_encryption_set.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyEncryptionLocalExpress    7
   /** The logical drive volatile key state cannot be changed because volatile key backup is only allowed in remote mode.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyNoLocalBackup             8
   /** The logical drive volatile key state cannot be changed because the logical drive is not encrypted.
    * @see storc_configuration_edit_encryption_ld_set_encryption.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyLogicalDriveNotEncrypted  9
   /** The logical drive volatile key state cannot be changed because the array is not a data array.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyNotDataArray              10
   /** The logical drive volatile key state cannot be changed because one of the volatile key
    * actions (backup/remove/restore) is in progress.
    * @see SA_GetLogicalDriveEncryptionInfo.
    * @see VOLATILE_KEY_STATUS.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyOpInProgress              11
   /** The logical drive volatile key state cannot be changed because the logical drive status is not 'OK'.
    * @see SA_GetLogicalDriveStatusInfo.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyLogicalDriveNotOK         12
   /** The logical drive volatile key state cannot be changed because the logical drive is pending rekeying/encoding.
    * @see SA_SetEditableLogicalDriveEncryption.
    * @see SA_GetEditableLogicalDriveFlags.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyPendingRekey              13
   /** The logical drive volatile key state is already in the desired state.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyNoChange                  14
   /** The logical drive volatile key state cannot be changed because volatile keys are not an enabled feature.
    * @disallows SA_SetEditableLogicalDriveVolatileKey.
    * @outputof SA_CanSetEditableLogicalDriveVolatileKey.
    */
   #define kCanSetEditableLogicalDriveVolatileKeyNotEnabled                15
/** @} */ /* Can Set Editable Logical Drive Volatile Key */

/** @name Encryption Set Volatile Key
 * @{ */
/** Set the volatile key setting of the editable logical drive.
 * @pre The user has used @ref SA_CanSetEditableLogicalDriveVolatileKey to ensure the operation can be performed.
 * @param[in]  cda                         Controller data area.
 * @param[in]  editable_ld_number          Target editable logical drive.
 * @param[in]  enable_volatile_key         @ref kTrue to enable volatile keys, @ref kFalse to disable.
 * @param[in]  enable_volatile_key_backup  @ref kTrue to enable volatile key backup, @ref kFalse to remove backup.
 * @return @ref kRekeyEncryptionVolumeKeyFailed when volume rekey fails.
 * @return @ref kRekeyEncryptionVolumeKeyOK when volume rekey is successful.
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableLogicalDriveVolatileKey(cda, 2, kTrue, kTrue) == kCanSetEditableLogicalDriveVolatileKeyOK)
 * {
 *    if (SA_SetEditableLogicalDriveVolatileKey(cda, 2, kTrue, kTrue) != kSetEditableLogicalDriveVolatileKeyOK)
 *    {
 *       fprintf(stderr, "Failed to enable volatile keys for logical drive %u\n", 2);
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableLogicalDriveVolatileKey(PCDA cda, SA_WORD editable_ld_number, SA_BOOL enable_volatile_key, SA_BOOL enable_volatile_key_backup);
   /** Failed to set volatile key settings.
    * @outputof SA_SetEditableLogicalDriveVolatileKey.
    */
   #define kSetEditableLogicalDriveVolatileKeyFailed       0x00
   /** Successfuly set volatile key settings.
    * @outputof SA_SetEditableLogicalDriveVolatileKey.
    */
   #define kSetEditableLogicalDriveVolatileKeyOK           0x01
/** @} */ /* Encryption Set Volatile Key */

/** @} */ /* storc_configuration_edit_encryption_ld_volatile_key */

/** @defgroup storc_configuration_edit_encryption_ctrl_fw_lock Encryption Firmware Lock
 * @brief Enable/disable the encryption controller firmware lockout setting.
 * @{ */

/** @name Controller Firmware Lock Configuration
 * @{ */
#define kControllerFirmwareUnlocked       0x00
#define kControllerFirmwareLocked         0x01
#define kControllerFirmwareLockInvalid    0xFF
/** @} */

/** @name Get Editable Encryption Controller Firmware Lock
 * @{ */
/** Return current value of editable controller's firmware lock.
 * @return [Controller Plaintext Configurations](@ref storc_configuration_edit_encryption_ctrl_fw_lock)
 */
SA_BYTE SA_GetEditableControllerFirmwareLock(PCDA cda);
/** @} */ /* Get Editable Encryption Controller Firmware Lock */

/** @name Can Set Editable Encryption Firmware Lock
 * @{ */
/** Can lock/unlock the encryption firmware lock
 * @post If valid, user may call @ref SA_SetEditableControllerFirmwareLock.
 * @param[in]  cda                    Controller data area.
 * @param[in]  firmware_lock          Set to @ref kTrue to enable the encryption firmware lock or @ref kFalse to unlock.
 * @return See [Can Set Editable Logical Drive Encryption Returns](@ref storc_configuration_edit_encryption_ctrl_fw_lock).
 */
SA_BYTE SA_CanSetEditableControllerFirmwareLock(PCDA cda, SA_BOOL firmware_lock);
   /** The encryption firmware lock state can be changed.
    * @allows SA_SetEditableControllerFirmwareLock.
    * @outputof SA_CanSetEditableControllerFirmwareLock.
    */
   #define kCanSetEditableControllerFirmwareLockOK                            0x01
   /** The encryption firmware lock state cannot be changed because the editable configuration has not been created.
    * @disallows SA_SetEditableControllerFirmwareLock.
    * @outputof SA_CanSetEditableControllerFirmwareLock.
    */
   #define kCanSetEditableControllerFirmwareLockNoEditableConfigCreated       0x02
   /** The encryption firmware lock state is already at the desired state.
    * @disallows SA_SetEditableControllerFirmwareLock.
    * @outputof SA_CanSetEditableControllerFirmwareLock.
    */
   #define kCanSetEditableControllerFirmwareLockAlreadySet                    0x03
   /** The encryption firmware lock state cannot be set while encryption is not configured.
    * @see storc_configuration_edit_encryption_set.
    * @disallows SA_SetEditableControllerFirmwareLock.
    * @outputof SA_CanSetEditableControllerFirmwareLock.
    */
   #define kCanSetEditableControllerFirmwareLockEncryptionNotConfigured       0x04
   /** The encryption firmware lock state cannot be set in the given encryption configuration/key manager mode.
    * @see storc_configuration_edit_encryption_set.
    * @disallows SA_SetEditableControllerFirmwareLock.
    * @outputof SA_CanSetEditableControllerFirmwareLock.
    */
   #define kCanSetEditableControllerFirmwareLockInvalidEncryptionConfig       0x05
   /** The encryption firmware lock state cannot be set while encryption is disabled.
    * @disallows SA_SetEditableControllerFirmwareLock.
    * @outputof SA_CanSetEditableControllerFirmwareLock.
    */
   #define kCanSetEditableControllerFirmwareLockEncryptionDisabled            0x06
   /** The encryption firmware lock state cannot be set because the feature is not enabled.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    * @disallows SA_SetEditableControllerFirmwareLock.
    * @outputof SA_CanSetEditableControllerFirmwareLock.
    */
   #define kCanSetEditableControllerFirmwareLockNotEnabled                    0x07
/** @} */ /* Can Set Editable Encryption Firmware Lock */

/** @name Set Editable Encryption Firmware Lock
 * @{ */
/** Lock/Unlock the encryption firmware lock
 * @pre The user has used @ref SA_CanSetEditableControllerFirmwareLock to ensure the operation can be performed.
 * @param[in]  cda                    Controller data area.
 * @param[in]  firmware_lock          Set to @ref kTrue to enable the encryption firmware lock or @ref kFalse to unlock.
 * @return See [Set Editable Logical Drive Encryption Returns](@ref storc_configuration_edit_encryption_ctrl_fw_lock).
 */
SA_BYTE SA_SetEditableControllerFirmwareLock(PCDA cda, SA_BOOL firmware_lock);
   /** The encryption firmware lock state was not changed.
    * @outputof SA_SetEditableControllerFirmwareLock.
    */
   #define kSetEditableControllerFirmwareLockFailedUnknown  0x00
   /** The encryption firmware lock state will be changed.
    * @outputof SA_SetEditableControllerFirmwareLock.
    */
   #define kSetEditableControllerFirmwareLockOK             0x01
/** @} */ /* Set Editable Encryption Firmware Lock */

/** @} */ /* storc_configuration_edit_encryption_ctrl_fw_lock */

/** @defgroup storc_configuration_edit_encryption_key_cache Encryption Key Cache
 * @brief Enable/disable controller encryption key cache.
 * @{ */

/** @name Get Editable Encryption Key Cache
 * @{ */
/** Get [editible] controller's encryption key cache status
 */
/** Obtain encryption key cache status info for an editable configuration.
 * @param[in] cda  Controller data area.
 * @return See [Encryption Key Cache Status Returns](@ref storc_encryption_key_cache)
 * @return Decode with @ref ENCRYPTION_KEY_CACHE_ATTEMPTS to get the editable configuration's encryption key cache configured maximum attempts.
 * @return Decode with @ref ENCRYPTION_KEY_CACHE_INTERVAL to get the editable configuration's encryption key cache retry interval in minutes.
 * @return Decode with @ref ENCRYPTION_KEY_CACHE_ENABLED to determine if the editable configuration's encryption key cache is enabled.
 *
 * @see SA_GetControllerEncryptionKeyCacheStatus
 */
SA_QWORD SA_GetEditableEncryptionKeyCache(PCDA cda);
/** @} */ /* Get Editable Encryption Key Cache */

/** @name Modify Editable Controller Encryption Key Cache Helpers
 * @{ */
/** Modify the value returned by @ref SA_GetEditableEncryptionKeyCache to enable key cache.
 * Used to modify the parameter given to @ref SA_CanSetEditableEncryptionKeyCache and
 * @ref SA_SetEditableEncryptionKeyCache.
 *
 * @param[in,out] key_cache_state  lValue to modify.
 * @param[in]     retry_count      Desired key cache retry attempts.
 * @param[in]     interval         Desired key cache retry attempt interval (in minutes).
 *
 * __Example__
 * @code
 * SA_QWORD key_cache_state = SA_GetEditableEncryptionKeyCache(cda);
 * ENABLE_ENCRYPTION_KEY_CACHE(key_cache_state, 2, 1);
 * if (SA_CanSetEditableEncryptionKeyCache(cda, key_cache_state) == kCanSetEditableEncryptionKeyCacheAllowed)
 * {
 *    if (SA_SetEditableEncryptionKeyCache(cda, key_cache_state) != kSetEditableEncryptionKeyCacheSuccess)
 *    {
 *        fprintf(stderr, "Failed to enable key cache.");
 *    }
 * }
 * @endcode
 */
#define ENABLE_ENCRYPTION_KEY_CACHE(key_cache_state, retry_count, interval) \
   (key_cache_state) = kControllerEncryptionKeyCacheStatusEnabledMask | \
   ((SA_QWORD)(key_cache_state) & kControllerEncryptionPersistentKeyCacheEnabledMask) | \
   ((((SA_QWORD)(retry_count)) << kControllerEncryptionKeyCacheStatusRetryAttemptsBitShift) & kControllerEncryptionKeyCacheStatusRetryAttemptsMask) | \
   ((((SA_QWORD)(interval)) << kControllerEncryptionKeyCacheStatusRetryIntervalBitShift) & kControllerEncryptionKeyCacheStatusRetryIntervalMask)

/** Modify the value returned by @ref SA_GetEditableEncryptionKeyCache to disable key cache.
 * Used to modify the parameter given to @ref SA_CanSetEditableEncryptionKeyCache and
 * @ref SA_SetEditableEncryptionKeyCache.
 *
 * @param[in,out] key_cache_state  lValue to modify.
 *
 * __Example__
 * @code
 * SA_QWORD key_cache_state = SA_GetEditableEncryptionKeyCache(cda);
 * DISABLE_ENCRYPTION_KEY_CACHE(key_cache_state);
 * if (SA_CanSetEditableEncryptionKeyCache(cda, key_cache_state) == kCanSetEditableEncryptionKeyCacheAllowed)
 * {
 *    if (SA_SetEditableEncryptionKeyCache(cda, key_cache_state) != kSetEditableEncryptionKeyCacheSuccess)
 *    {
 *        fprintf(stderr, "Failed to disable key cache.");
 *    }
 * }
 * @endcode
 */
#define DISABLE_ENCRYPTION_KEY_CACHE(key_cache_state) \
   (key_cache_state) = 0;
/** @} */ /* Modify Editable Controller Encryption Key Cache Helpers */

/** @name Modify Editable Controller Encryption Persistent key cache Helpers
 * @{ */
/** Modify the value returned by @ref SA_GetEditableEncryptionKeyCache to enable persistent key cache.
 * Used to modify the parameter given to @ref SA_CanSetEditableEncryptionKeyCache and
 * @ref SA_SetEditableEncryptionKeyCache.
 *
 * @param[in,out] key_cache_state  lValue to modify.
 *
 * __Example__
 * @code
 * SA_QWORD key_cache_state = SA_GetEditableEncryptionKeyCache(cda);
 * ENABLE_ENCRYPTION_PERSISTENT_KEY_CACHE(key_cache_state);
 * if (SA_CanSetEditableEncryptionKeyCache(cda, key_cache_state) == kCanSetEditableEncryptionKeyCacheAllowed)
 * {
 *    if (SA_SetEditableEncryptionKeyCache(cda, key_cache_state) != kSetEditableEncryptionKeyCacheSuccess)
 *    {
 *        fprintf(stderr, "Failed to enable Persistent key cache.");
 *    }
 * }
 * @endcode
*/
#define ENABLE_ENCRYPTION_PERSISTENT_KEY_CACHE(key_cache_state) \
   (key_cache_state) |= kControllerEncryptionPersistentKeyCacheEnabledMask;

/** Modify the value returned by @ref SA_GetEditableEncryptionKeyCache to disable persistent key cache.
 * Used to modify the parameter given to @ref SA_CanSetEditableEncryptionKeyCache and
 * @ref SA_SetEditableEncryptionKeyCache.
 *
 * @param[in,out] key_cache_state  lValue to modify.
 *
 * __Example__
 * @code
 * SA_QWORD key_cache_state = SA_GetEditableEncryptionKeyCache(cda);
 * DISABLE_ENCRYPTION_PERSISTENT_KEY_CACHE(key_cache_state);
 * if (SA_CanSetEditableEncryptionKeyCache(cda, key_cache_state) == kCanSetEditableEncryptionKeyCacheAllowed)
 * {
 *    if (SA_SetEditableEncryptionKeyCache(cda, key_cache_state) != kSetEditableEncryptionKeyCacheSuccess)
 *    {
 *        fprintf(stderr, "Failed to disable Persistent key cache.");
 *    }
 * }
 * @endcode
*/
#define DISABLE_ENCRYPTION_PERSISTENT_KEY_CACHE(key_cache_state) \
   (key_cache_state) &= ~kControllerEncryptionPersistentKeyCacheEnabledMask;
/** @} */ /* Modify Editable Controller Encryption Persistent Key Cache Helpers */

/** @name Can Set Editable Controller Encryption Key Cache
 * @{ */
/** Checks if key cache can be set to the desired state on the controller.
 * @post If valid, user may call @ref SA_SetEditableEncryptionKeyCache.
 * @param[in] cda                       Controller data area.
 * @param[in] key_cache_state           Set the desired key cache state and attributes (@see storc_encryption_key_cache).
 * @return See [Can Set Editable Key Cache Returns](@ref storc_configuration_edit_encryption_key_cache).
 *
 * __Example__
 * @code
 * SA_QWORD key_cache_state = SA_GetEditableEncryptionKeyCache(cda);
 * ENABLE_ENCRYPTION_KEY_CACHE(key_cache_state, 2, 1);
 * if (SA_CanSetEditableEncryptionKeyCache(cda, key_cache_state) == kCanSetEditableEncryptionKeyCacheAllowed)
 * {
 *    if (SA_SetEditableEncryptionKeyCache(cda, key_cache_state) != kSetEditableEncryptionKeyCacheSuccess)
 *    {
 *        fprintf(stderr, "Failed to enable key cache.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableEncryptionKeyCache(PCDA cda, SA_QWORD key_cache_state);
   /** Set Encryption Key Cache Allowed.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @allows SA_SetEditableEncryptionKeyCache.
    */
   #define kCanSetEditableEncryptionKeyCacheAllowed                                        1
   /** Encryption Key Cache cannot be set because there is no editable configuration.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableEncryptionKeyCacheNoEditableConfig                                2
   /** Encryption Key Cache cannot be set because it is unsupported or the feature is not supported.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanSetEditableEncryptionKeyCacheUnsupported                                    3
   /** Encryption Key Cache cannot be set because the maximum attempts are unsupported.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_ControllerEncryptionKeyCacheSupport.
    */
   #define kCanSetEditableEncryptionKeyCacheMaxAttemptsUnsupported                         4
   /** Encryption Key Cache Allowed cannot be set because the interval is unsupported.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_ControllerEncryptionKeyCacheSupport.
    */
   #define kCanSetEditableEncryptionKeyCacheIntervalUnsupported                            5
   /** Encryption Key Cache cannot be set because the maximum attempts are out of range.
    * This can occur if any of:
    * - The desired retry attempts is outside the range defined in [Controller Encryption Key Cache Support](@ref storc_features_ctrl_support_encryption_key_cache).
    * - User is trying to disable key cache meaning max retry attempts must be zero.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_ControllerEncryptionKeyCacheSupport.
    */
   #define kCanSetEditableEncryptionKeyCacheMaxAttemptsOutOfRange                          6
   /** Encryption Key Cache Allowed cannot be set because the interval is out of range.
    * This can occur if any of:
    * - The desired retry interval is outside the range defined in [Controller Encryption Key Cache Support](@ref storc_features_ctrl_support_encryption_key_cache).
    * - The max attempts is being set to zero which means interval should be zero.
    * - User is trying to disable key cache meaning interval must be zero.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_ControllerEncryptionKeyCacheSupport.
    */
   #define kCanSetEditableEncryptionKeyCacheIntervalOutOfRange                             7
   /** Encryption Key Cache cannot be set because encryption is not configured.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionKeyCacheEncryptionNotConfigured                        8
   /** Encryption Key Cache cannot be set because encryption is not enabled.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_ChangeEditableControllerEncryption.
    */
   #define kCanSetEditableEncryptionKeyCacheEncryptionNotEnabled                           9
   /** Encryption Key Cache cannot be set because remote key manager is not configured.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanSetEditableEncryptionKeyCacheRemoteKeyManagerNotConfigured                  10
   /** Encryption Key Cache cannot be set because of controller lockout (missing boot password).
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanSetEditableEncryptionKeyCacheControllerLockout                              11
    /** Persistent key cache cannot be enabled because the controller does not support persistent key cache.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_GetControllerEncryptionEnabledFeatures.
    */
   #define kCanSetEditableEncryptionKeyCachePersistentKeyCacheNotSupported                 12
    /** Persistent key cache cannot be enabled because the boot password is not set.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    * @see SA_GetControllerEncryptionStatus.
    */
   #define kCanSetEditableEncryptionKeyCacheBootPasswordNotSet                             13
   /** Persistent key cache cannot be enabled because the key cache is not configured.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    */
   #define kCanSetEditableEncryptionKeyCacheNotConfigured                                  14
   /** Persistent key cache cannot be enabled because the key cache attempts is zero.
    * @outputof SA_CanSetEditableEncryptionKeyCache.
    * @disallows SA_SetEditableEncryptionKeyCache.
    */
   #define kCanSetEditableEncryptionKeyCacheAttemptsIsZeroWhileEnablingPersistentKeyCache  15

/** @} */ /* Can Set Editable Controller Encryption Key Cache */

/** @name Set Editable Controller Encryption Key Cache
 * @{ */
/** Change the editable controller encryption key cache state.
 * @pre The user has used @ref SA_CanSetEditableEncryptionKeyCache to ensure the operation can be performed.
 * @param[in] cda                       Controller data area.
 * @param[in] key_cache_state           Set the desired key cache state and attributes (@see storc_encryption_key_cache).
 * @return See [Set Editable Key Cache Returns](@ref storc_configuration_edit_encryption_key_cache).
 *
 * __Example__
 * @code
 * SA_QWORD key_cache_state = SA_GetEditableEncryptionKeyCache(cda);
 * ENABLE_ENCRYPTION_KEY_CACHE(key_cache_state, 2, 1);
 * if (SA_CanSetEditableEncryptionKeyCache(cda, key_cache_state) == kCanSetEditableEncryptionKeyCacheAllowed)
 * {
 *    if (SA_SetEditableEncryptionKeyCache(cda, key_cache_state) != kSetEditableEncryptionKeyCacheSuccess)
 *    {
 *        fprintf(stderr, "Failed to enable key cache.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableEncryptionKeyCache(PCDA cda, SA_QWORD key_cache_state);
   /** Encryption Key Cache Set Successfully.
    * @outputof SA_SetEditableEncryptionKeyCache.
    */
   #define kSetEditableEncryptionKeyCacheSuccess         0x01
   /** Set Encryption Key Cache Failed.
    * @outputof SA_SetEditableEncryptionKeyCache.
    */
   #define kSetEditableEncryptionKeyCacheFailedUnknown   0x02
/** @} */ /* Set Editable Controller Encryption Key Cache */

/** @} */ /* storc_configuration_edit_encryption_key_cache */

/** @} */ /* storc_configuration_edit_encryption  */

/***********************************************************//**
 * @defgroup storc_configuration_edit_ld Editable Logical Drive
 * @brief Create/configure logical drives.
 * @{
**************************************************************/

/** @defgroup storc_configuration_edit_ld_enum Enumerate Editable Logical Drives
 * @brief Enumerate all existing/new logical drives in the editable configuration.
 * @{ */
/** Enumerate all the editable logical drives.
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Current logical drive number.
 * @return Next logical drive number after `ld_number`, or @ref kInvalidLDNumber.
 *
 * __Example__
 * @code
 * // Create logical drives.
 * SA_WORD ld_number = kInvalidLDNumber;
 * while (SA_EnumerateEditableLogicalDrives(cda, ld_number) != kInvalidLDNumber)
 * {
 *    // Edit 'ld_number' logical drive.
 * }
 * @endcode
 */
SA_WORD SA_EnumerateEditableLogicalDrives(PCDA cda, SA_WORD ld_number);
/** @} */ /* storc_configuration_edit_ld_enum */

/** @defgroup storc_configuration_edit_ld_create Create Editable Logical Drive
 * @brief Create an editable logical drive (create new logical drive).
 * @{ */

/** @name Can Create Editable Logical Drive
 * @{ */
/** Can a logical drive be created on the controller?
 * @post If valid, user may call @ref SA_CreateEditableLogicalDrive.
 * @param[in] cda  Controller data area.
 * @return Bitmap describing if the editable logical drive can be created or reason(s) otherwise (see [Can Create Editable Logical Drive Returns](@ref storc_configuration_edit_ld_create)).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     // Edit editable logical drive and commit
 *     // or delete editable logical drive.
 * }
 * @endcode
 */
SA_BYTE SA_CanCreateEditableLogicalDrive(PCDA cda);
   /** @ref SA_CanCreateEditableLogicalDrive return: The editable logical drive can be created.
    * @outputof SA_CanCreateEditableLogicalDrive.
    * @allows SA_CreateEditableLogicalDrive.
    */
   #define kCanCreateEditableLogicalDriveOK                                1
   /** The editable logical drive cannot be created because the editable configuration has not been created.
    * @outputof SA_CanCreateEditableLogicalDrive.
    * @disallows SA_CreateEditableLogicalDrive.
    */
   #define kCanCreateEditableLogicalDriveNoEditableConfigCreated           2
   /** The editable logical drive cannot be created because the controller status is not OK.
    * @outputof SA_CanCreateEditableLogicalDrive.
    * @disallows SA_CreateEditableLogicalDrive.
    */
   #define kCanCreateEditableLogicalDriveControllerNotOK                   3
   /** The editable logical drive cannot be created because no physical space is available for logical drive creation.
    * @outputof SA_CanCreateEditableLogicalDrive.
    * @disallows SA_CreateEditableLogicalDrive.
    */
   #define kCanCreateEditableLogicalDriveNoFreeSpaceAvailable              4
   /** The editable logical drive cannot be created because the Controller has reached the max number of logical drive.
    * @outputof SA_CanCreateEditableLogicalDrive.
    * @disallows SA_CreateEditableLogicalDrive.
    */
   #define kCanCreateEditableLogicalDriveMaxLDReached                      5
   /** The editable logical drive cannot be created because creation operations are no longer supported.
    * @deprecated No longer used.
    * @outputof SA_CanCreateEditableLogicalDrive.
    * @disallows SA_CreateEditableLogicalDrive.
    */
   #define kCanCreateEditableLogicalDriveNotAllowed                        6
   /** The editable logical drive cannot be created because the Controller is in encryption lockout state.
    * @see SA_GetControllerEncryptionStatus.
    * @outputof SA_CanCreateEditableLogicalDrive.
    * @disallows SA_CreateEditableLogicalDrive.
    */
#define kCanCreateEditableLogicalDriveControllerEncryptionLockoutState     7
/** @} */ /* Can Create Editable Logical Drive */

/** @name Create Editable Logical Drive
 * @{ */
/** Create a new editable logical drive.
 * @pre The user has used @ref SA_CanCreateEditableLogicalDrive to ensure the operation can be performed.
 * @param[in] cda Controller data area.
 * @return ld_number if creation succeeded, @ref kInvalidLDNumber if creation failed.
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     // Edit editable logical drive and commit
 *     // or delete editable logical drive.
 * }
 * @endcode
 */
SA_WORD SA_CreateEditableLogicalDrive(PCDA cda);
/** @} */ /* Create Editable Logical Drive */

/** @} */ /* storc_configuration_edit_ld_create */

/** @defgroup storc_configuration_edit_ld_delete Delete Editable Logical Drive
 * @brief Delete an existing or virtual editable logical drive.
 * @details
 * ### Examples
 * - @ref storc_config_example_delete
 * - @ref storc_config_example_delete_cached_ld
 * @{ */

/** @name Can Delete Editable Logical Drive
 * @{ */
/** Can the editable logical drive be deleted?
 * @post If valid, user may call @ref SA_DeleteEditableLogicalDrive.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target logical drive number.
 * @return See [Can Delete Editable Logical Drive Returns](@ref storc_configuration_edit_ld_delete).
 *
 * __Example__
 * @code
 * if (SA_CanDeleteEditableLogicalDrive(cda) == kCanDeleteEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_DeleteEditableLogicalDrive(cda);
 *     // Instead of committing editable logical drive.
 *     if (SA_CanDeleteEditableLogicalDrive(cda, ld_number) == kCanDeleteEditableLogicalDriveOK)
 *     {
 *         if (SA_DeleteEditableLogicalDrive(cda, ld_number) != kDeleteEditableLogicalDriveOK)
 *         {
 *             fprintf(stderr, "Failed to delete virtual logical drive\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanDeleteEditableLogicalDrive(PCDA cda, SA_WORD editable_ld_number);
   /** The editable logical drive can be deleted.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @allows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveOK                                   1
   /** The editable logical drive cannot be deleted because the editable configuration has not been created yet.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    * @see SA_DeleteEditableConfiguration.
    */
   #define kCanDeleteEditableLogicalDriveNoEditableConfigCreated              2
   /** The editable logical drive is invalid or does not exist.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    * @see SA_DeleteEditableConfiguration.
    */
   #define kCanDeleteEditableLogicalDriveInvalidLogicalDrive                  3
   /** The editable logical drive cannot be deleted because the logical drive has been finalized & locked.
    * @deprecated No longer used.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    * @see SA_LockEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveIsLocked                             4
   /** The editable logical drive cannot be deleted because the requested logical drive is not last in the Array.
    * @deprecated No longer used.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveNotLastLDInArray                     5
   /** The editable logical drive cannot be deleted because the Array has new/committed logical drives.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveArrayHasUnsavedLogicalDrives         6
   /** The editable logical drive cannot be deleted because the requested logical drive is not last in the array and move logical drive is not allowed.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveMoveLogicalDriveNotSupported         7
   /** The editable logical drive cannot be deleted because the requested logical drive is not last in the array and the controller is not in OK status.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveControllerStatusNotOK                8
   /** The editable logical drive cannot be deleted because the requested logical drive is not last in the array and the controller cashe is not in OK status.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveControllerCacheStatusNotOK           9
   /** The editable logical drive cannot be deleted because the requested logical drive is not last in the array and the array is not in OK status.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveArrayStatusNotOK                     10
   /** The editable logical drive cannot be deleted because the requested logical drive is not last in the array and its array's strip size is too large.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveExceedsTransformationMemory          11
   /** The editable logical drive cannot be deleted because the requested logical drive is not last in the array and the transformation queue is full.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveMaxTransformationsReached            12
   /** The editable logical drive cannot be deleted because the requested logical drive is not last in the array and transforming is not allowed due to LU caching.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveLUCachingEnabled                     13
   /** The editable logical drive cannot be deleted because it's caching LU logical drive must be deleted first.
    * @deprecated Allowed to create LU caching orphans until commit.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveHasLUCache                           14
   /** The editable logical drive cannot be deleted because the LU caching logical drive is not the last logical drive of the LU cache Array.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveLUCacheLDNotLastLDInCacheArray       15
   /** The editable logical drive cannot be deleted because the LU caching logical drive is using Write-Back policy and cannot be deleted.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveLUCacheWriteBackPolicy               16
   /** The editable lu cache logical drive cannot be deleted because the logical drive is still flushing data to its primary data drive.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveLUCacheFlushing                      17
   /** The editable logical drive cannot be deleted because the requested logical drive is not last in the array and data preservation is not available.
    * @see storc_status_ctrl_status.
    * @see storc_status_ctrl_backup_power_source_status.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveDataPreservationNotAvailable         18
   /** The editable logical drive cannot be deleted because it contains a locked SED.
    * In order to delete this logical drive, one of the following actions can be taken:
    * 1. Use @ref SA_DoClearConfiguration and @ref SA_CanClearConfiguration.
    * 2. Unlock all SEDs in the volume and perform a reboot (if necessary, see @ref SA_ControllerRebootRequired), then try deleting the logical drive.
    * @outputof SA_CanDeleteEditableLogicalDrive.
    * @disallows SA_DeleteEditableLogicalDrive.
    */
   #define kCanDeleteEditableLogicalDriveContainsLockedSED                    19
/** @} */ /* Can Delete Editable Logical Drive */

/** @name Delete Editable Logical Drive
 * @{ */
/** Delete the editable logical drive.
 * @pre The user has used @ref SA_CanDeleteEditableLogicalDrive to ensure the operation can be performed.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target logical drive number.
 * @return See [Delete Editable Logical Drive Returns](@ref storc_configuration_edit_ld_delete).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     // Instead of committing editable logical drive.
 *     if (SA_CanDeleteEditableLogicalDrive(cda, ld_number) == kCanDeleteEditableLogicalDriveOK)
 *     {
 *         if (SA_DeleteEditableLogicalDrive(cda, ld_number) != kDeleteEditableLogicalDriveOK)
 *         {
 *             fprintf(stderr, "Failed to delete virtual logical drive\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_DeleteEditableLogicalDrive(PCDA cda, SA_WORD editable_ld_number);
   /** Failed to delete the editable Logical Drive.
    * @outputof SA_DeleteEditableLogicalDrive.
    */
   #define kDeleteEditableLogicalDriveFailedUnknown 0x00
   /** Succcessfully deleted the editable Logical Drive.
    * @outputof SA_DeleteEditableLogicalDrive.
    */
   #define kDeleteEditableLogicalDriveOK            0x01
/** @} */ /* Delete Editable Logical Drive */

/** @} */ /* storc_configuration_edit_ld_delete */

/** @defgroup storc_configuration_edit_ld_flags Editable Logical Drive Flags
 * @brief Get basic information on an editable logical drive.
 * @{ */
/** Get the flags for the editable logical drive.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target logical drive number.
 * @return See [Editable Logical Drive Flags](@ref storc_configuration_edit_ld_flags).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     // Edit editable logical drive.
 *     printf("Is RPI enabled?: %s\n",
 *         (SA_GetEditableLogicalDriveFlags(cda, ld_number) & kEditableLogicalDriveFlagRapidParityInit
 *             ? "YES" : "NO"));
 * }
 * @endcode
 */
SA_DWORD SA_GetEditableLogicalDriveFlags(PCDA cda, SA_WORD editable_ld_number);
   /** @name Editable Logical Drive Flags
    * @outputof SA_GetEditableLogicalDriveFlags.
    * @{ */
   /** Flag: Editable logical drive is new.
    * @see SA_CreateEditableLogicalDrive.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagNewLogicalDrive             0x00000001
   /** Flag: Editable logical drive is locked.
    * @see SA_LockEditableLogicalDrive.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagLocked                      0x00000002
   /** Flag: Editable logical drive is marked for RPI.
    * @see SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagRapidParityInit             0x00000004
   /** Flag: Editable logical drive is marked for OPO.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagOPO                         0x00000008
   /** Flag: Editable logical drive is queued for transformation.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagTransformationQueued        0x00000010
   /** Flag: Editable logical drive is queued for encoding or volume re-key with data preservation (also considered a transformation).
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagEncodeQueued                0x00000020
   /** Flag: Editable logical drive missing RAID level.
    * @see SA_SetEditableLogicalDriveRAIDLevel.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagNoRAIDLevelSet              0x00000040
   /** Flag: Editable logical drive missing strip size.
    * @see SA_SetEditableLogicalDriveStripSize.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagNoStripSizeSet              0x00000080
   /** Flag: Editable logical drive missing size.
    * @see SA_SetEditableLogicalDriveSize.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagNoSizeSet                   0x00000100
   /** Flag: Editable logical drive is not yet assigned to an Array.
    * @see SA_SetEditableLogicalDriveArray.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagNoArraySet                  0x00000200
   /** Flag: Editable logical drive is (or will be) encrypted.
    * @see SA_EncodeEditableLogicalDrive.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagEncrypted                   0x00000400
   /** Flag: Editable logical drive has (or will have) volatile keys enabled.
    * @see SA_SetEditableLogicalDriveVolatileKey
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagVolatileKeyEnabled          0x00000800
   /** Flag: Editable logical drive has (or will have) volatile key backup enabled.
    * @see SA_SetEditableLogicalDriveVolatileKey
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagVolatileKeyBackupEnabled    0x00001000
   /** Flag: Editable lu cache logical drive is not yet assigned to a data logical drive.
    * @see SA_SetEditableLogicalDriveArray.
    * @outputof SA_GetEditableLogicalDriveFlags.
    */
   #define kEditableLogicalDriveFlagNoLUCacheLogicalDriveSet    0x00002000
   /** @} */ /* Editable Logical Drive Flags */

/** Return editable logical drive type.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target editable logical drive number.
 * @return [Logical drive types](@ref storc_properties_ld_basic_info) from @ref SA_GetLogicalDriveBasicInfo.
 * @see SA_GetLogicalDriveBasicInfo.
 */
SA_BYTE SA_GetEditableLogicalDriveType(PCDA cda, SA_WORD editable_ld_number);
/** @} */ /* storc_configuration_edit_ld_flags */

/** @defgroup storc_configuration_edit_ld_set_array Assign Editable Logical Drive To Array
 * @brief Assign a new or existing logical drive to an Array in the editable configuration.
 * @details
 * Assigning A New Logical Drive to A New Array
 * --------------------------------------------
 * @code
 * //           Array 1
 * //   ,------,      ,------,
 * //  (  PD 1  )    (  PD 2  )
 * //  |~------~|    |~------~|
 * //  |        |    |        |
 * //  | empty  |    | empty  | <= | LD 1 |
 * //  |        |    |        |
 * //   "------"      "------"
 * //
 * //           Array 1
 * //   ,------,      ,------,
 * //  (        )    (        )
 * //  |~------~|    |~------~|
 * //  |        |    |        | * No size/raid/stripsize set.
 * //  |  LD 1  |    |  LD 1  | * More LD's cannot be assigned until LD 1 locked.
 * //  |        |    |        |
 * //   "------"      "------"
 * //
 * @endcode
 *
 * Assigning A New Logical Drive to An Existing Array
 * --------------------------------------------------
 * @code
 * //           Array 1
 * //   ,------,      ,------,
 * //  (  PD 1  )    (  PD 2  )
 * //  |~------~|    |~------~|
 * //  |  LD 1  |    |  LD 1  |
 * //  |~------~|    |~------~|
 * //  |        |    |        | <= | LD 2 |
 * //  |        |    |        |
 * //   "------"      "------"
 * //
 * //           Array 1
 * //   ,------,      ,------,
 * //  (        )    (        )
 * //  |~------~|    |~------~|
 * //  |  LD 1  |    |  LD 1  | * No changes for LD 1
 * //  |~------~|    |~------~|
 * //  |  LD 2  |    |  LD 2  | * More LD's cannot be assigned until LD 2 locked.
 * //  |        |    |        | * No size/raid/stripsize set.
 * //   "------"      "------"
 * //
 * @endcode
 *
 * Moving A Logical Drive To A New Set Of Data Drives
 * --------------------------------------------------
 * @code
 * //    Array 1        Array 2
 * //   ,------,       ,------,
 * //  (  PD 1  )     (  PD 2  )
 * //  |~------~|     |~------~|
 * //  |  LD 1  | \   |  LD 3  |
 * //  |~------~|  \  |~------~|
 * //  |  LD 2  |   > |        |
 * //   "------"       "------"
 * //
 * //    Array 1       Array 2
 * //   ,------,      ,------,
 * //  (  PD 1  )    (  PD 2  )
 * //  |~------~|    |~------~|
 * //  |  LD 2  |    |  LD 3  |
 * //  |        |    |~------~|
 * //  |        |    |  LD 1  |
 * //   "------"      "------"
 * // * LD 1 will be in a transforming state (moving to Array 2)
 * // * LD 2 will be in a transforming state (moved up on Array 1)
 * //
 * @endcode
 *
 * @{
 */

/** @name Get Editable Logical Drive Array
  * @{ */
/** Get the array for the editable logical drive
 * @pre The user has used @ref SA_CanSetEditableLogicalDriveArray to ensure the operation can be performed.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_ld_number     Target logical drive number.
 * @return Target Array number.
 */
SA_WORD SA_GetEditableLogicalDriveArray(PCDA cda, SA_WORD editable_ld_number);
/** @} */ /* Get Editable Logical Drive Array */

/** @name Can Set Editable Logical Drive to Array
 * @{ */
/** Can the logical drive be assigned to the requested editable Array?
 * @post If valid, user may call @ref SA_SetEditableLogicalDriveArray.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_ld_number     Target logical drive number.
 * @param[in] editable_array_number  Target Array number.
 * @return See [Can Set Editable Logical Drive to Array Returns](@ref storc_configuration_edit_ld_set_array).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveArray(cda, ld_number, array_number) == kCanSetEditableLogicalDriveArrayOK)
 *     {
 *         if (SA_SetEditableLogicalDriveArray(cda, ld_number, array_number) != kSetEditableLogicalDriveArrayOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual Array of virtual logical drive\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableLogicalDriveArray(PCDA cda, SA_WORD editable_ld_number, SA_WORD editable_array_number);
   /** The editable logical drive can be assigned to the editable Array.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @allows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayOK                               1
   /** The editable logical drive cannot be assigned to the editable Array because the editable configuration has not been created yet.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableLogicalDriveArrayNoEditableConfigCreated          2
   /** The editable logical drive cannot be assigned to the editable Array because the logical drive does not exist.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayInvalidLogicalDrive              3
   /** The editable logical drive cannot be assigned to the editable Array because the Array does not exist.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayInvalidArray                     4
   /** The editable logical drive cannot be assigned to the editable Array because the Array has no data drives.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayArrayHasNoDataDrives             5
   /** The editable logical drive cannot be assigned to the editable Array because there is no space available on the Array.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayNoSpaceAvailableOnArray          6
   /** The editable logical drive cannot be assigned to the editable Array because required space is not curerntly available on the physical drives.
    * This usually means there is a pending/queued transformation in progress.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArraySpaceNotCurrentlyAvailable       7
   /** The editable logical drive cannot be assigned to the editable Array.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayChangeArrayNotAllowed            8
   /** The editable logical drive cannot be assigned to the editable Array because the logical drive has not been finalized & locked.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    * @see SA_LockEditableLogicalDrive.
    */
   #define kCanSetEditableLogicalDriveArrayLogicalDriveIsLocked             9
   /** The editable logical drive cannot be assigned to the editable Array because there is a logical drive on the Array that has not been finalized & locked.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    * @see SA_LockEditableLogicalDrive.
    */
   #define kCanSetEditableLogicalDriveArrayHasUnlockedLD                    10
   /** The editable logical drive cannot be moved to the editable Array because its current Array has a new/uncommitted logical drive.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    * @see SA_LockEditableLogicalDrive.
    */
   #define kCanSetEditableLogicalDriveArrayHasUnsavedLogicalDrive           11
   /** The editable logical drive cannot be assigned to the editable Array because the Array status is not OK.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayArrayStatusNotOK                 12
   /** The editable logical drive cannot be assigned to an Array at this stage of the configuration.
    * @deprecated No longer used.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayNotAllowed                       13
   /** The editable logical drive cannot be assigned to an Array because the operation is not supported by the controller.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayNotSupported                     14
   /** The editable logical drive cannot be assigned to a split mirror backup Array.
    *  This means that the array one of the following:
    *  - Caching Array
    *  - Split mirror primary Array
    *  - Split mirror backup Array
    *  - Split mirror backup orphan Array
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayNotDataArray                     15
   /** The editable logical drive cannot be moved to the Array because the number of data drives are not compatible.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayMismatchDriveCount               16
   /** The editable logical drive cannot be moved to the Array due to the controller status.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayControllerStatusNotOK            17
   /** The editable logical drive cannot be moved to the Array due to the controller cache status.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayControllerCacheStatusNotOK       18
   /** The editable logical drive cannot be moved to the Array due to a limitation of the controller's transformation memory capacity.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayExceedsTransformationMemory      19
   /** The editable logical drive is already assigned to the requested Array.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayAlreadySet                       20
   /** The editable logical drive cannot be moved to the Array because the max transformations have been reached.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayMaxTransformationsReached        21
   /** The editable logical drive cannot be moved while LU cache is enabled.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayLUCachingEnabled                 22
   /** There is not sufficient memory to add the editable logical drive to the LU cache Array.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayCacheMemoryExhausted             23
   /** The editable logical drive cannot be assigned to the editable Array the logical drive is too small to
    * exist on the target Array.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayLogicalDriveTooSmall             24
   /** The editable logical drive cannot be assigned to the editable Array because the target Array is configured with with
    * drives with an incompatible blocksize.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayIncompatibleBlockSize            25
   /** The editable logical drive cannot be assigned to the editable Array because data preservation is not available.
    * @see storc_status_ctrl_status.
    * @see storc_status_ctrl_backup_power_source_status.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayDataPreservationNotAvailable     26
   /** The editable logical drive cannot be assigned to the editable Array because its full stripe size would exceed the max allowed.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayFullStripeSizeOverflow           27
   /** The editable logical drive cannot be assigned to the editable Array because it is not allowed with the spare activation mode and spare type of the Array.
    * For example RAID0 not allowed while SAM is failure and spare type is 'auto-replace'.
    * @see storc_configuration_edit_ctrl_sam.
    * @see storc_configuration_edit_array_spare_type.
    * @outputof SA_CanSetEditableLogicalDriveArray.
    * @disallows SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveArrayInvalidSpareConfiguration        28
/** @} */ /* Can Set Editable Logical Drive to Array */

/** @name Set Editable Logical Drive to Array
 * @{ */
/** Set the array for the editable logical drive.
 * @warning This operation may initiate a transformation on existing logical drives.
 * @pre The user has used @ref SA_CanSetEditableLogicalDriveArray to ensure the operation can be performed.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_ld_number     Target logical drive number.
 * @param[in] editable_array_number  Target Array number.
 * @return See [Set Logical Drive to Array Returns](@ref storc_configuration_edit_ld_set_array).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveArray(cda, ld_number, array_number) == kCanSetEditableLogicalDriveArrayOK)
 *     {
 *         if (SA_SetEditableLogicalDriveArray(cda, ld_number, array_number) != kSetEditableLogicalDriveArrayOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual Array of virtual logical drive\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableLogicalDriveArray(PCDA cda, SA_WORD editable_ld_number, SA_WORD editable_array_number);
   /** Failed to assign the editable Logical Drive to the editable Array.
    * @outputof SA_SetEditableLogicalDriveArray
    */
   #define kSetEditableLogicalDriveArrayFailedUnknown            0x00
   /** Successfully assigned the editable Logical Drive to the editable Array.
    * @outputof SA_SetEditableLogicalDriveArray
    */
   #define kSetEditableLogicalDriveArrayOK                       0x01
/** @} */ /* Set Editable Logical Drive to Array */

/** @} */ /* storc_configuration_edit_ld_set_array */

/** @defgroup storc_configuration_edit_ld_set_raid Logical Drive RAID Configuration
 * @brief Set/change an editable logical drive's RAID level.
 * @{ */

/** @name Get Editable Logical Drive RAID
 * @{ */
/** Get the editable logical drive [RAID info](@ref storc_properties_ld_raid_info).
 * @param[in] cda        Controller data area.
 * @param[in] ld_number  Target logical drive number.
 * @return See return of @ref SA_GetLogicalDriveRAIDInfo.
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     // Edit editable logical drive.
 *     printf("Editabel logical drive RAID info: %u\n",
 *         SA_GetEditableLogicalDriveRAIDInfo(cda, ld_number));
 * }
 * @endcode
 */
SA_WORD SA_GetEditableLogicalDriveRAIDInfo(PCDA cda, SA_WORD ld_number);
/** @} */ /* Get Editable Logical Drive RAID */

/** @name Can Set Editable Logical Drive RAID
 * @{ */
/** Can we set the desired RAID level on the editable logical drive?
 * @post If valid, user may call @ref SA_SetEditableLogicalDriveRAIDLevel.
 * RAID levels are defined under @ref SA_ControllerSupportsRAIDLevel.
 * @see SA_ControllerSupportsRAIDLevel
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target logical drive number.
 * @param[in] raid_level          Desired RAID level.
 * @param[in] parity_group_count  Desired Parity group count (ignored if not applicable for target RAID level).
 * @return See [Can Set Editable Logical Drive RAID Returns](@ref storc_configuration_edit_ld_set_raid).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveRAIDLevel(cda, ld_number, kRAID50, 2) == kCanSetEditableLogicalDriveRAIDLevelOK)
 *     {
 *         if (SA_SetEditableLogicalDriveRAIDLevel(cda, ld_number, kRAID50, 2) != kSetEditableLogicalDriveRAIDLevelOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual logical drive's RAID level to 5+0 with 2 parity groups\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableLogicalDriveRAIDLevel(PCDA cda, SA_WORD editable_ld_number, SA_BYTE raid_level, SA_BYTE parity_group_count);
   /** The editable logical drive's RAID can be set.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @allows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelOK                            1
   /** The editable logical drive's RAID cannot be set because the editable configuration has not been created yet.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelNoEditableConfigCreated       2
   /** The editable logical drive's RAID cannot be set because the logical drive has been locked.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    * @see SA_LockEditableLogicalDrive.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelLogicalDriveIsLocked          3
   /** The editable logical drive's RAID cannot be set because the logical drive has not been assigned to an Array.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    * @see SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelNoArraySet                    4
   /** The editable logical drive's RAID cannot be set because the requested RAID level is invalid.
    * @deprecated No longer used.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelInvalidRAIDLevel              5
    /** The requested RAID level is not supported by the controller.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelUnsupportedRAIDLevel          6
   /** The editable logical drive's RAID cannot be set because the number of drives is unsupported.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelUnsupportedDriveCount         7
   /** The editable logical drive RAID is already set to the desired level
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelAlreadySet                    8
   /** The editable logical drive's RAID cannot be set because the full stripe size will be too large.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelFullStripeSizeOverflow        9
   /** The editable logical drive's RAID cannot be set because there is not enough space available.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelNotEnoughFreeSpaceForRAID     10
   /** The editable logical drive's RAID cannot be set/changed at this stage of configuration.
    * @deprecated No longer used.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelNotAllowed                    11
   /** The editable logical drive's RAID cannot be changed because the controller does not support RAID Migration.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelRAIDMigrationNotSupported     12
   /** The editable logical drive's RAID cannot be changed due to the controller status.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelControllerStatusNotOK         13
   /** The editable logical drive's RAID cannot be changed due to the controller's cache status
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelControllerCacheStatusNotOK    14
   /** The editable logical drive's RAID cannot be changed because the Array is a non-data Array.
    * I.e. Split mirror, caching, etc...
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelNotDataArray                  15
   /** The editable logical drive's RAID cannot be changed due to another a pending operation or status on the Array.
    * @see SA_GetEditableArrayStatusInfo.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelArrayStatusNotOK              16
   /** The editable logical drive RAID level cannot be set because the transformation queue is full.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelMaxTransformationsReached     17
   /** The editable logical drive RAID level cannot be set while transformations have been disabled while LU caching is enabled.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelLUCachingEnabled              18
   /** The editable logical drive RAID level cannot be set because the Array has new/uncommited logical drives.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelArrayHasUnsavedLogicalDrives  19
   /** The editable logical drive RAID level cannot be set because data preservation is not available.
    * @see storc_status_ctrl_status.
    * @see storc_status_ctrl_backup_power_source_status.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelDataPreservationNotAvailable  20
    /** The editable logical drive RAID level cannot be set because it is not allowed with the spare activation mode and spare type of the Array.
    * For example RAID0 not allowed while SAM is failure and spare type is 'auto-replace'.
    * @see storc_configuration_edit_ctrl_sam.
    * @see storc_configuration_edit_array_spare_type.
    * @outputof SA_CanSetEditableLogicalDriveRAIDLevel.
    * @disallows SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveRAIDLevelInvalidSpareConfiguration     21
/** @} */ /* Can Set Editable Logical Drive RAID */

/** @name Set Editable Logical Drive RAID
 * @{ */
/** Set the requested RAID level for the editable logical drive.
 * @pre The user has used @ref SA_CanSetEditableLogicalDriveRAIDLevel to ensure the operation can be performed.
 * @param[in] cda                                  Controller data area.
 * @param[in] editable_ld_number                   Target logical drive number.
 * @param[in] raid_level                           Desired RAID level.
 * @param[in] parity_group_count                   Desired Parity group count (ignored if not applicable for target RAID level).
 * @param[in] set_default_strip_size_and_max_size  Set to @ref kTrue to also set the default stripsize and max size for the logical drive, while @ref kFalse does not set size/stripsize.
 * @return See [Set Editable Logical Drive RAID Returns](@ref storc_configuration_edit_ld_set_raid).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveRAIDLevel(cda, ld_number, kRAID50, 2) == kCanSetEditableLogicalDriveRAIDLevelOK)
 *     {
 *         if (SA_SetEditableLogicalDriveRAIDLevel(cda, ld_number, kRAID50, 2) != kSetEditableLogicalDriveRAIDLevelOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual logical drive's RAID level to 5+0 with 2 parity groups\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableLogicalDriveRAIDLevel(PCDA cda, SA_WORD editable_ld_number, SA_BYTE raid_level, SA_BYTE parity_group_count, SA_BOOL set_default_strip_size_and_max_size);
   /** Failed to modify the editable logical drive's RAID level.
    * @outputof SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kSetEditableLogicalDriveRAIDLevelFailedUnknown      0x00
   /** Successfully modified the editable logical drive's RAID level.
    * @outputof SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kSetEditableLogicalDriveRAIDLevelOK                 0x01
/** @} */ /* Set Editable Logical Drive RAID */

/** @name Get Editable Array Parity Group Count Values
 * @{ */
/** Get the allowable and default parity group count values for the editable array.
 * @param[in] cda                         Controller data area.
 * @param[in] editable_array_number       Target array number.
 * @param[in] raid_level                  Target RAID level.
 * @param[in,out] max_parity_group_count  Max size of parity count array.
 * @param[out] parity_group_count_array   Array to hold parity count values.
 * @param[out] default_parity_group_count Default parity group count.
 * @return  On success, returns @ref kGetEditableArrayParityGroupCountValuesOK with default parity group count set in @p default_parity_group_count. @p parity_group_count_array holds a list of all parity group count values supported for @p raid_level. On failure or when parity group does not apply to @p raid_level, @p default_parity_group_count will be set to 0.
 *
 * __Example__
 * @code
 * #define MAX_PARITY_GROUP_COUNT  32
 *  SA_BYTE max_parity_group_count = MAX_PARITY_GROUP_COUNT;
 *  SA_BYTE parity_group_count_array[MAX_PARITY_GROUP_COUNT];
 *  SA_BYTE default_parity_group_count = 0;
 *  SA_BYTE index = 0;
 *
 *  if (SA_GetEditableArrayParityGroupCountValues(cda, 0, kRAID50, &max_parity_group_count, parity_group_count_array, &default_parity_group_count) == kGetEditableArrayParityGroupCountValuesOK
 *      &&
 *      default_parity_group_count != 0)
 *  {
 *     printf("(default npg: %u, allowable values: [", default_parity_group_count);
 *     for (; index < max_parity_group_count; index++)
 *     {
 *        printf("%u", parity_group_count_array[index]);
 *        if (index < max_parity_group_count - 1)
 *        {
 *           printf(", ");
 *        }
 *     }
 *     printf("])\n");
 *  }
 * @endcode
 */
SA_BYTE SA_GetEditableArrayParityGroupCountValues(PCDA cda, SA_WORD editable_array_number, SA_BYTE raid_level, SA_BYTE *max_parity_group_count, SA_BYTE *parity_group_count_array, SA_BYTE *default_parity_group_count);
   /** Successfully determined [and returned] valid parity group count values.
    * @outputof SA_GetEditableArrayParityGroupCountValues.
    */
   #define kGetEditableArrayParityGroupCountValuesOK                      0x01
   /** Failed to determined valid parity group count values because no editable configuration has been created.
    * @attention Outputs of @ref SA_GetEditableArrayParityGroupCountValues are undefined.
    * @outputof SA_GetEditableArrayParityGroupCountValues.
    */
   #define kGetEditableArrayParityGroupCountValuesNoEditableConfigCreated 0x02
   /** Failed to determined valid parity group count values because the logical drive has not been assigned to an Array.
    * @attention Outputs of @ref SA_GetEditableArrayParityGroupCountValues are undefined.
    * @outputof SA_GetEditableArrayParityGroupCountValues.
    */
   #define kGetEditableArrayParityGroupCountValuesNoArraySet              0x03
   /** Failed to determined valid parity group count values because @p editable_array_number is invalid.
    * @attention Outputs of @ref SA_GetEditableArrayParityGroupCountValues are undefined.
    * @outputof SA_GetEditableArrayParityGroupCountValues.
    */
   #define kGetEditableArrayParityGroupCountValuesNoDrivesAdded           0x04
   /** Failed to determined valid parity group count values becuase one or more arguments have are invalid.
    * @attention Outputs of @ref SA_GetEditableArrayParityGroupCountValues are undefined.
    * @outputof SA_GetEditableArrayParityGroupCountValues.
    */
   #define kGetEditableArrayParityGroupCountValuesInvalidArguments        0x05
/** @} */ /* Get Editable Array Parity Group Count Values */

/** @} */ /* storc_configuration_edit_ld_set_raid */

/** @defgroup storc_configuration_edit_ld_set_drive_label Logical Drive Label Configuration
 * @brief Set/change an editable logical drive's label/name.
 * @{ */

/** @name Get Editable Logical Drive Label
 * @{ */
/** Get the current logical drive label.
 *
 * @pre `buffer_size` must be at least 65 bytes to contain the label and a terminating null.
 *
 * @param[in] cda                  Controller handler.
 * @param[in] editable_ld_number   Target logical drive number.
 * @param[out] drive_label_buffer  Buffer to write the volume label.
 * @param[in] buffer_size          Size of output buffer.
 * @return See [Get Editable Logical Drive Label Returns](@ref storc_configuration_edit_ld_set_drive_label).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     char drive_label_buffer[65];
 *     SA_memset(drive_label_buffer, 0, sizeof(drive_label_buffer));
 *     // Get label for logical drive.
 *     if (SA_GetEditableLogicalDriveLabel(cda, ld_number, drive_label_buffer, sizeof(drive_label_buffer)))
 *     {
 *         printf("Logical Drive Label: %s\n", drive_label_buffer);
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_GetEditableLogicalDriveLabel(PCDA cda, SA_WORD editable_ld_number, char* drive_label_buffer, size_t buffer_size);
   /** Successfully retrieved the logical drive label.
    * @outputof SA_GetEditableLogicalDriveLabel.
    */
   #define kGetEditableLogicalDriveLabelOK                           0x01
   /** Failed to get the logical drive label because no editable configuration has been created.
    * @outputof SA_GetEditableLogicalDriveLabel.
    */
   #define kGetEditableLogicalDriveLabelNoEditableConfigCreated      0x02
   /** Failed to get the logical drive label because the output buffer is NULL.
    * @outputof SA_GetEditableLogicalDriveLabel.
    */
   #define kGetEditableLogicalDriveLabelNullBuffer                   0x03
   /** Failed to get the logical drive label because the output buffer is too small.
    * @outputof SA_GetEditableLogicalDriveLabel.
    */
   #define kGetEditableLogicalDriveLabelBufferTooSmall               0x04
/** @} */ /* Get Editable Logical Drive Label */

/** @name Can Set Editable Logical Drive Label
 * @{ */
/** Can we set the logical drive label to the input given?
 *
 * @pre `drive_label` must be smaller than 64 bytes (maximum size for drive label).
 * @post If valid, user may call @ref SA_SetEditableLogicalDriveLabel.
 *
 * @param[in] cda                   Controller data area.
 * @param[in] editable_ld_number    Target logical drive number.
 * @param[in] drive_label           Desired logical drive label.
 * @param[in] buffer_size           Size of desired logical drive label.
 * @return See [Can Set Editable Logical Drive Label Returns](@ref storc_configuration_edit_ld_set_drive_label).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     char drive_label[64];
 *     *drive_label = "Drive Label";
 *     // Edit editable logical drive.
 *     // Get label for logical drive.
 *     if (SA_CanSetEditableLogicalDriveLabel(cda, ld_number, drive_label, sizeof(drive_label)) == kCanSetEditableLogicalDriveLabelOK)
 *     {
 *         if (SA_SetEditableLogicalDriveLabel(cda, ld_number, drive_label, sizeof(drive_label)) != kSetEditableLogicalDriveLabelOK)
 *         {
 *             printf("Failed to set Logical Drive Label to %s\n", drive_label);
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableLogicalDriveLabel(PCDA cda, SA_WORD editable_ld_number, const char* drive_label, size_t buffer_size);
   /** The editable logical drive's label can be set.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @allows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelOK                        0x01
   /** The editable logical drive's label cannot be set because the editable configuration has not been created yet.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @disallows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelNoEditableConfigCreated   0x02
   /** The editable logical drive does not exist.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @disallows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelInvalidLogicalDrive       0x03
   /** The editable logical drive's label cannot be set because the logical drive has been locked.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @disallows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelLogicalDriveIsLocked      0x04
   /** The editable logical drive's label cannot be set because the logical drive has not been assigned to an array.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @disallows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelNoArraySet                0x05
   /** The editable logical drive's label cannot be set because the label contains invalid characters or embedded NULLs.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @disallows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelInvalidCharacter          0x06
   /** The editable logical drive's label cannot be set because the given buffer is invalid.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @disallows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelInvalidLabel              0x07
   /** The editable logical drive's label cannot be set because it is too long.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @disallows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelLabelTooLong              0x08
   /** The editable logical drive's label cannot for LU cache logical drives.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @disallows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelNotAllowedForLUCache      0x09
   /** The editable logical drive's label cannot be set because initialization is in progress.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @disallows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelInitializingLogicalDrive  0x0A
   /** The editable logical drive's label cannot be set because it is not @ref kLogicalDriveTypeData.
    * @outputof SA_CanSetEditableLogicalDriveLabel.
    * @disallows SA_SetEditableLogicalDriveLabel.
    */
   #define kCanSetEditableLogicalDriveLabelNotADataDrive             0x0B
/** @} */ /* Can Set Editable Logical Drive Label */

/** @name Set Editable Logical Drive Label
 * @{ */
/** Set the logical drive label.
 *
 * @pre The user has used @ref SA_CanSetEditableLogicalDriveLabel to ensure the operation can be performed.
 *
 * @param[in] cda                   Controller data area.
 * @param[in] editable_ld_number    Target logical drive number.
 * @param[in] drive_label           Desired logical drive label.
 * @param[in] buffer_size           Size of desired logical drive label.
 * @return See [Set Editable Logical Drive Label Returns](@ref storc_configuration_edit_ld_set_drive_label).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     char drive_label[64];
 *     *drive_label = "Drive Label";
 *     // Edit editable logical drive.
 *     // Get label for logical drive.
 *     if (SA_CanSetEditableLogicalDriveLabel(cda, ld_number, drive_label, sizeof(drive_label)) == kCanSetEditableLogicalDriveLabelOK)
 *     {
 *         if (SA_SetEditableLogicalDriveLabel(cda, ld_number, drive_label, sizeof(drive_label)) != kSetEditableLogicalDriveLabelOK)
 *         {
 *             printf("Failed to set Logical Drive Label to %s\n", drive_label);
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableLogicalDriveLabel(PCDA cda, SA_WORD editable_ld_number, const char *drive_label, size_t buffer_size);
   /** Failed to set the editable logical drive's label.
    * @outputof SA_SetEditableLogicalDriveLabel
    */
   #define kSetEditableLogicalDriveLabelFailedUnknown     0x00
   /** Successfully set the editable logical drive's label.
    * @outputof SA_SetEditableLogicalDriveLabel
    */
   #define kSetEditableLogicalDriveLabelOK                0x01
/** @} */ /* Set Editable Logical Drive Label */

/** @} */ /* storc_configuration_edit_ld_set_drive_label */

/** @defgroup storc_configuration_edit_ld_set_stripsize Logical Drive Strip Size Configuration
 * @brief Set/change an editable logical drive's strip-size
 * @{ */

/** @name Get Editable Logical Drive Strip Size Range
 * @{ */
/** Get the min, max and default strip size available for the editable logical drive.
 * @param[in] cda                      Controller data area.
 * @param[in] editable_ld_number       Target logical drive number.
 * @param[out] min_strip_size_exp      Minimum possible strip size.
 * @param[out] max_strip_size_exp      Maximum possible strip size.
 * @param[out] default_strip_size_exp  Default strip size.
 * @return See [Get Editable Logical Drive Strip Size Range Returns](@ref storc_configuration_edit_ld_set_stripsize).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_BYTE min_strip, max_strip, default_strip;
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     // Edit editable logical drive.
 *     // Get strip size for logical drive.
 *     if (SA_GetEditableLogicalDriveStripSizeRange(cda, ld_number, &min_strip, &max_strip, &default_strip) == kGetEditableLogicalDriveStripSizeRangeOK)
 *     {
 *         printf("Stripsizes Min: %u, Max: %u, Default: %u\n", min_strip, max_strip, default_strip);
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_GetEditableLogicalDriveStripSizeRange(PCDA cda, SA_WORD editable_ld_number, SA_BYTE *min_strip_size_exp, SA_BYTE *max_strip_size_exp, SA_BYTE *default_strip_size_exp);
   /** Successfully determined [and returned] valid stripsize ranges.
    * @outputof SA_GetEditableLogicalDriveStripSizeRange.
    */
   #define kGetEditableLogicalDriveStripSizeRangeOK                      0x01
   /** Failed to determined valid stripsize ranges because no editable configuration has been created.
    * @attention Outputs of @ref SA_GetEditableLogicalDriveStripSizeRange are undefined.
    * @outputof SA_GetEditableLogicalDriveStripSizeRange.
    */
   #define kGetEditableLogicalDriveStripSizeRangeNoEditableConfigCreated 0x02
   /** Failed to determined valid stripsize ranges because the logical drive has not been assigned to an Array.
    * @attention Outputs of @ref SA_GetEditableLogicalDriveStripSizeRange are undefined.
    * @outputof SA_GetEditableLogicalDriveStripSizeRange.
    */
   #define kGetEditableLogicalDriveStripSizeRangeNoArraySet              0x03
   /** Failed to determined valid stripsize ranges becuase the logical drive has not been assigned a RAID level.
    * @attention Outputs of @ref SA_GetEditableLogicalDriveStripSizeRange are undefined.
    * @outputof SA_GetEditableLogicalDriveStripSizeRange.
    */
   #define kGetEditableLogicalDriveStripSizeRangeNoRAIDLevelSet          0x04
   /** Failed to determined valid stripsize ranges becuase there exists no valid stripsizes for the configuration.
    * @attention Outputs of @ref SA_GetEditableLogicalDriveStripSizeRange are undefined.
    * @outputof SA_GetEditableLogicalDriveStripSizeRange.
    */
   #define kGetEditableLogicalDriveStripSizeRangeNoValidStripSize        0x05
   /** Unable to determine stripsize range for LU cache logical drive (use @ref SA_GetControllerStripSizeSupport instead).
    * @see SA_GetControllerStripSizeSupport
    * @attention Outputs of @ref SA_GetEditableLogicalDriveStripSizeRange are undefined.
    * @outputof SA_GetEditableLogicalDriveStripSizeRange.
    */
   #define kGetEditableLogicalDriveStripSizeInvalidLUCache               0x06
/** @} */ /* Get Editable Logical Drive Strip Size Range */

/** @name Can Set Editable Logical Drive Strip Size
 * @{ */
/** Can we set the desired strip size fot the editable logical drive?
 * @post If valid, user may call @ref SA_SetEditableLogicalDriveStripSize.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target logical drive number.
 * @param[in] strip_size_blocks   Desired strip size (in blocks).
 * @return See [Can Set Editable Logical Drive Strip Size Returns](@ref storc_configuration_edit_ld_set_stripsize).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveStripSize(cda, ld_number, 512) == kCanSetEditableLogicalDriveStripSizeOK)
 *     {
 *         if (SA_SetEditableLogicalDriveStripSize(cda, ld_number, 512) != kSetEditableLogicalDriveStripSizeOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual logical drive's stripsize to 512 blocks\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableLogicalDriveStripSize(PCDA cda, SA_WORD editable_ld_number, SA_DWORD strip_size_blocks);
   /** The editable logical drive's strip size can be set.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @allows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeOK                             1
   /** The editable logical drive's strip size cannot be set because the editable has not been created yet.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableLogicalDriveStripSizeNoEditableConfigCreated        2
   /** The editable logical drive's strip size cannot be set because the logical drive is locked.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    * @see SA_LockEditableLogicalDrive.
    */
   #define kCanSetEditableLogicalDriveStripSizeLogicalDriveIsLocked           3
   /** The editable logical drive's strip size cannot be set because the logical drive has not been assigned to an Array.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    * @see SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveStripSizeNoArraySet                     4
   /** The editable logical drive's strip size cannot be set because the logical drive has no assigned RAID level.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    * @see SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveStripSizeNoRAIDLevelSet                 5
   /** The editable logical drive's strip size cannot be set because the logical drive stripsize is already set to desired value
    * or has been set to the only allowed value (such as an LU cache logical drive).
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    * @see SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveStripSizeAlreadySet                     6
   /** The editable logical drive's strip size cannot be set because the input strip size is out-of-range.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeValueOutOfRange                7
   /** The editable logical drive's strip size cannot be set because the input strip size is invalid.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeInvalidValue                   8
   /** The editable logical drive's strip size cannot be set at the current stage of configuration.
    * @deprecated No longer used.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeNotAllowed                     9
   /** The editable logical drive's strip size cannot be changed because the controller does not support the operation.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeStripeMigrationNotSupported   10
   /** The editable logical drive's strip size cannot be changed because of the controller status.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeControllerStatusNotOK         11
   /** The editable logical drive's strip size cannot be changed because of the controller cache status.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeControllerCacheStatusNotOK    12
   /** The editable logical drive's strip size cannot be changed because the target Array is not a data Array.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeNotDataArray                  13
   /** The editable logical drive's strip size cannot be changed due to the Array status.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeArrayStatusNotOK              14
   /** The editable logical drive's strip size cannot be changed there is not enough free space on the Array.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeNotEnoughFreeSpaceForRAID     15
   /** The editable logical drive's strip size cannot be changed because of a limitation of the controller's memory.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeFullStripeSizeOverflow        16
   /** The editable logical drive's strip size cannot be set because the transformation queue is full.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeMaxTransformationsReached     17
   /** The editable logical drive strip size cannot be set while transformations have been disabled while LU caching is enabled.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeLUCachingEnabled              18
   /** The editable logical drive strip size cannot be set because the Array has new/uncommitted logical drives.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeArrayHasUnsavedLogicalDrives  19
   /** The editable logical drive strip size cannot be set because data preservation is not available.
    * @see storc_status_ctrl_status.
    * @see storc_status_ctrl_backup_power_source_status.
    * @outputof SA_CanSetEditableLogicalDriveStripSize.
    * @disallows SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveStripSizeDataPreservationNotAvailable  20
/** @} */ /* Can Set Editable Logical Drive Strip Size */

/** @name Set Editable Logical Drive Strip Size
 * @{ */
/** Set the requested strip size for the editable logical drive.
 * @pre The user has used @ref SA_CanSetEditableLogicalDriveStripSize to ensure the operation can be performed.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target logical drive number.
 * @param[in] strip_size_blocks   Desired strip size (in blocks).
 * @return See [Set Editable Logical Drive Strip Size Returns](@ref storc_configuration_edit_ld_set_stripsize).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveStripSize(cda, ld_number, 512) == kCanSetEditableLogicalDriveStripSizeOK)
 *     {
 *         if (SA_SetEditableLogicalDriveStripSize(cda, ld_number, 512) != kSetEditableLogicalDriveStripSizeOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual logical drive's stripsize to 512 blocks\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableLogicalDriveStripSize(PCDA cda, SA_WORD editable_ld_number, SA_DWORD strip_size_blocks);
   /** @name Set Editable Logical Drive Strip Size Returns
    * @outputof SA_SetEditableLogicalDriveStripSize.
    * @{ */
   /** Failed to set the editable logical drive's strip size.
    * @outputof SA_SetEditableLogicalDriveStripSize.
    */
   #define kSetEditableLogicalDriveStripSizeFailedUnknown      0x00
   /** Successfully set the editable logical drive's strip size.
    * @outputof SA_SetEditableLogicalDriveStripSize.
    */
   #define kSetEditableLogicalDriveStripSizeOK                 0x01
/** @} */ /* Set Editable Logical Drive Strip Size */

/** @} */ /* storc_configuration_edit_ld_set_stripsize */

/** @defgroup storc_configuration_edit_ld_set_size Logical Drive Size Configuration
 * @brief Set/change an editable logical drive's size.
 * @{ */

/** @name Get Editable Logical Drive Size Types
 * Input to @ref SA_GetEditableLogicalDriveSize.
 * @{ */
/** Specify output of @ref SA_GetEditableLogicalDriveSize as maximum size for logical drive size in bytes.
 * Input to @ref SA_GetEditableLogicalDriveSize.
 */
#define kLogicalDriveSizeMaxCreatableSizeBytes           0x20
/** Specify output of @ref SA_GetEditableLogicalDriveSize as maximum size for logical drive size in blocks.
 * Input to @ref SA_GetEditableLogicalDriveSize.
 */
#define kLogicalDriveSizeMaxCreatableSizeBlocks          0x21
/** Specify output of @ref SA_GetEditableLogicalDriveSize as maximum size for logical drive size in blocks per drive.
 * Input to @ref SA_GetEditableLogicalDriveSize.
 */
#define kLogicalDriveSizeMaxCreatableSizeBlocksPerDrive  0x22
/** Specify output of @ref SA_GetEditableLogicalDriveSize as minimum size for logical drive size in blocks.
 * Input to @ref SA_GetEditableLogicalDriveSize.
 */
#define kLogicalDriveSizeMinCreatableSizeBlocks          0x23
/** Specify output of @ref SA_GetEditableLogicalDriveSize as minimum size for logical drive size in bytes.
 * Input to @ref SA_GetEditableLogicalDriveSize.
 */
#define kLogicalDriveSizeMinCreatableSizeBytes           0x24
/** Specify output of @ref SA_GetEditableLogicalDriveSize as maximum size a logical drive can be extended to in bytes.
 * Input to @ref SA_GetEditableLogicalDriveSize.
 */
#define kLogicalDriveSizeMaxResizeableSizeBytes          0x25
/** Specify output of @ref SA_GetEditableLogicalDriveSize as maximum size a logical drive can be extended to in blocks.
 * Input to @ref SA_GetEditableLogicalDriveSize.
 */
#define kLogicalDriveSizeMaxResizeableSizeBlocks         0x26
 /** Specify output of @ref SA_GetEditableLogicalDriveSize as minimum size a logical drive can be resized to in bytes.
 * Input to @ref SA_GetEditableLogicalDriveSize.
 */
#define kLogicalDriveSizeMinResizeableSizeBytes          0x27
 /** Specify output of @ref SA_GetEditableLogicalDriveSize as minimum size a logical drive can be resized to in blocks.
 * Input to @ref SA_GetEditableLogicalDriveSize.
 */
#define kLogicalDriveSizeMinResizeableSizeBlocks         0x28
/** @} */ /* Get Editable Logical Drive Size Types */

/** @name Get Editable Logical Drive Size
 * @{ */
/** Get the min/max possible size for the editable logical drive.
 * @pre param `drive_size` cannot be NULL.
 * @param[in]  cda                 Controller data area.
 * @param[in]  editable_ld_number  Target LD number.
 * @param[in]  size_type           Type for return (see [Get Editable Logical Drive Size Types](@ref storc_configuration_edit_ld_set_size)).
 * @param[out] drive_size          Address to store Logical Drive size min/max.
 * @return See [Get Editable Logical Drive Size Returns](@ref storc_configuration_edit_ld_set_size).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_QWORD drive_size;
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     // Edit editable logical drive.
 *     if (SA_GetEditableLogicalDriveSize(cda, ld_number, kLogicalDriveSizeMaxCreatableSizeBytes, &drive_size) == kGetEditableLogicalDriveSizeOK)
 *     {
 *         printf("Max possible size of logical drive in bytes: %lu\n", drive_size);
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_GetEditableLogicalDriveSize(PCDA cda, SA_WORD editable_ld_number, SA_BYTE size_type, SA_QWORD *drive_size);
   /** Successfully stored the editable logical drive size to parameter `drive_size`.
    * @outputof SA_GetEditableLogicalDriveSize.
    */
   #define kGetEditableLogicalDriveSizeOK                      0x01
   /** Could not return the editable logical drive size because the editable configuration has not been created.
    * @outputof SA_GetEditableLogicalDriveSize.
    * @see SA_CreateEditableConfiguration.
    */
   #define kGetEditableLogicalDriveSizeNoEditableConfigCreated 0x02
   /** Could not return the editable logical drive size because the logical drive has not been assigned to an Array.
    * @outputof SA_GetEditableLogicalDriveSize.
    * @see SA_SetEditableLogicalDriveArray.
    */
   #define kGetEditableLogicalDriveSizeNoArraySet              0x03
   /** Could not return the editable logical drive size because the logical drive has not been assigned a RAID level.
    * @outputof SA_GetEditableLogicalDriveSize.
    * @see SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kGetEditableLogicalDriveSizeNoRAIDLevelSet          0x04
   /** Could not return the editable logical drive size because the logical drive has not been assigned a strip size.
    * @outputof SA_GetEditableLogicalDriveSize.
    * @see SA_SetEditableLogicalDriveStripSize.
    */
   #define kGetEditableLogicalDriveSizeNoStripSizeSet          0x05
    /** Could not return the editable logical drive size because the logical drive has not been assigned a size.
    * @outputof SA_GetEditableLogicalDriveSize.
    * @see SA_SetEditableLogicalDriveStripSize.
    */
   #define kGetEditableLogicalDriveSizeNoSizeSet               0x06
/** @} */ /* Get Editable Logical Drive Size */

/** @name Set Editable Logical Drive Size Types
 * Input to @ref SA_CanSetEditableLogicalDriveSize & @ref SA_SetEditableLogicalDriveSize.
 * @{ */
/** Interprets the size input in blocks.
 * Input to @ref SA_CanSetEditableLogicalDriveSize & @ref SA_SetEditableLogicalDriveSize.
 */
#define kEditableLogicalDriveSizeBlocks                 0x00
/** Interprets the size input in bytes.
 * Input to @ref SA_CanSetEditableLogicalDriveSize & @ref SA_SetEditableLogicalDriveSize.
 */
#define kEditableLogicalDriveSizeBytes                  0x01
/** @} */ /* Set Editable Logical Drive Size Types */

/** @name Can Set Editable Logical Drive Size
 * @{ */
/** Can the editable logical drive's size be set to the requested size?
 * @post If valid, user may call @ref SA_SetEditableLogicalDriveSize.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target logical drive number.
 * @param[in] size_type           Used to interpret `ld_size` (see [Set Editable Logical Drive Size Types](@ref storc_configuration_edit_ld_set_size))
 * @param[in] ld_size             Desired logical drive size.
 * @return See [Set Editable Logical Drive Size Types](@ref storc_configuration_edit_ld_set_size).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveSize(cda, ld_number, kEditableLogicalDriveSizeBytes, 1*1024*1024*1024*1024ULL) == kCanSetEditableLogicalDriveSizeOK)
 *     {
 *         if (SA_SetEditableLogicalDriveSize(cda, ld_number, kEditableLogicalDriveSizeBytes, 1*1024*1024*1024*1024ULL) != kSetEditableLogicalDriveSizeOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual logical drive's size to 1 TB\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableLogicalDriveSize(PCDA cda, SA_WORD editable_ld_number, SA_BYTE size_type, SA_QWORD ld_size);
   /** The editable logical drive size can be set.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @allows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeOK                                 1
   /** The editable logical drive size cannot be set because the editable configuration has not been created yet.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableLogicalDriveSizeNoEditableConfigCreated            2
   /** The editable logical drive size cannot be set because the logical drive is locked.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    * @see SA_LockEditableLogicalDrive.
    */
   #define kCanSetEditableLogicalDriveSizeLogicalDriveIsLocked               3
   /** The editable logical drive size cannot be set because the logical drive has not been assigned to an Array.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    * @see SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveSizeNoArraySet                         4
   /** The editable logical drive size cannot be set because the logical drive has not been assigned a RAID level.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    * @see SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveSizeNoRAIDLevelSet                     5
   /** The editable logical drive size cannot be set because the logical drive has not been assigned a strip size.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    * @see SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanSetEditableLogicalDriveSizeNoStripSizeSet                     6
   /** The editable logical drive size cannot be set because the input size type is invalid.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeInvalidSizeType                    7
   /** The editable logical drive size cannot be set because the input size is too small.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeRequestedSizeTooSmall              8
   /** The editable logical drive size cannot be set because the input size is too large.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeRequestedSizeTooLarge              9
   /** The editable logical drive size cannot be set at this stage of configuration.
    * @deprecated No longer used.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeNotAllowed                         10
   /** The editable logical drive size cannot be set because shrink is not allowed.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeRequestedSizeShrinkNotAllowed      11
   /** The editable logical drive size cannot be set because no size change.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeRequestedSizeNoSizeChange          12
   /** The editable logical drive size cannot be set because controller status is not OK.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeControllerStatusNotOK              13
   /** The editable logical drive size cannot be set because controller cache status is not OK.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeControllerCacheStatusNotOK         14
   /** The editable logical drive size cannot be set because it's status is not OK.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeArrayStatusNotOK                   15
   /** The editable logical drive size cannot be set because it is not a data drive
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeNotDataArray                       16
   /** The editable logical drive size cannot be set because controller doesn't support logical drive extension.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeLogicalDriveExtensionNotSupported  17
   /** The editable logical drive size cannot be set because its array's strip size is too large.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeExceedsTransformationMemory        18
   /** The editable logical drive size cannot be set because the transformation queue is full.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeMaxTransformationsReached          19
   /** The editable logical drive size cannot be set because transformations have been disabled while LU caching is enabled.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeLUCachingEnabled                   20
   /** The editable logical drive size cannot be set because data preservation is not available.
    * @see storc_status_ctrl_status.
    * @see storc_status_ctrl_backup_power_source_status.
    * @outputof SA_CanSetEditableLogicalDriveSize.
    * @disallows SA_SetEditableLogicalDriveSize.
    */
   #define kCanSetEditableLogicalDriveSizeDataPreservationNotAvailable       21
/** @} */ /* Can Set Editable Logical Drive Size */

/** @name Set Editable Logical Drive Size
 * @{ */
/** Set the editable logical drive's size.
 * @pre The user has used @ref SA_CanSetEditableLogicalDriveSize to ensure the operation can be performed.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target LD number.
 * @param[in] size_type           Same as @ref SA_CanSetEditableLogicalDriveSize (see [Set Editable Logical Drive Size Types](@ref storc_configuration_edit_ld_set_size)).
 * @param[in] ld_size             Target size of LD.
 * @return See [Set Editable Logical Drive Size Returns](@ref storc_configuration_edit_ld_set_size).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveSize(cda, ld_number, kEditableLogicalDriveSizeBytes, 1*1024*1024*1024*1024ULL) == kCanSetEditableLogicalDriveSizeOK)
 *     {
 *         if (SA_SetEditableLogicalDriveSize(cda, ld_number, kEditableLogicalDriveSizeBytes, 1*1024*1024*1024*1024ULL) != kSetEditableLogicalDriveSizeOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual logical drive's size to 1 TB\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableLogicalDriveSize(PCDA cda, SA_WORD editable_ld_number, SA_BYTE size_type, SA_QWORD ld_size);
   /** Failed to set the editable logical drive's size.
    * @outputof SA_SetEditableLogicalDriveSize
    */
   #define kSetEditableLogicalDriveSizeFailedUnknown     0x00
   /** Successfully set the editable logical drive's size.
    * @outputof SA_SetEditableLogicalDriveSize
    */
   #define kSetEditableLogicalDriveSizeOK                0x01
/** @} */ /* Set Editable Logical Drive Size */

/** @} */ /* storc_configuration_edit_ld_set_size */

/** @defgroup storc_configuration_edit_ld_set_init_method Logical Drive Initialization Method Configuration
 * @brief Set an editable logical drive's initialization method.
 * @{ */

/** @name Editable Logical Drive Initialization Methods (Mask)
 * Input to @ref SA_CanSetEditableLogicalDriveInitializationMethod & @ref SA_SetEditableLogicalDriveInitializationMethod.
 * @{ */
/** Used to set the editable logical drive's initialization method default (disables RPI & OPO).
 * Input to @ref SA_CanSetEditableLogicalDriveInitializationMethod & @ref SA_SetEditableLogicalDriveInitializationMethod.
 */
#define kLogicalDriveInitializationMethodDefault               0x00
/** Used to set the editable logical drive's initialization method to RPI (Rapid-Parity-Initialization).
 * Can be used in conjunction with @ref kLogicalDriveInitializationMethodOPO.
 * Input to @ref SA_CanSetEditableLogicalDriveInitializationMethod & @ref SA_SetEditableLogicalDriveInitializationMethod.
 */
#define kLogicalDriveInitializationMethodRapidParityInit       0x01
/** Used to set the editable logical drive's initialization method to (OPO) SSD Over-Provisioning Optimization.
 * Can be used in conjunction with @ref kLogicalDriveInitializationMethodRapidParityInit.
 * Input to @ref SA_CanSetEditableLogicalDriveInitializationMethod & @ref SA_SetEditableLogicalDriveInitializationMethod.
 */
#define kLogicalDriveInitializationMethodOPO                   0x02
/** @} */

/** @name Can Set Editable Logical Drive Initialization Method
 * @{ */
/** Can set the requested initialization method on the logical drive.
 * @post If valid, user may call @ref SA_SetEditableLogicalDriveInitializationMethod.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_ld_number     Target LD number.
 * @param[in] initialization_method  Desired parity initialization method (see [Editable Logical Drive Initialization Methods (Mask)](@ref storc_configuration_edit_ld_set_init_method)).
 * @return See [Can Set Editable Logical Drive Initialization Method Returns](@ref storc_configuration_edit_ld_set_init_method).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveInitializationMethod(cda, ld_number, kLogicalDriveInitializationMethodRapidParityInit | kLogicalDriveInitializationMethodOPO) == kCanSetEditableLogicalDriveInitializationMethodOK)
 *     {
 *         if (SA_SetEditableLogicalDriveInitializationMethod(cda, ld_number, kLogicalDriveInitializationMethodRapidParityInit | kLogicalDriveInitializationMethodOPO) != kSetEditableLogicalDriveInitializationMethodOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual logical drive's initialization to RPI with SSD Over-Provisioning Optimization\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableLogicalDriveInitializationMethod(PCDA cda, SA_WORD editable_ld_number, SA_BYTE initialization_method);
   /** The editable logical drive's initialization method can be set.
    * @allows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodOK                       1
   /** The editable logical drive's initialization method cannot be set because the editable configuration has not been created yet.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodNoEditableConfigCreated  2
   /** The editable logical drive's initialization method cannot be set because the target LD is locked.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    * @see SA_LockEditableLogicalDrive.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodLDIsLocked               3
   /** The editable logical drive's initialization method cannot be set because the logical drive has not been assigned to an Array.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    * @see SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodNoArraySet               4
   /** The editable logical drive's initialization method cannot be set because the logical drive has not been set a RAID level.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    * @see SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodNoRAIDLevelSet           5
   /** The editable logical drive's initialization method cannot be set because the logical drive does not support RPI.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod with @ref kLogicalDriveInitializationMethodRapidParityInit.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodRPINotSupported          6
   /** The editable logical drive's initialization method cannot be set because the logical drive's assigned RAID level does not support an initialization method.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodNotRequiredForRAID       7
   /** The editable logical drive's initialization method cannot be set because the logical drive does not support OPO.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod with @ref kLogicalDriveInitializationMethodOPO.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodOPONotSupported          8
   /** The editable logical drive's initialization method cannot be set because the logical drive's drives are not SSD's.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod with @ref kLogicalDriveInitializationMethodOPO.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodDriveTypeNotSSD          9
   /** The editable logical drive's initialization method cannot be set because the logical drive is not the first LD of the Array.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodNotFirstLDInArray        10
   /** The editable logical drive's initialization method cannot be set at this stage of configuration.
    * @deprecated No longer used.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodNotAllowed               11
   /** Initialization method cannot be set for LU cache logical drives.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodNotAllowedForLUCache     12
   /** Initialization method cannot be set for logical drives configured with LU cache.
    * @disallows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodLUCacheConfigured        13
   /** @ref kLogicalDriveInitializationMethodOPO cannot be set for arrays with only NVMe drives
    * @disallows SA_SetEditableLogicalDriveInitializationMethod.
    * @outputof SA_CanSetEditableLogicalDriveInitializationMethod.
    */
   #define kCanSetEditableLogicalDriveInitializationMethodNotAllowedForNVMeArray   14
/** @} */ /* Can Set Editable Logical Drive Initialization Method */

/** @name Set Editable Logical Drive Initialization Method
 * @{ */
/** Set the editable logical drive's initialization method.
 * @pre The user has used @ref SA_CanSetEditableLogicalDriveInitializationMethod to ensure the operation can be performed.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_ld_number     Target LD number.
 * @param[in] initialization_method  Desired parity initialization method (see [Editable Logical Drive Initialization Methods (Mask)](@ref storc_configuration_edit_ld_set_init_method)).
 * @return See [Set Editable Logical Drive Initialization Method Returns](@ref storc_configuration_edit_ld_set_init_method).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveInitializationMethod(cda, ld_number, kLogicalDriveInitializationMethodRapidParityInit | kLogicalDriveInitializationMethodOPO) == kCanSetEditableLogicalDriveInitializationMethodOK)
 *     {
 *         if (SA_SetEditableLogicalDriveInitializationMethod(cda, ld_number, kLogicalDriveInitializationMethodRapidParityInit | kLogicalDriveInitializationMethodOPO) != kSetEditableLogicalDriveInitializationMethodOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual logical drive's initialization to RPI with SSD Over-Provisioning Optimization\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableLogicalDriveInitializationMethod(PCDA cda, SA_WORD editable_ld_number, SA_BYTE initialization_method);
   /** Failed to set the editable logical drive's initialization method.
    * @outputof SA_SetEditableLogicalDriveInitializationMethod.
    */
   #define kSetEditableLogicalDriveInitializationMethodFailedUnknown 0x00
   /** Successfully set the the editable logical drive's initialization method.
    * @outputof SA_SetEditableLogicalDriveInitializationMethod.
    */
   #define kSetEditableLogicalDriveInitializationMethodOK            0x01
/** @} */ /* Set Editable Logical Drive Initialization Method */

/** @} */ /* storc_configuration_edit_ld_set_init_method */

/** @defgroup storc_configuration_edit_ld_set_accel Logical Drive Accelerator Configuration
 * @brief Set/change an editable logical drive's accelerator/cache settings.
 * @{ */

/** @name Get Editable Logical Drive Accelerator
 * @{ */
/** Get the current/virtual Array accelerator for the editable logical drive.
 * @pre param `drive_size` cannot be NULL.
 * @param[in]  cda                 Controller data area.
 * @param[in]  editable_ld_number  Target LD number.
 * @return One of the accelerator value of [Get Logical Drive Basic Info Returns](@ref storc_properties_ld_basic_info).
 * @return @ref kLogicalDriveAcceleratorUnknown
 * @return @ref kLogicalDriveAcceleratorNone
 * @return @ref kLogicalDriveAcceleratorControllerCache
 * @return @ref kLogicalDriveAcceleratorIOBypass
 * @return @ref kLogicalDriveAcceleratorLUCache
 */
SA_BYTE SA_GetEditableLogicalDriveAccelerator(PCDA cda, SA_WORD editable_ld_number);
/** @} */ /* Get Editable Logical Drive Accelerator */

/** @name Can Set Editable Logical Drive Accelerator
 * @{ */
/** Can the logical drive have the specified accelerator.
 * Accelerators are defined under @ref SA_GetLogicalDriveBasicInfo
 *
 * @post If valid, user may call @ref SA_SetEditableLogicalDriveAccelerator.
 * @see SA_GetLogicalDriveBasicInfo
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target LD number.
 * @param[in] accelerator         Desired accelerator type (see [Get Logical Drive Basic Info Returns](@ref storc_properties_ld_basic_info)).
 * @return See [Can Set Editable Logical Drive Accelerator Returns](@ref storc_configuration_edit_ld_set_accel).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveAccelerator(cda, ld_number, kLogicalDriveAcceleratorControllerCache) == kCanSetEditableLogicalDriveAcceleratorOK)
 *     {
 *         if (SA_SetEditableLogicalDriveAccelerator(cda, ld_number, kLogicalDriveAcceleratorControllerCache) != kSetEditableLogicalDriveAcceleratorOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual logical drive's accelerator to controller cache\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableLogicalDriveAccelerator(PCDA cda, SA_WORD editable_ld_number, SA_BYTE accelerator);
   /** The editable logical drive's accelerator can be set.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @allows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorOK                               1
   /** The editable logical drive's accelerator cannot be set because the editable logical drive has not been created yet.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableLogicalDriveAcceleratorLogicalDriveNotCreated           2
   /** The editable logical drive's accelerator cannot be set because the editable configuration has not been created yet.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableLogicalDriveAcceleratorNoEditableConfigCreated          3
   /** The editable logical drive's accelerator cannot be set because the logical drive has not been assigned to an Array.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    * @see SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveAcceleratorNoArraySet                       4
   /** The editable logical drive's accelerator cannot be set because the logical drive is not the first LD in the Array.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorNotTheFirstLDInArray             5
   /** The editable logical drive's accelerator cannot be set because the accelerator type is invalid.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorUnsupported                      6
   /** The editable logical drive's accelerator cannot be set because the controller only supports IO bypass acceleration.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorArrayUsesIOBypass                7
   /** The editable logical drive's accelerator cannot be set because the logical drive's drives are not SSD's.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorDriveTypeNotSSD                  8
    /** The editable logical drive's accelerator is already at the requested setting.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorAlreadySet                       9
   /** The editable logical drive's accelerator cannot be set at the current stage of configuration.
    * @deprecated No longer used.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorNotAllowed                       10
   /** The editable logical drive's accelerator cannot be set while array has transforming logical drive.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorArrayHasTransformingLogicalDrive 11
   /** The editable logical drive's accelerator cannot be set while array has recovering logical drive.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorArrayHasRecoveringLogicalDrive   12
   /** The editable logical drive's accelerator cannot be set for data/LU cache logical drives.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorLUCacheEnabled                   13
   /** The editable logical drive's accelerator cannot be set while the logical drive is initializing
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorInitializingLogicalDrive         14
   /** The editable logical drive's accelerator cannot be set while the cache's backup power source is missing or charging.
    * This rule can be bypassed if No-Battery-Write-Cache is enabled.
    * @deprecated No longer used.
    * @ref SA_GetControllerBackupPowerSourceStatus
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorBadPowerSource                   15
   /** The editable logical drive's accelerator cannot be set/changed for logical drives in a split mirror Array.
    * @see SA_GetLogicalDriveBasicInfo.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorSplitMirrorVolume                16
   /** The editable logical drive's accelerator cannot be set because the logical drive status does not allow the operation.
    * @see SA_GetLogicalDriveStatusInfo.
    * @outputof SA_CanSetEditableLogicalDriveAccelerator.
    * @disallows SA_SetEditableLogicalDriveAccelerator.
    */
   #define kCanSetEditableLogicalDriveAcceleratorLogicalDriveStatusNotOK          17
/** @} */ /* Can Set Editable Logical Drive Accelerator */

/** @name Set Editable Logical Drive Accelerator
 * @{ */
/** Set the accelerator for the editable logical drive.
 * @pre The user has used @ref SA_SetEditableLogicalDriveAccelerator to ensure the operation can be performed.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target LD number.
 * @param[in] accelerator         Desired accelerator type (see [Get Logical Drive Basic Info Returns](@ref storc_properties_ld_basic_info)).
 * @return See [Set Editable Logical Drive Accelerator Returns](@ref storc_configuration_edit_ld_set_accel).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     if (SA_CanSetEditableLogicalDriveAccelerator(cda, ld_number, kLogicalDriveAcceleratorControllerCache) == kCanSetEditableLogicalDriveAcceleratorOK)
 *     {
 *         if (SA_SetEditableLogicalDriveAccelerator(cda, ld_number, kLogicalDriveAcceleratorControllerCache) != kSetEditableLogicalDriveAcceleratorOK)
 *         {
 *             fprintf(stderr, "Failed to set virtual logical drive's accelerator to controller cache\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableLogicalDriveAccelerator(PCDA cda, SA_WORD editable_ld_number, SA_BYTE accelerator);
   /** Failed to set the editable logical drive's accelerator.
    * @outputof SA_SetEditableLogicalDriveAccelerator.
    */
   #define kSetEditableLogicalDriveAcceleratorFailedUnknown    0x00
   /** Successfully set the editable logical drive's accelerator.
    * @outputof SA_SetEditableLogicalDriveAccelerator.
    */
   #define kSetEditableLogicalDriveAcceleratorOK               0x01
/** @} */ /* Set Editable Logical Drive Accelerator */

/** @} */ /* storc_configuration_edit_ld_set_accel */

/** @defgroup storc_configuration_edit_ld_lock Lock Editable Logical Drive
 * @brief Lock a new editable logical drive's configuration.
 * @{ */

/** @name Can Lock Editable Logical Drive
 * @{ */
/** Can the editable logical drive configuration be locked?
 * @post If valid, user may call @ref SA_LockEditableLogicalDrive.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target LD number.
 * @return See [Can Lock Editable Logical Drive Returns](@ref storc_configuration_edit_ld_lock).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     // Edit logical drive.
 *     if (SA_CanLockEditableLogicalDrive(cda, ld_number) == kCanLockEditableLogicalDriveOK)
 *     {
 *         if (SA_LockEditableLogicalDrive(cda, ld_number) != kLockEditableLogicalDriveOK)
 *         {
 *             fprintf(stderr, "Failed to lock virtual logical drive\n");
 *         }
 *         else
 *         {
 *             // Commit editable logical drive
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanLockEditableLogicalDrive(PCDA cda, SA_WORD editable_ld_number);
   /** The editable logical drive can be locked.
    * @allows SA_LockEditableLogicalDrive.
    * @outputof SA_CanLockEditableLogicalDrive.
    */
   #define kCanLockEditableLogicalDriveOK                         0x01
   /** The editable logical drive cannot be locked because the logical drive is already locked.
    * @disallows SA_LockEditableLogicalDrive.
    * @outputof SA_CanLockEditableLogicalDrive.
    */
   #define kCanLockEditableLogicalDriveAlreadyLocked              0x02
   /** The editable logical drive cannot be locked because the editable configuration has not been created.
    * @disallows SA_LockEditableLogicalDrive.
    * @outputof SA_CanLockEditableLogicalDrive.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanLockEditableLogicalDriveNoEditableConfigCreated    0x03
   /** The editable logical drive cannot be locked because the logical drive has not been assigned to an Array.
    * @disallows SA_LockEditableLogicalDrive.
    * @outputof SA_CanLockEditableLogicalDrive.
    * @see SA_SetEditableLogicalDriveArray.
    */
   #define kCanLockEditableLogicalDriveNoArraySet                 0x04
   /** The editable logical drive cannot be locked because the logical drive has bit had a RAID level set.
    * @disallows SA_LockEditableLogicalDrive.
    * @outputof SA_CanLockEditableLogicalDrive.
    * @see SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanLockEditableLogicalDriveNoRAIDLevelSet             0x05
   /** The editable logical drive cannot be locked because the logical drive has not had a strip size set.
    * @disallows SA_LockEditableLogicalDrive.
    * @outputof SA_CanLockEditableLogicalDrive.
    * @see SA_SetEditableLogicalDriveStripSize.
    */
   #define kCanLockEditableLogicalDriveNoStripSizeSet             0x06
   /** The editable logical drive cannot be locked because the logical drive has not had a size set.
    * @disallows SA_LockEditableLogicalDrive.
    * @outputof SA_CanLockEditableLogicalDrive.
    * @see SA_SetEditableLogicalDriveSize.
    */
   #define kCanLockEditableLogicalDriveNoSizeSet                  0x07
   /** The editable LU cache logical drive cannot be locked because it has not been assigned to cache a logical drive.
    * @disallows SA_LockEditableLogicalDrive.
    * @outputof SA_CanLockEditableLogicalDrive.
    * @see SA_SetEditableLogicalDriveLUCachingLogicalDrive.
    */
   #define kCanLockEditableLogicalDriveNoCachingLogicalDriveSet   0x08
/** @} */ /* Can Lock Editable Logical Drive */

/** @name Lock Editable Logical Drive
 * @{ */
/** Lock the editable logical drive.
 * @attention When locking an editable logical drive that has not been supplied a logical drive accelerator
 * (see @ref SA_SetEditableLogicalDriveAccelerator), @ref SA_LockEditableLogicalDrive will attempt to assign
 * a default accelerator to the logical drive. It will first attempt to set the accelerator to @ref kLogicalDriveAcceleratorIOBypass, then
 * if that assignment is not allowed, attempt to set the accelerator to @ref kLogicalDriveAcceleratorControllerCache. If
 * neither of those accelerators are valid, then no accelerator will be assigned.
 * @pre The user has used @ref SA_CanLockEditableLogicalDrive to ensure the operation can be performed.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Target LD number.
 * @return See [Lock Editable Logical Drive Returns](@ref storc_configuration_edit_ld_lock).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     // Edit logical drive.
 *     if (SA_CanLockEditableLogicalDrive(cda, ld_number) == kCanLockEditableLogicalDriveOK)
 *     {
 *         if (SA_LockEditableLogicalDrive(cda, ld_number) != kLockEditableLogicalDriveOK)
 *         {
 *             fprintf(stderr, "Failed to lock virtual logical drive\n");
 *         }
 *         else
 *         {
 *             // Commit editable logical drive
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_LockEditableLogicalDrive(PCDA cda, SA_WORD editable_ld_number);
   /** Failed to lock the editable logical drive.
    * @outputof SA_LockEditableLogicalDrive.
    */
   #define kLockEditableLogicalDriveFailedUnknown  0x00
   /** Successfully locked the editable logical drive.
    * @outputof SA_LockEditableLogicalDrive.
    */
   #define kLockEditableLogicalDriveOK             0x01
/** @} */ /* Lock Editable Logical Drive */

/** @} */ /* storc_configuration_edit_ld_lock */

/** @defgroup storc_configuration_edit_lu_cache LU Cache Configuration
 * @brief Configure/modify a data/cache logical drive pair.
 * @details
 * ### Examples
 * - @ref storc_config_lu_cache
 * @see storc_features_ctrl_lu_cache
 * @see storc_properties_lu_ld
 * @{ */

/** @name Can Create Editable LU Cache Array
 * @{ */
/** Can an editable LU cache array be created on the controller?
 * @post If valid, user may call @ref SA_CreateEditableLUCacheArray.
 * @param[in] cda        Controller data area.
 * @return If the Array can be created, returns @ref kCanCreateEditableLUCacheArrayOK
 * @return If the Array cannot be created, returns a [Can Create Editable LU Cache Array](@ref storc_configuration_edit_lu_cache) reason.
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLUCacheArray(cda) == kCanCreateEditableLUCacheArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableLUCacheArray(cda);
 *     // Edit editable LU cache Array and commit
 *     // or delete Array.
 * }
 * @endcode
 */
SA_BYTE SA_CanCreateEditableLUCacheArray(PCDA cda);
   /** The editable Array can be created.
    * @outputof SA_CanCreateEditableLUCacheArray.
    * @allows SA_CreateEditableLUCacheArray.
    */
   #define kCanCreateEditableLUCacheArrayOK                           1
   /** The editable LU cache Array cannot be created because the editable configuration has not been created yet.
    * @outputof SA_CanCreateEditableLUCacheArray.
    * @disallows SA_CanCreateEditableLUCacheArray.
    */
   #define kCanCreateEditableLUCacheArrayNoEditableConfigCreated      2
   /** The editable LU cache Array cannot be created because the maximum number of supported logical drives has been reached.
    * @outputof SA_CanCreateEditableLUCacheArray.
    * @disallows SA_CanCreateEditableLUCacheArray.
    */
   #define kCanCreateEditableLUCacheArrayMaxLDCountReached            3
   /** The editable LU cache Array cannot be created because there are no available unassigned drives.
    * @outputof SA_CanCreateEditableLUCacheArray.
    * @disallows SA_CanCreateEditableLUCacheArray.
    */
   #define kCanCreateEditableLUCacheArrayNoUnassignedDrivesAvailable  4
   /** The editable LU cache Array cannot be created because there already exist an LU cache Array.
    * @outputof SA_CanCreateEditableLUCacheArray.
    * @disallows SA_CanCreateEditableLUCacheArray.
    */
   #define kCanCreateEditableLUCacheArrayLUCacheArrayAlreadyExist     5
   /** The editable LU cache Array cannot be created because LU caching is not supported on the controller.
    * @outputof SA_CanCreateEditableLUCacheArray.
    * @disallows SA_CanCreateEditableLUCacheArray.
    * @see SA_ControllerSupportsLUCache.
    */
   #define kCanCreateEditableLUCacheArrayNotSupported                 6
   /** The editable LU cache Array cannot be created due to the state of the controller cache module.
    * @outputof SA_CanCreateEditableLUCacheArray.
    * @disallows SA_CanCreateEditableLUCacheArray.
    */
   #define kCanCreateEditableLUCacheArrayCacheStatusNotOK             7
   /** The editable LU cache Array cannot be created due to a pending or outstanding transformation.
    * @outputof SA_CanCreateEditableLUCacheArray.
    * @disallows SA_CanCreateEditableLUCacheArray.
    */
   #define kCanCreateEditableLUCacheArrayHasTransformingLogicalDrive  8
/** @} */ /* Can Create Editable LU Cache Array */

/** @name Create Editable LU Cache Array
 * @{ */
/** Create a blank editable array for LU cache logical drives.
 * @pre The user has used @ref SA_CanCreateEditableLUCacheArray to ensure the operation can be performed.
 * @post Once created, use @ref storc_configuration_edit_array_create API to configure physical drives of the new array.
 * @param[in] cda        Controller data area.
 * @return The array number of the newly-created LU cache array.
 * @return @ref kInvalidLDNumber if the operation fails.
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLUCacheArray(cda) == kCanCreateEditableLUCacheArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableLUCacheArray(cda);
 *     // Edit editable lu cache Array and commit
 *     // or delete Array.
 * }
 * @endcode
 */
SA_WORD SA_CreateEditableLUCacheArray(PCDA cda);
/** @} */ /* Create Editable LU Cache Array */

/** @name Editable Logical Drive LU Cache Number
 * @{ */
/** Returns the editable logical drive number of either the caching or LU cached logical drive.
 * @param[in] cda        Controller data area.
 * @param[in] editable_ld_number  Current editable logical drive which is either an editable LU cached or caching logical drive.
 * @return If `editable_ld_number` is an editable data logical drive with an editable LU cached logical drive, returns the editable logical drive number of the caching logical drive.
 * @return If `editable_ld_number` is an editable caching logical drive, returns the editable logical drive number of the cached (data) logical drive.
 * @return If `editable_ld_number` is not a valid editable LU cached or caching logical drive, returns @ref kInvalidLDNumber.
 */
SA_WORD SA_GetEditableLogicalDriveLUCacheNumber(PCDA cda, SA_WORD editable_ld_number);
/** @} */ /* Editable Logical Drive LU Cache Number */

/** @name Editable Logical Drive LU Cache Write Policy
 * @{ */
/** Returns information of an editable LU cache logical drive.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_ld_number  Editable logical drive number which is either a LU cached or caching logical drive.
 * @return Editable LU cache's [write policy](@ref storc_properties_lu_cache_info).
 */
SA_BYTE SA_GetEditableLogicalDriveLUCacheWritePolicy(PCDA cda, SA_WORD editable_ld_number);
/** @} */ /* Editable Logical Drive LU Cache Number */

/** @name Can Set Logical Drive LU Cache
 * @{ */
SA_BYTE SA_CanSetEditableLogicalDriveLUCache(PCDA cda, SA_WORD editable_ld_number, SA_WORD editable_lu_ld_number, SA_BYTE lu_write_policy);
   /** The editable logical drive lu cache can be set.
    * @allows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheOK                          1
   /** The editable logical drive lu cache cannot be set because the editable configuration has not been created.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheNoEditableConfigCreated     2
   /** The editable logical drive lu cache cannot be set because the data logical drive `editable_ld_number` is invalid.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheInvalidDataLogicalDrive     3
   /** The editable logical drive lu cache cannot be set because the cache logical drive `editable_lu_ld_number` is invalid.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheInvalidLUCacheLogicalDrive  4
   /** The editable logical drive lu cache cannot be set because the cache logical drive `editable_lu_ld_number` has not been assigned to an Array.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    * @see SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveLUCacheLUCacheArrayNotSet          5
   /** The editable logical drive lu cache cannot be set because the cache logical drive `editable_lu_ld_number` does not have a RAID level set.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    * @see SA_SetEditableLogicalDriveRAIDLevel.
    */
   #define kCanSetEditableLogicalDriveLUCacheLUCacheRAIDNotSet           6
   /** The editable logical drive lu cache cannot be set because data logical drive `editable_ld_number` has not been assigned to an Array.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    * @see SA_SetEditableLogicalDriveArray.
    */
   #define kCanSetEditableLogicalDriveLUCacheDataArrayNotSet             7
   /** The editable logical drive lu cache cannot be set because the data logical drive is configured to use I/O bypass accelerator.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    * @see SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableLogicalDriveLUCacheIOBypassEnabled             8
   /** The editable logical drive lu cache cannot be set because the data/cache & policy are already set.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheAlreadySet                  9
   /** The editable logical drive lu cache cannot be set because the cache logical drive is already assigned to another data logical drive.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheReassignNotAllowed          10
   /** The editable logical drive lu cache cannot be set because the desired write policy is not supported.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    * @see SA_ControllerSupportsLUCacheRAIDPolicy.
    */
   #define kCanSetEditableLogicalDriveLUCachePolicyUnsupported           11
   /** The editable logical drive lu cache cannot be set because the desired write policy is invalid.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheInvalidPolicy               12
   /** The editable logical drive lu cache cannot be set because the data logical drive is too large to be cached by the cache logical drive.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheDataLogicalDriveTooLarge    13
   /** The editable logical drive lu cache cannot be set because the data logical drive is too small to be cached by the cache logical drive.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheDataLogicalDriveTooSmall    14
   /** The editable logical drive lu cache cannot be set because target data logical drive is not 'OK'.
    * @deprecated No longer used.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    * @see SA_GetLogicalDriveStatusInfo.
    */
   #define kCanSetEditableLogicalDriveLUCacheDataLogicalDriveNotOK       15
   /** The editable logical drive lu cache cannot be set because target cache logical drive is not 'OK'.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    * @see SA_GetLogicalDriveStatusInfo.
    */
   #define kCanSetEditableLogicalDriveLUCacheCacheLogicalDriveNotOK      16
   /** The editable logical drive lu cache cannot be set because data logical drive is already cached with another lu cache logical drive.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    * @see SA_GetEditableLogicalDriveLUCacheNumber.
    */
   #define kCanSetEditableLogicalDriveLUCacheAlreadyCached               17
   /** The editable logical drive lu cache cannot be set because the data logical drive is configured on drives with an unsupported blocksize for LU cache.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheUnsupportedBlockSize        18
   /** The editable logical drive lu cache cannot be set because the data logical drive is on a boot connector.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheCannotCacheBootVolumes      19
   /** The editable logical drive lu cache cannot be set because the cache logical drive is larger than the target data logical drive.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    */
   #define kCanSetEditableLogicalDriveLUCacheCacheLDLargerThanDataLD     20
   /** The editable logical drive lu cache cannot be set because the LU cache status is not OK.
    * @disallows SA_SetEditableLogicalDriveLUCache.
    * @outputof SA_CanSetEditableLogicalDriveLUCache.
    * @see SA_GetLogicalDriveLUCacheStatusInfo.
    */
   #define kCanSetEditableLogicalDriveLUCacheStatusNotOK                 21
/** @} */ /* Can Set Logical Drive LU Cache */

/** @name Set Logical Drive LU Cache
 * @{ */
SA_BYTE SA_SetEditableLogicalDriveLUCache(PCDA cda, SA_WORD editable_ld_number, SA_WORD editable_lu_ld_number, SA_BYTE lu_write_policy);
   #define kSetEditableLogicalDriveLUCacheFailedUnknown  1
   #define kSetEditableLogicalDriveLUCacheOK             2
/** @} */ /* Set Logical Drive LU Cache */

/** @name Set Logical Drive LU Cache Line Size
 * @{ */
/** Get the LU cache line size of an editable logical drive.
 * @param[in] cda [Controller data area](@ref storc_cda).
 * @param[in] ld_number LU cache or cached logical drive number.
 * @return LU cache line of the LU cache/cached logical drive (see [LU Cache Line Sizes](@ref storc_features_ctrl_lu_cache)).
 */
SA_BYTE SA_GetEditableLogicalDriveLUCacheLineSize(PCDA cda, SA_WORD ld_number);
/** @} */ /* Get Logical Drive LU Cache Line Size */

/** @name Can Set Logical Drive LU Cache Line Size
 * @{ */
/** Can the editable logical drive's LU cache line size be set?
 * @param[in] cda              [Controller data area](@ref storc_cda).
 * @param[in] ld_number        LU cache or cached logical drive number.
 * @param[in] cache_line_size  Desired LU cache line size (see [LU Cache Line Sizes](@ref storc_features_ctrl_lu_cache)).
 * @return Status indicating the LU cache line size can be set (see [Can Set Logical Drive LU Cache Line Size](@ref storc_configuration_edit_lu_cache)).
 */
SA_BYTE SA_CanSetEditableLogicalDriveLUCacheLineSize(PCDA cda, SA_WORD ld_number, SA_BYTE cache_line_size);
   /** The editable logical drive lu cache line size can be set.
    * @allows SA_SetEditableLogicalDriveLUCacheLineSize.
    * @outputof SA_CanSetEditableLogicalDriveLUCacheLineSize.
    */
   #define kCanSetEditableLogicalDriveLUCacheLineSizeOK                          1
   /** The editable logical drive lu cache line size cannot be set because the
    * editable configuration has not been created.
    * @disallows SA_SetEditableLogicalDriveLUCacheLineSize.
    * @outputof SA_CanSetEditableLogicalDriveLUCacheLineSize.
    */
   #define kCanSetEditableLogicalDriveLUCacheLineSizeNoEditableConfigCreated     2
   /** The editable logical drive lu cache line size cannot be set because the
    * target logical drive is not valid.
    * Either the logical drive does not exist or is a data logical drive with no
    * configured LU cache logical drive.
    * @disallows SA_SetEditableLogicalDriveLUCacheLineSize.
    * @outputof SA_CanSetEditableLogicalDriveLUCacheLineSize.
    */
   #define kCanSetEditableLogicalDriveLUCacheLineSizeInvalidLogicalDrive         3
   /** The editable logical drive lu cache line size cannot be set because the
    * cache logical drive is locked or already exists.
    * @disallows SA_SetEditableLogicalDriveLUCacheLineSize.
    * @outputof SA_CanSetEditableLogicalDriveLUCacheLineSize.
    */
   #define kCanSetEditableLogicalDriveLUCacheLineSizeLogicalDriveIsLocked        4
   /** The editable logical drive lu cache line size cannot be set because the
    * RAID level of the LU cache logical drive has not been set.
    * @disallows SA_SetEditableLogicalDriveLUCacheLineSize.
    * @outputof SA_CanSetEditableLogicalDriveLUCacheLineSize.
    */
   #define kCanSetEditableLogicalDriveLUCacheLineSizeRAIDNotSet                  5
   /** The editable logical drive lu cache line size cannot be set because the
    * desired cache line size is invalid.
    * @disallows SA_SetEditableLogicalDriveLUCacheLineSize.
    * @outputof SA_CanSetEditableLogicalDriveLUCacheLineSize.
    */
   #define kCanSetEditableLogicalDriveLUCacheLineSizeInvalidCacheLineSize        6
   /** The editable logical drive lu cache line size cannot be set because the
    * desired cache line size is not supported.
    * @disallows SA_SetEditableLogicalDriveLUCacheLineSize.
    * @outputof SA_CanSetEditableLogicalDriveLUCacheLineSize.
    */
   #define kCanSetEditableLogicalDriveLUCacheLineSizeUnsupportedCacheLineSize    7
   /** The editable logical drive lu cache line size cannot be set because the
    * cache line size is already the current setting.
    * @disallows SA_SetEditableLogicalDriveLUCacheLineSize.
    * @outputof SA_CanSetEditableLogicalDriveLUCacheLineSize.
    */
   #define kCanSetEditableLogicalDriveLUCacheLineSizeAlreadySet                  8
   /** The editable logical drive lu cache line size cannot be set because the
    * cache logical drive size is too large for the given cache line size.
    * @disallows SA_SetEditableLogicalDriveLUCacheLineSize.
    * @outputof SA_CanSetEditableLogicalDriveLUCacheLineSize.
    */
   #define kCanSetEditableLogicalDriveLUCacheLineSizeCacheLogicalDriveTooLarge   9
   /** The editable logical drive lu cache line size cannot be set because the
    * LU caching Array drive count is too large for the given RAID level and cache
    * line size.
    * @disallows SA_SetEditableLogicalDriveLUCacheLineSize.
    * @outputof SA_CanSetEditableLogicalDriveLUCacheLineSize.
    */
   #define kCanSetEditableLogicalDriveLUCacheLineSizeDriveCountTooLarge          10
/** @} */ /* Can Set Logical Drive LU Cache Line Size */

/** @name Set Logical Drive LU Cache Line Size
 * @{ */
 /** Set the cache line size of an editable logical drive.
  * @param[in] cda              [Controller data area](@ref storc_cda).
  * @param[in] ld_number        LU cache or cached logical drive number.
  * @param[in] cache_line_size  Desired LU cache line size (see [LU Cache Line Sizes](@ref storc_features_ctrl_lu_cache)).
  * @return Status indicating the LU cache line size was set (see [Set Logical Drive LU Cache Line Size](@ref storc_configuration_edit_lu_cache)).
  */
SA_BYTE SA_SetEditableLogicalDriveLUCacheLineSize(PCDA cda, SA_WORD ld_number, SA_BYTE cache_line_size);
   /** Failed to set the LU cache line size.
    * No changes to the editable configuration have been made.
    * @outputof SA_SetEditableLogicalDriveLUCacheLineSize.
    */
   #define kSetEditableLogicalDriveLUCacheLineSizeFailed  1
   /** Successfully set the editable logical drive's LU cache line size.
    * @outputof SA_SetEditableLogicalDriveLUCacheLineSize.
    */
   #define kSetEditableLogicalDriveLUCacheLineSizeOK      2
/** @} */ /* Set Logical Drive LU Cache Line Size */

/** @} */ /* storc_configuration_edit_lu_cache */

/** @} */ /* storc_configuration_edit_ld */

/************************************************************//**
 * @defgroup storc_configuration_edit_pd Editable Physical Drive
 * @brief Manage editable physical drives.
 * @{
***************************************************************/

/** @defgroup storc_configuration_edit_pd_enum Enumerate Editable Physical Drives
 * @{ */
/** Enumerate physical drives present in the controller editable configuration section.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Current physical drive number.
 * @return Next physical drive number after `pd_number`, or @ref kInvalidPDNumber.
 *
 * __Example__
 * @code
 * SA_WORD pd_number = kInvalidPDNumber;
 * while (SA_EnumerateEditableLogicalDrives(cda, pd_number) != kInvalidPDNumber)
 * {
 *    // Edit 'pd_number' physical drive.
 * }
 * @endcode
 */
SA_WORD SA_EnumerateEditablePhysicalDrives(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_configuration_edit_pd_enum */

/** @defgroup storc_configuration_edit_pd_usage_info Editable Physical Drives Usage Info
 * @{ */
/** Obtain usage info for an editable physical drive.
 * Use the same usage info as SA_GetPhysicalDriveUsageinfo.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Target physical drive number.
 * @return Usage data for editable physical drive (see [Physical Drive Usage Returns](@ref storc_properties_pd_usage)).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableLogicalDrive(cda) == kCanCreateEditableLogicalDriveOK)
 * {
 *     SA_QWORD drive_size;
 *     SA_WORD ld_number = SA_CreateEditableLogicalDrive(cda);
 *     // Edit editable logical drive.
 *     if (SA_GetEditablePhysicalDriveUsageInfo(cda, ld_number, kLogicalDriveSizeMaxCreatableSizeBytes, &drive_size) == kTrue)
 *     {
 *         // ...
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_GetEditablePhysicalDriveUsageInfo(PCDA cda, SA_WORD pd_number);
/** @} */ /* storc_configuration_edit_pd_usage_info */

/** @} */ /* storc_configuration_edit_pd */

/******************************************************//**
 * @defgroup storc_configuration_edit_array Editable Array
 * @brief Setup/configure new and existing editable Arrays.
 * @{
*********************************************************/

/** @defgroup storc_configuration_edit_array_enum Enumerate Editable Arrays
 * @brief Enumerate new or existing Arrays in the editable configuration.
 * @{ */
/** Enumerate the editable arrays.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Current Array number.
 * @return Next Array number, or @ref kInvalidArrayNumber.
 *
 * __Example__
 * @code
 * SA_WORD array_number = kInvalidArrayNumber;
 * while (SA_EnumerateEditableArrays(cda, array_number) != kInvalidArrayNumber)
 * {
 *    // Edit 'array_number' Array.
 * }
 * @endcode
 */
SA_WORD SA_EnumerateEditableArrays(PCDA cda, SA_WORD array_number);
/** @} */ /* storc_configuration_edit_array_enum */

/** @defgroup storc_configuration_edit_array_pd_enum Enumerate Editable Array Physical Drives
 * @brief Enumerate physical drives of a new or existing editable Array.
 * @{ */
/** Enumerate editable array data drive(s).
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Current Array number.
 * @param[in] pd_number     Current Array data drive number.
 * @return Next data drive number in Array, or @ref kInvalidPDNumber.
 *
 * __Example__
 * @code
 * SA_WORD pd_number = kInvalidPDNumber;
 * while (SA_EnumerateEditableArrayDataDrives(cda, 1, pd_number) != kInvalidPDNumber)
 * {
 *    // Edit 'pd_number' data drive of Array # 1.
 * }
 * @endcode
 */
SA_WORD SA_EnumerateEditableArrayDataDrives(PCDA cda, SA_WORD array_number, SA_WORD pd_number);

/** Enumerate editable array spare drive(s).
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Current Array number.
 * @param[in] pd_number     Current Array spare drive number.
 * @return Next spare drive number in Array, or @ref kInvalidPDNumber.
 *
 * __Example__
 * @code
 * SA_WORD pd_number = kInvalidPDNumber;
 * while (SA_EnumerateEditableArraySpareDrives(cda, 1, pd_number) != kInvalidPDNumber)
 * {
 *    // Edit 'pd_number' spare drive of Array # 1.
 * }
 * @endcode
 */
SA_WORD SA_EnumerateEditableArraySpareDrives(PCDA cda, SA_WORD array_number, SA_WORD pd_number);
/** @} */ /* storc_configuration_edit_array_pd_enum */

/** @defgroup storc_configuration_edit_array_ld_enum Enumerate Editable Array Logical Drives
 * @brief Enumerate the new or existing logical drives of a new or existing editable Array.
 * @{ */
/** Enumerate editable array logical drive(s).
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Current Array number.
 * @param[in] ld_number     Current Array logical drive number.
 * @return Next logical drive number in Array, or @ref kInvalidLDNumber.
 *
 * __Example__
 * @code
 * SA_WORD ld_number = kInvalidLDNumber;
 * while (SA_EnumerateEditableArrayLogicalDrives(cda, 1, ld_number) != kInvalidLDNumber)
 * {
 *    // Edit 'ld_number' logical drive of Array # 1.
 * }
 * @endcode
 */
SA_WORD SA_EnumerateEditableArrayLogicalDrives(PCDA cda, SA_WORD array_number, SA_WORD ld_number);
/** @} */ /* storc_configuration_edit_array_ld_enum */

/** @defgroup storc_configuration_edit_array_flags Editable Array Flags
 * @brief Get basic information of a new or existing editable Array.
 * @{ */
/** Get the flags for the editable array.
 * @param[in] cda                 Controller data area.
 * @param[in] editable_array_number  Target array number.
 * @return See [Editable Array Flags](@ref storc_configuration_edit_array_flags).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableArray(cda) == kCanCreateEditableArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableArray(cda);
 *     // Edit editable array.
 *     printf("Is RPI enabled?: %s\n",
 *         (SA_GetEditableArrayFlags(cda, array_number) & kEditableArrayFlagRapidParityInit
 *             ? "YES" : "NO"));
 * }
 * @endcode
 */
SA_DWORD SA_GetEditableArrayFlags(PCDA cda, SA_WORD editable_array_number);
   /** @name Editable Array Flags
    * @outputof SA_GetEditableArrayFlags.
    * @{ */
   /** Flag: Editable Array is new.
    * @outputof SA_GetEditableArrayFlags.
    */
   #define kEditableArrayFlagNewArray   0x00000001
   /** Flag: Editable Array is marked for IO Bypass Acceleration.
    * @outputof SA_GetEditableArrayFlags.
    */
   #define kEditableArrayFlagIOBypass   0x00000002
   /** Flag: Editable Array is a LU caching Array.
    * @outputof SA_GetEditableArrayFlags.
    */
   #define kEditableArrayFlagLUCaching  0x00000004
   /** Flag: Editable Array is on a boot connector.
    * @outputof SA_GetEditableArrayFlags.
    */
   #define kEditableArrayBootConnector  0x00000008
   /** @} */ /* Editable Array Flags */
/** @} */ /* storc_configuration_edit_array_flags */

/** @defgroup storc_configuration_edit_array_create Create Editable Array
 * @brief Create a new editable Array.
 * @{ */

/** @name Can Create Editable Array
 * @{ */
/** Can an array be created on the controller?
 * @post If valid, user may call @ref SA_CreateEditableArray.
 * @param[in] cda        Controller data area.
 * @return See [Can Create Editable Array Returns](@ref storc_configuration_edit_array_create).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableArray(cda) == kCanCreateEditableArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableArray(cda);
 *     // Edit editable Array and commit
 *     // or delete Array.
 * }
 * @endcode
 */
SA_BYTE SA_CanCreateEditableArray(PCDA cda);
   /** The editable Array can be created.
    * @outputof SA_CanCreateEditableArray.
    * @allows SA_CreateEditableArray.
    */
   #define kCanCreateEditableArrayOK                          1
   /** The editable Array cannot be created because the editable configuration has not been created yet.
    * @outputof SA_CanCreateEditableArray.
    * @disallows SA_CreateEditableArray.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanCreateEditableArrayNoEditableConfigCreated     2
   /** The editable Array cannot be created because the maximum number of supported logical drives has been reached.
    * @outputof SA_CanCreateEditableArray.
    * @disallows SA_CreateEditableArray.
    */
   #define kCanCreateEditableArrayMaxLDCountReached           3
   /** The editable Array cannot be created because there are no available unassigned drives.
    * @outputof SA_CanCreateEditableArray.
    * @disallows SA_CreateEditableArray.
    */
   #define kCanCreateEditableArrayNoUnassignedDrivesAvailable 4
   /** The editable Array cennot be created because creates are no longer allowed on the editable configuration.
    * @deprecated No longer used.
    * @outputof SA_CanCreateEditableArray.
    * @disallows SA_CreateEditableArray.
    */
   #define kCanCreateEditableArrayCreatesNotAllowed           5
/** @} */ /* Can Create Editable Array */

/** @name Create Editable Array
 * @{ */
/** Create a blank editable array.
 * @pre The user has used @ref SA_CanCreateEditableArray to ensure the operation can be performed.
 * @param[in] cda        Controller data area.
 * @return The array number of the newly-created array.
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableArray(cda) == kCanCreateEditableArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableArray(cda);
 *     // Edit editable Array and commit
 *     // or delete Array.
 * }
 * @endcode
 */
SA_WORD SA_CreateEditableArray(PCDA cda);
/** @} */ /* Create Editable Array */

/** @} */ /* storc_configuration_edit_array_create */

/** @defgroup storc_configuration_edit_array_delete Delete Editable Array
 * @brief Delete a new or existing editable Array.
 * @{ */

/** @name Can Delete Editable Array
 * @{ */
/** Can an array be deleted?
 * @post If valid, user may call @ref SA_DeleteEditableArray.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @return See [Can Delete Editable Array Returns](@ref storc_configuration_edit_array_delete).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableArray(cda) == kCanCreateEditableArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableArray(cda);
 *     if (SA_CanDeleteEditableArray(cda, array_number) == kCanDeleteEditableArrayOK)
 *     {
 *         if (SA_DeleteEditableArray(cda, array_number) != kDeleteEditableArrayOK)
 *         {
 *             fprintf(stderr, "Failed to delete virtual Array\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanDeleteEditableArray(PCDA cda, SA_WORD editable_array_number);
   /** The editable Array can be deleted.
    * @outputof SA_CanDeleteEditableArray.
    * @allows SA_DeleteEditableArray.
    */
   #define kCanDeleteEditableArrayOK                                1
   /** The editable Array cannot be deleted because the editable configuration has not been created yet.
    * @outputof SA_CanDeleteEditableArray.
    * @disallows SA_DeleteEditableArray.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanDeleteEditableArrayNoEditableConfigCreated           2
   /** The editable Array cannot be deleted because the array number is invalid or does not exist.
    * @outputof SA_CanDeleteEditableArray.
    * @disallows SA_DeleteEditableArray.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanDeleteEditableArrayInvalidArray                      3
   /** The editable Array cannot be deleted because the Array has new/uncommited logical drives.
    * @outputof SA_CanDeleteEditableArray.
    * @disallows SA_DeleteEditableArray.
    */
   #define kCanDeleteEditableArrayContainsUnsavedLogicalDrives      4
   /** The editable Array cannot be deleted because the operation is no longer allowed.
    * @deprecated No longer used.
    * @outputof SA_CanDeleteEditableArray.
    * @disallows SA_DeleteEditableArray.
    */
   #define kCanDeleteEditableArrayNotAllowed                        5
   /** The editable Array cannot be deleted because there are logical drives with assigned LU cache volumes.
    * @outputof SA_CanDeleteEditableArray.
    * @disallows SA_DeleteEditableArray.
    */
   #define kCanDeleteEditableArrayContainsLogicalDrivesWithLUCache  6
   /** The editable Array cannot be deleted because there are lu cache logical drives with write-back policy.
    * @outputof SA_CanDeleteEditableArray.
    * @disallows SA_DeleteEditableArray.
    */
   #define kCanDeleteEditableArrayContainsWriteBackLUCache          7
   /** The editable Array cannot be deleted because there are lu cache logical drives currently flushing.
    * @outputof SA_CanDeleteEditableArray.
    * @disallows SA_DeleteEditableArray.
    */
   #define kCanDeleteEditableArrayContainsFlushingLUCache           8
   /** The editable Array cannot be deleted because it contains a locked SED.
    * In order to delete this array, one of the following actions can be taken:
    * 1. Use @ref SA_DoClearConfiguration and @ref SA_CanClearConfiguration.
    * 2. Unlock all SEDs in the volume and perform a reboot (if necessary, see @ref SA_ControllerRebootRequired), then try deleting the array.
    * @outputof SA_CanDeleteEditableArray.
    * @disallows SA_DeleteEditableArray.
    */
   #define kCanDeleteEditableArrayContainsLockedSED                    9
/** @} */ /* Can Delete Editable Array */

/** @name Delete Editable Array
 * @{ */
/** Delete the editable array.
 * @pre The user has used @ref SA_CanDeleteEditableArray to ensure the operation can be performed.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @return See [Delete Editable Array Returns](@ref storc_configuration_edit_array_delete).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableArray(cda) == kCanCreateEditableArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableArray(cda);
 *     if (SA_CanDeleteEditableArray(cda, array_number) == kCanDeleteEditableArrayOK)
 *     {
 *         if (SA_DeleteEditableArray(cda, array_number) != kDeleteEditableArrayOK)
 *         {
 *             fprintf(stderr, "Failed to delete virtual Array\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_DeleteEditableArray(PCDA cda, SA_WORD editable_array_number);
   /** Failed to delete the editable Array.
    * @outputof SA_DeleteEditableArray.
    */
   #define kDeleteEditableArrayFailedUnknown   0x00
   /** Successfully deleteed the editable Array.
    * @outputof SA_DeleteEditableArray.
    */
   #define kDeleteEditableArrayOK              0x01
/** @} */ /* Delete Editable Array */

/** @} */ /* storc_configuration_edit_array_delete */

/** @defgroup storc_configuration_edit_edit_array_status_info Editable Array Status Info
 * @brief Get status of a new or existing editable Array.
 * @{ */
/** Get the status of the editable array.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @return @ref storc_status_array.
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableArray(cda) == kCanCreateEditableArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableArray(cda);
 *     printf("Array Status: %u\n", SA_GetEditableArrayStatusInfo(cda, array_number));
 * }
 * @endcode
 */
SA_DWORD SA_GetEditableArrayStatusInfo(PCDA cda, SA_WORD editable_array_number);
/** @} */ /* storc_configuration_edit_array_status_info */

/** @defgroup storc_configuration_edit_array_spare_config Editable Array Spare Configuration.
 * @brief Manage spare drives of a new or existing editable Array.
 * @{ */

/** @defgroup storc_configuration_edit_array_spare_type Editable Array Spare Type
 * @brief Manage the spare type of a new or existing editable Array.
 * @{ */
/** Get the spare type of the editable array.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @return See [Get Editable Array Spare Type Returns](@ref storc_properties_array_spare_type).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableArray(cda) == kCanCreateEditableArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableArray(cda);
 *     // Edit Array ...
 *     SA_BYTE spare_type = SA_GetEditableArraySpareType(cda, array_number);
 *     printf("Array spare type: %s\n", spare_type == kSpareDriveTypeAutoReplace
 *                                      ? "Auto Replace"
 *                                      : spare_type == kSpareDriveTypeDedicated
 *                                          ? "Dedicated" : "UNKNOWN");
 * }
 * @endcode
 */
SA_BYTE SA_GetEditableArraySpareType(PCDA cda, SA_WORD editable_array_number);
/** @} */ /* storc_configuration_edit_array_spare_type */

/** @defgroup storc_configuration_edit_array_add_spare_drive Add Editable Spare Drive
 * @brief Add a spare drive to a new or existing editable Array.
 * @{ */

/** @name Can Add Editable Spare Drive To Editable Array
 * @{ */
/** Can the selected physical drive be added to the array as a spare drive?
 * @post If valid, user may call @ref SA_AddEditableArraySpareDrive.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @param[in] requested_spare_type   Spare type (see [Editable Array Spare Types](@ref storc_configuration_edit_array_spare_type)).
 * @param[in] pd_number              Target spare drive number.
 * @return See [Can Add Editable Spare Drive To Editable Array Returns](@ref storc_configuration_edit_array_add_spare_drive).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableArray(cda) == kCanCreateEditableArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableArray(cda);
 *     if (SA_CanAddEditableArraySpareDrive(cda, array_number, kSpareDriveTypeAutoReplace, pd_number) == kCanAddEditableArraySpareDriveOK)
 *     {
 *         if (SA_AddEditableArraySpareDrive(cda, array_number, kSpareDriveTypeAutoReplace, pd_number) != kAddEditableSpareDriveOK)
 *         {
 *             fprintf(stderr, "Failed to add auto replace spare drive to virtual Array\n");
 *         }
 *     }
 * }
 * @endcode
 *
 * @remark It is recommended to warn clients if they attempt to add an SED to an array. If the SED becomes locked, the volumes in
 * the array will be inaccessible until all SEDs in the array are unlocked. After unlocking the SED, a reboot may be required
 * (use @ref SA_ControllerRebootRequired). Use @ref SA_GetPhysicalDriveEncryptionInfo to determine if a selected physical drive
 * is a SED. See [Add Editable Data Drive](@ref storc_configuration_edit_array_add_data_drive) for an example.
 */
SA_BYTE SA_CanAddEditableArraySpareDrive(PCDA cda, SA_WORD editable_array_number, SA_BYTE requested_spare_type, SA_WORD pd_number);
   /** The spare drive can be added to the Array.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @allows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveOK                            1
   /** The spare drive cannot be added to the editable Array because the editable configuration has not been created yet.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanAddEditableArraySpareDriveNoEditableConfigCreated       2
   /** The spare drive cannot be added to the editable Array because the target drive is not unassigned.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveDriveIsNotUnassigned          3
    /** Cannot add the editable spare drive to the editable Array because the drive is not configurable.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
    #define kCanAddEditableArraySpareDriveDriveIsNotConfigurable       4
   /** The spare drive cannot be added to the editable Array because the Array has no assigned data drives.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveArrayHasNoDataDrives          5
   /** The spare drive cannot be added to the editable Array because the spare drive type does not match the Array's data drives.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveCannotMixDriveTypes           6
   /** The spare drive cannot be added to the editable Array because of an incompatibility of the Array drive's blocksizes.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveIncompatibleBlockSize         7
   /** The spare drive cannot be added to the editable Array because the spare drive is too small.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveTooSmall                      8
   /** The spare drive cannot be added to the editable Array because the max number of spare drives has been reached.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveMaxSpareDriveCountReached     9
   /** The spare drive cannot be added to the editable Array because the spare drive type has not been set.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveNoSpareTypeSet                10
   /** The spare drive cannot be added to the editable Array because the spare drive type is invalid.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveInvalidSpareType              11
   /** The spare drive cannot be added to the editable Array because the Array contains only RAID 0 logical drives and the controller's SAM is set to 'Failure'.
    * For Array's with only RAID 0 volumes, the controller SAM must be 'Predictive'
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveInvalidSAM                    12
   /** The spare drive cannot be added to the editable Array because the desired spare type is invalid
    * for the Array while it contains at least 1 RAID 0 logical drive and the controller's SAM is set to 'Failure'.
    * For Array's with only RAID 0 volumes, the controller SAM must be 'Predictive'
    * or change the desired spare type.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveInvalidSpareTypeForRAID0      13
   /** The spare drive cannot be added to the editable Array because the spare drive type cannot be changed.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveCannotChangeSpareType         14
   /** The spare drive cannot be added to the editable Array because the Array status is not OK.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveArrayStatusNotOK              15
   /** The spare drive cannot be added to the editable Array because the Array has a failed spare drive assigned.
    * The failed spare drives must first be removed using @ref SA_RemoveEditableArraySpareDrive.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveArrayHasFailedSpareDrive      16
   /** The spare drive cannot be added to the editable Array because the spare drive is already assigned and not shareable.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveDriveIsNotUnassignedOrShareableStandby 17
    /** The spare drive cannot be added to the editable Array because the spare drive is already assigned as a spare drive.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveDriveAlreadyUsedAsSpare       18
   /** Cannot add the editable spare drive to the editable Array because PD changes to Arrays are no longer allowed.
    * @deprecated No longer used.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveNotAllowed                    19
    /** The spare drive cannot be added to the editable Array because the controller does not support auto replace spare type.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveAutoReplaceSpareNotSupported  20
   /** The editable spare drive cannot be added to the editable array because the array not a data array.
    *  This means that the array one of the following:
    *  - Caching Array
    *  - Split mirror primary Array
    *  - Split mirror backup Array
    *  - Split mirror backup orphan Array
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveNotDataArray                  21
   /** The editable spare drive cannot be added to the editable array because the drives are not on the same port type.
    * This can occur when one drive/Array is on a boot connector and the other is not.
    * @see SA_ConnectorIsBootConnector.
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveIncompatibleConnectors        22
   /** The editable spare drive cannot be added to the editable array because the drive is in a predictive failure state.
    * This can occur when the controller Spare Activation mode is type Predictive.
    * @see SA_GetPhysicalDriveStatusInfo
    * @see SA_GetEditableControllerSAM
    * @outputof SA_CanAddEditableArraySpareDrive.
    * @disallows SA_AddEditableArraySpareDrive.
    */
   #define kCanAddEditableArraySpareDriveIsPredictiveFailure           23
/** @} */ /* Can Add Editable Spare Drive To Editable Array */

/** @name Add Editable Spare Drive To Editable Array
 * @{ */
/** Add a spare drive to the editable array.
 * @pre The user has used @ref SA_CanAddEditableArraySpareDrive to ensure the operation can be performed.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @param[in] spare_type             Spare type (see [Editable Array Spare Types](@ref storc_configuration_edit_array_spare_type)).
 * @param[in] pd_number              Target spare drive number.
 * @return See [Add Editable Spare Drive To Editable Array Returns](@ref storc_configuration_edit_array_add_spare_drive).
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableArray(cda) == kCanCreateEditableArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableArray(cda);
 *     if (SA_CanAddEditableArraySpareDrive(cda, array_number, kSpareDriveTypeAutoReplace, pd_number) == kCanAddEditableArraySpareDriveOK)
 *     {
 *         if (SA_AddEditableArraySpareDrive(cda, array_number, kSpareDriveTypeAutoReplace, pd_number) != kAddEditableSpareDriveOK)
 *         {
 *             fprintf(stderr, "Failed to add auto replace spare drive to virtual Array\n");
 *         }
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_AddEditableArraySpareDrive(PCDA cda, SA_WORD editable_array_number, SA_BYTE spare_type, SA_WORD pd_number);
   /** Failed to add the spare drive to the editable Array.
    * @outputof SA_AddEditableArraySpareDrive.
    */
   #define kAddEditableSpareDriveFailedUnknown  0x00
   /** Successfully added the spare drive to the editable Array.
    * @outputof SA_AddEditableArraySpareDrive.
    */
   #define kAddEditableSpareDriveOK             0x01
/** @} */ /* Add Editable Spare Drive To Editable Array */

/** @} */ /* storc_configuration_edit_array_add_spare_drive */

/** @defgroup storc_configuration_edit_array_remove_spare_drive Remove Editable Spare Drive
 * @brief Remove a spare drive from a new or existing editable Array.
 * @{ */

/** @name Can Remove Editable Array Spare Drive
 * @{ */
/** Checks if the selected spare drive can be removed from the editable array.
 * @post If valid, user may call @ref SA_RemoveEditableArraySpareDrive.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @param[in] pd_number              Target spare drive number.
 * @return See [Can Remove Editable Array Spare Drive Returns](@ref storc_configuration_edit_array_remove_spare_drive).
 *
 * __Example__
 * @code
 * if (SA_CanRemoveEditableArraySpareDrive(cda, array_number, pd_number) == kCanRemoveEditableArraySpareDriveOK)
 * {
 *     if (SA_RemoveEditableArraySpareDrive(cda, array_number, pd_number) != kRemoveEditableArraySpareDriveOK)
 *     {
 *         fprintf(stderr, "Failed to remove spare drive from virtual Array\n");
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanRemoveEditableArraySpareDrive(PCDA cda, SA_WORD editable_array_number, SA_WORD pd_number);
   /** The editable spare drive can be removed from the editable array.
    * @outputof SA_CanRemoveEditableArraySpareDrive.
    * @allows SA_RemoveEditableArraySpareDrive.
    */
   #define kCanRemoveEditableArraySpareDriveOK                         1
   /** The editable spare drive cannot be removed from the editable array because the editable configuration has not been created yet.
    * @outputof SA_CanRemoveEditableArraySpareDrive.
    * @disallows SA_RemoveEditableArraySpareDrive.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanRemoveEditableArraySpareDriveNoEditableConfigCreated    2
   /** The editable spare drive cannot be removed from the editable array because the specified drive does not exist.
    * @outputof SA_CanRemoveEditableArraySpareDrive.
    * @disallows SA_RemoveEditableArraySpareDrive.
    */
   #define kCanRemoveEditableArraySpareDriveNotFound                   3
   /** The editable spare drive cannot be removed from the editable array because the array status is not OK.
    * @outputof SA_CanRemoveEditableArraySpareDrive.
    * @disallows SA_RemoveEditableArraySpareDrive.
    */
   #define kCanRemoveEditableArraySpareDriveArrayArrayStatusNotOK      4
   /** The editable spare drive cannot be removed from the editable array because physical drive changes to arrays are no longer allowed.
    * @deprecated No longer used.
    * @outputof SA_CanRemoveEditableArraySpareDrive.
    * @disallows SA_RemoveEditableArraySpareDrive.
    */
   #define kCanRemoveEditableArraySpareDriveNotAllowed                 5
   /** The editable spare drive cannot be removed from the editable array because the array is not a data array.
    *  This means that the array one of the following:
    *  - Caching Array
    *  - Split mirror primary Array
    *  - Split mirror backup Array
    *  - Split mirror backup orphan Array
    * @outputof SA_CanRemoveEditableArraySpareDrive.
    * @disallows SA_RemoveEditableArraySpareDrive.
    */
   #define kCanRemoveEditableArraySpareDriveNotDataArray                6
/** @} */ /* Can Remove Editable Array Spare Drive */

/** @name Remove Editable Array Spare Drive
 * @{ */
/** Removes the targeted spare drive from the targeted editable array.
 * @pre The user has used @ref SA_CanRemoveEditableArraySpareDrive to ensure the operation can be performed.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @param[in] pd_number              Target spare drive number.
 * @return See [Remove Editable Spare Drive Returns](@ref storc_configuration_edit_array_add_spare_drive).
 *
 * __Example__
 * @code
 * if (SA_CanRemoveEditableArraySpareDrive(cda, array_number, pd_number) == kCanRemoveEditableArraySpareDriveOK)
 * {
 *     if (SA_RemoveEditableArraySpareDrive(cda, array_number, pd_number) != kRemoveEditableArraySpareDriveOK)
 *     {
 *         fprintf(stderr, "Failed to remove spare drive from virtual Array\n");
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_RemoveEditableArraySpareDrive(PCDA cda, SA_WORD editable_array_number, SA_WORD pd_number);
   /** Failed to remove the spare drive from the editable array.
    * @outputof SA_RemoveEditableArraySpareDrive.
    */
   #define kRemoveEditableArraySpareDriveFailedUnknown  0x00
   /** Successfully removed the spare drive from the editable array.
    * @outputof SA_RemoveEditableArraySpareDrive.
    */
   #define kRemoveEditableArraySpareDriveOK             0x01
/** @} */ /* Remove Editable Array Spare Drive */

/** @} */ /* storc_configuration_edit_array_remove_spare_drive */

/** @} */ /* storc_configuration_edit_array_spare_config */

/** @defgroup storc_configuration_edit_array_add_data_drive Add Editable Data Drive
 * @brief Add a data drive to a new or existing editable Array.
 *
 * @details
 * # Examples
 * ## Basic Expand
 * @code
 * //           Array 1
 * //    Array 1                  Unassigned
 * //   ,------,                   ,------,
 * //  (        )                 (        )
 * //  |~------~|                 |~------~|
 * //  |        |                 |        |
 * //  |  PD 2  |                 |  PD 3  |
 * //  |        |                 |        |
 * //   "------"                   "------"
 * //
 * //         Add PD 3
 * //
 * //           Array 1
 * //   ,------,      ,------,
 * //  (        )    (        )
 * //  |~------~|    |~------~|
 * //  |        |    |        |
 * //  |  PD 2  |    |  PD 3  |
 * //  |        |    |        |
 * //   "------"      "------"
 * //
 * SA_WORD pd_number = 3;
 * if (SA_CanAddEditableArrayDataDrives(cda, 1, &pd_number, 1, kExpandTypeAddParityGroups) == kCanAddEditableArrayDataDrivesOK)
 * {
 *    if (SA_AddEditableArrayDataDrives(cda, 1, &pd_number, 1, kExpandTypeAddParityGroups) != kAddEditableArrayDataDrivesOK)
 *    {
 *       fprintf(stderr, "Failed to add data drive 3 to Array 1\n");
 *    }
 * }
 * @endcode
 *
 * ## Expand By Adding Drives To Each Parity Groups (Preserve NPG)
 * @code
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
 * SA_WORD pd_numbers[] = { 2, 4 };
 * if (SA_CanAddEditableArrayDataDrives(cda, 1, pd_numbers, 2, kExpandTypeAddDrivesToParityGroups) == kCanAddEditableArrayDataDrivesOK)
 * {
 *    if (SA_AddEditableArrayDataDrives(cda, 1, pd_numbers, 2, kExpandTypeAddDrivesToParityGroups) != kAddEditableArrayDataDrivesOK)
 *    {
 *       fprintf(stderr, "Failed to add data drives 2 & 4 to Array 1\n");
 *    }
 * }
 * @endcode
 *
 * ## Expand By Adding Parity Groups (Preserve Number of Drives Per Parity Group)
 * @code
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
 * SA_WORD pd_numbers[] = { 2, 4, 9 }; // Note that the removed drives can be part of the same parity group (or not).
 * if (SA_CanAddEditableArrayDataDrives(cda, 1, pd_numbers, 3, kExpandTypeAddParityGroups) == kCanAddEditableArrayDataDrivesOK)
 * {
 *    if (SA_AddEditableArrayDataDrives(cda, 1, pd_numbers, 3, kExpandTypeAddParityGroups) != kAddEditableArrayDataDrivesOK)
 *    {
 *       fprintf(stderr, "Failed to add data drives 2, 4, & 9 to Array 1\n");
 *    }
 * }
 * @endcode
 *
 * @remark It is recommended to warn clients if they attempt to add an SED to an array. If the SED becomes locked, the volumes in
 * the array will be inaccessible until all SEDs in the array are unlocked. After unlocking the SED, a reboot may be required
 * (use @ref SA_ControllerRebootRequired). Use @ref SA_GetPhysicalDriveEncryptionInfo to determine if a selected physical drive
 * is a SED.
 *
 * ### Warn Users When Adding an SED to an Array
 * @code
 * SA_WORD pd_numbers[] = { 2, 4 };
 * if (SA_CanAddEditableArrayDataDrives(cda, 1, pd_numbers, 2, kExpandTypeAuto) == kCanAddEditableArrayDataDrivesOK)
 * {
 *    int i = 0;
 *    for (i = 0; i < 2; i++)
 *    {
 *       SA_WORD pd_number = pd_list[pd_i];
 *       SA_UINT64 pd_encryption_info = SA_GetPhysicalDriveEncryptionInfo(cda, pd_number);
 *       if (pd_encryption_info & kPhysicalDriveEncryptionSEDDrive)
 *       {
 *          printf("PD %d is a SED. If this drive becomes locked in the future, logical drives on this array may become inaccessible.\n", pd_number);
 *       }
 *    }
 *    if (SA_AddEditableArrayDataDrives(cda, 1, pd_numbers, 2, kExpandTypeAuto) != kAddEditableArrayDataDrivesOK)
 *    {
 *       fprintf(stderr, "Failed to add data drives 2 & 4 to Array 1\n");
 *    }
 * }
 * @endcode
 *
 * @{ */

/** @name Logical Drive Parity Expand Behavior Types
 *
 * Input to @ref SA_CanAddEditableArrayDataDrives.
 * Input to @ref SA_AddEditableArrayDataDrives.
 * @{
 */
/** Tells @ref SA_CanAddEditableArrayDataDrives/@ref SA_AddEditableArrayDataDrives to add an equal of number drives to each parity group. */
#define kExpandTypeAddDrivesToParityGroups   0x01
/** Tells @ref SA_CanAddEditableArrayDataDrives/@ref SA_AddEditableArrayDataDrives to add drives by adding entire parity groups. */
#define kExpandTypeAddParityGroups           0x02
/** For each logical drive in the Array, @ref kExpandTypeAddDrivesToParityGroups will be attempted first and then @ref kExpandTypeAddParityGroups. */
#define kExpandTypeAuto                      0x03
/** @} */ /* Logical Drive Parity Expand Behavior Types */

/** @name Find Drives To Add To Array
 * @{ */
/** Enumerate through available drives that can be used in a list for @ref SA_CanAddEditableArrayDataDrives/@ref SA_AddEditableArrayDataDrives.
 * @param[in] cda          Controller handler.
 * @param[in] array_number Editable Array number.
 * @param[in] pd_number    Current enumerated physical drive number (or @ref kInvalidPDNumber).
 * @return Given the current drive number (`pd_number`), returns the 'next' drive number of a drive that can be added
 * to an Array using a call to @ref SA_CanAddEditableArrayDataDrives/@ref SA_AddEditableArrayDataDrives.
 * @return @ref kInvalidPDNumber if at the end of physical drive that can be added to an Array.
 */
SA_WORD SA_EnumerateEditableArrayAddablePhysicalDrives(PCDA cda, SA_WORD array_number, SA_WORD pd_number);
/** @} */ /* Find Drives to Add to Array */

/** @name Can Add Editable Data Drives To Array
 * @{ */
/** Can the selected physical drives be added to the array?
 * @post If valid, user may call @ref SA_AddEditableArrayDataDrives.
 *
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @param[in] pd_list                List of physical drive numbers.
 * @param[in] pd_list_size           Number of pd numbers in `pd_list` parameter.
 * @param[in] ld_parity_expand_type  Specify if drives are addeded to each parity group or entire parity groups are added; One of [Logical Drive Parity Expand Behavior Types](@ref storc_configuration_edit_array_add_data_drive).
 * @return See [Can Add Editable Data Drive To Array Returns](@ref storc_configuration_edit_array_add_data_drive).
 * @return See [Can Add Editable Data Drives To Array Returns](@ref storc_configuration_edit_array_add_data_drive).
 */
SA_BYTE SA_CanAddEditableArrayDataDrives(PCDA cda, SA_WORD editable_array_number, const SA_WORD *pd_list, size_t pd_list_size, SA_BYTE ld_parity_expand_type);
   /** Can add the editable data drive(s) to the editable Array.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @allows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesOK                                1
   /** Cannot add the editable data drive(s) to the Array because the editable configuration has not been created yet.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanAddEditableArrayDataDrivesNoEditableConfigCreated           2
   /** Cannot add the editable data drive(s) to the Array because selected Array is invalid or does not exist.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesInvalidArray                      3
   /** Cannot add the editable data drive(s) to the Array because the given value for `ld_parity_expand_type` is invalid.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesInvalidExpandType                 4
   /** Cannot add the editable data drive(s) to the Array because the selected drive list is invalid or empty.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesInvalidDriveList                  5
   /** Cannot add the editable data drive(s) to the Array because the selected drive list contains duplicate drives.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesDuplicateDrivesInList             6
   /** Cannot add the editable data drive to the Array because the drive list contains an invalid drive number.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesInvalidDrive                      7
   /** Cannot add the editable data drive to the Array because the data drive is not unassigned.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesDriveIsNotUnassigned              8
   /** Cannot add the editable data drive to the Array because the data drive(s) are too small.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesDriveTooSmall                     9
   /** Cannot add the editable data drive to the Array because the drive type of the target drive does not match the Array.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesCannotMixDriveTypes               10
   /** Cannot add the editable data drive to the Array because of an incompatibility with the current Array drive's blocksize.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesIncompatibleBlockSize             11
   /** Cannot add the editable data drive to the Array because the max number of data drives for the Array has been reached.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesMaxDataDriveCountReached          12
    /** Cannot add the editable data drive to the editable Array because the drive is not configurable.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesDriveIsNotConfigurable            13
   /** Cannot add the editable data drive(s) to the Array because the Array contains un-locked logical drives.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesArrayContainsUnsavedLogicalDrives 14
   /** Cannot add the editable data drive(s) to the editable Array because PD changes to Arrays are no longer allowed.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesNotAllowed                        15
   /** Cannot add the editable data drive(s) to the editable Array because the transformation is not supported by the controller.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesNotSupported                      16
   /** Cannot add the editable data drive(s) to the Array because the Array is not a data Array.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesNotDataArray                      17
   /** Cannot add the editable data drive(s) to the Array because the controller status is not 'OK'
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesControllerStatusNotOK             18
   /** Cannot add the editable data drive(s) to the Array because the controller cache status is not 'OK'
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesControllerCacheStatusNotOK        19
   /** Cannot add the editable data drive(s) to the Array because the target Array status is not 'OK'
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesArrayStatusNotOK                  20
   /** Cannot add the editable data drive(s) to the Array because the Array contains a logical drive that cannot be transformed.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesExceedsTransformationMemory       21
   /** Cannot add the editable data drive(s) to the Array because the maximum number of transformations would be exceeded.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesMaxTransformationsReached         22
   /** Cannot add the editable data drive(s) to the Array because the number of remaining drives for the logical drives is invalid.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesInvalidDriveCount                 23
   /** Cannot add the editable data drive(s) to the Array because LU caching is enabled disabling transformations.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesLUCachingEnabled                  25
   /** Cannot add the editable data drive(s) to the editable Array because the Array contains new/uncommitted logical drives.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesHasUnsavedLogicalDrives           26
   /** Cannot add the editable data drive(s) to the editable Array because the Array contains drives with an incompatible drive types.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesIncompatibleDriveType             27
   /** Cannot add the editable data drive(s) to the editable Array because data preservation is not available.
    * @see storc_status_ctrl_status.
    * @see storc_status_ctrl_backup_power_source_status.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesDataPreservationNotAvailable      28
   /** Cannot add the editable data drive(s) to the editable Array because the drives are not on the same port type.
    * This can occur when one drive/Array is on a boot connector and the other is not.
    * @see SA_ConnectorIsBootConnector.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesIncompatibleConnectors            29
   /** Cannot add the editable data drive(s) to the editable Array because at least one drive is in a predictive failure state.
    * This can occur when the controller Spare Activation mode is type Predictive.
    * @see SA_GetPhysicalDriveStatusInfo
    * @see SA_GetEditableControllerSAM
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesDriveIsPredictiveFailure          30
   /** Cannot add the editable data drive(s) to the editable Array because a logical drive's full stripe size would exceed the limit.
    * @outputof SA_CanAddEditableArrayDataDrives.
    * @disallows SA_AddEditableArrayDataDrives.
    */
   #define kCanAddEditableArrayDataDrivesFullStripeSizeOverflow            31
/** @} */ /* Can Add Editable Data Drives To Array */

/** @name Add Editable Data Drives To Array
 * @{ */
/** Add a set data drives to the editable array.
 * @pre The user has used @ref SA_CanAddEditableArrayDataDrives to ensure the operation can be performed.
 *
 * @param[in] cda                    Controller data area.
 * @param[in] array_number           Target Array number.
 * @param[in] pd_list                List of physical drive numbers.
 * @param[in] pd_list_size           Number of pd numbers in `pd_list` parameter.
 * @param[in] ld_parity_expand_type   Specify if drives are added from each parity group or entire parity groups are added; One of [Logical Drive Parity Expand Behavior Types](@ref storc_configuration_edit_array_add_data_drive).
 * @return See [Add Editable Data Drives To Array Returns](@ref storc_configuration_edit_array_add_data_drive).
 */
SA_BYTE SA_AddEditableArrayDataDrives(PCDA cda, SA_WORD array_number, const SA_WORD * pd_list, size_t pd_list_size, SA_BYTE ld_parity_expand_type);
   /** Failed to add the set of editable data drives from the editable Array.
    * @outputof SA_AddEditableArrayDataDrives.
    */
   #define kAddEditableArrayDataDrivesFailedUnknown        0x00
   /** Successfully added the set of data drives from the editable Array.
    * @outputof SA_AddEditableArrayDataDrives.
    */
   #define kAddEditableArrayDataDrivesOK                   0x01
/** @} */ /* Add Editable Data Drives To Array */

/** @} */ /* storc_configuration_edit_array_add_data_drive */

/** @defgroup storc_configuration_edit_array_remove_data_drive Remove Editable Data Drive
 * Brief Remove a data drive from a new or existing editable Array.
 * @details
 * # Examples
 * ## Basic Shrink
 * @code
 * //           Array 1
 * //   ,------,      ,------,
 * //  (        )    (        )
 * //  |~------~|    |~------~|
 * //  |        |    |        |
 * //  |  PD 2  |    |  PD 3  |
 * //  |        |    |        |
 * //   "------"      "------"
 * //
 * //         Remove PD 3
 * //
 * //    Array 1                  Unassigned
 * //   ,------,                   ,------,
 * //  (        )                 (        )
 * //  |~------~|                 |~------~|
 * //  |        |                 |        |
 * //  |  PD 2  |                 |  PD 3  |
 * //  |        |                 |        |
 * //   "------"                   "------"
 * //
 * SA_WORD pd_number = 3;
 * if (SA_CanRemoveEditableArrayDataDrives(cda, 1, &pd_number, 1, kShrinkTypeRemoveParityGroups) == kCanRemoveEditableArrayDataDrivesAllowed)
 * {
 *    if (SA_RemoveEditableArrayDataDrives(cda, 1, &pd_number, 1, kShrinkTypeRemoveParityGroups) != kRemoveEditableArrayDataDriveOK)
 *    {
 *       fprintf(stderr, "Failed to remove data drive 3 from Array 1\n");
 *    }
 * }
 * @endcode
 *
 * ## Shrink By Removing Drives From Parity Groups (Preserve NPG)
 * @code
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
 * SA_WORD pd_numbers[] = { 2, 4 }; // Note that the removed drives can be part of the same parity group (or not).
 * if (SA_CanRemoveEditableArrayDataDrives(cda, 1, pd_numbers, 2, kShrinkTypeRemoveDrivesFromParityGroups) == kCanRemoveEditableArrayDataDrivesAllowed)
 * {
 *    if (SA_RemoveEditableArrayDataDrives(cda, 1, pd_numbers, 2, kShrinkTypeRemoveDrivesFromParityGroups) != kRemoveEditableArrayDataDriveOK)
 *    {
 *       fprintf(stderr, "Failed to remove data drives 2 & 4 from Array 1\n");
 *    }
 * }
 * @endcode
 *
 * ## Shrink By Removing Parity Groups (Preserve Number of Drives Per Parity Group)
 * @code
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
 * SA_WORD pd_numbers[] = { 2, 4, 9 }; // Note that the removed drives can be part of the same parity group (or not).
 * if (SA_CanRemoveEditableArrayDataDrives(cda, 1, pd_numbers, 3, kShrinkTypeRemoveParityGroups) == kCanRemoveEditableArrayDataDrivesAllowed)
 * {
 *    if (SA_RemoveEditableArrayDataDrives(cda, 1, pd_numbers, 3, kShrinkTypeRemoveParityGroups) != kRemoveEditableArrayDataDriveOK)
 *    {
 *       fprintf(stderr, "Failed to remove data drives 2, 4, & 9 from Array 1\n");
 *    }
 * }
 * @endcode
 *
 * @{
 */

/** @name Logical Drive Parity Shrink Behavior Types
 *
 * Input to @ref SA_CanRemoveEditableArrayDataDrives.
 * Input to @ref SA_RemoveEditableArrayDataDrives.
 * @{
 */
/** Tells @ref SA_CanRemoveEditableArrayDataDrives/@ref SA_RemoveEditableArrayDataDrives to remove an equal of drives from each parity group. */
#define kShrinkTypeRemoveDrivesFromParityGroups 0x01
/** Tells @ref SA_CanRemoveEditableArrayDataDrives/@ref SA_RemoveEditableArrayDataDrives to remove drives by removing entire parity groups. */
#define kShrinkTypeRemoveParityGroups           0x02
/** For each logical drive in the Array, @ref kShrinkTypeRemoveDrivesFromParityGroups will be attempted first and then @ref kShrinkTypeRemoveParityGroups. */
#define kShrinkTypeAuto                         0x03
/** @} */ /* Logical Drive Parity Shrink Behavior Types */

/** @name Can Remove Editable Data Drives From Array
 * @{ */
/** Can the selected physical drives be removed from the array?
 * @post If valid, user may call @ref SA_RemoveEditableArrayDataDrives.
 *
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @param[in] pd_list                List of physical drive numbers.
 * @param[in] pd_list_size           Number of pd numbers in `pd_list` parameter.
 * @param[in] ld_parity_shrink_type   Specify if drives are removed from each parity group or entire parity groups are removed; One of [Logical Drive Parity Shrink Behavior Types](@ref storc_configuration_edit_array_remove_data_drive).
 * @return See [Can Remove Editable Data Drive From Array Returns](@ref storc_configuration_edit_array_remove_data_drive).
 * @return See [Can Remove Editable Data Drives From Array Returns](@ref storc_configuration_edit_array_remove_data_drive).
 */
SA_BYTE SA_CanRemoveEditableArrayDataDrives(PCDA cda, SA_WORD editable_array_number, const SA_WORD *pd_list, size_t pd_list_size, SA_BYTE ld_parity_shrink_type);
   /** Can remove the editable data drive(s) from the editable Array.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @allows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesOK                                1
   /** Cannot remove the editable data drive(s) from the Array because the editable configuration has not been created yet.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanRemoveEditableArrayDataDrivesNoEditableConfigCreated           2
   /** Cannot remove the editable data drive(s) from the Array because selected Array is invalid or does not exist.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesInvalidArray                      3
   /** Cannot remove the editable data drive(s) from the Array because the given value for `ld_parity_shrink_type` is invalid.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesInvalidShrinkType                 4
   /** Cannot remove the editable data drive(s) from the Array because the selected drive list is invalid or empty.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesInvalidDriveList                  5
   /** Cannot remove the editable data drive(s) from the Array because the selected drive list contains duplicate drives.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesDuplicateDrivesInList             6
   /** Cannot remove the editable data drive(s) from the Array because selected data drive is invalid or does not exist.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesInvalidDrive                      7
   /** Cannot remove the editable data drive(s) from the Array because the Array contains un-locked logical drives.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesArrayContainsUnsavedLogicalDrives 8
   /** Cannot remove the editable data drive(s) from the editable Array because PD changes to Arrays are no longer allowed.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesNotAllowed                        9
   /** Cannot remove the editable data drive(s) from the editable Array because the Array is not a data array.
    *  This means that the array one of the following:
    *  - Caching Array
    *  - Split mirror primary Array
    *  - Split mirror backup Array
    *  - Split mirror backup orphan Array
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesNotSupported                      10
   /** Cannot remove the editable data drive(s) from the Array because the Array is not a data Array.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesNotDataArray                      11
   /** Cannot remove the editable data drive(s) from the Array because the is not enough free space left on the remaining drives.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesNotEnoughSpaceForLogicalDrives    12
   /** Cannot remove the editable data drive(s) from the Array because the controller status is not 'OK'
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesControllerStatusNotOK             13
   /** Cannot remove the editable data drive(s) from the Array because the controller cache status is not 'OK'
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesControllerCacheStatusNotOK        14
   /** Cannot remove the editable data drive(s) from the Array because the target Array status is not 'OK'
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesArrayStatusNotOK                  15
   /** Cannot remove the editable data drive(s) from the Array because the Array contains a logical drive that cannot be transformed.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesExceedsTransformationMemory       16
   /** Cannot remove the editable data drive(s) from the Array because the maximum number of transformations would be exceeded.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesMaxTransformationsReached         17
   /** Cannot remove the editable data drive(s) from the Array because the number of remaining drives for the logical drives is invalid.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesInvalidDriveCount                 18
   /** Cannot remove the editable data drive(s) from the Array while transformations are disabled by LU caching.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesLUCachingEnabled                  19
   /** Cannot remove the editable data drive(s) from the Array because it would cause an invalid spare configuration.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesInvalidSpareConfiguration         20
   /** Cannot remove the editable data drive(s) from the Array because data preservation is not available.
    * @see storc_status_ctrl_status.
    * @see storc_status_ctrl_backup_power_source_status.
    * @outputof SA_CanRemoveEditableArrayDataDrives.
    * @disallows SA_RemoveEditableArrayDataDrives.
    */
   #define kCanRemoveEditableArrayDataDrivesDataPreservationNotAvailable      21
/** @} */ /* Can Remove Editable Data Drives From Array */

/** @name Remove Editable Data Drives From Array
 * @{ */
/** Remove a set data drives from the editable array.
 * @pre The user has used @ref SA_CanRemoveEditableArrayDataDrives to ensure the operation can be performed.
 *
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @param[in] pd_list                List of physical drive numbers.
 * @param[in] pd_list_size           Number of pd numbers in `pd_list` parameter.
 * @param[in] ld_parity_shrink_type   Specify if drives are removed from each parity group or entire parity groups are removed; One of [Logical Drive Parity Shrink Behavior Types](@ref storc_configuration_edit_array_remove_data_drive).
 * @return See [Remove Editable Data Drives From Array Returns](@ref storc_configuration_edit_array_remove_data_drive).
 */
SA_BYTE SA_RemoveEditableArrayDataDrives(PCDA cda, SA_WORD editable_array_number, const SA_WORD *pd_list, size_t pd_list_size, SA_BYTE ld_parity_shrink_type);
   /** Failed to remove the set of editable data drives from the editable Array.
    * @outputof SA_RemoveEditableArrayDataDrives.
    */
   #define kRemoveEditableArrayDataDrivesFailedUnknown        0x00
   /** Successfully removed the set of data drives from the editable Array.
    * @outputof SA_RemoveEditableArrayDataDrives.
    */
   #define kRemoveEditableArrayDataDrivesOK                   0x01
/** @} */ /* Remove Editable Data Drives From Array */

/** @} */ /* storc_configuration_edit_array_remove_data_drive */

/** @defgroup storc_configuration_edit_array_move Move/Swap Editable Array Data Drives
 * @brief Swap a physical drive of a new or existing editable Array with an available physical drive.
 * @{ */

/** @name Can Swap Editable Data Drive
 * @{ */
/** Can the selected (unassigned) physical drive be swapped with a data drive of the editable Array?
 * @post If valid, user may call @ref SA_SwapEditableArrayDataDrive.
 * @pre If the editable array contains any existing logical drives, this operation cannot be performed
 * once any create operations have performed (i.e. @ref SA_CreateEditableArray/@ref SA_CreateEditableLogicalDrive).
 * @param[in] cda              Controller data area.
 * @param[in] array_number     Target Array number.
 * @param[in] array_pd_number  Drive number of a data drive currently part of the array.
 * @param[in] new_pd_number    Target data drive number of the drive to swap with `array_pd_number`.
 * @return See [Can Swap Editable Data Drive Returns](@ref storc_configuration_edit_array_move).
 *
 * __Example__
 * @code
 * //           Array 1                Unassigned
 * //   ,------,      ,------,          ,------,
 * //  (        )    (        )        (        )
 * //  |~------~|    |~------~|        |~------~|
 * //  |        |    |        |  Swap  |        |
 * //  |  PD 2  |    |  PD 3  | <====> |  PD 4  |
 * //  |        |    |        |        |        |
 * //   "------"      "------"          "------"
 * //
 * //           Array 1                Unassigned
 * //   ,------,      ,------,          ,------,
 * //  (        )    (        )        (        )
 * //  |~------~|    |~------~|        |~------~|
 * //  |        |    |        |        |        |
 * //  |  PD 2  |    |  PD 4  |        |  PD 3  |
 * //  |        |    |        |        |        |
 * //   "------"      "------"          "------"
 * //
 * if (SA_CanSwapEditableArrayDataDrive(cda, 1, 3, 4) == kCanSwapEditableArrayDataDriveAllowed)
 * {
 *    if (SA_SwapEditableArrayDataDrive(cda, 1, 3, 4) != kSwapEditableArrayDataDriveOK)
 *    {
 *       fprintf(stderr, "Failed to swap Array 1 data drive 3 with drive 4\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSwapEditableArrayDataDrive(PCDA cda, SA_WORD array_number, SA_WORD array_pd_number, SA_WORD new_pd_number);
   /** Can swap the drive with an editable data drive of the editable Array.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @allows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveAllowed                         1
   /** Cannot swap the drive with an editable data drive of the Array because the editable configuration has not been created yet.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSwapEditableArrayDataDriveNoEditableConfigCreated         2
   /** Cannot swap the drive with an editable data drive of the Array because The target editable Array does not exist.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveArrayNotFound                   3
   /** Cannot swap the drive with an editable data drive of the Array because the target editable Array is not a data array.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveArrayNotDataArray               4
   /** Cannot swap the drive with an editable data drive of the Array because the current data drive number is not part of the Array.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveNotPartOfArray                  5
   /** Cannot swap the drive with an editable data drive of the Array because the new drive number does not exist.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveNotFound                        6
   /** Cannot swap the drive with an editable data drive of the Array because the target drive type is not compatilble with the Array.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveMixDriveTypes                   7
   /** Cannot swap the drive with an editable data drive of the Array because the target drive blocksize is not compatible with the Array.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveIncompatibleBlockSize           8
   /** Cannot swap the drive with an editable data drive of the Array because the target drive is too small to be part of the Array.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveIsTooSmall                      9
   /** Cannot swap the drive with an editable data drive of the Array because the target drive is current not usable for configuration.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveIsNotConfigurable               10
   /** Cannot swap the drive with an editable data drive of the Array because the target drive is already assigned to an Array.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveNotAvailable                    11
   /** Cannot swap the drive with an editable data drive of the Array because the controller status is not OK.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanSwapEditableArrayDataDriveControllerStatusNotOK           12
   /** Cannot swap the drive with an editable data drive of the Array because the controller's cache status is not OK.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    * @see SA_GetControllerStatusInfo.
    */
   #define kCanSwapEditableArrayDataDriveControllerCacheStatusNotOK      13
   /** Cannot swap the drive with an editable data drive of the Array because the Array status is not OK.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    * @see SA_GetEditableArrayStatusInfo.
    */
   #define kCanSwapEditableArrayDataDriveArrayStatusNotOK                14
   /** Cannot swap the drive with an editable data drive of the Array because the Array contains a logical drive that cannot be transformed.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    * @see SA_GetEditableArrayStatusInfo.
    */
   #define kCanSwapEditableArrayDataDriveExceedsTransformationMemory     15
   /** Cannot swap the drive with an editable data drive of the Array because Array move is currently not allowed in the editable configuration.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveIsNotAllowed                    16
   /** Cannot swap the drive with an editable data drive of the Array because Array move is not supported by the controller.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveNotSupported                    17
   /** Cannot swap the drive with an editable data drive of the Array because the transformation queue is full.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveMaxTransformationsReached       18
   /** Cannot swap the drive with an editable data drive of the Array because transformations have been disabled by LU cache.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveLUCachingEnabled                19
   /** Cannot swap the drive with an editable data drive of the Array as this would create an invalid spare configuration.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveInvalidSpareConfiguration       20
   /** Cannot swap the drive with an editable data drive of the Array because data preservation is not available.
    * @see storc_status_ctrl_status.
    * @see storc_status_ctrl_backup_power_source_status.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveDataPreservationNotAvailable    21
   /** Cannot swap the drive with an editable data drive of the Array because the drives are not on the same port type.
    * This can occur when one drive/Array is on a boot connector and the other is not.
    * @see SA_ConnectorIsBootConnector.
    * @outputof SA_CanSwapEditableArrayDataDrive.
    * @disallows SA_SwapEditableArrayDataDrive.
    */
   #define kCanSwapEditableArrayDataDriveIncompatibleConnectors          22
/** @} */ /* Can Swap Editable Data Drive */

/** @name Swap Editable Data Drive
 * @{ */
/** Swap the selected (unassigned) physical drive with a data drive of the editable Array.
 * @pre The user has used @ref SA_CanSwapEditableArrayDataDrive to ensure the operation can be performed.
 * @param[in] cda              Controller data area.
 * @param[in] array_number     Target Array number.
 * @param[in] array_pd_number  Drive number of a data drive currently part of the array.
 * @param[in] new_pd_number    Target data drive number of the drive to swap with `array_pd_number`.
 * @return See [Swap Editable Data Drive Returns](@ref storc_configuration_edit_array_move).
 *
 * __Example__
 * @code
 * if (SA_CanSwapEditableArrayDataDrive(cda, 1, 3, 4) == kCanSwapEditableArrayDataDriveAllowed)
 * {
 *    if (SA_SwapEditableArrayDataDrive(cda, 1, 3, 4) != kSwapEditableArrayDataDriveOK)
 *    {
 *       fprintf(stderr, "Failed to swap Array 1 data drive 3 with drive 4\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SwapEditableArrayDataDrive(PCDA cda, SA_WORD array_number, SA_WORD array_pd_number, SA_WORD new_pd_number);
   /** Failed to swap the drive with an editable data drive of the Array.
    * @outputof SA_SwapEditableArrayDataDrive.
    */
   #define kSwapEditableArrayDataDriveFailedUnknown  0x00
   /** Successfully swapped the drive with an editable data drive of the Array.
    * @outputof SA_SwapEditableArrayDataDrive.
    */
   #define kSwapEditableArrayDataDriveOK             0x01
/** @} */ /* Swap Editable Data Drive */

/** @} */ /* storc_configuration_edit_array_move */

/** @defgroup storc_configuration_edit_array_set_iobypass Editable Array I/O Bypass
 * @brief Manage the I/O accelerator of a new or existing editable Array.
 * @{ */

/** @name Can Set Editable Array I/O Bypass
 * @{ */
/** Can I/O bypass be enabled/disabled for the selected Array?
 * @post If valid, user may call @ref SA_SetEditableArrayIOBypass.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number   Target Array number.
 * @param[in] toggle                 @ref kTrue to enable I/O bypass, @ref kFalse to disable I/O bypass.
 * @return See [Can Set Editable Array I/O Bypass](@ref storc_configuration_edit_array_set_iobypass).
 * @see SA_GetEditableArrayFlags and @ref kEditableArrayFlagIOBypass.
 * __Example__
 * @code
 * if (SA_CanSetEditableArrayIOBypass(cda, 1, kTrue) == kCanSetEditableArrayIOBypassOK)
 * {
 *    if (SA_SetEditableArrayIOBypass(cda, 1, kTrue) != kSetEditableArrayIOBypassOK)
 *    {
 *       fprintf(stderr, "Failed to enable I/O bypass for Array 1\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetEditableArrayIOBypass(PCDA cda, SA_WORD editable_array_number, SA_BOOL toggle);
   /** I/O bypass may be enabled or disabled using @ref SA_SetEditableArrayIOBypass.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @allows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassOK                          1
   /** Cannot enable/disable I/O bypass for the Array because the editable configuration has not been created yet.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    * @see SA_CreateEditableConfiguration.
    */
   #define kCanSetEditableArrayIOBypassNoEditableConfigCreated     2
   /** Cannot enable/disable I/O bypass for the Array because the target Array was not found.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassInvalidArray                3
   /** Cannot enable/disable I/O bypass for the Array because I/O bypass is already enabled/disabled.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassAlreadySet                  4
   /** Cannot enable/disable I/O bypass for the Array because I/O bypass is not supported.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassNotSupported                5
   /** Cannot enable/disable I/O bypass for the Array because all data drives in the Array are not SSDs.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassDriveTypeNotSSD             6
   /** Cannot enable/disable I/O bypass for the Array because the Array has no data drives assigned.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassNoDataDrives                7
   /** Cannot enable/disable I/O bypass for the Array while array has transforming logical drive.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassHasTransformingLogicalDrive 8
   /** Cannot enable/disable I/O bypass for the Array while array has recovering logical drive.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassHasRecoveringLogicalDrive   9
   /** Cannot enable/disable I/O bypass for the Array while array has initializing logical drive.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassHasInitializingLogicalDrive 10
   /** Cannot enable/disable I/O bypass for an LU caching Array.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassLUCachingArray              11
   /** Cannot enable/disable I/O bypass for Arrays with logical drives configured with LU cache volumes.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassLUCacheConfigured           12
   /** Cannot enable/disable I/O bypass for the Array while array has failed logical drive(s).
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassHasFailedLogicalDrive       13
   /** Cannot enable/disable I/O bypass for the non-Data Array (such as a Split Mirror Array)
    * @see SA_GetArrayType.
    * @outputof SA_CanSetEditableArrayIOBypass.
    * @disallows SA_SetEditableArrayIOBypass.
    */
   #define kCanSetEditableArrayIOBypassNotDataArray                14
/** @} */ /* Can Set Editable Array I/O Bypass */

/** @name Set Editable Array I/O Bypass
 * @{ */
/** Enable/disable I/O bypass for the selected Array.
 * @post If valid, user may call @ref SA_SetEditableArrayIOBypass.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number   Target Array number.
 * @param[in] toggle                 @ref kTrue to enable I/O bypass, @ref kFalse to disable I/O bypass.
 * If I/O bypass is disabled, all logical drives will be set with I/O accelerator to @ref kLogicalDriveAcceleratorNone.
 * After I/O bypass has been disabled, the client may use @ref SA_SetEditableLogicalDriveAccelerator to enable/disable controller
 * caching as the accelerator.
 * @return See [Set Editable Array I/O Bypass](@ref storc_configuration_edit_array_set_iobypass).
 *
 * __Example__
 * @code
 * if (SA_CanSetEditableArrayIOBypass(cda, 1, kTrue) == kCanSetEditableArrayIOBypassOK)
 * {
 *    if (SA_SetEditableArrayIOBypass(cda, 1, kTrue) != kSetEditableArrayIOBypassOK)
 *    {
 *       fprintf(stderr, "Failed to enable I/O bypass for Array 1\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_SetEditableArrayIOBypass(PCDA cda, SA_WORD editable_array_number, SA_BOOL toggle);
   /** Failed to enable/disable I/O bypass for the Array.
    * @outputof SA_SetEditableArrayIOBypass.
    */
   #define kSetEditableArrayIOBypassFailedUnknown  0x00
   /** Successfully enabled/disabled I/O bypass for the Array.
    * @outputof SA_SetEditableArrayIOBypass.
    */
   #define kSetEditableArrayIOBypassOK             0x01
/** @} */ /* Set Editable Array I/O Bypass */

/** @} */ /* storc_configuration_edit_array_set_iobypass */

/** @defgroup storc_configuration_edit_array_set_size Editable Array Size
 * @brief Get the size of a new or existing editable Array.
 * @{ */
/** Get the size of the editable array.
 *
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Target Array number.
 * @param[in] size_type              Same as @ref SA_GetArraySize.
 * @return The size of the Array (units are determined by param `size_type`).
 * @see SA_GetArraySize.
 *
 * __Example__
 * @code
 * if (SA_CanCreateEditableArray(cda) == kCanCreateEditableArrayOK)
 * {
 *     SA_WORD array_number = SA_CreateEditableArray(cda);
 *     // Edit Array ...
 *     printf("Free space on editable Array (in bytes): %lu", SA_GetEditableArraySize(cda, array_number, kArrayFreeSizeBytes));
 * }
 * @endcode
 */
SA_QWORD SA_GetEditableArraySize(PCDA cda, SA_WORD editable_array_number, SA_BYTE size_type);
/** @} */ /* storc_configuration_edit_array_set_size */

/** @defgroup storc_configuration_edit_array_counts Editable Array Counts
 * @brief Get device counts for a new or existing editable Array.
 * @{ */
/** Get number of physical drives on an Editable Array.
 * @pre `editable_array_number` is a valid Editable Array number.
 * @param[in] cda                    Controller data area.
 * @param[in] editable_array_number  Valid target Editable Array number.
 * @return [Get Array Physical Drive Count Returns](@ref storc_properties_array_pd_count).
 * @return Decode with @ref NUM_DATA_PHYSICAL_DRIVES to get number of data drives on the Editable Array.
 * @return Decode with @ref NUM_SPARE_PHYSICAL_DRIVES to get number of spare drives on the Editable Array.
 * @return Decode with @ref NUM_FAILED_PHYSICAL_DRIVES to get number of failed drives on the Editable Array.
 * @return Decode with @ref NUM_PHYSICAL_DRIVES to get the total number of drives on the Editable Array.
 */
SA_DWORD SA_GetEditableArrayPhysicalDriveCounts(PCDA cda, SA_WORD editable_array_number);

/** @} */ /* storc_configuration_edit_array_counts */

/** @} */ /* storc_configuration_edit_array */

/** @} */ /* storc_configuration_virtual */

/*********************************************//**
 * @defgroup storc_configuration_direct Direct Configuration/Modification
 * @brief Configuration changes that apply immediately.
 * @details
 * A direct change to a configuration takes effect when the function is called.
 * These changes usually recommend reinitializing a CDA to ensure the API
 * returns newest values for all properties that might have been changed due to
 * a modification.
 *
 * Functions under the [Configuration](@ref storc_configuration) module that do
 * __NOT__ include *Editable* in the name are functions that execute direct changes.
 *
 * @{
 ************************************************/

/*********************************************//**
* @defgroup storc_configuration_ctrl Controller
* @{
************************************************/

/** @defgroup storc_configuration_ctrl_clear Clear Configuration
 * @brief Clear the controller's configuration.
 * @{ */
/** @name Controller Clear Configuration Action Flags
 * Inputs to @ref SA_CanClearConfiguration & @ref SA_DoClearConfiguration.
 * @{ */
   /** Specify to delete a Controller's Logical Drives.
    * Input to @ref SA_CanClearConfiguration & @ref SA_DoClearConfiguration.
    */
   #define kClearConfigurationRemoveLogicalDrives         0x01
   /** Specify to remove a Controller's License Keys.
    * Input to @ref SA_CanClearConfiguration & @ref SA_DoClearConfiguration.
    */
   #define kClearConfigurationRemoveLicenseKeys           0x02
   /** Specify to delete a Controller's encryption configuration.
    * Input to @ref SA_CanClearConfiguration & @ref SA_DoClearConfiguration.
    */
   #define kClearConfigurationRemoveEncryption            0x04
   /** Specify to delete a Controller's NVRAM.
    * Input to @ref SA_CanClearConfiguration & @ref SA_DoClearConfiguration.
    */
   #define kClearConfigurationClearNVRAM                  0x08
   /** Specify to delete controller configuration metadata on physical drives.
    * Input to @ref SA_CanClearConfiguration & @ref SA_DoClearConfiguration.
    */
   #define kClearConfigurationClearPhysicalDriveCCM       0x10
   /** Specify to complete reset a Controller's configuration to factory defaults.
    * Input to @ref SA_CanClearConfiguration & @ref SA_DoClearConfiguration.
    */
   #define kClearConfigurationReturnToFactory             0x1F
/** @} */

/** @name Can Clear Controller Configuration
 * @{ */
/** Can the Controller's configuration be cleared?
 *
 * What part of a Controller's configuration is can be cleared depends on
 * the `action_flags` parameter (see [Controller Clear Configuration Action Flags](@ref storc_configuration_ctrl_clear)).
 *
 * @post If valid, user may call @ref SA_DoClearConfiguration.
 * @param[in] cda           Controller data area.
 * @param[in] action_flags  Bitmap to limit checks to determine if a subset of the Controller's configuration can be cleared (see [Controller Clear Configuration Action Flags](@ref storc_configuration_ctrl_clear)).
 * @return Bitmap showing what parts of Controller's configuration can be cleared and/or reason(s) (see [Controller Can Clear Configuration Returns](@ref storc_configuration_ctrl_clear)).
 *
 * __Example__
 * @code
 * // Delete logical drives
 * if (SA_CanClearConfiguration(cda, kClearConfigurationRemoveLogicalDrives) == kClearConfigurationRemoveLogicalDrivesAllowed)
 * // or
 * // if (SA_CanClearConfiguration(cda, kClearConfigurationReturnToFactory) == kClearConfigurationRemoveLogicalDrivesAllowed)
 * {
 *    if (SA_DoClearConfiguration(cda, kClearConfigurationRemoveLogicalDrives) != ClearConfigurationLogicalDrivesRemoved)
 *    {
 *        fprintf(stderr, "Failed to delete logical drives from configuration\n");
 *    }
 * }
 * @endcode
 */
SA_DWORD SA_CanClearConfiguration(PCDA cda, SA_DWORD action_flags);
   /** If bit is set, Controller's Logical Drives can be deleted.
    * @outputof SA_CanClearConfiguration.
    * @allows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveLogicalDrives.
    */
   #define kClearConfigurationRemoveLogicalDrivesAllowed            0x00000001
   /** If bit is set, Controller's License Keys can be deleted.
    * @outputof SA_CanClearConfiguration.
    * @allows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveLicenseKeys.
    */
   #define kClearConfigurationRemoveLicenseKeysAllowed              0x00000002
   /** If bit is set, Controller's encryption configuration can be cleared.
    * @outputof SA_CanClearConfiguration.
    * @allows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveEncryption
    */
   #define kClearConfigurationRemoveEncryptionAllowed               0x00000004
   /** If bit is set, Controller's NVRAM can be deleted.
    * @outputof SA_CanClearConfiguration.
    * @allows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationClearNVRAM.
    */
   #define kClearConfigurationClearNVRAMAllowed                     0x00000008
   /** If bit is set, controller configuration metadata can be deleted on at least 1 physical drives.
    * @outputof SA_CanClearConfiguration.
    * @allows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationClearPhysicalDriveCCM.
    */
   #define kClearConfigurationClearPhysicalDriveCCMAllowed          0x00000010

   /** If bit is set, no logical drives present in the controller.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveLogicalDrives.
    */
   #define kClearConfigurationNoLogicalDrives                       0x00000020
   /** Controller is in HBA Mode or pending HBA Mode.
    * @outputof SA_CanClearConfiguration.
   */
   #define kClearConfigurationHBAMode                               0x00000040
   /** All physical drives are failed.
    * @deprecated No longer used.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration.
    */
   #define kClearConfigurationAllPhysicalDrivesFailed               0x00000080
   /** If bit is set, Controller does not support encryption.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveEncryption.
    */
   #define kClearConfigurationNoEncryptionSupport                   0x00000100
   /** Encryption is not configured on the controller.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveEncryption.
    */
   #define kClearConfigurationNoEncryptionConfiguration             0x00000200
   /** Express encryption is enabled.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveEncryption.
    */
   #define kClearConfigurationExpressEncryptionEnabled              0x00000400
   /** Logical drives exist.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveEncryption.
   */
   #define kClearConfigurationEncryptionLogicalDrivesExist          0x00000800
   /** An encryption-dependent operation is in progress.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveEncryption.
   */
   #define kClearConfigurationEncodeOrRekeyInProgress               0x00001000
   /** Controller does not support license keys.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveLicenseKeys.
    */
   #define kClearConfigurationNoLicenseKeySupport                   0x00002000
   /** Controller does not have license keys registered.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveLicenseKeys.
    */
   #define kClearConfigurationNoLicenseKeysRegistered               0x00004000
   /** Controller does not support clearing the NVRAM.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationClearNVRAM.
    */
   #define kClearConfigurationClearNVRAMNotSupported                0x00008000
   /** The controller has no attached physical drives.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationClearPhysicalDriveCCM.
    */
   #define kClearConfigurationClearPhysicalDriveCCMNoDrivesAttached 0x00010000
   /** The controller does not support the command for clearing CCM.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationClearPhysicalDriveCCM.
    */
   #define kClearConfigurationClearPhysicalDriveCCMNoCCM            0x00020000
   /** The controller is missing the encryption boot password needed to clear CCM.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationClearPhysicalDriveCCM.
    */
   #define kClearConfigurationClearPhysicalDriveCCMEncryptionLocked 0x00040000
   /** The controller is attached to at least 1 physical drive that does not support clearing CCM.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationClearPhysicalDriveCCM.
    */
   #define kClearConfigurationClearPhysicalDriveCCMNotSupported     0x00080000
   /** The controller has a volume that is in an abnormal state.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationRemoveLogicalDrives.
    */
   #define kClearConfigurationAbnormalVolumeExists                  0x00100000
   /** The controller is attached to at least 1 physical drive that does not support clearing CCM.
    * @outputof SA_CanClearConfiguration.
    * @disallows SA_DoClearConfiguration with `action_flag` @ref kClearConfigurationClearPhysicalDriveCCM.
    */
   #define kClearConfigurationControllerOFAActive                   0x80000000
/** @} */ /* Can Clear Controller Configuration Returns */

/** @name Clear Controller Configuration
 * @{ */
/** Clear the controller configuration for the requested items set in the flags.
 *
 * What part of a Controller's configuration is cleared depends on
 * the `action_flags` parameter (see [Controller Clear Configuration Action Flags](@ref storc_configuration_ctrl_clear)).
 *
 * @pre The user has used @ref SA_CanClearConfiguration to ensure the operation can be performed.
 *
 * @param[in] cda           Controller data area.
 * @param[in] action_flags  Bitmap to limits what subset of the Controller's configuration is cleared  (see [Controller Clear Configuration Action Flags](@ref storc_configuration_ctrl_clear)).
 * @return Bitmap showing what parts of Controller's configuration were cleared and/or reason(s) (see [Controller Clear Configuration Returns](@ref storc_configuration_ctrl_clear)).
 *
 * __Example__
 * @code
 * // Delete logical drives
 * if (SA_CanClearConfiguration(cda, kClearConfigurationRemoveLogicalDrives) == kClearConfigurationRemoveLogicalDrivesAllowed)
 * // or
 * // if (SA_CanClearConfiguration(cda, kClearConfigurationReturnToFactory) == kClearConfigurationRemoveLogicalDrivesAllowed)
 * {
 *    if (SA_DoClearConfiguration(cda, kClearConfigurationRemoveLogicalDrives) != ClearConfigurationLogicalDrivesRemoved)
 *    {
 *        fprintf(stderr, "Failed to delete logical drives from configuration\n");
 *    }
 * }
 * @endcode
 */
SA_WORD SA_DoClearConfiguration(PCDA cda, SA_DWORD action_flags);
   /** Logical Drives were removed.
    * @outputof SA_DoClearConfiguration.
    */
   #define ClearConfigurationLogicalDrivesRemoved            0x0001
   /** The license keys were cleared.
    * @outputof SA_DoClearConfiguration.
    */
   #define ClearConfigurationLicenseKeysCleared              0x0002
   /** The encryption configuration was cleared.
    * @outputof SA_DoClearConfiguration.
    */
   #define ClearConfigurationEncryptionConfigurationCleared  0x0004
   /** The controller NVRAM was cleared.
    * @outputof SA_DoClearConfiguration.
    */
   #define ClearConfigurationNVRAMCleared                    0x0008
   /** The controller configuration metadata was cleared on physical drives.
    * @outputof SA_DoClearConfiguration.
    */
   #define ClearConfigurationPhysicalDriveCCMCleared         0x0010
/** @} */ /* Clear Controller Configuration */

/** @} */ /* storc_configuration_ctrl_clear */

/** @defgroup storc_configuration_ctrl_bootable_volumes Bootable Volumes
 * @brief Manage the controller's legacy BIOS boot volume settings.
 * @{ */

/** @name Can Set Controller Bootable Volume
 * @{ */
/** Checks if we're able to set the primary/secondary bootable volume.
 * @post If valid, user may call @ref SA_DoSetBootableVolume.
 * @param[in] cda           Controller data area.
 * @param[in] precedence    Precedent of the bootable drive (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @param[in] drive_type    Type to specify if target drive is a logical or physical drive (see [Bootable Volume Drive Type](@ref storc_properties_ctrl_boot_volume)).
 * @param[in] drive_number  Target drive number of LD or PD.
 * @return See [Controller Can Set Bootable Volume Returns](@ref storc_configuration_ctrl_bootable_volumes).
 *
 * __Example__
 * @code
 * if (SA_CanSetBootableVolume(cda, kBootableVolumePrecedencePrimary, kBootableVolumeDriveTypeLogical, 1) == kSetBootableVolumeOperationAllowed)
 * {
 *    if (SA_DoSetBootableVolume(cda, kBootableVolumePrecedencePrimary, kBootableVolumeDriveTypeLogical, 1) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set primary bootable logical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetBootableVolume(PCDA cda, SA_BYTE precedence, SA_BYTE drive_type, SA_WORD drive_number);
   /** Set bootable volume is allowed.
    * @outputof SA_CanSetBootableVolume.
    * @allows SA_DoSetBootableVolume.
    */
   #define kSetBootableVolumeOperationAllowed              1
   /** The operation is not supported by this controller.
    * @outputof SA_CanSetBootableVolume.
    * @disallows SA_DoSetBootableVolume.
    */
   #define kSetBootableVolumeUnsupportedOperation          2
   /** The intended precedence is not supported by this controller.
    * @outputof SA_CanSetBootableVolume.
    * @disallows SA_DoSetBootableVolume.
    */
   #define kSetBootableVolumeUnsupportedPrecedence         3
   /** The intended drive type is not supported by this controller.
    * @outputof SA_CanSetBootableVolume.
    * @disallows SA_DoSetBootableVolume.
    */
   #define kSetBootableVolumeUnsupportedDriveType          4
   /** The intended logical drive is not valid.
    * @outputof SA_CanSetBootableVolume.
    * @disallows SA_DoSetBootableVolume.
    */
   #define kSetBootableVolumeInvalidLogicalDrive           5
   /** The intended physical drive is not valid.
    * @outputof SA_CanSetBootableVolume.
    * @disallows SA_DoSetBootableVolume.
    */
   #define kSetBootableVolumeInvalidPhysicalDrive          6
   /** The intended remote volume is not valid.
    * @outputof SA_CanSetBootableVolume.
    * @disallows SA_DoSetBootableVolume.
    */
   #define kSetBootableVolumeInvalidRemoteVolume           7
   /** The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanSetBootableVolume.
    * @disallows SA_DoSetBootableVolume.
    */
   #define kSetBootableVolumeOperationControllerOFAActive  8
/** @} */ /* Can Set Controller Bootable Volume */

/** @name Set Controller Bootable Volume
 * @{ */
/** Set the bootable volume with the requested precedence to the requested volume.
 * @pre The user has used @ref SA_CanSetBootableVolume to ensure the operation can be performed.
 * @param[in] cda           Controller data area.
 * @param[in] precedence    Precedent of the bootable drive (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @param[in] drive_type    Type to specify if target drive is a logical or physical drive (see [Bootable Volume Drive Type](@ref storc_properties_ctrl_boot_volume)).
 * @param[in] drive_number  Target drive number of LD or PD.
 * @return @ref kTrue if the drive was set bootable, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanSetBootableVolume(cda, kBootableVolumePrecedencePrimary, kBootableVolumeDriveTypeLogical, 1) == kSetBootableVolumeOperationAllowed)
 * {
 *    if (SA_DoSetBootableVolume(cda, kBootableVolumePrecedencePrimary, kBootableVolumeDriveTypeLogical, 1) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set primary bootable logical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_DoSetBootableVolume(PCDA cda, SA_BYTE precedence, SA_BYTE drive_type, SA_WORD drive_number);
/** @} */ /* Can Set Controller Bootable Volume */

/** @name Can Clear Controller Bootable Volume
 * @{ */
/** Checks if we're able to clear the primary/secondary bootable volume.
 * @post If valid, user may call @ref SA_DoClearBootableVolume.
 * @param[in] cda           Controller data area.
 * @param[in] precedence    Precedent of the bootable drive (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @return See [Controller Can Clear Bootable Volume Returns](@ref storc_configuration_ctrl_bootable_volumes).
 *
 * __Example__
 * @code
 * if (SA_CanClearBootableVolume(cda, kBootableVolumePrecedencePrimary) == kClearBootableVolumeOperationAllowed)
 * {
 *    if (SA_DoClearBootableVolume(cda, kBootableVolumePrecedencePrimary) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to clear primary bootable logica/physical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanClearBootableVolume(PCDA cda, SA_BYTE precedence);
   /** Clear bootable volume is allowed.
    * @outputof SA_CanClearBootableVolume.
    * @allows SA_DoClearBootableVolume.
    */
   #define kClearBootableVolumeOperationAllowed              0x01
   /** The operation is not supported by this controller.
    * @outputof SA_CanClearBootableVolume.
    * @disallows SA_DoClearBootableVolume.
    */
   #define kClearBootableVolumeUnsupportedOperation          0x02
   /** The intended precedence is not supported by this controller.
    * @outputof SA_CanClearBootableVolume.
    * @disallows SA_DoClearBootableVolume.
    */
   #define kClearBootableVolumeUnsupportedPrecedence         0x03
   /** The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanClearBootableVolume.
    * @disallows SA_DoClearBootableVolume.
    */
   #define kClearBootableVolumeOperationControllerOFAActive  0x04
/** @} */ /* Can Clear Controller Bootable Volume */

/** @name Clear Controller Bootable Volume
 * @{ */
/** Clear the bootable volume with the requested precedence.
 * @pre The user has used @ref SA_CanClearBootableVolume to ensure the operation can be performed.
 * @param[in] cda           Controller data area.
 * @param[in] precedence    Precedent of the bootable drive (see [Bootable Volume Precedence](@ref storc_properties_ctrl_boot_volume)).
 * @return @ref kTrue if bootable volume configuration was cleared, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanClearBootableVolume(cda, kBootableVolumePrecedencePrimary) == kClearBootableVolumeOperationAllowed)
 * {
 *    if (SA_DoClearBootableVolume(cda, kBootableVolumePrecedencePrimary) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to clear primary bootable logica/physical drive\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_DoClearBootableVolume(PCDA cda, SA_BYTE precedence);
/** @} */ /* Clear Controller Bootable Volume */

/** @} */ /* storc_configuration_ctrl_bootable_volumes */

/** @defgroup storc_configuration_ctrl_mode Controller Mode Configuration
 * @brief Configure the controller operating mode (RAID/HBA/Mixed/...) (or configure modes of all a controller's ports).
 * @details
 * @see storc_features_ctrl_mode
 * @see storc_properties_ctrl_mode
 * @{ */
/** @name Controller Modes
 * Inputs to @ref SA_CanChangeControllerMode & @ref SA_DoChangeControllerMode.
 * @{ */
   /** Parameter to specify the intended Controller mode as RAID.
    * Input to @ref SA_CanChangeControllerMode & @ref SA_DoChangeControllerMode.
    */
   #define kControllerModeRAID  0
   /** Parameter to specify the intended Controller mode as HBA.
    * Input to @ref SA_CanChangeControllerMode & @ref SA_DoChangeControllerMode.
    */
   #define kControllerModeHBA   1
   /** Parameter to specify the intended Controller mode as Mixed.
    * Input to @ref SA_CanChangeControllerMode & @ref SA_DoChangeControllerMode.
    */
   #define kControllerModeMixed 2
/** @} */

/** @name Can Change Controller Mode
 * @{ */
/** Can the Controller's mode be changed?
 *
 * @post If valid, user may call @ref SA_DoChangeControllerMode.
 * @param[in] cda                       Controller data area.
 * @param[in] intended_controller_mode  Desired Controller mode (see [Controller Modes](@ref storc_configuration_ctrl_mode)).
 * @return Bitmap showing if the Controller mode can be changed and reason(s) (see [Controller Can Change Mode Returns](@ref storc_configuration_ctrl_mode)).
 *
 * __Example__
 * @code
 * if (SA_CanChangeControllerMode(cda, kControllerModeMixed) == kChangeControllerModeOperationAllowed)
 * {
 *    if (SA_DoChangeControllerMode(cda, kControllerModeMixed) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set controller mode to Mixed\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeControllerMode(PCDA cda, SA_BYTE intended_controller_mode);
   /** Change controller mode is allowed.
    * @outputof SA_CanChangeControllerMode.
    * @allows SA_DoChangeControllerMode.
    */
   #define kChangeControllerModeOperationAllowed      0x01
   /** The operation is not supported by this controller.
    * @outputof SA_CanChangeControllerMode.
    */
   #define kChangeControllerModeUnsupportedOperation  0x02
   /** The intended mode is not supported by this controller.
    * @outputof SA_CanChangeControllerMode.
    * @disallows SA_DoChangeControllerMode.
    */
   #define kChangeControllerModeUnsupportedMode       0x04
   /** The intended mode is already pending.
    * @outputof SA_CanChangeControllerMode.
    * @disallows SA_DoChangeControllerMode.
    */
   #define kChangeControllerModeSelectedModePending   0x08
   /** Cannot change to this mode because logical drives exist.
    * @outputof SA_CanChangeControllerMode.
    * @disallows SA_DoChangeControllerMode.
    */
   #define kChangeControllerModeLogicalDrivesExist    0x10
   /** Cannot change to this mode because encryption is enabled.
    * @outputof SA_CanChangeControllerMode.
    * @disallows SA_DoChangeControllerMode.
    */
   #define kChangeControllerModeEncryptionIsEnabled   0x20
   /** Cannot change to this mode because of an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanChangeControllerMode.
    * @disallows SA_DoChangeControllerMode.
    */
   #define kChangeControllerModeControllerOFAActive   0x40
/** @} */ /* Can Change Controller Mode */

/** @name Change Controller Mode
 * @{ */
/** Change the controller mode to the requested value.
 *
 * @pre The user has used @ref SA_CanChangeControllerMode to ensure the operation can be performed.
 * @param[in] cda                       Controller data area.
 * @param[in] intended_controller_mode  Desired Controller mode (see [Controller Modes](@ref storc_configuration_ctrl_mode)).
 * @return @ref kTrue if Controller mode was changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeControllerMode(cda, kControllerModeMixed) == kChangeControllerModeOperationAllowed)
 * {
 *    if (SA_DoChangeControllerMode(cda, kControllerModeMixed) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set controller mode to Mixed\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_DoChangeControllerMode(PCDA cda, SA_BYTE intended_controller_mode);
/** @} */ /* Change Controller Mode */

/** @} */ /* storc_configuration_ctrl_mode */

/** @defgroup storc_configuration_ctrl_power_mode Controller Power Mode Configuration
 * @brief Configure controller power mode.
 * @details
 * @see storc_features_ctrl_power_mode
 * @see storc_configuration_edit_survivalmode
 * @see storc_properties_get_power_mode
 * @{ */
/** @name Power Modes
 * Input to @ref SA_CanChangePowerMode.
 * @{ */
   /** Power mode __Minimum Power__.
    * Input to @ref SA_CanChangePowerMode.
    */
   #define kSetPowerModeMinimumPower   1
   /** Power mode __Balanced__.
    * Input to @ref SA_CanChangePowerMode.
    */
   #define kSetPowerModeBalanced       2
   /** Power mode __Max Performance__.
    * Input to @ref SA_CanChangePowerMode.
    */
   #define kSetPowerModeMaxPerformance 4
/** @} */ /* Power Modes */

/** @name Can Change Power Mode
 * @{ */
/** Checks if power mode change be changed on the Controller.
 * @post If valid, user may call @ref SA_DoChangePowerMode.
 * @pre `connector_index` is a valid connector.
 * @param[in] cda                  Controller data area.
 * @param[in] intended_power_mode  Desired power mode (see [Power Modes](@ref storc_configuration_ctrl_power_mode)).
 * @return See [Can Change Power Mode Returns](@ref storc_configuration_ctrl_power_mode).
 *
 * __Example__
 * @code
 * if (SA_CanChangePowerMode(cda, kSetPowerModeMaxPerformance) == kChangePowerModeOperationAllowed)
 * {
 *    if (SA_DoChangePowerMode(cda, kSetPowerModeMaxPerformance) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set power mode to max performance\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangePowerMode(PCDA cda, SA_BYTE intended_power_mode);
   /** Change power mode is allowed.
    * @outputof SA_CanChangePowerMode.
    * @allows SA_DoChangePowerMode.
    */
   #define kChangePowerModeOperationAllowed      0x01
   /** The intended mode is not supported by this controller.
    * @outputof SA_CanChangePowerMode.
    * @disallows SA_DoChangePowerMode.
    */
   #define kChangePowerModeUnsupportedMode       0x02
   /** The intended mode is already pending.
    * @outputof SA_CanChangePowerMode.
    * @disallows SA_DoChangePowerMode.
    */
   #define kChangePowerModeSelectedModePending   0x03
   /** The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanChangePowerMode.
    * @disallows SA_DoChangePowerMode.
    */
   #define kChangePowerModeControllerOFAActive   0x04
/** @} */ /* Can Change Power Mode */

/** @name Change Power Mode
 * @{ */
/** Change the controller power mode to the requested value.
 * @pre The user has used @ref SA_CanChangePowerMode to ensure the operation can be performed.
 * @param[in] cda                  Controller data area.
 * @param[in] intended_power_mode  Desired power mode (see [Power Modes](@ref storc_configuration_ctrl_power_mode)).
 * @return @ref kTrue if power mode was successfully change, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangePowerMode(cda, kSetPowerModeMaxPerformance) == kChangePowerModeOperationAllowed)
 * {
 *    if (SA_DoChangePowerMode(cda, kSetPowerModeMaxPerformance) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to set power mode to max performance\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_DoChangePowerMode(PCDA cda, SA_BYTE intended_power_mode);
/** @} */ /* Change Power Mode */

/** @} */ /* storc_configuration_ctrl_power_mode */

/** @defgroup storc_configuration_ctrl_time Controller Time Configuration
 * @brief Set the controller's real time clock.
 * @brief storc_properties_ctrl_time
 * @{ */

/** Set the current controller time
 * @param[in] cda              Controller data area.
 * @param[in] controller_time  Current time in seconds since the UNIX epoch.
 * @return @ref kTrue if setting the controller time succeeded, @ref kFalse if failed.
 */
SA_BOOL SA_SetControllerTime(PCDA cda, SA_QWORD controller_time);
/** @} */ /* storc_configuration_ctrl_time */

/** @defgroup storc_configuration_oob Out Of Band Configuration
 * @details
 * Manage the controller's out of band configuration. This includes modifying the
 * transport and messaging configuration and the I2C address of the controller.
 *
 * For reading the out-of-band configuration, see @ref storc_properties_ctrl_oob_config.
 * @{ */

/** @defgroup storc_configuration_oob_config Transport/Messaging Configuration
 * @brief Set/change/disable/reset the controller's OOB transport and messaging configuration.
 * @details
 * # Examples
 * See [OOB configuration structure](@ref storc_properties_ctrl_oob_config) for details about
 * the OOBConfiguration input argument.
 *
 * __Example__
 * Disable OOB entirely
 * @code
 * OOBConfiguration oob_config;
 * oob_config.interface_type = kOOBInterfaceTypeDisabled;
 *
 * if (SA_CanChangeControllerOOBConfiguration(cda, &oob_config) == kCanChangeControllerOOBConfigurationAllowed)
 * {
 *    if (!(SA_DoChangeControllerOOBConfiguration(cda, &oob_config))
 *    {
 *        fprintf(stderr, "Failed to disable controller's OOB configuration.");
 *    }
 * }
 * @endcode
 *
 * __Example__
 * Set/change MCTP
 * To perform a read-modify-write on the configuration, use @ref SA_GetControllerPendingOOBConfiguration
 * to populate an [OOB configuration structure](@ref storc_properties_ctrl_oob_config).
 *
 * @note The fields in the OOB configuration struction differ when setting the interface type to MCTP and
 * PBSI. Make sure the correct fields are populated for the desired interface type.
 * @code
 * OOBConfiguration oob_config;
 * oob_config.interface_type = kOOBInterfaceTypeMCTP;
 * oob_config.i2c_configuration.mctp.smbus_device_type = kMCTPSMBUSDeviceTypeDefault;
 * oob_config.i2c_configuration.mctp.enable_smbus_physical_channel = 1;
 * oob_config.i2c_configuration.mctp.use_static_eids_on_init = 0;
 * oob_config.i2c_configuration.mctp.vdm_discovery_notify = 1;
 * oob_config.i2c_configuration.slave_address = 0xD0;
 *
 * if (SA_CanChangeControllerOOBConfiguration(cda, &oob_config) == kCanChangeControllerOOBConfigurationAllowed)
 * {
 *    if (!(SA_DoChangeControllerOOBConfiguration(cda, &oob_config))
 *    {
 *        fprintf(stderr, "Failed to set controller's OOB MCTP configuration.");
 *    }
 * }
 * @endcode
 *
 * __Example__
 * Set/change PBSI
 * To perform a read-modify-write on the configuration, use @ref SA_GetControllerPendingOOBConfiguration
 * to populate an [OOB configuration structure](@ref storc_properties_ctrl_oob_config).
 *
 * @note The fields in the OOB configuration struction differ when setting the interface type to MCTP and
 * PBSI. Make sure the correct fields are populated for the desired interface type.
 * @code
 * OOBConfiguration oob_config;
 * oob_config.interface_type = kOOBInterfaceTypePBSI;
 * oob_config.i2c_configuration.pbsi.clock_stretching = 1;
 * oob_config.i2c_configuration.pbsi.clock_speed = kPBSIClockSpeed100KHZ;
 * oob_config.i2c_configuration.slave_address = 0xD0;
 *
 * if (SA_CanChangeControllerOOBConfiguration(cda, &oob_config) == kCanChangeControllerOOBConfigurationAllowed)
 * {
 *    if (!(SA_DoChangeControllerOOBConfiguration(cda, &oob_config))
 *    {
 *        fprintf(stderr, "Failed to set controller's OOB PBSI configuration.");
 *    }
 * }
 * @endcode
 *
 * __Example__
 * Reset OOB configuration to factory defaults
 * @code
 * OOBConfiguration oob_config;
 * oob_config.interface_type = kOOBInterfaceTypeReset;
 *
 * if (SA_CanChangeControllerOOBConfiguration(cda, &oob_config) == kCanChangeControllerOOBConfigurationAllowed)
 * {
 *    if (!(SA_DoChangeControllerOOBConfiguration(cda, &oob_config))
 *    {
 *        fprintf(stderr, "Failed to reset controller's OOB configuration.");
 *    }
 * }
 * @endcode
 *
 * @{ */

/** @name Can Change Controller OOB Configuration
 * @{ */
/** Can the controller's OOB configuration be changed?
 * @post If valid, user may call @ref SA_DoChangeControllerOOBConfiguration.
 * @param[in] cda         Controller data area.
 * @param[in] oob_config  [OOB configuration structure](@ref storc_properties_ctrl_oob_config) with desired input.
 * @return See [Can Change Controller OOB Configuration Returns](@ref storc_configuration_oob_config).
 */
SA_UINT32 SA_CanChangeControllerOOBConfiguration(PCDA cda, OOBConfiguration* oob_config);
/** Change of controller's OOB configuration allowed.
 * @outputof SA_CanChangeControllerOOBConfiguration
 * @allows SA_DoChangeControllerOOBConfiguration
 */
#define kCanChangeControllerOOBConfigurationAllowed                  1
/** Change of controller's OOB configuration not allowed because the input is invalid.
 * @outputof SA_CanChangeControllerOOBConfiguration
 * @disallows SA_DoChangeControllerOOBConfiguration
 */
#define kCanChangeControllerOOBConfigurationInvalidInput             2
/** Change of controller's OOB configuration not allowed because it is not supported.
 * @outputof SA_CanChangeControllerOOBConfiguration
 * @disallows SA_DoChangeControllerOOBConfiguration
 */
#define kCanChangeControllerOOBConfigurationNotSupported             3
/** Change of controller's OOB configuration not allowed because the [interface type](@ref OOBConfiguration.interface_type) is invalid.
 * @outputof SA_CanChangeControllerOOBConfiguration
 * @disallows SA_DoChangeControllerOOBConfiguration
 */
#define kCanChangeControllerOOBConfigurationInvalidInterfaceType     4
/** Change of controller's OOB configuration not allowed because the [clock speed](@ref storc_properties_ctrl_oob_config) is invalid.
 * @outputof SA_CanChangeControllerOOBConfiguration
 * @disallows SA_DoChangeControllerOOBConfiguration
 */
#define kCanChangeControllerOOBConfigurationInvalidClockSpeed        5
/** Change of controller's OOB configuration not allowed because the [SMBUS device type](@ref storc_properties_ctrl_oob_config) is invalid.
 * @outputof SA_CanChangeControllerOOBConfiguration
 * @disallows SA_DoChangeControllerOOBConfiguration
 */
#define kCanChangeControllerOOBConfigurationInvalidSMBUSDeviceType   6
/** Change of controller's OOB configuration not allowed because the controller has an active or pending online firmware activation operation.
 * @outputof SA_CanChangeControllerOOBConfiguration
 * @disallows SA_DoChangeControllerOOBConfiguration
 */
#define kCanChangeControllerOOBConfigurationOFAActive                7
/** Change of controller's OOB configuration not allowed because the controller status is not ok.
 * @outputof SA_CanChangeControllerOOBConfiguration
 * @disallows SA_DoChangeControllerOOBConfiguration
 */
#define kCanChangeControllerOOBConfigurationControllerStatusNotOk    8
/** @} */ /* Can Change Controller OOB Configuration */

/** @name Do Change Controller OOB Configuration
 * @{ */
/** Do Change the controller's OOB configuration
 * @pre User has called @ref SA_CanChangeControllerOOBConfiguration.
 * @param[in] cda         Controller data area.
 * @param[in] oob_config  [OOB configuration structure](@ref storc_properties_ctrl_oob_config) with desired input.
 * @return @ref kTrue if successful, @ref kFalse otherwise.
 */
SA_BOOL SA_DoChangeControllerOOBConfiguration(PCDA cda, OOBConfiguration* oob_config);
/** @} */ /* Do Change Controller OOB Configuration */

/** @} */ /* storc_configuration_oob_config */

/** @defgroup storc_configuration_oob_i2c I2C Configuration
 * @brief Change the controller's OOB I2C configuration.
 * @details
 * Change the controller's OOB I2C configuration.
 *
 * __Example__
 * @code
 * if (SA_CanSetControllerI2CAddress(cda, 0) == kCanSetControllerI2CAddressAllowed)
 * {
 *    if (!SA_SetControllerI2CAddress(cda, 0))
 *    {
 *        fprintf(stderr, "Failed to set controller's I2C slave address.");
 *    }
 * }
 * @endcode
 * @{ */

/** @name Can Set I2C Address
 * @{ */

/** Can the controller's I2C slave address be set?
 * @post If valid, user may call @ref SA_SetControllerI2CAddress.
 * @param[in] cda                Controller data area.
 * @param[in] i2c_slave_address  Desired I2C slave address.
 * @return See [Can Set Controller I2C Configuration Returns](@ref storc_configuration_oob_i2c).
 */
SA_BYTE SA_CanSetControllerI2CAddress(PCDA cda, SA_BYTE i2c_slave_address);
/** Set controller's I2C configuration Allowed.
 * @outputof SA_CanSetControllerI2CAddress.
 * @allows SA_SetControllerI2CAddress.
 */
#define kCanSetControllerI2CAddressAllowed              0x01
/** Set controller's I2C configuration not allowed because it is already set to the desired value.
 * @outputof SA_CanSetControllerI2CAddress.
 * @disallows SA_SetControllerI2CAddress.
 */
#define kCanSetControllerI2CAddressAlreadySet           0x02
/** Set controller's I2C configuration not allowed because the I2C interface is not enabled on the controller.
 * @outputof SA_CanSetControllerI2CAddress.
 * @disallows SA_SetControllerI2CAddress.
 */
#define kCanSetControllerI2CAddressInactive             0x03
/** Set controller's I2C configuration not allowed because it is not supported by the controller.
 * @outputof SA_CanSetControllerI2CAddress.
 * @disallows SA_SetControllerI2CAddress.
 */
#define kCanSetControllerI2CAddressUnsupported          0x04
/** Set controller's I2C configuration not allowed because of an active or pending online firmware activation operation.
 * @see storc_status_ctrl_online_firmware_activation
 * @outputof SA_CanSetControllerI2CAddress.
 * @disallows SA_SetControllerI2CAddress.
 */
#define kCanSetControllerI2CAddressControllerOFAActive  0x05

/** @} */ /* Can Set I2C Address */

/** @name Set I2C Address
 * @{ */

/** Set the controller's I2C slave address.
 * @pre The user has used @ref SA_CanSetControllerI2CAddress to ensure the operation can be performed.
 * @param[in] cda                Controller data area.
 * @param[in] i2c_slave_address  Desired I2C slave address.
 * @return @ref kTrue if successful, @ref kFalse otherwise.
 */
SA_BOOL SA_SetControllerI2CAddress(PCDA cda, SA_BYTE i2c_slave_address);

/** @} */ /* Set I2C Address */

/** @} */ /* storc_configuration_oob_i2c */

/** @} */ /* storc_configuration_oob */

/** @defgroup storc_configuration_ctrl_expander_minimum_scan_duration Controller Expander Minimum Scan Duration
 * @brief Configure the minimum scan duration for expanders.
 * @details
 * @see storc_features_ctrl_expander_minimum_scan_duration
 * @see storc_properties_ctrl_expander_scan_duration
 *
 * __Example__
 * @code
 * if (SA_CanSetControllerExpanderMinimumScanDuration(cda, 150) == kCanSetControllerExpanderMinimumScanDurationAllowed)
 * {
 *    if (SA_SetControllerExpanderMinimumScanDuration(cda, 150))
 *    {
 *       printf("Expander scan duration set successfully.");
 *    }
 *    else
 *    {
 *       printf("Failed to set expander scan duration.")
 *    }
 * }
 * else if (SA_CanSetControllerExpanderMinimumScanDuration(cda, 150) == kCanSetControllerExpanderMinimumScanDurationInvalidValue)
 * {
 *    printf("Invalid value - Maximum allowable scan duration is %d.", SA_GetControllerExpanderMaximumAllowableScanDuration(cda));
 * }
 * @endcode
 * @{ */

/** @name Can Set Controller Expander Minimum Scan Duration
 * @{ */
/** Can the minimum expander scan duration be set?
 *
 * @post If valid, user may call @ref SA_SetControllerExpanderMinimumScanDuration.
 * @param[in] cda             Controller data area.
 * @param[in] scan_duration   Desired scan duration in seconds.
 * @return See [Can Set Expander Minimum Scan Duration returns](@ref storc_configuration_ctrl_expander_minimum_scan_duration).
 */
SA_BYTE SA_CanSetControllerExpanderMinimumScanDuration(PCDA cda, SA_WORD scan_duration);
   /** Setting the minimum expander scan duration is allowed.
    * @outputof SA_CanSetControllerExpanderMinimumScanDuration.
    * @allows SA_SetControllerExpanderMinimumScanDuration.
    */
   #define kCanSetControllerExpanderMinimumScanDurationAllowed       0
   /** Setting the minimum expander scan duration is not supported.
    * @outputof SA_CanSetControllerExpanderMinimumScanDuration.
    * @disallows SA_SetControllerExpanderMinimumScanDuration.
    */
   #define kCanSetControllerExpanderMinimumScanDurationNotSupported  1
   /** Cannot set the minimum expander scan duration because it is already set to the desired value.
    * @outputof SA_CanSetControllerExpanderMinimumScanDuration.
    * @disallows SA_SetControllerExpanderMinimumScanDuration.
    */
   #define kCanSetControllerExpanderMinimumScanDurationAlreadySet    2
   /** Cannot set the minimum expander scan duration because the desired value is invalid or out of range.
    * @outputof SA_CanSetControllerExpanderMinimumScanDuration.
    * Use @ref SA_GetControllerExpanderMaximumAllowableScanDuration to determine the maximum allowable value.
    * @disallows SA_SetControllerExpanderMinimumScanDuration.
    */
   #define kCanSetControllerExpanderMinimumScanDurationInvalidValue  3
   /** Cannot set the minimum expander scan duration because of an unknown error.
    * @outputof SA_CanSetControllerExpanderMinimumScanDuration.
    * @disallows SA_SetControllerExpanderMinimumScanDuration.
    */
   #define kCanSetControllerExpanderMinimumScanDurationUnknown       4
/** @} */ /* Can Set Controller Expander Minimum Scan Duration */

/** @name Set Controller Expander Minimum Scan Duration
 * @{ */
/** Set the minimum expander scan duration.
 *
 * @pre The user has called @ref SA_CanSetControllerExpanderMinimumScanDuration to ensure the operation can be performed.
 * @param[in] cda             Controller data area.
 * @param[in] scan_duration   Desired scan duration in seconds.
 * @return @ref kTrue if the minimum expander scan duration was set successfully, @ref kFalse otherwise.
 */
SA_BOOL SA_SetControllerExpanderMinimumScanDuration(PCDA cda, SA_WORD scan_duration);
/** @} */ /* Set Controller Expander Minimum Scan Duration */

/** @} */ /* storc_configuration_ctrl_expander_minimum_scan_duration */

/** @defgroup storc_configuration_ctrl_pcie_ordering Controller PCIe Ordering
 * @brief Change the controller's PCIe ordering mode.
 * @details
 * Change the controller's PCIe ordering mode. This requires a reboot to take effect.
 *
 * __Example__
 * @code
 * if (SA_CanChangeControllerPCIeOrdering(cda, kChangeControllerPCIeOrderingStrict))
 * {
 *    if (SA_DoChangeControllerPCIeOrdering(cda, kChangeControllerPCIeOrderingStrict))
 *    {
 *       printf("Successfully changed the controller's PCIe ordering.\n");
 *    }
 *    else
 *    {
 *       printf("Failed to change the controller's PCIe ordering.\n");
 *    }
 * }
 * @endcode
 * @{ */

/** @name PCIe Ordering Modes
 * Inputs to @ref SA_CanChangeControllerPCIeOrdering and @ref SA_DoChangeControllerPCIeOrdering.
 * @{ */
/** PCIe Ordering Strict
 */
#define kChangeControllerPCIeOrderingStrict  0
/** PCIe Ordering Relaxed
 */
#define kChangeControllerPCIeOrderingRelaxed 1
/** @} */ /* PCIe Ordering Modes */

/** @name Can Change PCIe Ordering
 * @{ */
/** Can the controller's PCIe ordering be changed?
 * @post If @ref kCanChangeControllerPCIeOrderingAllowed is returned, user may call @ref SA_DoChangeControllerPCIeOrdering.
 * @param[in] cda            Controller data area.
 * @param[in] pcie_ordering  Desired PCIe ordering mode. See [PCIe Ordering Modes](@ref storc_configuration_ctrl_pcie_ordering).
 * @return See [Can Change Controller PCIe Ordering Returns](@ref storc_configuration_ctrl_pcie_ordering).
 */
SA_BYTE SA_CanChangeControllerPCIeOrdering(PCDA cda, SA_BYTE pcie_ordering);
/** Changing the controller's PCIe ordering mode is allowed.
 * @outputof SA_CanChangeControllerPCIeOrdering.
 * @allows SA_DoChangeControllerPCIeOrdering.
 */
#define kCanChangeControllerPCIeOrderingAllowed      0x01
/** Changing the controller's PCIe ordering mode is not supported.
 * @outputof SA_CanChangeControllerPCIeOrdering.
 * @disallows SA_DoChangeControllerPCIeOrdering.
 */
#define kCanChangeControllerPCIeOrderingNotSupported 0x02
/** The desired PCIe ordering mode is already set.
 * @outputof SA_CanChangeControllerPCIeOrdering.
 * @disallows SA_DoChangeControllerPCIeOrdering.
 */
#define kCanChangeControllerPCIeOrderingAlreadySet   0x03
/** The desired PCIe ordering mode is invalid.
 * @outputof SA_CanChangeControllerPCIeOrdering.
 * @disallows SA_DoChangeControllerPCIeOrdering.
 */
#define kCanChangeControllerPCIeOrderingInvalid      0x04
/** @} */ /* Can Change PCIe Ordering */

/** @name Change PCIe Ordering
 * @{ */
/** Change the controller's PCIe ordering.
 * @pre User has called @ref SA_CanChangeControllerPCIeOrdering.
 * @param[in] cda            Controller data area.
 * @param[in] pcie_ordering  Desired PCIe ordering mode. See [PCIe Ordering Modes](@ref storc_configuration_ctrl_pcie_ordering).
 * @return @ref kTrue if successful, @ref kFalse otherwise.
 */
SA_BOOL SA_DoChangeControllerPCIeOrdering(PCDA cda, SA_BYTE pcie_ordering);
/** @} */ /* Change PCIe Ordering */

/** @} */ /* storc_configuration_ctrl_pcie_ordering */

/** @} */ /* storc_configuration_ctrl */

/************************************************//**
* @defgroup storc_configuration_port Port/Connector
* @{
***************************************************/

/** @defgroup storc_configuration_port_mode Connector Mode Configuration
 * @brief Configure the operating mode (RAID/HBA/Mixed/...) of the controller's ports/connectors.
 * @details
 * @see storc_properties_ctrl_port_mode
 * @{ */
/** @name Connector Modes
 * Input to @ref SA_CanChangeConnectorMode.
 * @{ */
   /** Connector mode __RAID__.
    * Input to @ref SA_CanChangeConnectorMode.
    */
   #define kConnectorModeRAID  0
   /** Connector mode __HBA__.
    * Input to @ref SA_CanChangeConnectorMode.
    */
   #define kConnectorModeHBA   1
   /** Connector mode __Mixed__.
    * Input to @ref SA_CanChangeConnectorMode.
    */
   #define kConnectorModeMixed 2
/** @} */

/** @name Can Change Connector Mode
 * @deprecated Use [Can Set Connector Mode](@ref storc_configuration_port_mode).
 * @{ */
/** Checks if the connector mode can be changed on the Controller's port.
 * @post If valid, user may call @ref SA_DoChangeConnectorMode.
 * @pre `connector_index` is a valid connector.
 * @param[in] cda              Controller data area.
 * @param[in] connector_index  Target connector index.
 * @param[in] intended_connector_mode  Desired connector mode (see [Connector Modes](@ref storc_configuration_port_mode)).
 * @return See [Can Change Connector Mode Returns](@ref storc_configuration_port_mode).
 *
 * __Example__
 * @code
 * if (SA_CanChangeConnectorMode(cda, 1, kConnectorModeMixed) == kChangeConnectorModeOperationAllowed)
 * {
 *    if (SA_DoChangeConnectorMode(cda, 1, kConnectorModeMixed) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to set connector mode to Mixed\n");
 *    }
 * }
 * @endcode
 * @deprecated Use @ref SA_CanSetConnectorMode.
 */
SA_BYTE SA_CanChangeConnectorMode(PCDA cda, SA_WORD connector_index, SA_BYTE intended_connector_mode);
   /** Change connector mode is allowed.
    * @outputof SA_CanChangeConnectorMode.
    * @allows SA_DoChangeConnectorMode.
    */
   #define kChangeConnectorModeOperationAllowed      0x01
   /** The operation is not supported in this configuration.
    * @outputof SA_CanChangeConnectorMode.
    * @disallows SA_DoChangeConnectorMode.
    */
   #define kChangeConnectorModeUnsupportedOperation  0x02
   /** The intended mode is not supported by this controller.
    * @outputof SA_CanChangeConnectorMode.
    * @disallows SA_DoChangeConnectorMode.
    */
   #define kChangeConnectorModeUnsupportedMode       0x03
   /** The intended mode is already pending.
    * @outputof SA_CanChangeConnectorMode.
    * @disallows SA_DoChangeConnectorMode.
    */
   #define kChangeConnectorModeSelectedModePending   0x04
   /** Cannot change to this mode because logical drives exist.
    * @outputof SA_CanChangeConnectorMode.
    * @disallows SA_DoChangeConnectorMode.
    */
   #define kChangeConnectorModeLogicalDrivesExist    0x05
   /** Cannot change to this mode because encryption is enabled.
    * @outputof SA_CanChangeConnectorMode.
    * @disallows SA_DoChangeConnectorMode.
    */
   #define kChangeConnectorModeEncryptionIsEnabled   0x06
/** @} */ /* Can Change Connector Mode */

/** @name Change Connector Mode
 * @deprecated Use [Set Connector Mode](@ref storc_configuration_port_mode).
 * @{ */
/** Change the controller mode to the requested value.
 * @pre The user has used @ref SA_CanChangeConnectorMode to ensure the operation can be performed.
 * @param[in] cda              Controller data area.
 * @param[in] connector_index  Target connector index.
 * @param[in] intended_connector_mode  Desired connector mode (see [Connector Modes](@ref storc_configuration_port_mode)).
 * @return @ref kTrue if connector mode was successfully changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeConnectorMode(cda, 1, kConnectorModeMixed) == kChangeConnectorModeOperationAllowed)
 * {
 *    if (SA_DoChangeConnectorMode(cda, 1, kConnectorModeMixed) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to set connector mode to Mixed\n");
 *    }
 * }
 * @endcode
 * @deprecated Use @ref SA_DoSetConnectorMode.
 */
SA_BOOL SA_DoChangeConnectorMode(PCDA cda, SA_WORD connector_index, SA_BYTE intended_connector_mode);
/** @} */ /* Change Connector Mode */

/** @name Can Set Connector Mode
 * @{ */
/** Check if multiple ports can be changed to a single mode.
 * @post If valid, user may call @ref SA_DoSetConnectorMode.
 * @pre `connector_indexes` are valid connector indexes.
 * @param[in] cda                Controller data area.
 * @param[in] port_mode          Desired port mode (see [Connector Modes](@ref storc_configuration_port_mode)).
 * @param[in] connector_indexes  Target port numbers.
 * @param[in] n                  Number of port numbers.
 * @return See [Can Set Connector Mode Returns](@ref storc_configuration_port_mode).
 *
 * __Example__
 * @code
 * SA_WORD connector_indexes = 5;
 * if (SA_CanSetConnectorMode(cda, kConnectorModeMixed, &connector_indexes, 1) == kSetConnectorModeOperationAllowed)
 * {
 *    if (SA_DoSetConnectorMode(cda, kConnectorModeMixed, &connector_indexes, 1) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to set connector mode to Mixed\n");
 *    }
 * }
 * @endcode
*/
SA_BYTE SA_CanSetConnectorMode(PCDA cda, SA_BYTE port_mode, const SA_WORD *connector_indexes, size_t n);
   /** Set connector mode is allowed.
    * @outputof SA_CanSetConnectorMode.
    * @allows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeOperationAllowed      1
   /** The operation is not supported in this configuration.
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeUnsupportedOperation  2
   /** The intended mode is not supported by this controller.
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeUnsupportedMode       3
   /** The intended mode is already pending.
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeSelectedModePending   4
   /** Cannot set this mode because logical drives exist.
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeLogicalDrivesExist    5
   /** Cannot set this mode because encryption is enabled.
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeEncryptionIsEnabled   6
   /** Cannot set this mode port list is invalid.
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeInvalidPortList       7
   /** Cannot set this mode because list has duplicate port.
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeDuplicatePortInList   8
   /** Cannot set invalid port in list.
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeInvalidPort           9
   /** Cannot set this mode because of multi-domain path conflict.
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModePathConflict          10
   /** Cannot set this mode because of a missing/failed physical drive.
    * The failed drive must be removed/replaced before changing connector modes.
    * @see SA_GetPhysicalDriveStatusInfo.
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeFailedPhysicalDrive   11
   /** Cannot set this mode because of an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanSetConnectorMode.
    * @disallows SA_DoSetConnectorMode.
    */
   #define kCanSetConnectorModeControllerOFAActive   12
/** @} */ /* Can Set Connector Mode */

/** @name Set Connector Mode
 * @{ */
/** Set multiple ports to a single mode.
 * @pre The user has used @ref SA_CanSetConnectorMode to ensure the operation can be performed.
 * @param[in] cda                Controller data area.
 * @param[in] port_mode          Desired port mode (see [Connector Modes](@ref storc_configuration_port_mode)).
 * @param[in] connector_indexes  Target port numbers.
 * @param[in] n                  Number of port numbers.
 * @return @ref kTrue if all connector modes are successfully changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * SA_WORD connector_indexes = 5;
 * if (SA_CanSetConnectorMode(cda, kConnectorModeMixed, &connector_indexes, 1) == kSetConnectorModeOperationAllowed)
 * {
 *    if (SA_DoSetConnectorMode(cda, kConnectorModeMixed, &connector_indexes, 1) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to set connector mode to Mixed\n");
 *    }
 * }
 * @endcode
*/
SA_BOOL SA_DoSetConnectorMode(PCDA cda, SA_BYTE port_mode, const SA_WORD *connector_indexes, size_t n);
/** @} */ /* Change Connector Mode */

/** @} */ /* storc_configuration_port_mode */

/** @defgroup storc_configuration_port_phyrate Connector Max Allowed Phy Rate
 * @brief Configure each ports max allowed phy rate.
 * @{ */
/** Used only as input to @ref SA_CanSetConnectorMaxPhyRate / @ref SA_SetConnectorMaxPhyRate.
 * Disables setting a max phy rate, allowing the controller to use a default/hardware max.
 */
#define kMaxPhyRateDisable 0x00

/** @name Can Set Connector Max Phy Rate
 * @{ */
/** Checks if the max allowed phy rate can be changed on the Controller's port/connector.
 * @post If valid, user may call @ref SA_SetConnectorMaxPhyRate.
 * @pre `connector_index` is a valid connector.
 * @param[in] cda              Controller data area.
 * @param[in] connector_index  Target connector index.
 * @param[in] max_phy_rate     Desired connector max phy rate (see [Connector Phy Rates](@ref storc_properties_ctrl_port_mode)) or @ref kMaxPhyRateDisable.
 * @return [See return codes](@ref storc_configuration_port_phyrate).
 *
 * __Example__
 * @code
 * if (SA_CanSetConnectorMaxPhyRate(cda, 1, kPhyRate12G) == kCanSetConnectorMaxPhyRateAllowed)
 * {
 *    if (SA_SetConnectorMaxPhyRate(cda, 1, kPhyRate12G) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to set connector max phy rate to 12 Gb/s\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetConnectorMaxPhyRate(PCDA cda, SA_WORD connector_index, SA_BYTE max_phy_rate);
   /** Set controller phy rate.
    * @outputof SA_CanSetConnectorMaxPhyRate.
    * @allows SA_SetConnectorMaxPhyRate.
    */
   #define kCanSetConnectorMaxPhyRateAllowed                   0x01
   /** The operation is not supported.
    * @outputof SA_CanSetConnectorMaxPhyRate.
    * @disallows SA_SetConnectorMaxPhyRate.
    */
   #define kCanSetConnectorMaxPhyRateUnsupportedOperation      0x02
   /** The specified phy rate is not valid.
    * @outputof SA_CanSetConnectorMaxPhyRate.
    * @disallows SA_SetConnectorMaxPhyRate.
    */
   #define kCanSetConnectorMaxPhyRateInvalid                   0x03
   /** The specified phy rate is not supported.
    * @outputof SA_CanSetConnectorMaxPhyRate.
    * @disallows SA_SetConnectorMaxPhyRate.
    */
   #define kCanSetConnectorMaxPhyRateUnsupported               0x04
   /** The specified phy rate is already set to the max value.
    * @outputof SA_CanSetConnectorMaxPhyRate.
    * @disallows SA_SetConnectorMaxPhyRate.
i   */
   #define kCanSetConnectorMaxPhyRateAlreadySet                0x05
   /** The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanSetConnectorMaxPhyRate.
    * @disallows SA_SetConnectorMaxPhyRate.
i   */
   #define kCanSetConnectorMaxPhyRateControllerOFAActive       0x06
/** @} */ /* Can Set Connector Max Phy Rate */

/** @name Set Connector Max Phy Rate
 * @{ */
/** Set the max allowed phy rate on the Controller's port/connector.
 * @pre `connector_index` is a valid connector.
 * @pre The user has used @ref SA_CanSetConnectorMaxPhyRate to ensure the operation can be performed.
 * @param[in] cda              Controller data area.
 * @param[in] connector_index  Target connector index.
 * @param[in] max_phy_rate     Desired connector max phy rate (see [Connector Phy Rates](@ref storc_properties_ctrl_port_mode)) or @ref kMaxPhyRateDisable.
 * @return @ref kTrue if connecotr max phy was set successfully, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanSetConnectorMaxPhyRate(cda, 1, kPhyRate12G) == kCanSetConnectorMaxPhyRateAllowed)
 * {
 *    if (SA_SetConnectorMaxPhyRate(cda, 1, kPhyRate12G) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to set connector max phy rate to 12 Gb/s\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_SetConnectorMaxPhyRate(PCDA cda, SA_WORD connector_index, SA_BYTE max_phy_rate);
/** @} */ /* Set Connector Max Phy Rate */

/** @} */ /* storc_configuration_port_phyrate */

/** @} */ /* storc_configuration_port */

/*********************************************//**
* @defgroup storc_configuration_pd Physical Drive
* @{
************************************************/

/** @defgroup storc_configuration_pd_clear_ccm Clear Physical Drive CCM
 * @brief Clear invalid configuration meta data from a physical drive.
 * @{ */

/** @name Can Clear Physical Drive CCM
 * @{ */
/** Checks if Controller Configuration Metadata can be cleared from the physical drive.
 * @post If valid, user may call @ref SA_ClearPhysicalDriveCCM.
 * @pre `pd_number` is a valid physical drive.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Target physical drive.
 * @return See [Can Clear Physical Drive CCM Returns](@ref storc_configuration_pd_clear_ccm).
 *
 * __Example__
 * @code
 * if (SA_CanClearPhysicalDriveCCM(cda, 1) == kCanClearPhysicalDriveCCMAllowed)
 * {
 *    if (SA_ClearPhysicalDriveCCM(cda, 1) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to clear physical drive CCM\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanClearPhysicalDriveCCM(PCDA cda, SA_WORD pd_number);
   /** Clear physical drive CCM is allowed.
    * @outputof SA_CanClearPhysicalDriveCCM.
    * @allows SA_ClearPhysicalDriveCCM.
    */
   #define kCanClearPhysicalDriveCCMAllowed              0x01
    /** The physical drive is invalid.
    * @outputof SA_CanClearPhysicalDriveCCM.
    * @disallows SA_ClearPhysicalDriveCCM.
    */
   #define kCanClearPhysicalDriveCCMInvalidTarget        0x02
   /** The operation is not supported by the controller.
    * @outputof SA_CanClearPhysicalDriveCCM.
    * @disallows SA_ClearPhysicalDriveCCM.
    */
   #define kCanClearPhysicalDriveCCMNotSupported         0x03
   /** Controller is locked because the encryption boot password is missing.
    * @outputof SA_CanClearPhysicalDriveCCM.
    * @disallows SA_ClearPhysicalDriveCCM.
    */
   #define kCanClearPhysicalDriveCCMControllerLocked     0x04
   /** The physical drive does not have CCM.
    * @outputof SA_CanClearPhysicalDriveCCM.
    * @disallows SA_ClearPhysicalDriveCCM.
    */
   #define kCanClearPhysicalDriveCCMNoCCM                0x05
   /** The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanClearPhysicalDriveCCM.
    * @disallows SA_ClearPhysicalDriveCCM.
    */
   #define kCanClearPhysicalDriveCCMControllerOFAActive  0x06
/** @} */ /* Can Clear Physical Drive CCM */

/** @name Clear Physical Drive CCM
 * @{ */
/** Clear Controller Configuration Metadata from the physical drive.
 * @pre The user has used @ref SA_CanClearPhysicalDriveCCM to ensure the operation can be performed.
 * @param[in] cda        Controller data area.
 * @param[in] pd_number  Target physical drive.
 * @return @ref kTrue if CCM data was successfully cleared, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanClearPhysicalDriveCCM(cda, 1) == kCanClearPhysicalDriveCCMAllowed)
 * {
 *    if (SA_ClearPhysicalDriveCCM(cda, 1) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to clear physical drive CCM\n");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_ClearPhysicalDriveCCM(PCDA cda, SA_WORD pd_number);
/** @} */ /* Clear Physical Drive CCM */

/** @} */ /* storc_configuration_pd_clear_ris */

/** @} */ /* storc_configuration_pd */

/*********************************************//**
* @defgroup storc_configuration_array Array
* @{
************************************************/
/** @defgroup storc_configuration_array_add_spare_drive Add Spare Drive
 * @brief Add spare drives to an Array.
 * @{ */

/** @name Can Add Spare Drive To Array
 * @{ */
/** Can the selected physical drive be added to the array as a spare drive?
 * @post If valid, user may call @ref SA_AddArraySpareDrive.
 * @param[in] cda                    Controller data area.
 * @param[in] array_number           Target Array number.
 * @param[in] requested_spare_type   Spare type (see [Array Spare Types](@ref storc_properties_array_spare_type)).
 * @param[in] pd_number              Target spare drive number.
 * @return See [Can Add Spare Drive To Array Returns](@ref storc_configuration_array_add_spare_drive).
 *
 * __Example__
 * @code
 * if (SA_CanAddArraySpareDrive(cda, array_number, kArrayAddSpareModeAutoReplace, pd_number) == kCanAddArraySpareDriveAllowed)
 * {
 *     if (SA_AddArraySpareDrive(cda, array_number, kArrayAddSpareModeAutoReplace, pd_number) != kAddSpareDriveOK)
 *     {
 *         fprintf(stderr, "Failed to add auto replace spare drive to virtual Array\n");
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanAddArraySpareDrive(PCDA cda, SA_WORD array_number, SA_BYTE requested_spare_type, SA_WORD pd_number);
   /** The spare drive can be added to the Array.
    * @outputof SA_CanAddArraySpareDrive.
    * @allows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveAllowed                       1
   /** The spare drive cannot be added to the Array because the target array does not exist.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveArrayDoesNotExist             2
   /** The spare drive cannot be added to the Array because the target drive is not unassigned.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveDriveIsNotUnassigned          3
   /** The spare drive cannot be added to the Array because the spare drive type does not match the Array's data drives.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveCannotMixDriveTypes           4
   /** The spare drive cannot be added to the Array because of an incompatibility of the Array drive's blocksizes.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveIncompatibleBlockSize         5
   /** The spare drive cannot be added to the Array because the spare drive is too small.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveTooSmall                      6
   /** The spare drive cannot be added to the Array because the max number of physical drives has been reached.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveMaxPhysicalDriveCountReached  7
   /** The spare drive cannot be added to the Array because the max number of spare drives has been reached.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveMaxSpareDriveCountReached     8
   /** The spare drive cannot be added to the Array because the spare drive type has not been set.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveNoSpareTypeSet                9
   /** The spare drive cannot be added to the Array because the spare drive type is invalid.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveInvalidSpareType              10
   /** The spare drive cannot be added to the Array because the Array contains only RAID 0 logical drives and the controller's SAM is set to 'Failure'.
    * For Array's with only RAID 0 volumes, the controller SAM must be 'Predictive'
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveInvalidSAM                    11
   /** The spare drive cannot be added to the Array because the desired spare type is invalid
    * for the Array while it contains at least 1 RAID 0 logical drive and the controller's SAM is set to 'Failure'.
    * For Array's with only RAID 0 volumes, the controller SAM must be 'Predictive'
    * or change the spare desired type.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveInvalidSpareTypeForRAID0      12
   /** The spare drive cannot be added to the Array because the spare drive type cannot be changed.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveCannotChangeSpareType         13
   /** The spare drive cannot be added to the Array because the Array status is not OK.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveArrayStatusNotOK              14
   /** The spare drive cannot be added to the Array because the spare drive is not unassigned drive and spare type is auto replace.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveDriveIsNotUnassignedOrShareableStandby 15
   /** The spare drive cannot be added to the Array because the spare drive is already assigned as a spare drive.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveDriveAlreadyUsedAsSpare       16
   /** The spare drive cannot be added to the Array because the controller does not support auto replace spare type.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveAutoReplaceSpareNotSupported  17
   /** The array is not a data array. Spares cannot be added to caching or split mirror arrays.
    *  This means that the array one of the following:
    *  - Caching Array
    *  - Split mirror primary Array
    *  - Split mirror backup Array
    *  - Split mirror backup orphan Array
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveNotDataArray                  18
   /** The requested drive is not on the same port type as the Array.
    * This can occur when one drive/Array is on a boot connector and the other is not.
    * @see SA_ConnectorIsBootConnector.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveIncompatibleConnectors        19
   /** The requested drive is in predictive failure state and the controller spare activation mode is predictive.
    * @see SA_GetControllerSAM.
    * @see SA_GetPhysicalDriveStatusInfo.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
   #define kCanAddArraySpareDriveIsPredictiveFailure           20
   /** Cannot add the spare drive to the Array because the drive is not configurable.
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    * @see SA_GetPhysicalDriveStatusInfo.
    */
    #define kCanAddArraySpareDriveDriveIsNotConfigurable       21
   /** Cannot add the spare drive to the Array because the controller has an active or pending online firmware activation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanAddArraySpareDrive.
    * @disallows SA_AddArraySpareDrive.
    */
    #define kCanAddArraySpareDriveControllerOFAActive          22
/** @} */ /* Can Add Spare Drive To Array */

/** @name Add Spare Drive To Array
 * @{ */
/** Add the selected physical drive to the array as a spare drive
 * @post If valid, user may call @ref SA_AddArraySpareDrive.
 * @warning All failed spare drives will be removed from the Array.
 * @param[in] cda                    Controller data area.
 * @param[in] array_number           Target Array number.
 * @param[in] requested_spare_type   Spare type @ref SA_AddArraySpareDrive.
 * @param[in] pd_number              Target spare drive number.
 * @return See [Add Spare Drive To Array Returns](@ref storc_configuration_array_add_spare_drive).
 *
 * __Example__
 * @code
 * if (SA_CanAddArraySpareDrive(cda, array_number, kArrayAddSpareModeAutoReplace, pd_number) == kCanAddArraySpareDriveAllowed)
 * {
 *     if (!SA_AddArraySpareDrive(cda, array_number, kSpareDriveTypeAutoReplace, pd_number))
 *     {
 *         fprintf(stderr, "Failed to add auto replace spare drive to virtual Array\n");
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_AddArraySpareDrive(PCDA cda, SA_WORD array_number, SA_BYTE requested_spare_type, SA_WORD pd_number);
   /** The spare drive was successfully added to the array.
    * @outputof SA_AddArraySpareDrive.
    */
   #define kAddArraySpareDriveOK                            1
   /** The spare drive could not be added to the array and no changes were made to any logical drives.
    * @outputof SA_AddArraySpareDrive.
    */
   #define kAddArraySpareDriveFailedNoChanges               2
   /** The spare drive could not be added to all logical drives of the array.
    * @outputof SA_AddArraySpareDrive.
    */
   #define kAddArraySpareDriveFailedWithChanges             3
/** @} */ /* Add Spare Drive To Array */

/** @} */ /* storc_configuration_array_add_spare_drive */

/** @defgroup storc_configuration_array_remove_spare_drive Remove Spare Drive
 * @brief Remove spare drive from an Array.
 * @{ */

/** @name Can Remove Array Spare Drive
 * @{ **/
/** Checks if the selected spare drive be removed from the array.
 * @post If valid, user may call @ref SA_RemoveArraySpareDrive.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Target Array number.
 * @param[in] pd_number     Target spare drive number.
 * @return See [Can Remove Array Spare Drive Returns](@ref storc_configuration_array_remove_spare_drive).
 *
 * __Example__
 * @code
 * if (SA_CanRemoveArraySpareDrive(cda, array_number, pd_number) == kCanRemoveArraySpareDriveAllowed)
 * {
 *     if (SA_RemoveArraySpareDrive(cda, array_number, pd_number) != kRemoveArraySpareDriveOK)
 *     {
 *         fprintf(stderr, "Failed to remove spare drive from Array\n");
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_CanRemoveArraySpareDrive(PCDA cda, SA_WORD array_number, SA_WORD pd_number);
   /** The spare drive can be removed from the array.
    * @outputof SA_CanRemoveArraySpareDrive.
    * @allows SA_RemoveArraySpareDrive.
    */
   #define kCanRemoveArraySpareDriveAllowed                    1
   /** The spare drive cannot be removed from the array because the specified drive does not exist.
    * @outputof SA_CanRemoveArraySpareDrive.
    * @disallows SA_RemoveArraySpareDrive.
    */
   #define kCanRemoveArraySpareDriveNotFound                   2
   /** The spare drive cannot be removed from the array because the array status is not OK.
    * @outputof SA_CanRemoveArraySpareDrive.
    * @disallows SA_RemoveArraySpareDrive.
    */
   #define kCanRemoveArraySpareDriveArrayArrayStatusNotOK      3
   /** The spare drive cannot be removed from the array because the array is not a data array.
    *  This means that the array one of the following:
    *  - Caching Array
    *  - Split mirror primary Array
    *  - Split mirror backup Array
    *  - Split mirror backup orphan Array
    * @outputof SA_CanRemoveArraySpareDrive.
    * @disallows SA_RemoveArraySpareDrive.
    */
   #define kCanRemoveArraySpareDriveNotDataArray               4
   /** The spare drive cannot be removed from the array because of an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanRemoveArraySpareDrive.
    * @disallows SA_RemoveArraySpareDrive.
    */
   #define kCanRemoveArraySpareDriveControllerOFAActive        5
/** @} */ /* Can Remove Array Spare Drive */

/** @name Remove Array Spare Drive
 * @{ **/
/** Removes the targeted spare drive from the targeted array.
 * @pre The user has used @ref SA_CanRemoveArraySpareDrive to ensure the operation can be performed.
 * @warning All failed spare drives will be removed from the Array.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Target Array number.
 * @param[in] pd_number     Target spare drive number.
 * @return See [Remove Array Spare Drive Returns](@ref storc_configuration_array_add_spare_drive).
 *
 * __Example__
 * @code
 * if (SA_CanRemoveArraySpareDrive(cda, array_number, pd_number) == kCanRemoveArraySpareDriveAllowed)
 * {
 *     if (SA_RemoveArraySpareDrive(cda, array_number, pd_number) != kRemoveArraySpareDriveOK)
 *     {
 *         fprintf(stderr, "Failed to remove spare drive from array.\n");
 *     }
 * }
 * @endcode
 */
SA_BYTE SA_RemoveArraySpareDrive(PCDA cda, SA_WORD array_number, SA_WORD pd_number);
   /** Successfully removed the spare drive from the array.
    * @outputof SA_RemoveArraySpareDrive.
    */
   #define kRemoveArraySpareDriveOK                        1
   /** The spare drive could not be removed from the array and no changes were made to any logical drives.
    * @outputof SA_RemoveArraySpareDrive.
    */
   #define kRemoveArraySpareDriveFailedNoChanges           2
   /** The spare drive could not be removed from all logical drives of the array.
    * @outputof SA_RemoveArraySpareDrive.
    */
   #define kRemoveArraySpareDriveFailedWithChanges         3
/** @} */ /* Remove Array Spare Drive */

/** @} */ /* storc_configuration_array_remove_spare_drive */

/** @defgroup storc_configuration_array_change_spare_type Change Spare Type
 * @brief Change spare type of an Array.
 * @{ */

/** @name Can Change Array Spare Type
 * @{ */
/** Checks if the array spare drive type can be changed.
 * @post If valid, user may call @ref SA_ChangeArraySpareType.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Target array.
 * @param[in] spare_type    Spare type (see [Array Spare Types](@ref storc_properties_array_spare_type)).
 * @return See [Can Change Array Spare Type Returns](@ref storc_configuration_array_change_spare_type).
 *
 * __Example__
 * @code
 * if (SA_CanChangeArraySpareType(cda, 1) == kCanChangeArraySpareTypeAllowed)
 * {
 *    if (SA_ChangeArraySpareType(cda, 1) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change array spare type\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanChangeArraySpareType(PCDA cda, SA_WORD array_number, SA_BYTE spare_type);
   /** Changing the array spare type is allowed.
    * @outputof SA_CanChangeArraySpareType.
    * @allows SA_ChangeArraySpareType.
    */
   #define kCanChangeArraySpareTypeAllowed                 0
   /** The array has no spare drives.
    * @outputof SA_CanChangeArraySpareType.
    * @disallows SA_ChangeArraySpareType.
    */
   #define kCanChangeArraySpareTypeNoSparesFound           1
   /** The spare type is already set.
    * @outputof SA_CanChangeArraySpareType.
    * @disallows SA_ChangeArraySpareType.
    */
   #define kCanChangeArraySpareTypeSelectedTypeAlreadySet  2
   /** The input spare type is not supported.
    * @outputof SA_CanChangeArraySpareType.
    * @disallows SA_ChangeArraySpareType.
    */
   #define kCanChangeArraySpareTypeInvalid                 3
   /** The controller does not support auto-replace spares.
    * @outputof SA_CanChangeArraySpareType.
    * @disallows SA_ChangeArraySpareType.
    */
   #define kCanChangeArraySpareTypeNotSupported            4
   /** The requested spare type cannot be set while the Array has shared spares.
    * @outputof SA_CanChangeArraySpareType.
    * @disallows SA_ChangeArraySpareType.
    */
   #define kCanChangeArraySpareTypeSpareIsShared           5
   /** The spare type cannot be set because the desired spare type is invalid
    * for the Array while it contains at least 1 RAID 0 logical drive and the
    * controller's SAM is set to 'Failure'.
    * For Array's with only RAID 0 volumes, the controller SAM must be 'Predictive'
    * or change the desired spare type.
    * @outputof SA_CanChangeArraySpareType.
    * @disallows SA_ChangeArraySpareType.
    */
   #define kCanChangeArraySpareTypeInvalidForRAID0         6
   /** The requested spare type cannot be set while there is an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanChangeArraySpareType.
    * @disallows SA_ChangeArraySpareType.
    */
   #define kCanChangeArraySpareTypeControllerOFAActive     7
   /** The requested spare type cannot be set because the Array status is not OK.
    * @see storc_status_array_status
    * @outputof SA_CanChangeArraySpareType.
    * @disallows SA_ChangeArraySpareType.
    */
   #define kCanChangeArraySpareTypeArrayStatusNotOK        8
/** @} */ /* Can Change Array Spare Type */

/** @name Change Array Spare Type
 * @{ */
/** Change the array spare drive type.
 * @pre The user has used @ref SA_CanChangeArraySpareType to ensure the operation can be performed.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Target array.
 * @param[in] spare_type    Spare type (see [Array Spare Types](@ref storc_properties_array_spare_type)).
 * @return @ref kTrue if the array spare type was changed successfully, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeArraySpareType(cda, 1, kSpareDriveTypeAutoReplace) == kCanChangeArraySpareTypeAllowed)
 * {
 *    if (SA_ChangeArraySpareType(cda, 1, kSpareDriveTypeAutoReplace) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to change array spare type\n");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_ChangeArraySpareType(PCDA cda, SA_WORD array_number, SA_BYTE spare_type);
   /** Changing the array spare type is allowed.
    * @outputof SA_CanChangeArraySpareType.
    * @allows SA_ChangeArraySpareType.
    */
   #define kChangeArraySpareTypeOK                 0x01
   /** The array has no spare drives.
    * @outputof SA_CanChangeArraySpareType.
    * @disallows SA_ChangeArraySpareType.
    */
   #define kChangeArraySpareTypeFailedNoChanges    0x02
   /** The spare type is already set.
    * @outputof SA_CanChangeArraySpareType.
    * @disallows SA_ChangeArraySpareType.
    */
   #define kChangeArraySpareTypeFailedWithChanges  0x03
/** @} */ /* Change Array Spare Type */

/** @} */ /* storc_configuration_array_change_spare_type */

/** @defgroup storc_configuration_array_split_mirror Array Split Mirror Management
 * @brief Create/join/manage Array split mirror.
 * @{ */

/** @defgroup storc_configuration_array_join_split_mirror Join Array Split Mirror
 * @{ */

/** @name Join Types
 * Input to @ref SA_JoinArraySplitMirror.
 * @{ */
/** Join Type Rollback. The array data will revert to its state at the type of the Split Mirror Backup operation.
 * Changes made to the Primary array will be discarded.
 * Input to @ref SA_JoinArraySplitMirror.
 */
#define kJoinArraySplitMirrorRollback                0xF0
/** Join Type Remirror. The backup array data will be discarded.
 * Input to @ref SA_JoinArraySplitMirror.
 */
#define kJoinArraySplitMirrorRemirror                0xF1
/** @} */

/** @name Can Join Array Split Mirror
 * @{ */
/** Checks if a split mirror can be joined with its Backup Array pair.
 * @post If return is @ref kCanJoinArraySplitMirrorAllowed, user may call @ref SA_JoinArraySplitMirror.
 * @param[in]  cda           Controller data area.
 * @param[in]  array_number  Target array to join, this must be array type kArrayTypeSplitMirrorPrimary.
 * @return @ref kCanJoinArraySplitMirrorAllowed if possible. See [Can Join Split Mirror Returns](@ref storc_configuration_array_join_split_mirror).
 */
SA_BYTE SA_CanJoinArraySplitMirror(PCDA cda, SA_WORD array_number);
/** Joining a split mirror is allowed.
 * Output of @ref SA_CanJoinArraySplitMirror.
 */
#define kCanJoinArraySplitMirrorAllowed                  0x00
/** The primary or backup Array status is not 'OK'.
  * Output of @ref SA_CanJoinArraySplitMirror.
 */
#define kCanJoinArraySplitMirrorArrayNotOK               0x01
/** The controller does not support the operation.
  * Output of @ref SA_CanJoinArraySplitMirror.
 */
#define kCanJoinArraySplitMirrorNotSupported             0x02
/** The RAID configuration on the array is not supported for a join.
  * Output of @ref SA_CanJoinArraySplitMirror.
 */
#define kCanJoinArraySplitMirrorUnsupportedRAID          0x03
/** The array is a caching array, which is not supported for this operation.
  * Output of @ref SA_CanJoinArraySplitMirror.
 */
#define kCanJoinArraySplitMirrorArrayIsCaching           0x04
/** The array type is caching, not a type supported for this operation.
  * Output of @ref SA_CanJoinArraySplitMirror.
 */
#define kCanJoinArraySplitMirrorArrayIsInvalidType       0x05
/** The array has a volume which is not a type supported for this operation.
  * Output of @ref SA_CanJoinArraySplitMirror.
 */
#define kCanJoinArraySplitMirrorArrayHasInvalidRAIDLevel 0x06
/** There is no array that matches the target array for this operation.
  * Output of @ref SA_CanJoinArraySplitMirror.
 */
#define kCanJoinArraySplitMirrorNoMatchingArray          0x07
/** There is no volume that matches one or more of the target volumes for this operation.
  * Output of @ref SA_CanJoinArraySplitMirror.
 */
#define kCanJoinArraySplitMirrorNoMatchingVolume         0x08
/** There controller status is not OK.
 * Output of @ref SA_CanJoinArraySplitMirror.
 */
#define kCanJoinArraySplitMirrorControllerStatusNotOK    0x09
/** There controller has an active or pending online firmware activation operation.
 * @see storc_status_ctrl_online_firmware_activation
 * @outputof SA_CanJoinArraySplitMirror
 * @disallows SA_JoinArraySplitMirror
 */
#define kCanJoinArraySplitMirrorControllerOFAActive      0x0A
/** Split-mirror management is not allowed while LU cache is enabled.
 * @outputof SA_CanJoinArraySplitMirror
 * @disallows SA_JoinArraySplitMirror
 */
#define kCanJoinArraySplitMirrorLUCacheEnabled           0x0B
/** @} */ /* Can Join Array Split Mirror */

/** @name Join Array Split Mirror
 * @{ */
/** Join the selected Array with its matching Backup Array
 * @param[in] cda                     Controller data area.
 * @param[in] array_number            Target Array number. This must be
 *                                    array type kArrayTypeSplitMirrorPrimary.
 * @param[in] action                  @ref kJoinArraySplitMirrorRollback or @ref kJoinArraySplitMirrorRemirror
 * @return @ref kTrue if successful, otherwise return @ref kFalse.
 *
 * __Example__
 * @code
 * if (SA_CanJoinArraySplitMirror(cda, array_number) == kCanJoinArraySplitMirrorAllowed)
 * {
 *     if (!SA_JoinArraySplitMirror(cda, array_number, kJoinArraySplitMirrorRollback))
 *     {
 *         fprintf(stderr, "Failed to Join Array\n");
 *     }
 * }
 * @endcode
 */
SA_BOOL SA_JoinArraySplitMirror(PCDA cda, SA_WORD array_number, SA_WORD action);
/** @} */ /* Join Array Split Mirror */

/** @} */ /* storc_configuration_array_join_split_mirror */

/** @defgroup storc_configuration_array_activate_split_mirror Activate Array Split Mirror
 * @{ */

/** @name Can Activate Array Split Mirror Backup
 * @{ */
/** Checks if the split mirror backup array can be activated.
 * @post If valid, user may call @ref SA_ActivateArraySplitMirrorBackup.
 * @param[in]  cda           Controller data area.
 * @param[in]  array_number  Target array, which is the backup array or the orphan array.
 * @return See [Can Activate Split Mirror Backup Returns](@ref storc_configuration_array_activate_split_mirror).
 */
SA_BYTE SA_CanActivateArraySplitMirrorBackup(PCDA cda, SA_WORD array_number);
   /** Activating a split mirror backup is allowed.
    * @outputof SA_CanActivateArraySplitMirrorBackup.
    * @allows SA_ActivateArraySplitMirrorBackup.
    */
   #define kCanActivateArraySplitMirrorBackupAllowed                      0x00
   /** The controller is not in OK status.
    * @outputof SA_CanActivateArraySplitMirrorBackup.
    * @disallows SA_ActivateArraySplitMirrorBackup.
    */
   #define kCanActivateArraySplitMirrorBackupControllerNotOK              0x01
   /** The controller does not support the operation.
    * @outputof SA_CanActivateArraySplitMirrorBackup.
    * @disallows SA_ActivateArraySplitMirrorBackup.
    */
   #define kCanActivateArraySplitMirrorBackupNotSupported                 0x02
   /** The array was not found or the array status was not OK.
    * @outputof SA_CanActivateArraySplitMirrorBackup.
    * @disallows SA_ActivateArraySplitMirrorBackup.
    */
   #define kCanActivateArraySplitMirrorBackupArrayNotOK                   0x03
   /** The array is not a split mirror backup or orphan. Can not activate it.
    * @outputof SA_CanActivateArraySplitMirrorBackup.
    * @disallows SA_ActivateArraySplitMirrorBackup.
    */
   #define kCanActivateArraySplitMirrorBackupNotBackupArrayOrOrphan       0x04
   /** The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanActivateArraySplitMirrorBackup.
    * @disallows SA_ActivateArraySplitMirrorBackup.
    */
   #define kCanActivateArraySplitMirrorBackupControllerOFAActive          0x05
   /** Split-mirror management is not allowed while LU cache is enabled.
    * @outputof SA_CanActivateArraySplitMirrorBackup
    * @disallows SA_ActivateArraySplitMirrorBackup
    */
   #define kCanActivateArraySplitMirrorBackupLUCacheEnabled               0x06
/** @} */ /* Can Activate Array Split Mirror Backup */

/** @name Activate Array Split Mirror Backup
 * @{ */
/** Activate the selected Array
 * @param[in] cda                     Controller data area.
 * @param[in] array_number            Target Array number. Must be array type kArrayTypeSplitMirrorBackup or kArrayTypeSplitMirrorBackupOrphan.
 * @return @ref kTrue if successful, otherwise return @ref kFalse.
 *
 * __Example__
 * @code
 * if (SA_CanActivateArraySplitMirrorBackup(cda, array_number) == kCanActivateArraySplitMirrorBackupAllowed)
 * {
 *     if (!SA_ActivateArraySplitMirrorBackup(cda, array_number))
 *     {
 *         fprintf(stderr, "Failed to Activate volumes in the Array\n");
 *     }
 * }
 * @endcode
 */
SA_BOOL SA_ActivateArraySplitMirrorBackup(PCDA cda, SA_WORD array_number);
/** @} */ /* Activate Array Split Mirror Backup */

/** @} */ /* storc_configuration_array_activate_split_mirror */

/** @defgroup storc_configuration_array_create_split_mirror Create Array Split Mirror Backup
 * @{ */

/** @name Can Create Array Split Mirror Backup
 * @{ */
/** Checks if split mirror back up can be performed on the array.
 * @post If valid, user may call @ref SA_CreateArraySplitMirrorBackup.
 * @param[in]  cda           Controller data area.
 * @param[in]  array_number  Existing Array number (will be the primary Array).
 * @return See [Can Create Array Split Mirror Backup Returns](@ref storc_configuration_array_create_split_mirror).
 *
 * __Example__
 * @code
 * SA_WORD array_number = 1;
 * if (SA_CanCreateArraySplitMirrorBackup(cda, array_number) == kCanCreateArraySplitMirrorBackupAllowed)
 * {
 *    SA_WORD backup_array_number = SA_CreateArraySplitMirrorBackup(cda, array_number);
 *    if (backup_array_number == kInvalidArrayNumber)
 *    {
 *        fprintf(stderr, "Failed to create split mirror backup\n");
 *    }
 *    else
 *    {
 *        printf("Created backup array %u for array %u\n", backup_array_number, array_number);
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanCreateArraySplitMirrorBackup(PCDA cda, SA_WORD array_number);
   /** Creating a split mirror backup is allowed.
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @allows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupAllowed                    0x00
   /** The array was not found.
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupArrayNotOK                 0x01
   /** The controller does not support the operation.
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupNotSupported               0x02
   /** The RAID configuration on the array is not supported for split mirror backup.
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupUnsupportedRAID            0x04
   /** The maximum number of logical drives has been reached on the controller.
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupMaxNumberOfLogicalDrives   0x05
   /** The array has volatile keys enabled on at least one logical drive or
    *  a logical drive is undergoing a volatile key operation.
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupVolatileKeys               0x06
   /** A license key is required to mirror a @ref kRAID1Triple or @ref kRAID10Triple volume.
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupNWAYMirroringNotSupported  0x07
   /** The array is not a data array and cannot be split.
    *  This means that the array one of the following:
    *  - Caching Array
    *  - Split mirror primary Array
    *  - Split mirror backup Array
    *  - Split mirror backup orphan Array
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupNotDataArray               0x08
   /** A split mirror backup cannot be created because the controller is locked.
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupControllerLocked           0x09
   /** A split mirror backup cannot be created because the controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupControllerOFAActive        0x0A
   /** A split mirror backup cannot be created because a logical drive in the Array has an LU caching logical drive assigned.
    * @see storc_properties_lu_ld
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupHasLUCache                 0x0B
   /** A split mirror backup cannot be created while LU cache is enabled.
    * @outputof SA_CanCreateArraySplitMirrorBackup.
    * @disallows SA_CreateArraySplitMirrorBackup.
    */
   #define kCanCreateArraySplitMirrorBackupLUCacheEnabled             0x0C
/** @} */ /* Can Create Array Split Mirror Backup */

/** @name Can Create Array Split Mirror Backup
 * @{ */
/** Create split mirror backup.
 * @pre The user has used @ref SA_CanCreateArraySplitMirrorBackup to ensure the operation can be performed.
 * @param[in] cda           Controller data area.
 * @param[in] array_number  Existing Array number (will be the primary Array).
 * @return Array number of the backup array if the split mirror backup was successfully created, @ref kInvalidArrayNumber otherwise.
 *
 * __Example__
 * @code
 * SA_WORD array_number = 1;
 * if (SA_CanCreateArraySplitMirrorBackup(cda, array_number) == kCanCreateArraySplitMirrorBackupAllowed)
 * {
 *    SA_WORD backup_array_number = SA_CreateArraySplitMirrorBackup(cda, array_number);
 *    if (backup_array_number == kInvalidArrayNumber)
 *    {
 *        fprintf(stderr, "Failed to create split mirror backup\n");
 *    }
 *    else
 *    {
 *        printf("Created backup array %u for array %u\n", backup_array_number, array_number);
 *    }
 * }
 * @endcode
 */
SA_WORD SA_CreateArraySplitMirrorBackup(PCDA cda, SA_WORD array_number);
/** @} */ /* Create Array Split Mirror Backup */

/** @} */ /* storc_configuration_array_create_split_mirror */

/** @} */ /* storc_configuration_array_split_mirror */

/** @defgroup storc_configuration_array_heal Heal Array
 * @brief Heal degraded and re-enable failed logical drives of an Array with different data drives.
 * @{ */

/** @name Find Physical Drive to Heal Array
 * @{ */
/** Determine if a physical drive can be used to heal the array.
 * @pre `array_number` is a valid Array number.
 * @pre `pd_number` is a valid physical drive number.
 * @param[in] cda          Controller data area.
 * @param[in] array_number Valid Array number.
 * @param[in] pd_number    Current physical drive number (or @ref kInvalidPDNumber to get the first physical drive).
 * @return See returns of @ref SA_CanHealArray.
 */
SA_BYTE SA_CanHealArrayWithPhysicalDrive(PCDA cda, SA_WORD array_number, SA_WORD pd_number);
/** @} */ /* Find Physical Drive to Heal Array */

/** @name Can Heal Array
 * @{ */
/** Checks if the array is a valid target to heal with the provided physical drives.
 * @pre `array_number` is a valid Array number.
 * @pre `pd_list` contains enough valid physical drives to heal the Array.
 * @post If valid, user may call @ref SA_HealArray.
 * @param[in] cda             Controller data area.
 * @param[in] array_number    The target Array to heal.
 * @param[in] pd_list         A list of physical drive numbers to use as replacement drives to heal the Array.
 * @param[in] pd_list_size    Number of physical drive numbers in the pd_list list.
 * @return List of reasons the selected drives cannot be used to heal the target Array (see [Can Heal Array](@ref storc_configuration_array_heal))
 *
 * __Example__
 * @code
 * SA_WORD pd_list[2] - { 4, 5 };
 * if (SA_CanHealArray(cda, 2, pd_list, 2) == kCanHealArrayAllowed)
 * {
 *    if (SA_HealArray(cda, 2, pd_list, 2) != kHealArraySuccess)
 *    {
 *       fprintf(stderr, "Failed to heal array.");
 *    }
 * }
 * else if (SA_CanHealArray(cda, 2, pd_list, 2) == kCanHealArrayLoginNeeded)
 * {
 *    if (SA_HealArrayWithLoginSafe(cda, 2, pd_list, 2, kControllerEncryptionCryptoOfficer, "MyPassword", sizeof("MyPassword")) != kHealArraySuccess)
 *    {
 *       fprintf(stderr, "Failed to heal array.");
 *    }
 * } * @endcode
 */
SA_BYTE SA_CanHealArray(PCDA cda, SA_WORD array_number, SA_WORD * pd_list, size_t pd_list_size);
/** Heal array is allowed.
 * @outputof SA_CanHealArray.
 * @allows SA_HealArray.
 */
#define kCanHealArrayAllowed                                       1
/** Array is not failed or degraded.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayNoFailedDrives                                2
/** The controller does not have enough transformation memory.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayExceedsTransformationMemory                   3
/** The controller does not support heal array or heal array is not licensed.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayNotSupported                                  4
/** The array is currently transforming.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayIsTransforming                                5
/** Healing is not supported for the RAID level on a split mirror primary array.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayRAIDLevelNotSupported                         6
/** At least one physical drive in the list is not unassigned.
 * @see SA_CanHealArrayWithPhysicalDrive.
 * @outputof SA_CanHealArray.
 * @outputof SA_CanHealArrayWithPhysicalDrive.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayDriveNotUnassigned                            7
/** At least one physical drive in the list is not configurable.
 * @see SA_CanHealArrayWithPhysicalDrive.
 * @outputof SA_CanHealArray.
 * @outputof SA_CanHealArrayWithPhysicalDrive.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayDriveNotConfigurable                          8
/** At least one physical drive in the list is too small.
 * @see SA_CanHealArrayWithPhysicalDrive.
 * @outputof SA_CanHealArray.
 * @outputof SA_CanHealArrayWithPhysicalDrive.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayDriveTooSmall                                 9
/** At least one physical drive in the list does not match the array block size.
 * @see SA_CanHealArrayWithPhysicalDrive.
 * @outputof SA_CanHealArray.
 * @outputof SA_CanHealArrayWithPhysicalDrive.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayDriveBlockSizeMismatch                        10
/** At least one physical drive in the list does not match the array drive type.
 * @see SA_CanHealArrayWithPhysicalDrive.
 * @outputof SA_CanHealArray.
 * @outputof SA_CanHealArrayWithPhysicalDrive.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayDriveTypeMismatch                              11
/** At least one physical drive in the list is invalid.
 * @see SA_CanHealArrayWithPhysicalDrive.
 * @outputof SA_CanHealArray.
 * @outputof SA_CanHealArrayWithPhysicalDrive.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayDriveInvalid                                   12
/** Not enough physical drives provided to heal the array.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayNotEnoughReplacementDrives                     13
/** Too many physical drives provided to heal the array.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayTooManyReplacementDrives                       14
/** Target array is invalid.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayInvalidArray                                   15
/** Cannot heal Array because the controller status is not 'OK'.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayControllerStatusNotOK                          16
/** Cannot heal Array because the controller's cache status is not 'OK'.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayControllerCacheStatusNotOK                     17
/** Duplicate drive numbers found in given list of replacement drives.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayDuplicateDrivesInList                          18
/** Array Heal not allowed while LU cache is enabled.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayLUCachingEnabled                               19
/** Array Heal not allowed because the drives are not on the same port type.
 * This can occur when one drive/Array is on a boot connector and the other is not.
 * @see SA_ConnectorIsBootConnector.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayDriveIncompatibleConnectors                    20
/** Array Heal not allowed because transformations are not allowed.
 * @see storc_status_ctrl_status.
 * @see storc_status_ctrl_backup_power_source_status.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayCannotTransformUnknownReason                   21
/** Array Heal not allowed because data preservation is not available.
 * @see storc_status_ctrl_status.
 * @see storc_status_ctrl_backup_power_source_status.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayDataPreservationNotAvailable                   22
/** Array Heal not allowed because encryption login is needed.
 * Call @ref SA_HealArrayWithLoginSafe to heal the array with encryption login.
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayLoginNeeded                                    23
/** Array Heal not allowed because of an active or pending online firmware activation.
 * @see storc_status_ctrl_online_firmware_activation
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayControllerOFAActive                            24
/** Array Heal not allowed on split mirror backup or backup orphan Arrays.
 * @see storc_properties_array_type
 * @outputof SA_CanHealArray.
 * @disallows SA_HealArray.
 */
#define kCanHealArraySplitMirrorBackup                              25
/** Array Heal not allowed because no logical drive exists on the Array.
 * @see SA_CanHealArrayWithPhysicalDrive.
 * @outputof SA_CanHealArray.
 * @outputof SA_CanHealArrayWithPhysicalDrive.
 * @disallows SA_HealArray.
 */
#define kCanHealArrayNoLogicalDriveFound                            26
/** @} */ /* Can Heal Array */

/** @name Heal Array
 * @{ */
/** Perform heal Array with replacement physical drives.
 * @pre `array_number` is a valid Array number.
 * @pre `pd_list` contains enough valid physical drives to heal the Array.
 * @pre The user has used @ref SA_CanHealArray to ensure the operation can be performed.
 * @attention The Heal Array operation may take awhile to complete.
 * @param[in] cda             Controller data area.
 * @param[in] array_number    The target Array to heal.
 * @param[in] pd_list         A list of physical drive numbers to use as replacement drives to heal the Array.
 * @param[in] pd_list_size    Number of physical drive numbers in the pd_list list.
 * @return Value representing if the operation was performed successfully or failed with/without changes.
 * @return List of returns at [Heal Array](@ref storc_configuration_array_heal).
 *
 * __Example__
 * @code
 * SA_WORD pd_list[2] - { 4, 5 };
 * if (SA_CanHealArray(cda, 2, pd_list, 2) == kCanHealArrayAllowed)
 * {
 *    if (SA_HealArray(cda, 2, pd_list, 2) != kHealArraySuccess)
 *    {
 *       fprintf(stderr, "Failed to heal array.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_HealArray(PCDA cda, SA_WORD array_number, SA_WORD * pd_list, size_t pd_list_size);
/** Array heal failed to start and no changes to the current configuration have been made.
 * @outputof SA_HealArray.
 */
#define kHealArrayFailedNoChanges      0x00
/** Array heal failed to start on all logical drives of the Array.
 * @outputof SA_HealArray.
 * @outputof SA_HealArrayWithLoginSafe.
 */
#define kHealArrayFailedWithChanges    0x01
/** Array heal has been started successfully.
 * @outputof SA_HealArray.
 * @outputof SA_HealArrayWithLoginSafe.
 */
#define kHealArraySuccess              0x02
/** Array heal failed to start on due to invalid user
 * @outputof SA_HealArray.
 * @outputof SA_HealArrayWithLoginSafe.
 */
#define kHealArrayInvalidUser          0x03
/** Array heal failed to start on due to user's permission
 * @outputof SA_HealArray.
 * @outputof SA_HealArrayWithLoginSafe.
 */
#define kHealArrayPermissionDenied     0x04
/** Array heal failed to start on due to failed login
 * @outputof SA_HealArray.
 * @outputof SA_HealArrayWithLoginSafe.
 */
#define kHealArrayLoginFailed          0x05
/** @} */ /* Heal Array */

/** @name Heal Array With Login
* @{ */
/** Perform heal Array with replacement physical drives and encryption login.
 *
 * @deprecated Use @ref SA_HealArrayWithLoginSafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_HealArrayWithLoginSafe).
 *
 * When the controller has encryption enabled and the array to be healed has failed plaintext
 * logical drives, the encryption login is required to heal this array.
 * @pre `array_number` is a valid Array number.
 * @pre `pd_list` contains enough valid physical drives to heal the Array.
 * @pre The user has used @ref SA_CanHealArray to ensure the operation can be performed.
 * @attention The Heal Array operation may take awhile to complete.
 * @param[in] cda             Controller data area.
 * @param[in] array_number    The target Array to heal.
 * @param[in] pd_list         A list of physical drive numbers to use as replacement drives to heal the Array.
 * @param[in] pd_list_size    Number of physical drive numbers in the pd_list list.
 * @param[in] user            Encryption user for login (ignored if changes do not require login).
 * @param[in] password        Null-terminated encryption user password string for login (ignored if changes do not require login).
 * @return Value representing if the operation was performed successfully or failed.
 * @return List of returns at [Heal Array](@ref storc_configuration_array_heal).
 *
 * __Example__
 * @code
 * SA_WORD pd_list[2] - { 4, 5 };
 * if (SA_CanHealArray(cda, 2, pd_list, 2) == kCanHealArrayLoginNeeded)
 * {
 *    if (SA_HealArrayWithLogin(cda, 2, pd_list, 2, kControllerEncryptionCryptoOfficer, "MyPassword") != kHealArraySuccess)
 *    {
 *       fprintf(stderr, "Failed to heal array.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_HealArrayWithLogin(PCDA cda, SA_WORD array_number, SA_WORD * pd_list, size_t pd_list_size, SA_BYTE user, const char *password);
/** @} */ /* Heal Array With Login */

/** @name Heal Array With Login (Safe)
 * @{ */
/** Perform heal Array with replacement physical drives and encryption login.
 * When the controller has encryption enabled and the array to be healed has failed plaintext
 * logical drives, the encryption login is required to heal this array.
 * @pre `array_number` is a valid Array number.
 * @pre `pd_list` contains enough valid physical drives to heal the Array.
 * @pre The user has used @ref SA_CanHealArray to ensure the operation can be performed.
 * @attention The Heal Array operation may take awhile to complete.
 * @param[in] cda             Controller data area.
 * @param[in] array_number    The target Array to heal.
 * @param[in] pd_list         A list of physical drive numbers to use as replacement drives to heal the Array.
 * @param[in] pd_list_size    Number of physical drive numbers in the pd_list list.
 * @param[in] user            Encryption user for login (ignored if changes do not require login).
 * @param[in] password        Null-terminated encryption user password string for login (ignored if changes do not require login).
 * @param[in] password_size   Size of encryption user password string for login.
 * @return Value representing if the operation was performed successfully or failed.
 * @return List of returns at [Heal Array](@ref storc_configuration_array_heal).
 *
 * __Example__
 * @code
 * SA_WORD pd_list[2] - { 4, 5 };
 * if (SA_CanHealArray(cda, 2, pd_list, 2) == kCanHealArrayLoginNeeded)
 * {
 *    if (SA_HealArrayWithLoginSafe(cda, 2, pd_list, 2, kControllerEncryptionCryptoOfficer, "MyPassword", sizeof("MyPassword")) != kHealArraySuccess)
 *    {
 *       fprintf(stderr, "Failed to heal array.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_HealArrayWithLoginSafe(PCDA cda, SA_WORD array_number, SA_WORD * pd_list, size_t pd_list_size, SA_BYTE user, const char *password, size_t password_size);
/** @} */ /* Heal Array With Login (Safe) */
/** @} */ /* storc_configuration_array_heal */

/** @defgroup storc_configuration_array_fill Fill Gaps In Array
 * @brief Re-align logical drives of an Array to remove gaps between logical drives.
 *
 * Array gaps can occur in rare cases where there is unused disk space between logical drives on the Array
 * (or there is unused space from the start of the disk and the first logical drive).
 *
 * @verbatim
 *      Array
 *    ,----------,
 *   (            )
 *   |~----------~|
 *   |            | <- Gap
 *   |~----------~|
 *   |    LD 1    |
 *   |~----------~|
 *   |            | <- Gap
 *   |~----------~|
 *   |    LD 2    |
 *   |~----------~|
 *   |            |
 *    "----------"
 * @endverbatim
 * @{ */

/** @name Can Close Array Gaps?
 * @{ */

/** Checks if the Array has gaps that can be closed using @ref SA_CloseArrayGaps.
 *
 * @pre `array_number` is a valid Array number.
 * @post If valid, user may call @ref SA_CloseArrayGaps.
 * @param[in] cda             Controller data area.
 * @param[in] array_number    The target Array.
 * @return List of reasons the selected Array cannot have gaps closed (see [Can Close Array Gaps?](@ref storc_configuration_array_fill))
 *
 * __Example__
 * @code
 * if (SA_CanCloseArrayGaps(cda, 2) == kCanCloseArrayGapsAllowed)
 * {
 *    if (SA_CloseArrayGaps(cda, 2) != kCloseArrayGapsSuccess)
 *    {
 *       fprintf(stderr, "Failed to close gaps of Array 2.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanCloseArrayGaps(PCDA cda, SA_WORD array_number);
/** Closing Array gaps is allowed.
 * @outputof SA_CanCloseArrayGaps.
 * @allows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsAllowed                                          1
/** Cannot close Array gaps because the target Array is invalid or does not exist.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsInvalidArray                                     2
/** Cannot close Array gaps because the Array does not have gaps.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsArrayDoesNotHaveGaps                             3
/** Cannot close Array gaps because the Array is not a data Array.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsNotDataArray                                     4
/** Cannot close Array gaps because the Array contains logical drives that are transforming.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsArrayHasTransformingLogicalDrive                 5
/** Cannot close Array gaps because the Array status is not OK.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsArrayStatusNotOK                                 6
/** Cannot close Array gaps because the controller status is not OK.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsControllerStatusNotOK                            7
/** Cannot close Array gaps because the controller cache is not OK.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsControllerCacheStatusNotOK                       8
/** Cannot close Array gaps because the controller does not have the transformation memory capacity needed.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsExceedsTransformationMemory                      9
/** Cannot close Array gaps because the operation is not supported.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsNotSupported                                     10
/** Cannot close Array gaps because the operation is not allowed while LU caching is enabled.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsLUCachingEnabled                                 11
/** Cannot close Array gaps because data preservation is not available.
 * @see storc_status_ctrl_status.
 * @see storc_status_ctrl_backup_power_source_status.
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsDataPreservationNotAvailable                     12
/** Cannot close Array gaps because the controller has an active or pending online firmware activation.
 * @see storc_status_ctrl_online_firmware_activation
 * @outputof SA_CanCloseArrayGaps.
 * @disallows SA_CloseArrayGaps.
 */
#define kCanCloseArrayGapsControllerOFAActive                              13
/** @} */ /* Can Close Array Gaps? */

/** @name Close Array Gaps
 * @{ */
/** Fill Array gaps by repositioning logical drives.
 * @pre `array_number` is a valid Array number.
 * @pre The user has used @ref SA_CanCloseArrayGaps to ensure the operation can be performed.
 * @attention The Fill Array gaps operation may take awhile to complete.
 * @param[in] cda             Controller data area.
 * @param[in] array_number    The target Array.
 * @return Value representing if the operation was performed successfully or failed with/without changes.
 * @return List of returns at [Close Array Gaps](@ref storc_configuration_array_fill).
 *
 * __Example__
 * @code
 * if (SA_CanCloseArrayGaps(cda, 2) == kCanCloseArrayGapsAllowed)
 * {
 *    if (SA_CloseArrayGaps(cda, 2) != kCloseArrayGapsSuccess)
 *    {
 *       fprintf(stderr, "Failed to close gaps of Array 2.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CloseArrayGaps(PCDA cda, SA_WORD array_number);
/** Failed to close Array gaps and no changes to the current configuration have been made.
 * @outputof SA_HealArray.
 */
#define kCloseArrayGapsFailedNoChanges       0x00
/** Failed to close all Array gaps but some changes have occured to the configuration.
 * @outputof SA_HealArray.
 */
#define kCloseArrayGapsFailedWithChanges     0x01
/** Close some Array gaps successfully but some gaps remain do to transformation memory limitations.
 * In order to close all Array gaps re-run the operation when current transformations complete.
 * @outputof SA_HealArray.
 */
#define kCloseArrayGapsSuccessButIncomplete  0x02
/** Array gaps has been closed successfully.
 * @outputof SA_HealArray.
 */
#define kCloseArrayGapsSuccess               0x03
/** @} */ /* Close Array Gaps */

/** @} */ /* storc_configuration_array_fill */

/** @} */ /* storc_configuration_array */

/************************************************//**
* @defgroup storc_configuration_lk License Key
* @{
***************************************************/
/** @defgroup storc_configuration_lk_install License Key Install
 * @{ */

/** @name Can Add License Key
 * @{ */
/** Checks if a license key can be added to the controller.
 * @deprecated Use @ref SA_CanAddLicenseKeySafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_CanAddLicenseKeySafe).
 * @post If valid, user may call @ref SA_AddLicenseKey.
 * @param[in] cda                          Controller data area.
 * @param[in] intended_license_key_buffer  The key intend to add. It must be null terminated 25-character alphanumeric string.
 * @return See [Can Add License Key Returns](@ref storc_configuration_lk_install).
 *
 * __Example__
 * @code
 * if (SA_CanAddLicenseKey(cda, "AAAAABBBBBCCCCCDDDDDEEEEE") == kCanAddLicenseKeyAllowed)
 * {
 *    if (SA_AddLicenseKey(cda, "AAAAABBBBBCCCCCDDDDDEEEEE") == kFalse)
 *    {
 *       fprintf(stderr, "Failed to add the license key.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanAddLicenseKey(PCDA cda, const char *intended_license_key_buffer);
/** @} */ /* Can Add License Key */

/** @name Can Add License Key (Safe)
 * @{ */
/** Checks if a license key can be added to the controller.
 * @post If valid, user may call @ref SA_AddLicenseKey.
 * @param[in] cda                                Controller data area.
 * @param[in] intended_license_key_buffer        The key intend to add. It must be null terminated 25-character alphanumeric string.
 * @param[in] intended_license_key_buffer_size   Size of intended_license_key_buffer.
 * @return See [Can Add License Key Returns](@ref storc_configuration_lk_install).
 *
 * __Example__
 * @code
 * if (SA_CanAddLicenseKeySafe(cda, "AAAAABBBBBCCCCCDDDDDEEEEE", sizeof("AAAAABBBBBCCCCCDDDDDEEEEE")) == kCanAddLicenseKeyAllowed)
 * {
 *    if (SA_AddLicenseKeySafe(cda, "AAAAABBBBBCCCCCDDDDDEEEEE", sizeof("AAAAABBBBBCCCCCDDDDDEEEEE")) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to add the license key.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanAddLicenseKeySafe(PCDA cda, const char *intended_license_key_buffer, size_t intended_license_key_buffer_size);
/** Add License Key is allowed.
 * @outputof SA_CanAddLicenseKey.
 * @outputof SA_CanAddLicenseKeySafe.
 * @allows SA_AddLicenseKey.
 * @allows SA_AddLicenseKeySafe.
 */
#define kCanAddLicenseKeyAllowed                      0x01
/** The operation is not supported in this configuration.
 * @outputof SA_CanAddLicenseKey.
 * @outputof SA_CanAddLicenseKeySafe.
 * @disallows SA_AddLicenseKey.
 * @disallows SA_AddLicenseKeySafe.
 */
#define kCanAddLicenseKeyUnsupportedOperation         0x02
/** The max number of license keys has been reached on this controller.
 * @outputof SA_CanAddLicenseKey.
 * @outputof SA_CanAddLicenseKeySafe.
 * @disallows SA_AddLicenseKey.
 * @disallows SA_AddLicenseKeySafe.
 */
#define kCanAddLicenseKeyMaxLicenseKeysReached        0x03
/** Cannot add, the intended license key already exists.
 * @outputof SA_CanAddLicenseKey.
 * @outputof SA_CanAddLicenseKeySafe.
 * @disallows SA_AddLicenseKey.
 * @disallows SA_AddLicenseKeySafe.
 */
#define kCanAddLicenseKeyKeyAlreadyExist              0x04
/** Cannot add, the length of the license key is invalid. Expected 25 characters.
 * @outputof SA_CanAddLicenseKey.
 * @outputof SA_CanAddLicenseKeySafe.
 * @disallows SA_AddLicenseKey.
 * @disallows SA_AddLicenseKeySafe.
 */
#define kCanAddLicenseKeyLengthIncorrect              0x05
/** Cannot add, the license key contains non-alphanumeric characters.
 * @outputof SA_CanAddLicenseKey.
 * @outputof SA_CanAddLicenseKeySafe.
 * @disallows SA_AddLicenseKey.
 * @disallows SA_AddLicenseKeySafe.
 */
#define kCanAddLicenseKeyKeyWithInvalidCharacters     0x06
/** Cannot add, the controller has an active or pending online firmware activation status.
 * @see storc_status_ctrl_online_firmware_activation
 * @outputof SA_CanAddLicenseKey.
 * @outputof SA_CanAddLicenseKeySafe.
 * @disallows SA_AddLicenseKey.
 * @disallows SA_AddLicenseKeySafe.
 */
#define kCanAddLicenseKeyControllerOFAActive          0x07
/** @} */ /* Can Add License Key (Safe) */

/** @name Add License Key
 * @{ */
/** Add a license key to the controller
 * @deprecated Use @ref SA_AddLicenseKeySafe instead.
 * @warning This is an unsafe function which is a security risk. Please use the safe version of this function (@ref SA_AddLicenseKeySafe).
 * @pre The user has used @ref SA_CanAddLicenseKey to ensure the operation can be performed.
 * @param[in] cda                          Controller data area.
 * @param[in] intended_license_key_buffer  The intended key to add. It must be null terminated 25-character alphanumeric string.
 * @return See [Add License Key Returns](@ref storc_configuration_lk_install).
 *
 * __Example__
 * @code
 * if (SA_CanAddLicenseKey(cda, "AAAAABBBBBCCCCCDDDDDEEEEE") == kCanAddLicenseKeyAllowed)
 * {
 *    if (SA_AddLicenseKey(cda, "AAAAABBBBBCCCCCDDDDDEEEEE") != kAddLicenseKeySuccess)
 *    {
 *        fprintf(stderr, "Failed to add the license key.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_AddLicenseKey(PCDA cda, const char *intended_license_key_buffer);
/** @} */ /* Add License Key */

/** @name Add License Key (Safe)
 * @{ */
/** Add a license key to the controller
 * @pre The user has used @ref SA_CanAddLicenseKeySafe to ensure the operation can be performed.
 * @param[in] cda                                Controller data area.
 * @param[in] intended_license_key_buffer        The intended key to add. It must be null terminated 25-character alphanumeric string.
 * @param[in] intended_license_key_buffer_size   Size of intended_license_key_buffer.
 * @return See [Add License Key Returns](@ref storc_configuration_lk_install).
 *
 * __Example__
 * @code
 * if (SA_CanAddLicenseKeySafe(cda, "AAAAABBBBBCCCCCDDDDDEEEEE", sizeof("AAAAABBBBBCCCCCDDDDDEEEEE")) == kCanAddLicenseKeyAllowed)
 * {
 *    if (SA_AddLicenseKeySafe(cda, "AAAAABBBBBCCCCCDDDDDEEEEE", sizeof("AAAAABBBBBCCCCCDDDDDEEEEE")) != kAddLicenseKeySuccess)
 *    {
 *        fprintf(stderr, "Failed to add the license key.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_AddLicenseKeySafe(PCDA cda, const char *intended_license_key_buffer, size_t intended_license_key_buffer_size);
/** License Key was added successfully.
 * @outputof SA_AddLicenseKey.
 * @outputof SA_AddLicenseKeySafe.
 */
#define kAddLicenseKeySuccess                             0x01
/** Add license Key failed due to unknown error.
 * @outputof SA_AddLicenseKey.
 * @outputof SA_AddLicenseKeySafe.
 */
#define kAddLicenseKeyFailedUnknownError                  0x02
/** Add license Key failed due to duplicate feature name.
 * @outputof SA_AddLicenseKey.
 * @outputof SA_AddLicenseKeySafe.
 */
#define kAddLicenseKeyFailedDuplicateFeatureName          0x03
/** Add license Key failed due to invalid key.
 * @outputof SA_AddLicenseKey.
 * @outputof SA_AddLicenseKeySafe.
 */
#define kAddLicenseKeyFailedInvalidKey                    0x04
/** Add license Key failed due to unknown feature name.
 * @outputof SA_AddLicenseKey.
 * @outputof SA_AddLicenseKeySafe.
 */
#define kAddLicenseKeyFailedUnknownFeatureName            0x05
/** Add license Key failed due to expired key.
 * @outputof SA_AddLicenseKey.
 */
#define kAddLicenseKeyFailedExpiredKey                    0x06
/** Add license Key failed due to unsupported key.
 * @outputof SA_AddLicenseKey.
 * @outputof SA_AddLicenseKeySafe.
 */
#define kAddLicenseKeyFailedUnsupportedKey                0x07
/** @} */ /* Add License Key (Safe) */

/** @} */ /* storc_configuration_lk_install */

/** @defgroup storc_configuration_lk_remove License Key Remove
 * @{ */

/** @name Can Remove License Key
 * @{ */
/** Checks if a license key can be removed from the controller.
 * @post If valid, user may call @ref SA_RemoveLicenseKey.
 * @param[in] cda       Controller data area.
 * @param[in] lk_index  The index of intended key to remove.
 * @return See [Can Remove License Key Returns](@ref storc_configuration_lk_remove).
 *
 * __Example__
 * @code
 * if (SA_CanRemoveLicenseKey(cda, 0) == kCanRemoveLicenseKeyAllowed)
 * {
 *    if (SA_RemoveLicenseKey(cda, 0) != kRemoveLicenseKeySuccess)
 *    {
 *        fprintf(stderr, "Failed to remove the license key.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanRemoveLicenseKey(PCDA cda, SA_WORD lk_index);
/** Remove License Key is allowed.
 * @outputof SA_CanRemoveLicenseKey.
 * @allows SA_RemoveLicenseKey.
 */
#define kCanRemoveLicenseKeyAllowed                      0x01
/** The operation is not supported in this configuration.
 * @outputof SA_CanRemoveLicenseKey.
 * @disallows SA_RemoveLicenseKey.
 */
#define kCanRemoveLicenseKeyUnsupportedOperation         0x02
/** Cannot find the key intended to be removed.
 * @outputof SA_CanRemoveLicenseKey.
 * @disallows SA_RemoveLicenseKey.
 */
#define kCanRemoveLicenseKeyKeyNotFound                  0x03
/** The controller has an active or pending online firmware activation operation.
 * @see storc_status_ctrl_online_firmware_activation.
 * @outputof SA_CanRemoveLicenseKey.
 * @disallows SA_RemoveLicenseKey.
 */
#define kCanRemoveLicenseKeyControllerOFAActive          0x04
/** @} */ /* Can Remove License Key */

/** @name Remove License Key
 * @{ */
/** Remove a license key from the controller
 * @pre The user has used @ref SA_CanRemoveLicenseKey to ensure the operation can be performed.
 * @param[in] cda       Controller data area.
 * @param[in] lk_index  The index of intended key to remove.
 * @return See [Remove License Key Returns](@ref storc_configuration_lk_remove).
 *
 * __Example__
 * @code
 * if (SA_CanRemoveLicenseKey(cda, 0) == kCanRemoveLicenseKeyAllowed)
 * {
 *    if (SA_RemoveLicenseKey(cda, 0) == kFalse)
 *    {
 *        fprintf(stderr, "Failed to remove the license key.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_RemoveLicenseKey(PCDA cda, SA_WORD lk_index);
/** License Key was removed successfully.
 * @outputof SA_RemoveLicenseKey.
 */
#define kRemoveLicenseKeySuccess                          0x01
/** Remove license Key failed due to unknown error.
 * @outputof SA_RemoveLicenseKey.
 */
#define kRemoveLicenseKeyFailedUnknownError               0x02
/** Remove license Key failed due to invalid key.
 * @outputof SA_RemoveLicenseKey.
 */
#define kRemoveLicenseKeyFailedInvalidKey                 0x03
/** @} */ /* Remove License Key */

/** @} */ /* storc_configuration_lk_remove */

/** @} */ /* storc_configuration_lk */

/** @defgroup storc_configuration_direct_os_bootable_drive_count Number of OS Bootable Drives
 * @brief Set the controller's number of OS bootable drives used by BIOS/UEFI driver.
 * @{ */
/** @name Can Set Number of OS Bootable Drives
 * @{ */
/** Checks if the number of OS bootable drives can be set on the controller.
 * @post If valid, user may call @ref SA_SetControllerOSBootableDriveCount.
 * @param[in] cda     Controller data area.
 * @param[in] count   The desired number of OS bootable drives.
 * @return See [Can Set Number of OS Bootable Drives Returns](@ref storc_configuration_direct_os_bootable_drive_count).
 *
 * __Example__
 * @code
 * if (SA_CanSetControllerOSBootableDriveCount(cda, 2) == kCanSetControllerOSBootableDriveCountAllowed)
 * {
 *    if (!SA_SetControllerOSBootableDriveCount(cda, 2))
 *    {
 *        fprintf(stderr, "Failed to set number of OS bootable drives.");
 *    }
 * }
 * @endcode
 */
SA_BYTE SA_CanSetControllerOSBootableDriveCount(PCDA cda, SA_BYTE count);
   /** Setting the number of OS bootable drives is allowed.
    * @outputof SA_CanSetControllerOSBootableDriveCount.
    * @allows SA_SetControllerOSBootableDriveCount.
    */
   #define kCanSetControllerOSBootableDriveCountAllowed              1
   /** The number of OS bootable drives is already set to the desired value.
    * @see SA_GetControllerOSBootableDriveCount.
    * @outputof SA_CanSetControllerOSBootableDriveCount.
    * @disallows SA_SetControllerOSBootableDriveCount.
    */
   #define kCanSetControllerOSBootableDriveCountAlreadySet           2
   /** The controller has an active or pending online firmware activation operation.
    * @see storc_status_ctrl_online_firmware_activation
    * @outputof SA_CanSetControllerOSBootableDriveCount.
    * @disallows SA_SetControllerOSBootableDriveCount.
    */
   #define kCanSetControllerOSBootableDriveCountControllerOFAActive  3
/** @} */ /* Can Set Number of OS Bootable Drives */

/** @name Set Number of OS Bootable Drives
 * @{ */
/** Set the configured number of OS bootable drives.
 * @pre The user has used @ref SA_CanSetControllerOSBootableDriveCount to ensure the operation can be performed.
 * @param[in] cda     Controller data area.
 * @param[in] count   The desired number of OS bootable drives.
 * @return @ref kTrue iff the number of OS bootable drives was set to desired value, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanSetControllerOSBootableDriveCount(cda, 2) == kCanSetControllerOSBootableDriveCountAllowed)
 * {
 *    if (!SA_SetControllerOSBootableDriveCount(cda, 2))
 *    {
 *        fprintf(stderr, "Failed to set number of OS bootable drives.");
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_SetControllerOSBootableDriveCount(PCDA cda, SA_BYTE count);
/** @} */ /* Set Number of OS Bootable Drives */

/** @} */ /* storc_configuration_direct_os_bootable_drive_count */

/** @} */ /* storc_configuration_direct */

/** @defgroup storc_configuration_ctrl_port_discovery Connector Discovery Configuration
 * @brief Configure discovery information for connectors on the controller.
 * @details
 * @pre @ref kControllerChangeConnectorDiscoveryConfigurationSupported is set by @ref SA_GetControllerConnectorDiscoveryConfigurationSupport.
 * See [Connector Discovery Protocol](@ref storc_properties_ctrl_port_discovery_configuration)
 * for a list of supported connector discovery protocols.
 *
 * __Example__
 * @code
 * // Configure discovery configuration for all connectors on a controller.
 * void ConfigureDiscoveryConfigurationForAllConnectors(PCDA cda, SA_BYTE discovery_protocol)
 * {
 *   void *connector_discovery_config_info = NULL;
 *   SA_BYTE result;
 *   SA_WORD connector_index;
 *   SA_BOOL can_update = kFalse;
 *
 *   // Get the connector discovery configuration info for update buffer.
 *   result = SA_GetConnectorDiscoveryConfigurationInfoForUpdate(cda, &connector_discovery_config_info);
 *   if (result != kGetConnectorDiscoveryConfigurationInfoForUpdateSuccess)
 *   {
 *      printf("Failed to get connector discovery configuration info for update buffer (%u)\n", result);
 *      return kFalse;
 *   }
 *
 *   // Set the requested discovery protocol for all connectors in connector discovery
 *   // protocol info for update buffer.
 *   FOREACH_CONNECTOR(cda, connector_index)
 *   {
 *      if (SA_DoChangeConnectorDiscoveryProtocol(connector_discovery_config_info, connector_index, discovery_protocol) != kTrue)
 *      {
 *         result = SA_CanChangeConnectorDiscoveryProtocol(connector_discovery_config_info, connector_index, discovery_protocol);
 *         printf("Cannot change connector #%u discovery protocol to %u (Reason: %u)\n",
 *            connector_index, discovery_protocol, result);
 *         continue;
 *      }
 *
 *      // Set can_update to kTrue since we were able to set discovery protocol
 *      // for at least one connector in connector discovery protocol info for
 *      // update buffer.
 *      can_update = kTrue;
 *   }
 *
 *   // Actually perform the update to apply connector discovery configuration info.
 *   if (SA_UpdateConnectorDiscoveryConfigurationInfo(connector_discovery_config_info) != kTrue)
 *   {
 *      printf("Failed to update connector discovery configuration\n");
 *   }
 *
 *   // Destroy the connector discovery configuration info for update buffer.
 *   SA_DestroyConnectorDiscoveryConfigurationInfoForUpdate(connector_discovery_config_info);
 * }
 * @endcode
 * @{ */

/** Check if connector's discovery protocol can be changed to the requested discovery protocol.
 * @pre `connector_index` references an existing connector. Make sure connector index is in the range of
 *      [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @pre @ref kControllerChangeConnectorDiscoveryConfigurationSupported is set by @ref SA_GetControllerConnectorDiscoveryConfigurationSupport.
 * @post If valid, user may call @ref SA_DoChangeConnectorDiscoveryProtocol.
 * @param[in] connector_discovery_config_info    Connector discovery protocol info for update object created by @ref SA_GetConnectorDiscoveryConfigurationInfoForUpdate.
 * @param[in] connector_index                    Target connector on the controller.
 * @param[in] discovery_protocol                 Intended discovery protocol (see [Connector Discovery Protocol](@ref storc_properties_ctrl_port_discovery_configuration)).
 * @return See [Can Change Connector Discovery Protocol Returns](@ref storc_configuration_ctrl_port_discovery).
 */
SA_BYTE SA_CanChangeConnectorDiscoveryProtocol(void* connector_discovery_config_info, SA_WORD connector_index, SA_BYTE discovery_protocol);
   /** @name Can Change Connector Discovery Protocol Returns
    * @{ */
   /** Change connector discovery protocol is allowed.
    * @outputof SA_CanChangeConnectorDiscoveryProtocol.
    * @allows SA_DoChangeConnectorDiscoveryProtocol.
    */
   #define kCanChangeConnectorDiscoveryProtocolAllowed                           0
   /** An invalid @p connector_discovery_config_info argument was given.
    * @outputof SA_CanChangeConnectorDiscoveryProtocol.
    * @disallows SA_DoChangeConnectorDiscoveryProtocol.
    */
   #define kCanChangeConnectorDiscoveryProtocolInvalidConnectorConfigInfo        1
   /** The selected discovery protocol is invalid.
    * See [Connector Discovery Protocol](@ref storc_properties_ctrl_port_discovery_configuration)
    * for a list of supported connector discovery protocols.
    * @outputof SA_CanChangeConnectorDiscoveryProtocol.
    * @disallows SA_DoChangeConnectorDiscoveryProtocol.
    */
   #define kCanChangeConnectorDiscoveryProtocolInvalidProtocol                   2
   /** The controller does not support changing the discovery protocol.
    * Make sure @ref kControllerChangeConnectorDiscoveryConfigurationSupported is set
    * by @ref SA_GetControllerConnectorDiscoveryConfigurationSupport.
    * @outputof SA_CanChangeConnectorDiscoveryProtocol.
    * @disallows SA_DoChangeConnectorDiscoveryProtocol.
    */
   #define kCanChangeConnectorDiscoveryProtocolChangingNotSupported              3
   /** The selected connector is invalid.
    * Make sure @p connector_index is in the range of
    * [0 and output of @ref SA_GetControllerTotalConnectorCount).
    * @outputof SA_CanChangeConnectorDiscoveryProtocol.
    * @disallows SA_DoChangeConnectorDiscoveryProtocol.
    */
   #define kCanChangeConnectorDiscoveryProtocolInvalidConnector                  4
   /** The selected discovery protocol is unsupported by the port.
    * Make sure @p discovery_protocol is supported by the connector using @ref SA_GetConnectorDiscoveryProtocolSupport.
    * @outputof SA_CanChangeConnectorDiscoveryProtocol.
    * @disallows SA_DoChangeConnectorDiscoveryProtocol.
    */
   #define kCanChangeConnectorDiscoveryProtocolUnsupportedByPort                 5
   /** The selected discovery protocol is already pending.
    * @outputof SA_CanChangeConnectorDiscoveryProtocol.
    * @disallows SA_DoChangeConnectorDiscoveryProtocol.
    */
   #define kCanChangeConnectorDiscoveryProtocolAlreadyPending                    6
   /** The requested protocol is not supported on a UBM backplane.
    * @outputof SA_CanChangeConnectorDiscoveryProtocol.
    * @disallows SA_DoChangeConnectorDiscoveryProtocol.
    */
   #define kCanChangeConnectorDiscoveryProtocolConnectorHasUBMBackplane          7
   /** The requested protocol is not supported on a VPP backplane.
    * @outputof SA_CanChangeConnectorDiscoveryProtocol.
    * @disallows SA_DoChangeConnectorDiscoveryProtocol.
    */
   #define kCanChangeConnectorDiscoveryProtocolConnectorHasVPPBackplane          8
   /** The requested protocol requires a valid target count be set.
    * See [Connector Direct-Attached Cable Target Count](@ref storc_configuration_ctrl_port_direct_attach)
    * @outputof SA_CanChangeConnectorDiscoveryProtocol.
    * @disallows SA_DoChangeConnectorDiscoveryProtocol.
    */
   #define kCanChangeConnectorDiscoveryProtocolTargetCountNotSet                 9
   /** @} */ /* Can Change Connector Discovery Protocol Returns */

/** Change port to the requested discovery protocol.
 * @pre The user has used @ref SA_CanChangeConnectorDiscoveryProtocol to ensure the operation can be performed.
 * @pre `connector_index` is a valid connector index. Make sure connector index is in the range of
 *      [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @param[in] connector_discovery_config_info    Connector discovery protocol info for update object created by @ref SA_GetConnectorDiscoveryConfigurationInfoForUpdate.
 * @param[in] connector_index                    Target port number.
 * @param[in] discovery_protocol                 Intended discovery protocol (see [Connector Discovery Protocol](@ref storc_properties_ctrl_port_discovery_configuration)).
 * @return @ref kTrue if connector discovery protocol is changed, @ref kFalse otherwise.
 */
SA_BOOL SA_DoChangeConnectorDiscoveryProtocol(void* connector_discovery_config_info, SA_WORD connector_index, SA_BYTE discovery_protocol);

/** Update connector discovery configuration info
 * @param[in] connector_discovery_config_info   A connector discovery configuration info for update object created using @ref SA_GetConnectorDiscoveryConfigurationInfoForUpdate.
 * @return @ref kTrue/@ref kFalse if command was successful/unsuccessful.
 */
SA_BOOL SA_UpdateConnectorDiscoveryConfigurationInfo(void* connector_discovery_config_info);

/** Destroy connector discovery configuration info for update object.
 * @post Memory at @p connector_discovery_config_info will no longer be valid.
 *
 * @param[in] connector_discovery_config_info A connector discovery configuration info for update object created by @ref SA_GetConnectorDiscoveryConfigurationInfoForUpdate.
 */
void SA_DestroyConnectorDiscoveryConfigurationInfoForUpdate(void* connector_discovery_config_info);

/** Get connector discovery configuration info for update.
 * The following functions need the connector discovery configuration info for update object:
 * - @ref SA_CanChangeConnectorDiscoveryProtocol
 * - @ref SA_DoChangeConnectorDiscoveryProtocol
 * - @ref SA_UpdateConnectorDiscoveryConfigurationInfo
 * - @ref SA_DestroyConnectorDiscoveryConfigurationInfoForUpdate
 * @param[in] cda     Controller data area.
 * @param[in] connector_discovery_config_info   A connector discovery configuration info for update object that can be used in other functions.
 * @return See [Get Connector Discovery Configuration Info For Update Returns] (@ref storc_configuration_ctrl_port_discovery)
 */
SA_BYTE SA_GetConnectorDiscoveryConfigurationInfoForUpdate(PCDA cda, void** connector_discovery_config_info);
   /** @name Get Connector Discovery Configuration Info For Update Returns
    * @{ */
   /** Get Connector Discovery Configuration Info For Update Success.
    * @outputof SA_GetConnectorDiscoveryConfigurationInfoForUpdate
    */
   #define kGetConnectorDiscoveryConfigurationInfoForUpdateSuccess           0
   /** Get Connector Discovery Configuration Info For Update Not Supported.
    * @outputof SA_GetConnectorDiscoveryConfigurationInfoForUpdate
    */
   #define kGetConnectorDiscoveryConfigurationInfoForUpdateNotSupported      1
   /** Get Connector Discovery Configuration Info For Update Out Of Memory.
    * @outputof SA_GetConnectorDiscoveryConfigurationInfoForUpdate
    */
   #define kGetConnectorDiscoveryConfigurationInfoForUpdateOutOfMemory       2
   /** Get Connector Discovery Configuration Info For Update Failed.
    * @outputof SA_GetConnectorDiscoveryConfigurationInfoForUpdate
    */
   #define kGetConnectorDiscoveryConfigurationInfoForUpdateFailed            3
   /** Get Connector Discovery Configuration Info For Update Invalid Argument.
    * @outputof SA_GetConnectorDiscoveryConfigurationInfoForUpdate
    */
   #define kGetConnectorDiscoveryConfigurationInfoForUpdateInvalidArgument   4
   /** @} */ /* Get Connector Discovery Configuration Info For Update Returns */

/** Reset connector's discovery configuration to its default configuration.
 * @attention @ref SA_UpdateConnectorDiscoveryConfigurationInfo will overwrite the
 *            changes of @ref SA_ResetConnectorDiscoveryConfiguration if connector
 *            discovery configuration info for update buffer has not been destroyed
 *            using @ref SA_DestroyConnectorDiscoveryConfigurationInfoForUpdate
 * @pre @p connector_index references an existing connector. Make sure connector
 *       index is in the range of [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @pre @ref kControllerChangeConnectorDiscoveryConfigurationSupported is set by @ref SA_GetControllerConnectorDiscoveryConfigurationSupport.
 * @param[in] cda              Controller data area.
 * @param[in] connector_index  Target connector index.
 * @return See [Reset Connector Discovery Configuration Returns] (@ref storc_configuration_ctrl_port_discovery)
 */
SA_BYTE SA_ResetConnectorDiscoveryConfiguration(PCDA cda, SA_WORD connector_index);
   /** @name Reset Connector Discovery Configuration Returns
    * @outputof SA_GetConnectorDiscoveryProtocolSupport.
    * @{ */
   /** Successfully reset connector's discovery configuration to default.
    * @outputof SA_ResetConnectorDiscoveryConfiguration.
    */
   #define kResetConnectorDiscoveryConfigurationSuccess               0
   /** Command is not supported.
    * @ref kControllerChangeConnectorDiscoveryConfigurationSupported is set by @ref SA_GetControllerConnectorDiscoveryConfigurationSupport.
    * @outputof SA_ResetConnectorDiscoveryConfiguration.
    */
   #define kResetConnectorDiscoveryConfigurationNotSupported          1
   /** Invalid connector index.
    * Make sure @p connector_index is in the range of [0 and output of @ref SA_GetControllerTotalConnectorCount).
    * @outputof SA_ResetConnectorDiscoveryConfiguration.
    */
   #define kResetConnectorDiscoveryConfigurationInvalidConnectorIndex 2
   /** Failed to reset connector's discovery configuration to default.
    * @outputof SA_ResetConnectorDiscoveryConfiguration.
    */
   #define kResetConnectorDiscoveryConfigurationFailed                3
   /** @} */ /* Reset Connector Discovery Configuration Returns */
/** @} */ /* storc_configuration_ctrl_port_discovery */

/** @defgroup storc_configuration_ctrl_port_direct_attach Connector Direct-Attached Cable Target Count
 * @brief Configure the connector's target count for direct-attached cable protocol
 * @details
 * @pre @ref kConnectorSupportsDiscoveryProtocolDirectAttachedCable is set by @ref SA_GetConnectorDiscoveryProtocolSupport.
 * See [Connector Discovery Protocol](@ref storc_properties_ctrl_port_discovery_configuration)
 * for a list of supported connector discovery protocols.
 *
 * __Example__
 * @code
 * // Configure discovery configuration for all connectors on a controller.
 * // Set all ports to direct-attached cable protocol and selected target count.
 * SA_BOOL SetConnectorToDirectAttachedCableProtocolWithTargetCount(PCDA cda, SA_BYTE target_count)
 * {
 *   void *connector_discovery_config_info = NULL;
 *   SA_BYTE result;
 *   SA_WORD connector_index = 0;
 *   SA_BOOL can_update = kFalse;
 *   SA_BOOL count_supported = 0;
 *
 *   // Get the connector discovery configuration info for update buffer.
 *   result = SA_GetConnectorDiscoveryConfigurationInfoForUpdate(cda, &connector_discovery_config_info);
 *   if (result != kGetConnectorDiscoveryConfigurationInfoForUpdateSuccess)
 *   {
 *      printf("Failed to get connector discovery configuration info for update buffer (%u)\n", result);
 *      return kFalse;
 *   }
 *
 *   // Set the direct-attached cable discovery protocol and target count to all connectors
 *   // in discovery protocol info for update buffer.
 *   FOREACH_CONNECTOR(cda, connector_index)
 *   {
 *      // Set the direct-attached cable discovery protocol
 *      if (SA_DoChangeConnectorDiscoveryProtocol(connector_discovery_config_info, connector_index, kConnectorSupportsDiscoveryProtocolDirectAttachedCable) != kTrue)
 *      {
 *         result = SA_CanChangeConnectorDiscoveryProtocol(connector_discovery_config_info, connector_index, kConnectorSupportsDiscoveryProtocolDirectAttachedCable);
 *         printf("Cannot change connector #%u discovery protocol to direct-attached cable protocol (Reason: %u)\n",
 *            connector_index, result);
 *         continue;
 *      }
 *
 *      // Set the target count
 *      if (SA_DoChangeConnectorDirectAttachTargetCount(connector_discovery_config_info, connector_index, target_count) != kTrue)
 *      {
 *         result = SA_CanChangeConnectorDirectAttachTargetCount(connector_discovery_config_info, connector_index, target_count);
 *         printf("Cannot change connector #%u target count to %u (Reason: %u)\n",
 *            connector_index, target_count, result);
 *         if(result == kCanChangeConnectorDirectAttachTargetCountNotSupportedCount)
 *         {
 *            count_supported = SA_GetConnectorDirectAttachTargetCountSupport(pcda, connector_index);
 *            printf("The supported target count are: %s %s %s %s\n",
 *               CONNECTOR_SUPPORTS_ONE_DIRECT_ATTACHED_CABLE_TARGET(count_supported) ? "1": "",
 *               CONNECTOR_SUPPORTS_TWO_DIRECT_ATTACHED_CABLE_TARGETS(count_supported) ? "2": "",
 *               CONNECTOR_SUPPORTS_FOUR_DIRECT_ATTACHED_CABLE_TARGETS(count_supported) ? "4": "",
 *               CONNECTOR_SUPPORTS_EIGHT_DIRECT_ATTACHED_CABLE_TARGETS(count_supported) ? "8": "")
 *         }
 *         continue;
 *      }
 *
 *      // Set can_update to kTrue since we were able to set at least 1 connector for
 *      // update buffer.
 *      can_update = kTrue;
 *   }
 *
 *   // Actually perform the update to apply connector discovery configuration info.
 *   if (can_update && SA_UpdateConnectorDiscoveryConfigurationInfo(connector_discovery_config_info) != kTrue)
 *   {
 *      can_update = kFalse;
 *      printf("Failed to update connector discovery configuration\n");
 *   }
 *
 *   // Destroy the connector discovery configuration info for update buffer.
 *   SA_DestroyConnectorDiscoveryConfigurationInfoForUpdate(connector_discovery_config_info);
 *
 *   return can_update;
 * }
 * @endcode
 * @{ */

/** Check if connector's direct-attached target count can be changed.
 * @pre `connector_index` references an existing connector. Make sure connector index is in the range of
 *      [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @pre @ref kControllerChangeConnectorDiscoveryConfigurationSupported is set by @ref SA_GetControllerConnectorDiscoveryConfigurationSupport.
 * @post If valid, user may call @ref SA_DoChangeConnectorDirectAttachTargetCount.
 * @param[in] connector_discovery_config_info    Connector discovery protocol info for update object created by @ref SA_GetConnectorDiscoveryConfigurationInfoForUpdate.
 * @param[in] connector_index                    Target connector on the controller.
 * @param[in] target_count                       Intended target count (see @ref SA_GetConnectorDirectAttachTargetCountSupport).
 * @return See [Can Change Connector Direct-Attached Cable Target Count Returns](@ref storc_configuration_ctrl_port_direct_attach ).
 */
SA_BYTE SA_CanChangeConnectorDirectAttachTargetCount(void* connector_discovery_config_info, SA_WORD connector_index, SA_BYTE target_count);
   /** @name Can Change Connector Direct-Attached Cable Target Count Returns
    * @{ */
   /** Change connector direct-attached cable target count is allowed.
    * @outputof SA_CanChangeConnectorDirectAttachTargetCount.
    * @allows SA_DoChangeConnectorDirectAttachTargetCount.
    */
   #define kCanChangeConnectorDirectAttachTargetCountAllowed                      0
   /** The selected connector is invalid.
    * Make sure @p connector_index is in the range of
    * [0 and output of @ref SA_GetControllerTotalConnectorCount).
    * @outputof SA_CanChangeConnectorDirectAttachTargetCount.
    * @disallows SA_DoChangeConnectorDirectAttachTargetCount.
    */
   #define kCanChangeConnectorDirectAttachTargetCountInvalidConnector             1
   /** The controller does not support changing the selected target count.
    * See @ref SA_GetConnectorDirectAttachTargetCountSupport
    * for a list of supported connector direct-attached cable target count.
    * @outputof SA_CanChangeConnectorDirectAttachTargetCount.
    * @disallows SA_DoChangeConnectorDirectAttachTargetCount.
    */
   #define kCanChangeConnectorDirectAttachTargetCountNotSupportedCount            2
   /** The selected direct-attached cable target count is already pending.
    * @outputof SA_CanChangeConnectorDirectAttachTargetCount.
    * @disallows SA_DoChangeConnectorDirectAttachTargetCount.
    */
   #define kCanChangeConnectorDirectAttachTargetCountAlreadyPending               3
   /** An invalid @p connector_discovery_config_info argument was given.
    * @outputof SA_CanChangeConnectorDirectAttachTargetCount.
    * @disallows SA_DoChangeConnectorDirectAttachTargetCount.
    */
   #define kCanChangeConnectorDirectAttachTargetCountInvalidConnectorConfigInfo   4
   /** @} */ /* Can Change Connector Direct-Attached Cable Target Count Returns */

/** Change port direct-attached cable connection to the requested target count.
 * @pre The user has used @ref SA_CanChangeConnectorDiscoveryProtocol to ensure the operation can be performed.
 * @pre `connector_index` is a valid connector index. Make sure connector index is in the range of
 *      [0 and output of @ref SA_GetControllerTotalConnectorCount).
 * @param[in] connector_discovery_config_info    Connector discovery protocol info for update object created by @ref SA_GetConnectorDiscoveryConfigurationInfoForUpdate.
 * @param[in] connector_index                    Target connector on the controller.
 * @param[in] target_count                       Intended direct-attached cable target count (see @ref SA_GetConnectorDirectAttachTargetCountSupport).
 * @return @ref kTrue if connector direct-attached cable target count is changed, @ref kFalse otherwise.
 *
 * __Example__
 * @code
 * if (SA_CanChangeConnectorDirectAttachTargetCount(connector_discovery_config_info, 1, 2) == kCanChangeConnectorDirectAttachTargetCountAllowed)
 * {
 *    if (SA_DoChangeConnectorDirectAttachTargetCount(connector_discovery_config_info, 1, 2) == kFalse)
 *    {
 *       fprintf(stderr, "Failed to set connector direct-attached cable target count to 2\n");
 *    }
 *    else
 *    {
 *       if (SA_UpdateConnectorDiscoveryConfigurationInfo(connector_discovery_config_info) == kTrue)
 *       {
 *          printf("Successful update of connector discovery configuration info\n");
 *       }
 *       else
 *       {
 *          printf("Failed to update connector discovery configuration info\n");
 *       }
 *    }
 * }
 * @endcode
 */
SA_BOOL SA_DoChangeConnectorDirectAttachTargetCount(void* connector_discovery_config_info, SA_WORD connector_index, SA_BYTE target_count);
/** @} */ /* storc_configuration_ctrl_port_direct_attach */

#if defined(__cplusplus)
}
#endif

/** @} */ /* storc_configuration */

/** @} */ /* storcore */

#endif /* PSTORC_CONFIGURATION_H */
