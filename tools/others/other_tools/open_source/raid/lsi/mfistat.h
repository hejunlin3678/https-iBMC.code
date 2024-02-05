/*
 * MegaRAID MFISTAT Interface Header File
 *
 * Copyright (C) LSI Logic, 2004
 *
 * This header file defines the MFI status completion codes for the  MegaRAID
 * Firmware Interface (MFI).
 *
 */
#ifndef INCLUDE_MFISTAT
#define INCLUDE_MFISTAT

/*
 * define MFI command completion codes
 */
typedef enum _MFI_STAT {
    MFI_STAT_OK                         = 0x00,     /* command completed successfully */
    MFI_STAT_INVALID_CMD                = 0x01,     /* invalid command */
    MFI_STAT_INVALID_DCMD               = 0x02,     /* DCMD opcode is invalid */
    MFI_STAT_INVALID_PARAMETER          = 0x03,     /* input parameters are invalid */
    MFI_STAT_INVALID_SEQUENCE_NUMBER    = 0x04,     /* invalid sequence number */
    MFI_STAT_ABORT_NOT_POSSIBLE         = 0x05,     /* abort isn't possible for the requested command */
    MFI_STAT_APP_HOST_CODE_NOT_FOUND    = 0x06,     /* application 'host' code not found */
    MFI_STAT_APP_IN_USE                 = 0x07,     /* application already in use - try later */
    MFI_STAT_APP_NOT_INITIALIZED        = 0x08,     /* application not initialized */
    MFI_STAT_ARRAY_INDEX_INVALID        = 0x09,     /* given array index is invalid */
    MFI_STAT_ARRAY_ROW_NOT_EMPTY        = 0x0a,     /* unable to add missing drive to array, as row has no empty slots */
    MFI_STAT_CONFIG_RESOURCE_CONFLICT   = 0x0b,     /* some of the CFG resources conflict with each other or the current config */
    MFI_STAT_DEVICE_NOT_FOUND           = 0x0c,     /* invalid device ID / select-timeout */
    MFI_STAT_DRIVE_TOO_SMALL            = 0x0d,     /* drive is too small for requrested operation */
    MFI_STAT_FLASH_ALLOC_FAIL           = 0x0e,     /* flash memory allocation failed */
    MFI_STAT_FLASH_BUSY                 = 0x0f,     /* flash download already in progress */
    MFI_STAT_FLASH_ERROR                = 0x10,     /* flash operation failed */
    MFI_STAT_FLASH_IMAGE_BAD            = 0x11,     /* flash image was bad */
    MFI_STAT_FLASH_IMAGE_INCOMPLETE     = 0x12,     /* downloaded flash image is incomplete */
    MFI_STAT_FLASH_NOT_OPEN             = 0x13,     /* flash OPEN was not done */
    MFI_STAT_FLASH_NOT_STARTED          = 0x14,     /* flash sequence is not active */
    MFI_STAT_FLUSH_FAILED               = 0x15,     /* flush command failed */
    MFI_STAT_HOST_CODE_NOT_FOUND        = 0x16,     /* specified application doesn't have host-resident code */
    MFI_STAT_LD_CC_IN_PROGRESS          = 0x17,     /* LD operation not possibe - CC is in progress */
    MFI_STAT_LD_INIT_IN_PROGRESS        = 0x18,     /* LD initialization in progress */
    MFI_STAT_LD_LBA_OUT_OF_RANGE        = 0x19,     /* LBA is out of range */
    MFI_STAT_LD_MAX_CONFIGURED          = 0x1a,     /* maximum LDs are already configured */
    MFI_STAT_LD_NOT_OPTIMAL             = 0x1b,     /* LD is not OPTIMAL */
    MFI_STAT_LD_RBLD_IN_PROGRESS        = 0x1c,     /* LD Rebuild is in progress */
    MFI_STAT_LD_RECON_IN_PROGRESS       = 0x1d,     /* LD is undergoing reconstruction */
    MFI_STAT_LD_WRONG_RAID_LEVEL        = 0x1e,     /* LD RAID level is wrong for requested operation */
    MFI_STAT_MAX_SPARES_EXCEEDED        = 0x1f,     /* too many spares assigned */
    MFI_STAT_MEMORY_NOT_AVAILABLE       = 0x20,     /* scratch memory not available - try command again later */
    MFI_STAT_MFC_HW_ERROR               = 0x21,     /* error writing MFC data to SEEPROM */
    MFI_STAT_NO_HW_PRESENT              = 0x22,     /* required HW is missing (i.e. Alarm or BBU) */
    MFI_STAT_NOT_FOUND                  = 0x23,     /* item not found */
    MFI_STAT_NOT_IN_ENCL                = 0x24,     /* LD drives are not within an enclosure */
    MFI_STAT_PD_CLEAR_IN_PROGRESS       = 0x25,     /* PD CLEAR operation is in progress */
    MFI_STAT_PD_TYPE_WRONG              = 0x26,     /* unable to use SATA(SAS) drive to replace SAS(SATA) */
    MFI_STAT_PR_DISABLED                = 0x27,     /* Patrol Read is disabled */
    MFI_STAT_ROW_INDEX_INVALID          = 0x28,     /* given row index is invalid */
    MFI_STAT_UNUSED_1                   = 0x29,     /* unused error code - reserved for future use */
    MFI_STAT_UNUSED_2                   = 0x2a,     /* unused error code - reserved for future use */
    MFI_STAT_UNUSED_3                   = 0x2b,     /* unused error code - reserved for future use */
    MFI_STAT_UNUSED_4                   = 0x2c,     /* unused error code - reserved for future use */
    MFI_STAT_SCSI_DONE_WITH_ERROR       = 0x2d,     /* SCSI command done, but non-GOOD status was received-see mf.hdr.extStatus for SCSI_STATUS */
    MFI_STAT_SCSI_IO_FAILED             = 0x2e,     /* IO request for MFI_CMD_OP_PD_SCSI failed - see extStatus for DM error */
    MFI_STAT_SCSI_RESERVATION_CONFLICT  = 0x2f,     /* matches SCSI RESERVATION_CONFLICT */
    MFI_STAT_SHUTDOWN_FAILED            = 0x30,     /* one or more of the flush operations failed */
    MFI_STAT_TIME_NOT_SET               = 0x31,     /* FW real-time currently not set */
    MFI_STAT_WRONG_STATE                = 0x32,     /* command issues while FW in wrong state (i.e. GET RECON when op not active) */
    MFI_STAT_LD_OFFLINE                 = 0x33,     /* LD is not OFFLINE - IO not possible */
    MFI_STAT_PEER_NOTIFICATION_REJECTED = 0x34,     /* peer controller rejected request (possibly due to resource conflict) */
    MFI_STAT_PEER_NOTIFICATION_FAILED   = 0x35,     /* unable to inform peer of communication changes (retry may be appropriate) */
    MFI_STAT_RESERVATION_IN_PROGRESS    = 0x36,     /* LD reservation already in progress */
    MFI_STAT_I2C_ERRORS_DETECTED        = 0x37,     /* I2C errors were detected */
    MFI_STAT_PCI_ERRORS_DETECTED        = 0x38,     /* PCI errors occurred during XOR/DMA operation */
    MFI_STAT_DIAG_FAILED                = 0x39,     /* diagnostics failed - see event log for details */
    MFI_STAT_BOOT_MSG_PENDING           = 0x3a,     /* unable to process command as boot messages are pending */
    MFI_STAT_FOREIGN_CONFIG_INCOMPLETE  = 0x3b,     /* Returned in case if foreign configurations are imcomplete */
    MFI_STAT_INVALID_SGL                = 0x3c,     /* malformed SGL - counts don't match hdr.length (only returned if debug code enabled) */
    MFI_STAT_UNSUPPORTED_HW             = 0x3d,     /* Returned in case if a command is tried on unsupported hardware */
    MFI_STAT_CC_SCHEDULE_DISABLED       = 0x3e,     /* CC scheduling is disabled */
    MFI_STAT_PD_COPYBACK_IN_PROGRESS    = 0x3f,     /* PD CopyBack operation is in progress */
    MFI_STAT_MULTIPLE_PDS_IN_ARRAY      = 0x40,     /* selected more than one PD per array */
    MFI_STAT_FW_DOWNLOAD_ERROR          = 0x41,     /* microcode update operation failed */
    MFI_STAT_FEATURE_SECURITY_NOT_ENABLED = 0x42,   /* unable to process command as drive security feature is not enabled */
    MFI_STAT_LOCK_KEY_ALREADY_EXISTS    = 0x43,     /* controller already has a lock key */
    MFI_STAT_LOCK_KEY_BACKUP_NOT_ALLOWED= 0x44,     /* lock key cannot be backed-up */
    MFI_STAT_LOCK_KEY_VERIFY_NOT_ALLOWED= 0x45,     /* lock key backup cannot be verified */
    MFI_STAT_LOCK_KEY_VERIFY_FAILED     = 0x46,     /* lock key from backup failed verification */
    MFI_STAT_LOCK_KEY_REKEY_NOT_ALLOWED = 0x47,     /* rekey operation not allowed, unless controller already has a lock key */
    MFI_STAT_LOCK_KEY_INVALID           = 0x48,     /* lock key is not valid, cannot authenticate */
    MFI_STAT_LOCK_KEY_ESCROW_INVALID    = 0x49,     /* lock key from escrow cannot be used */
    MFI_STAT_LOCK_KEY_BACKUP_REQUIRED   = 0x4a,     /* lock key backup (pass-phrase) is required */
    MFI_STAT_SECURE_LD_EXISTS           = 0x4b,     /* secure LD exist */
    MFI_STAT_LD_SECURE_NOT_ALLOWED      = 0x4c,     /* LD secure operation is not allowed */
    MFI_STAT_REPROVISION_NOT_ALLOWED    = 0x4d,     /* reprovisioning is not allowed */
    MFI_STAT_PD_SECURITY_TYPE_WRONG     = 0x4e,     /* drive security type (FDE or non-FDE) is not appropriate for requested operation */
    MFI_STAT_LD_ENCRYPTION_TYPE_INVALID = 0x4f,     /* LD encryption type is not supported */
    MFI_STAT_CONFIG_FDE_NON_FDE_MIX_NOT_ALLOWED         = 0x50, /* cannot mix FDE and non-FDE drives in same array */
    MFI_STAT_CONFIG_LD_ENCRYPTION_TYPE_MIX_NOT_ALLOWED  = 0x51, /* cannot mix secure and unsecured LD in same array */
    MFI_STAT_SECRET_KEY_NOT_ALLOWED     = 0x52,     /* secret key not allowed */
    MFI_STAT_PD_HW_ERRORS_DETECTED      = 0x53,     /* Physical device errors were detected */
    MFI_STAT_LD_CACHE_PINNED            = 0x54,     /* Controller has LD cache pinned */
    MFI_STAT_POWER_STATE_SET_IN_PROGRESS = 0x55,    /* Requested operation is already in progress */
    MFI_STAT_POWER_STATE_SET_BUSY       = 0x56,     /* Another power state set operation is in progress */
    MFI_STAT_POWER_STATE_WRONG          = 0x57,     /* Power state of device is not correct */
    MFI_STAT_PR_NO_AVAILABLE_PD_FOUND   = 0x58,     /* No PD is available for patrol read */
    MFI_STAT_CTRL_RESET_REQUIRED        = 0x59,     /* Controller resert is required */
    MFI_STAT_LOCK_KEY_EKM_NO_BOOT_AGENT = 0x5a,     /* No EKM boot agent detected */
    MFI_STAT_SNAP_NO_SPACE              = 0x5b,     /* No space on the snapshot repositiry VD */
    MFI_STAT_SNAP_PARTIAL_FAILURE       = 0x5c,     /* For consistency SET PiTs, some PiT creations may fail and some succeed */
    MFI_STAT_UPGRADE_KEY_INCOMPATIBLE   = 0x5d,     /* Secondary iButton cannot be used and is incompatible with controller */
    MFI_STAT_PFK_INCOMPATIBLE           = 0x5e,     /* PFK doesn't match or cannot be applied to the controller */
    MFI_STAT_PD_MAX_UNCONFIGURED        = 0x5f,     /* maximum allowed unconfigured (configurable) PDs exist */
    MFI_STAT_IO_METRICS_DISABLED        = 0x60,     /* IO metrics are not being collected */
    MFI_STAT_AEC_NOT_STOPPED            = 0x61,     /* AEC capture need to be stopped before proceeding */
    MFI_STAT_PI_TYPE_WRONG              = 0x62,     /* Unsupported level of protection information */
    MFI_STAT_LD_PD_PI_INCOMPATIBLE      = 0x63,     /* PDs in LD have incompatible EEDP types */
    MFI_STAT_PI_NOT_ENABLED             = 0x64,     /* Request cannot be completed because protection information is not enabled */
    MFI_STAT_LD_BLOCK_SIZE_MISMATCH     = 0x65,     /* PDs in LD have different block sizes */
    MFI_STAT_LD_SSCD_CACHE_PRESENT      = 0x66,     /* LD Cached data is present on a (this) SSCD */
    MFI_STAT_CONFIG_SEQ_MISMATCH        = 0x67,     /* config sequence number mismatch */
    MFI_STAT_FLASH_UNSUPPORTED          = 0x68,     /* flash image is not supported */
    MFI_STAT_RESETNOW_NOT_ALLOWED       = 0x69,     /* controller cannot be online-reset */
    MFI_STAT_HSM_MODE                   = 0x6a,     /* controller booted to safe mode, command is not supported in this mode */
    MFI_STAT_SSC_MEMORY_NOT_AVAILABLE   = 0x6b,     /* SSC memory is unvailable to complete the operation */
    MFI_STAT_PEER_INCOMPATIBLE          = 0x6c,     /* Peer node is incompatible */
    MFI_STAT_DEDICATED_SPARE_NOT_ALLOWED = 0x6d,    /* Dedicated hot spare assignment is limited to array(s) with same LDs. */
    MFI_STAT_FLASH_SIGNED_COMPONENT_NOT_PRESENT = 0x6e,   /* Signed component is not part of the image */
    MFI_STAT_FLASH_AUTHENTICATION_FAILURE = 0x6f,   /* Authentication failure of the signed firmware image */
    MFI_STAT_FLASH_OK_RESTART_NOT_REQUIRED = 0x70,  /* FLASHing was ok, but FW restart is not required, ex: No change in FW from current */
    MFI_STAT_RESTRICTED_MODE            = 0x71,     /* FW is in some form of restricted mode, example: passive in A/P HA mode */
    MFI_STAT_EXCEED_MAX_SUPPORTED_COUNT = 0x72,     /* exceed number of entries */
    MFI_STAT_SSC_FLUSH_ALREADY_RUNNING  = 0x73,     /* Subsequent flush can't start because previous flush is active*/
    MFI_STAT_OK_REBOOT_REQUIRED         = 0x74,     /* Status ok but reboot is required to take effect */
    MFI_STAT_BGOP_IN_PROGRESS           = 0x75,     /* Operation can't perform due to background operation in progress */
    MFI_STAT_OPERATION_NOT_POSSIBLE     = 0x76,     /* Operation is not possible */
    MFI_STAT_FW_FLASH_IN_PROGRESS_ON_PEER = 0x77,   /* FW update is in progress on the peer node */
    MFI_STAT_LD_PARTIALLY_HIDDEN        = 0x78,     /* Hidden policy not set for all LDs in Drive Group containing this LD */
    MFI_STAT_SECURE_SYSTEM_PD_EXISTS    = 0x79,     /* Indicates that there are one or more secure system drives in the system */
    MFI_STAT_LD_BOOT_DEVICE             = 0x7A,     /* Boot LD can't be hidden */
    MFI_STAT_EXCEED_MAX_TRANSPORT_LD_COUNT             = 0x7B,     /* LD count > max transportable LD count */
    MFI_STAT_DHSP_MULTIPLE_ASSOCIATION  = 0x7C,     /* DHSP is associated with more than one disk group - force is needed if dcmd.mbox.b[5] is 0 */
    MFI_STAT_TRANSPORT_READY_LD_EXISTS  = 0x7D,     /* Operation not possible since the configuration has some LD in transport ready state */
    MFI_STAT_SCSI_DATA_UNDERRUN         = 0x7E,     /* IO request encountered a SCSI DATA UNDERRUN, MFI_HDR.length is set to bytesTransferred */
    MFI_STAT_FLASH_NOT_ALLOWED          = 0x7F,     /* FW Flash not allowed in the current mode */
    MFI_STAT_PD_IN_TRANSPORT_READY_STATE = 0x80,    /* Operation not possible since the device is in transport ready state */
    MFI_STAT_LD_IN_TRANSPORT_READY_STATE = 0x81,    /* Operation not possible since the LD is in transport ready state */
    MFI_STAT_LD_NOT_IN_TRANSPORT_READY_STATE = 0x82,/* Operation not possible since the LD is not in transport ready state */
    MFI_STAT_PD_READY_FOR_REMOVAL       = 0x83,     /* Operation not possible since the PD in removal ready state */
    MFI_STAT_HOST_REBOOT_REQUIRED       = 0x84,     /* Status ok but host reboot is required for changes to take effect */
    MFI_STAT_MICROCODE_UPDATE_PENDING   = 0x85,     /* Microcode update is pending on the device */
    MFI_STAT_MICROCODE_UPDATE_IN_PROGRESS = 0x86,   /* Microcode update is in progress on the device*/
    MFI_STAT_PD_ERASE_TYPE_WRONG        = 0x87,     /* Mismatch between the drive type and the erase option */
    MFI_STAT_AUTOCONFIG_EXISTS          = 0x88,     /* Operation not possible since the automatic created configuration exists */
    MFI_STAT_SECURE_DEVICE_EXISTS       = 0x89,     /* secure device exist - EPD or EPD-PASSTHRU */
    MFI_STAT_FRU_INVALID_HOST_DATA      = 0x8A,     /* Operation not possible because host FRU data is invalid */
    MFI_STAT_FRU_INVALID_CTRL_DATA      = 0x8B,     /* Operation not possible because controller FRU data is invalid */
    MFI_STAT_IMAGE_NOT_FOUND            = 0x8C,     /* Requested image not found */
    MFI_STAT_NVCACHE_ERROR              = 0x8D,     /* NVCache related error */
    MFI_STAT_LD_SIZE_LESS_THAN_MINIMUM_LIMIT = 0x8E,/* Requested LD size is less than MINIMUM SIZE LIMIT */
    MFI_STAT_INVALID_DRIVE_COUNT        = 0x8F,     /* Requested drive count is invalid for this raid level */
    MFI_STAT_SC_AUTH_FAILURE            = 0x90,     /* OEM specific backplane authentication failure */
    MFI_STAT_SC_NOT_FOUND               = 0x91,     /* OEM specific backplane not found */
    MFI_STAT_FLASH_SAME_IMAGE           = 0x92,     /* Flashing image is not possible because downloaded and running firmware on the controller are same */
    MFI_STAT_UNMAP_NOT_SUPPORTED        = 0x93,     /* Unmap is not supported on the device*/
    MFI_STAT_PD_SANITIZE_TYPE_UNSUPPORTED  = 0x94,  /* Device doesn't support the sanitize type thats specified */
    MFI_STAT_SNAPDUMP_NOT_AVAILABLE     = 0x95,     /* In case there is no valid snap dump available */
    MFI_STAT_SNAPDUMP_NOT_ENABLED       = 0x96,     /* In case snapdump feature is not enabled */
    MFI_STAT_INVALID_POLICY             = 0x97,     /* LD/device does not support the requested policy */
    MFI_STAT_CONFIG_PRESENT_ERROR       = 0x98,     /* Requested operation cannot be performed because of existing configuration */
    MFI_STAT_OK_SHUTDOWN_REQUIRED       = 0x99,     /* Status ok but shutdown is required to take effect */
    MFI_STAT_PD_NOT_ELIGIBLE_FOR_LD     = 0x9A,     /* PD cannot participate in RAID Configuration */
    MFI_STAT_SECURE_BOOT_KEY_SLOTS_FULL = 0x9B,     /* Secure boot needs another key slot and the efuse is full */
    MFI_STAT_SNAPDUMP_CLEAR_REQUIRED    = 0x9C,     /* Clear snapdump before proceeding */
    MFI_STAT_CONFIG_UNMAP_AND_NON_UNMAP_MIX_NOT_ALLOWED   = 0x9D,     /* Cannot mix unmap and non-unmap physical drives in same array */

    MFI_STAT_INVALID_STATUS             = 0xFF      /* invalid status - used for polling command completion */
} MFI_STAT;

#endif /* INCLUDE_MFISTAT */
