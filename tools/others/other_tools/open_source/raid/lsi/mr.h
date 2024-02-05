/*
 * MR.H - LSI Logic MegaRAID Firmware API Header File
 *
 * © 2004-2018 Broadcom.  All Rights Reserved.  Broadcom Confidential
 * The term “Broadcom” refers to Broadcom Inc. and/or its subsidiaries.
 *
 * Revision History:
 *
 * Date      Initials  Change Description
 * --------  --------  --------------------------------------------------------------------
 * 10/11/04  SB/GES    Initial development version
 *
 * 11/02/04  Basu      Incorporated review comments and also opcode headers, return codes defined,
 *                     structure alignments
 *
 * 11/17/04  Basu/GES  Return code updates, Minor updates in structures, Patrol read dcmd changes, structure
 *                     alignments, re-order the opcodes, BBU, MFC, re-order structures..etc
 *                     Cluster and Diagnostic opcodes still TBD.
 *                     Ver 1.0
 *
 * 12/07/04  GES       1. Added interrupt throttling fields to MR_CTRL_PROP structure.  Warning: field order has changed.
 *                     2. Updated MR_DCMD_PD_MISSING_REPLACE comments to use Array Reference rather than Array Index.
 *                     3. Increased count of back-end SAS ports from 4 to 8 in MR_CTRL_INFO structure.
 *                     4. Fixed size/alignment problem with MR_CTRL_INFO structure.
 *
 * 12/10/04  GES       1. Added new exit code (MFI_STAT_APP_NOT_INITIALIZED) for some MR_DCMD_APP_* opcodes.
 *                     2. Fixed declaration of struct _MR_DCMD_APP_INFO.
 *
 * 12/21/04  GES       1. Numerous formatting, grammar, and spelling corrections.
 *                     2. Added mbox.b[0] field to MR_DCMD_CTRL_SHUTDOWN command to inform FW is drives should be
 *                        spun down based on controller settings.  When the user is shutting down the system, this
 *                        field should be set. When hibernating or restarting, this field should be clear.
 *                     3. Changed opcode values for MR_DCMD_CTRL_TIME_GET and MR_DCMD_CTRL_TIME_SET.
 *                     4. Added new opcodes MR_DCMD_CTRL_TIME_SECS_GET and MR_DCMD_CTRL_TIME_SECS_SET to set
 *                        controller time using seconds past 1/1/00.
 *                     5. In MR_CTRL_PROP structure, made the following changes:
 *                         a. Renamed "intThrottleTime" to "intThrottleTimeUs" to reflect units.
 *                         b. Removed "targetId" field, as it is not meaningful for SAS.
 *                         c. Added "alarmEnable" field to allow enabling/disabling of alarm.
 *                     6. In MR_CTRL_INFO structure, made the following changes:
 *                         a. Added "addr" field to "hostInterface" seciton of MR_CTRL_INFO structure and
 *                            adjusted padding accordingly.
 *                         b. Redefined fields on pdMixSupported structure to better reflect actual firmware
 *                            usage/limitations.
 *                     7. Renamed BOOT_MESSAGE_DATA to MR_BOOT_MESSAGE_DATA.
 *                     8. In MR_PD_ADDRESS, changed sasAddr from U8[8] to U64 for easier usage.
 *                     9. In MR_PD_LIST, added "size" field to indicate the total possible structure size
 *                        (simplifies applciation logic).
 *                     10. In MR_PD_INFO, changed pathInfo.sasAddr from U8[8] to U64.
 *                     11. In MR_CONFIG_DATA, renamed initial "count" field to "size" to better reflect its purpose.
 *                     12. In MR_ENCL_LIST, swapped order of "count" and "size" fields to place "size" first for
 *                         simplified application usage.
 *                     13. Renamed MR_DCMD_APP_EXIT_NEED_INFO to MR_DCMD_APP_EXIT_NEED_INPUT.
 *                     14. Uppdated MR_DCMD_APP_EXEC_RESULTS structure to use U16 fields rather than U32 so the
 *                         complete structure will fit in a 32-bit word.
 *                     15. Renamed MR_BIOS_PRIVATE_DATA to MR_BIOS_DATA.  Added field definitions for this structure.
 *                     16. In MR_MFC_DEFAULTS, made the following changes:
 *                         a. Changed sasAddr from U8[8] to U64.
 *                         b. Renamed "rebuildRate" to "backgroundRate".
 *                         c. Removed "autoHotSpareRestore".
 *                         d. Added "spindownMode"
 *
 * 12/22/04  GES       1. Increase size of "logicalVPDPage83" in MR_LD_INFO.
 *                     2. Changed meaning of dcmd.mbox.w[1] for MR_DCMD_CTRL_EVENT_GET; description updated accordingly.
 *
 * 12/23/04  GES       1. Correct various typos.
 *                     2. Add new exit code to MR_DCMD_CTRL_ALARM_GET for no HW present.
 *                     3. Removed MR_DCMD_CTRL_EVENT_GET_TIME opcode.
 *                     4. Added MR_DCMD_CTRL_EVENT_COUNT opcode.
 *                     5. Updated MR_DCMD_CTRL_TTY_READ command to return structure with offset/size data.
 *                     6. Updated documentation for MR_DCDM_CTRL_ENV_GET_DATA command.
 *                     7. Renamed MR_DCMD_CTRL_BIOS_PRIVATE_DATA_GET to MR_DCMD_CTRL_BIOS_DATA_GET.
 *                     8. Renamed MR_DCMD_CTRL_BIOS_PRIVATE_DATA_SET to MR_DCMD_CTRL_BIOS_DATA_SET.
 *                     9. Updated usage of MR_DCMD_CTRL_BIOS_BOOT_MSG_* opcodes.
 *                     10. Renamed MR_DCMD_CTRL_FLUSH_CACHE to MR_DCMD_CTRL_CACHE_FLUSH.
 *                     11. Added MR_ENV_DATA structure.
 *                     12. Updated field locations and names in MR_CTRL_INFO structure (deviceInterface and
 *                         hostInterface structures).
 *                     13. Updated MR_BOOT_MESSAGE_DATA structure fields.
 *                     14. Added MR_BOOT_ACTION enum.
 *                     15. Changed MR_PD_STATE names and values.
 *                     16. Updated MR_ARRAY structure to use MR_PD_REF rather than pdId.
 *                     17. Removed MR_PD_REF array from MR_CONFIG_DATA structure, as it is no longer needed
 *                         because MR_PD_REF is not included in the MR_ARRAY structure.
 *                     18. Updated comments in MR_MFC_DEFAULTS structure to provide information in meanings.
 *                     19. Added MR_TTY_DATA structure.
 *
 * 01/10/04  GES       1. Added MAX_TARGET_ID macro for maximum allowed LD target ID
 *                     2. Removed MR_DCMD_CTRL_PR_PAUSE and MR_DCMD_CTRL_PR_RESUME opcodes.
 *                     3. Added new MFI_STAT_PD_CLEAR_IN_PROGRESS exit code to MR_DCMD_PD_CLEAR_START,
 *                        MR_DCMD_CFG_ADD, and MR_DCMD_CFG_MAKE_SPARE opcodes.
 *                     4. Added new error codes to MR_DCMD_PD_MISSING_REPLACE
 *                     5. Renamed MFI_STAT_DEVICE_TYPE_WRONG to MFI_STAT_PD_TYPE_WRONG.
 *                     6. Removed diskWriteCacheDisable policy from MR_CTRL_PROP .
 *                     7. Clarified definition of currentFwTime in MR_CTRL_INFO.
 *                     8. Added allowMixedRedundancyOnArray field to MR_CTRL_INFO.adapterOperations.
 *                     9. Converted MR_PROGRESS into a union with previous fields in an unnamed structure.  Added
 *                        MR_PROGRESS.w tag for managing as a 32-bit quantity.
 *                     10. Converted MR_PD_REF into a union with previous fields in an unnamed structure.  Added
 *                         MR_PD_REF.ref tag for managing as a 32-bit quantity.
 *                     11. Added MR_ prefix to PD_STATE_* enums.
 *                     12. Converted MR_LD_REF into a union with previous fields in an unnamed structure.  Added
 *                         MR_LD_REF.ref tag for managing as a 32-bit quantity.
 *                     12. Added definitions for LD cache and access policy settings.
 *                     13. Updated MR_LD_PROPERTIES structure fields and definitions.l
 *                     14. Updated MR_LD_PARAMETERS structure - changed state definition.
 *                     15. Added size field to MR_ARRAY.  Also added fwState to PD info for utility usage.
 *                     16. Added new MR_DCMD_APP_EXIT_OUTPUT_FLUSH enum to MR_DCMD_APP_EXIT.
 *
 * 01/14/04  GES        1. Added history comments from Clear Case
 *                      2. Added spindown option to MR_DCMD_CTRL_HIBERNATE_STANDBY.
 *                      3. Updated MR_DCMD_CTRL_FLASH_FW_FLASH to return progress during flash operation.
 *                      4. Removed MR_DCMD_CTRL_FLASH_FW_PROGRESS opcode.
 *                      5. Updated MFI_STAT_* completion codes for MR_DCMD_CTRL_FLASH_FW_FLASH opcode.
 *                      6. Updated descripion of MR_DCMD_PD_MISSING_MARK error description for MFI_STAT_WRONG_STATE
 *                         error code.
 *                      7. Updated MR_DCMD_PD_MISSING_REPLACE description to reflect new error code MFI_STAT_WRONG_STATE.
 *                      8. Added new exit code MFI_STAT_MEMORY_NOT_AVAILABLE to MR_DCMD_CFG_READ.
 *                      9. Added new exit codes to MR_DCMD_CFG_ADD:  MFI_STAT_INVALID_SEQUENCE_NUMBER,
 *                         MFI_STAT_MEMORY_NOT_AVAILABLE, MFI_STAT_WRONG_STATE, MFI_STAT_DEVICE_NOT_FOUND,
 *                         MFI_STAT_DRIVE_TOO_SMALL.
 *                     10. Updated description of MR_DCMD_CFG_ADD to provide more detail.
 *                     11. Defined new constant MR_PD_INVALID to be used for missing/invalid PD Device IDs.
 *                     12. Added LD state to returned data for MR_LD_LIST.
 *                     13. Defined new constants MR_PD_CACHE_MAX, MR_LD_CACHE_MASK, and MR_LD_ACCESS_MASK
 *                         to assist in structure validation.
 *                     14. Increased size of reserved area in MR_CONFIG_DATA to fix an alignment problem.
 *
 * 02/03/05  GES        1. Added MAX_ENCLOSURES macro.
 *                      2. Renamed MR_DCMD_SAS_CONFIG_READ to MR_DCMD_CONFIG_PAGE_READ_PASTHROUGH to match comments.
 *                      3. Added new typedef MR_FLASH_COMP_INFO to describe flash components in MR_CTRL_INFO.
 *                      4. Fixed sizes in comment fields of certain typedefs.
 *                      5. Added MR_PD_SPEED enum definition to describe device speeds.
 *                      6. In MR_PD_INFO, renamed "speed" to "deviceSpeed", and added "linkSpeed" field.
 *                      7. In MR_PD_INFO, removed "pdState" structure from "ddf" union, as information is already
 *                         conveyed in PD state.
 *                      8. In MR_LD_PROPERTIES, renamed "cachePolicy: to "defaultCachePolicy", and added new
 *                         "currentCachePolicy", and "noBGI" fields.
 *                      9. Updated MR_ENCL_LIST to use a static size, rather than variable.
 *                     10. In MR_MFC_DEFAULTS, added two new fields: "disableCtrlR", and "enableWebBios".
 *
 * 02/08/05  GES        1. Increase MAX_SPARES_FOR_THE_CONTROLLER to equal MAX_PHYSICAL_DEVICES.
 *                      2. Added MR_DCMD_CFG_RESIZE_ARRAY opcode.
 *                      3. Updated BBU opcodes and structures.
 *                      4. Removed MR_DCMD_CONFIG_PAGE_READ_PASSTHROUGH opcode and replaced with
 *                         MR_DCMD_CONFIG_GET_PHY_CONNECTIONS.  Also added supporting structures.
 *                      5. Renamed MR_LD_CACHE_WRITE_CACHE_BBU to MR_LD_WRITE_CACHE_BAD_BBU.
 *                      6. Added structure MR_LD_CACHE_STRUCT as alternate method to reference cache policy.
 *                      7. Added MR_RECON_OP to specify reconstruction opcodes.
 *                      8. Updated MR_RECON structure to use MR_PD_REF rather than only device id.
 *                      9. Removed 'isActive' field from MR_SPARE structure.
 *                     10. Moved 'arrayCount' field in MR_SPARE structure.
 *
 * 02/14/05  GES        1. Added MAX_LDS_PER_ARRAY macros to define limit for LDs allowed in one array.
 *                      2. Fixed equation in comment for progress calculaiton in MR_PROGRESS.
 *                      3. Fixed comment in MR_BIOS_DATA to clarify definition of checksum.
 *                      4. Fixed MR_BBU_STATUS to be a struct rather than a union.
 * 03/07/05  GES/BH     1. Fixed mbox[0]=reserved for MR_DCMD_SAS_CONFIG_GET_PHY_CONNECTIONS
 *                      2. Added MFI_STAT_PEER_NOTIFICATION_REJECTED - peer controller rejected request
 *                         (possibly due to resource conflict)
 *                      3. Added MFI_STAT_PEER_NOTIFICATION_FAILED   - unable to inform peer of communication
 *                      4. Added MR_DCMD_APP_GET_MEM_REQUIREMENTS
 *                      5. MR_DCMD_APP_LOAD updates to mbox
 *                      6. Added MR_DCMD_CLUSTER_RESET_ALL and MR_DCMD_CLUSTER_RESET_LD
 *                      7. MR_CTRL_PROP structure update - field disableAutoRebuild added
 *                      8. MR_PD_INFO structure update: Changed connectedAdapterPort to connectedPortBitmap
 *                         bitmap of ports on which this device is connected (bit 0=port 0, bit 1=port 1, etc).
 *                      9. Defined helper macros (MR_LD_CACHE_POLICY_xxx) for traditional MegaRAID cache policies
 *                     10. MR_BBU_PROPERTIES struct updates - Added learnDelayInterval field
 * 03/22/05 GES/BH      1. Corrected comments for various commands
 *                      2. MR_ARRAY - tag for the array (may change when logical drives are deleted)
 *                      3. MR_BIOS_DATA - added field to expose all or 8 ld's to the system
 *                      4. MR_MFC_DEFAULTS - added physplit polarity field
 *
 * 04/21/05 GES/BH      1. maxStripsPerIo field added in ctrlInfo. This will tell the drivers as to how much the max I/O size
 *                         can be.
 *                      2. Added field reconstructActive to MR_LD_INFO
 *                      3. Changed comments for spanDepth,
 *                      4. Typo changes
 *
 * 05/09/05 BH          1. min and max of stripeSizeOptions values changed
 *
 * 05/10/05 GES         1. Add MFI_STAT_LD_CC_IN_PROGRESS as valid exit code for MR_DCMD_LD_CC_START.
 *                      2. Add "isConsistent" field to MR_LD_PARAMETERS to indicate when redundant LDs are consistent.
 *                      3. Added "pdDirectMapping" flag to MR_MFC_DEFAULTS structure to allow direct PD mapping to be
 *                         defined for arbiutrary controllers, rather than by PCI ID.
 *                      4. Added "biosEnumerateLds" field to MR_MFC_DEFAULTS structure to inform BIOS to enumerate
 *                         LDs at POST.
 *                      5. Added MFI_STAT_CONFIG_RESOURCE_CONFLICT as valid exit code for MR_DCMD_PD_MISSING_REPLACE.
 *
 * 05/18/05 GES         1. Added new capability: MR_CTRL_INFO.adapterOperations.globalHotSpares.
 *
 * 06/01/05 GES         1. Added new MR_DCMD_CTRL_DIAG_* commands.
 *                      2. Add new MFI_STA_DIAG_FAILED status code.
 *
 * 06/15/05 GES         1. Added new field: MR_CTRL_INFO.ldOperations.diskCachePolicy to indicate that the disk cache
 *                         policy may be changed for logical drives.
 *                      2. Added new events MR_EVT_CTRL_MEM_ECC_SINGLE_BIT_WARNING, MR_EVT_CTRL_MEM_ECC_SINGLE_BIT_CRITICAL,
 *                         and MR_EVT_CTRL_MEM_ECC_SINGLE_BIT_DISABLED for single-bit ECC error logging.
 *
 * 06/22/05 GES         1. Added new events and event argument types to event.h and eventmsg.h.
 *
 * 06/24/05 GES         1. Added new field: MR_CTRL_PROP.disableBatteryWarning to cause FW to NOT warn if battery HW
 *                         is missing or not functional.
 *                      2. Add new type MR_BBU_AUTO_LEARN to describe battery learn modes.
 *                      3. Add new field MR_BBU_PROPERTIES.autoLearnMode to reflect current BBU learn mode.
 *
 * 06/27/05 BH          1. Add new event for enclosure reference temperature threshold exceeded.
 *
 * 06/28/05 GES         1. Abstract DDF type into a stand-alone structure to enable reuse by applications.
 *
 * 07/12/05 GES         1. Changed 'stripe' references to 'strip' where possible (i.e. in comments) to more-properly reflect
 *                         the desired terminology.  A 'strip' is one drive's data.  A 'stripe' contains the set of 'strips'
 *                         from a set of drives.
 *                      2. Updated MR_EVT_ARGS_LD_COUNT argument order to fix alignment problem.
 *                      3. Added new macros MR_FLASH_NAME_APP and MR_FLASH_NAME_BIOS for APP and BIOS flash module names.
 *
 * 07/13/05 GES         1. Fixed MR_EVT_ARGS_LD_COUNT 'count' argument type - changed from U64 to U32.  Restored previous
 *                         field order.
 *
 * 07/29/05 BH          1. Added new field: MR_MFC_DEFAULTS.restoreHotSpareOnInsertion to indicate that FW shouls automatically
 *                         make newly inserted drives Hot Spares when replacing failed drive that were logically replaced by
 *                         hot spares.
 *
 * 08/10/05 BH/GES      1. Removed MFI_STAT_LD_RECON_IN_PROGRESS and MFI_STAT_LD_RBLD_IN_PROGRESS exit codes from
 *                         MR_DCMD_LD_DELETE opcode because FW allows deletion during these operations.
 *                      2. Removed MFI_STAT_LD_RECON_IN_PROGRESS exit code from MR_DCMD_CFG_CLEAR because FW allows clear
 *                         during reconstruction.
 *                      3. Fixed spelling of PD_REBUILD_MEDUIM_ERROR to PD_REBUILD_MEDIUM_ERROR.
 *                      4. Fixed spelling of PD_RECOVER_MEDUIM_ERROR to PD_RECOVER_MEDIUM_ERROR.
 *                      5. Removed redundant "PD" references from event message text.
 *                      6. Removed redundant "LD" references from event message text.
 *                      7. Changed remaining "LD" and "logical disk" references to "VD" or "virtual disk" to conform with
 *                         revised terminology.
 *                      8. Added new field: MR_PD_INFO.badBlockTableFull to indicate drive's bad block table is full.
 *                      9. Added new type MR_SPARE_TYPE to declare the spare-type bits as a unique structure for application usage.
 *                         Updated MR_SPARE to use this new structure; no syntactical change in usage.
 *                     10. Updated text of MR_EVT_ENCL_FIRMWARE_MISMATCH and MR_EVT_ENCL_BAD_PHY events.
 *
 * 08/12/05 GES         1. Fixed spelling in text for MR_EVT_PD_RBLD_FAILED_BAD_TARGET.
 *                      2. Add new fields to MR_CTRL_PROP: eccBucketSize, and eccBucketLeakRate.  These fields allow control over
 *                         the thresholds used for single-bit ECC error reporting.
 *                      3. Added new field MR_CTRL_INFO.eccBucketCount to return the current bucket count.
 *                      4. Added new field MR_MFC_DEFAULTS.exposeEnclosureDevices to allow drivers to determine if enclsoure
 *                         devices should be exposed to the operating system.
 *
 * 08/14/05 GES         1. Updated MR_SPARE_TYPE to be a union and added new 'type' reference field to simplify application usage.
 *                      2. Added new event MR_EVT_PD_NOT_CERTIFIED to note drives that are not vendor-certified.  (Certification
 *                         algorithm is vendor-specific).
 *                      3. Added new fields: MR_CTRL_PROP.restoreHotSpareOnInsertion and MR_CTRL_PROP.exposeEnclosureDevices.  These
 *                         fields are also in MR_MFC_DEFAULTS, but the MFC values are only used to define the 'default' state of
 *                         the fields when a MR_DCMD_CTRL_SET_FACTORY_DEFAULTS operation is performed.
 *
 * 08/17/05 GES         1. Added new MFI status code MFI_STAT_BOOT_MSG_PENDING.
 *                      2. Added new MFI state MFI_STATE_BOOT_MSG_PENDING.
 *                      3. Added new fault code MFI_FAULT_DIRTY_CACHE.
 *                      4. Added the following new events: MR_EVT_CTRL_CACHE_DISCARD_BY_USER,
 *                                                         MR_EVT_CTRL_BOOT_MISSING_PDS,
 *                                                         MR_EVT_CTRL_BOOT_LDS_WILL_GO_OFFLINE,
 *                                                         MR_EVT_CTRL_BOOT_LDS_MISSING,
 *                                                         MR_EVT_CTRL_BOOT_CONFIG_MISSING,
 *                                                         MR_EVT_BBU_CHARGE_COMPLETE
 *                      5. Updated MR_EVT_PD_SPARE_DEDICATED_DISABLED to include spare detail in event text.
 *                      6. Updated MR_EVT_PD_SPARE_GLOBAL_DISABLED to include spare detail in event text.
 *                      7. Updated MR_EVT_PD_SPARE_GLOBAL_NOT_COVERING_ALL_ARRAYS to include PD reference.
 *
 * 08/25/05 BH          1. eccBucketCount size changed from U8 to U16
 *                      2. eccBukcetLeakRate comment changed from hours to minutes
 *                      3. notSupported comment updates
 *                      4. Return codes added
 *
 * 9/19/05 GES          1. Add the following new events: MR_EVT_PD_SPARE_DEDICATED_IMPORTED_AS_GLOBAL
 *                                                       MR_EVT_PD_NO_REBUILD_SAS_SATA_MIX_NOT_ALLOWED_IN_LD
 *                                                       MR_EVT_SEP_IS_BEING_REBOOTED
 *                      2. Remove "EmagRAID" from event text to allow use in other LSI RAID products.
 *                      3. Added the following fields to MR_CTRL_INFO:
 *                              hwPresent.memory        - to indicate local memory is present
 *                              hwPresent.flash         - to indicate local flash is present
 *                              raidLevels.raidLevel_10 - to indicate RAID 1 spanning is supported
 *                              raidLevels.raidLevel_50 - to indicate RAID 5 spanning is supported
 *                              raidLevels.raidLevel_60 - to indicate RAID 6 spanning is supported
 *                              raidLevels.SRL_03       - to indicate DDF SRL==3 is supported
 *                              ldOperations.reconstruction - to indicate reconstruction operations are supported
 * 9/23/05 BH           1. Add field connectedPortBitmap to MR_PD_ADDRESS structure
 *                      2. Add field slotOffset in MR_ENCL_INFO. This is the slot offset of the PD in the enclosure
 *                      3. Add unusableInCurrentConfig. TRUE when this drive can't be used as currently configured
 *
 * 10/12/05 GES         1. Added new events:
 *                              MR_EVT_LD_OPTIMAL
 *                              MR_EVT_LD_PARTIALLY_DEGRADED
 *                              MR_EVT_LD_DEGRADED
 *                              MR_EVT_LD_OFFLINE
 *                              MR_EVT_BBU_RELEARN_REQUESTED
 *                      2. Updated event text and string format for MR_PD_INSERTED_EXT and MR_EVT_PD_REMOVED_PD events.
 *                      3. Added comments to MR_LD_PROPERTIES.name field, indicating string is NULL-terminated
 *                      4. Added new fields to MR_ARRAY called encl.pd and encl.slot to retain the current/previous
 *                         enclosure and slot in which a drive was located.
 *                      5. Fixed comments for MR_BBU_PROPERTIES.autoLearnPeriod
 *
 * 11/03/05 GES         1. Added comments to MR_BBU_PROPERTIES fields indicating which are read-only.
 *
 * 01/30/06 GES         1. Added new field: MR_MFC_DEFAULTS.maintainPdFailHistory to indicaate whether f/w should track failed drives
 *                         in order to retain failure information across reboots for specific disks.
 *                      2. Added new filed: MR_CTRL_PROP.maintainPdFailHistory
 *
 * 02/03/06 BGH         1. Comment changes for MAX_LOGICAL_DRIVES and allowedDeviceTypes.
 *
 * 02/09/06 GES         1. Updated event error classifications to better-reflect true meaning.  The following changes were made:
 *                              MR_EVT_CTRL_FLASH_BAD_IMAGE                         Change from EC_I to EC_W
 *                              MR_EVT_LD_BGI_CORRECTED_MEDIUM_ERROR                Change from EC_W to EC_I
 *                              MR_EVT_LD_CC_ABORTED                                Change from EC_W to EC_I
 *                              MR_EVT_LD_CC_CORRECTED_MEDIUM_ERROR                 Change from EC_W to EC_I
 *                              MR_EVT_LD_CC_INCONSISTENT_PARITY                    Change from EC_W to EC_I
 *                              MR_EVT_LD_INIT_ABORTED                              Change from EC_W to EC_I
 *                              MR_EVT_PD_HS_SMART_POLL_FAILED                      Change from EC_C to EC_W
 *                              MR_EVT_PD_PR_CORRECTED                              Change from EC_W to EC_I
 *                              MR_EVT_PD_PREDICTIVE_THRESHOLD_EXCEEDED             Change from EC_C to EC_W
 *                              MR_EVT_PD_REMOVED                                   Change from EC_I to EC_W
 *                              MR_EVT_PD_SENSE                                     Change from EC_W to EC_I
 *                              MR_EVT_PD_SPARE_DEDICATED_NOT_USEFUL_FOR_ALL_ARRAYS Change from EC_C to EC_W
 *                              MR_EVT_PD_SPARE_GLOBAL_NOT_COVERING_ALL_ARRAYS      Change from EC_C to EC_W
 *                              MR_EVT_BBU_TEMPERATURE_HIGH                         Change from EC_C to EC_W
 *                              MR_EVT_BBU_RELEARN_TIMEOUT                          Change from EC_C to EC_W
 *                              MR_EVT_BBU_BATTERY_REMOVED                          Change from EC_I to EC_W
 *                              MR_EVT_BBU_BATTERY_CAP_BELOW_SOH_THRESHOLD          Change from EC_I to EC_W
 *                              MR_EVT_ENCL_FAN_REMOVED                             Change from EC_C to EC_W
 *                              MR_EVT_ENCL_POWER_REMOVED                           Change from EC_C to EC_W
 *                              MR_EVT_ENCL_SAS_SATA_MIXING_DETECTED                Change from EC_I to EC_W
 *                              MR_EVT_ENCL_SES_HOTPLUG_DETECTED                    Change from EC_I to EC_W
 *                              MR_EVT_ENCL_POWER_SUPPLY_OFF                        Change from EC_C to EC_W
 *                              MR_EVT_ENCL_POWER_SUPPLY_CABLE_REMOVED              Change from EC_C to EC_W
 *                              MR_EVT_SATA_BAD_BLOCK_REMAPED                       Change from EC_W to EC_I
 *                              MR_EVT_CTRL_BOOT_LDS_WILL_GO_OFFLINE                Change from EC_I to EC_W
 *                              MR_EVT_CTRL_BOOT_LDS_MISSING                        Change from EC_I to EC_W
 *                              MR_EVT_CTRL_BOOT_CONFIG_MISSING                     Change from EC_I to EC_W
 *                          2. Added new events:
 *                              MR_EVT_LD_DISABLED_NO_SUPPORT_FOR_RAID5
 *                              MR_EVT_LD_DISABLED_NO_SUPPORT_FOR_RAID6
 *                              MR_EVT_LD_DISABLED_NO_SUPPORT_FOR_SAS
 *
 * 02/24/06 NN          1. Added new fields to MR_CTRL_INFO.  These are used to determine the controller's capabilities:
 *                              adapterOperations.denySCSIPassthrough
 *                              adapterOperations.denySMPPassthrough
 *                              adapterOperations.denySTPPassthrough
 *                              ldOperations.denyLocate
 *                              ldOperations.denyCC
 *                              pdOperations.denyForceFailed
 *                              pdOPerations.denyGoodBad
 *                              pdOPerations.denyMissingReplace
 *                              pdOPerations.denyClear
 *                              pdOPerations.denyLocate
 *
 * 03/01/06 GES         1. Added new MFI_STAT_INVALID_SGL exit code
 *
 * 03/02/06 GES         1. Added new MR_BBU_TYPE.MR_BBU_TYPE_ZCR_LEGACY enum type.
 *                      2. Added new MFI_STAT_UNSUPPORTED_HW status code.
 *
 * 03/30/06 NN          1. Added new field to MR_PD_INFO: vpdPage83Ext.
 *
 * 05/11/06 NN          1. Changed event description for the following events
 *                              MR_EVT_LD_BGI_DOUBLE_MEDIUM_ERRORS
 *                              MR_EVT_LD_CC_DOUBLE_MEDIUM_ERRORS
 *                              MR_EVT_LD_RECON_DOUBLE_MEDIUM_ERRORS
 *                              Changed "double medium errors" to "multiple medium errors" in these events.
 *                      2. Added new field MR_CTRL_PROP.disallowHostRequestReordering.
 *
 * 05/16/06 NN          1. Added new field to MR_CTRL_INFO, adapterOperations.supportMoreThan8Phys
 *                              The API wasn't compliant with MAX_PHYS_PER_CONTROLLER.  This new field indicates
 *                              that the API doesn't assume that there are only 8 Phys.  Following fields are
 *                              introduced/updated to support this capability.
 *                              MR_CTRL_INFO.deviceInterfacePortAddr2
 *                              MR_PD_ADDRESS.connectedPortNumbers, replaces MR_PD_ADDRESS.connectedPortBitmap
 *                         Note:
 *                              MAX_PHYS_PER_CONTROLLER will be increased as required.  Consumers of this API
 *                              should not make assumptions to the size of structures dependent on MAX_PHYS_PER_CONTROLLER.
 *                              Future API revisions may increase this value.
 *                      2. Added new structures
 *                              MR_PD_ALLOWED_OPS
 *                              MR_PD_ALLOWED_OPS_LIST
 *                              MR_LD_ALLOWED_OPS
 *                              MR_LD_ALLOWED_OPS_LIST
 *                              MR_CONFIG_ALLOWED_OPS
 *                      3. Added new fields
 *                              MR_PD_INFO.allowedOps
 *                              MR_LD_INFO.allowedOps
 *                      4. Added new DCMDs
 *                              MR_DCMD_PD_GET_ALLOWED_OPS
 *                              MR_DCMD_PD_GET_ALLOWED_OPS_ALL_PDS
 *                              MR_DCMD_LD_GET_ALLOWED_OPS
 *                              MR_DCMD_LD_GET_ALLOWED_OPS_ALL_LDS
 *                              MR_DCMD_CFG_GET_ALLOWED_OPS
 *
 * 06/23/06 NN          1. Added MAX_SAS_QUADS_PER_CONTROLLER
 *                      2. Added new enum MR_SAS_MUX_MODE
 *                      3. Added new structure MR_SAS_MUX_LIST
 *                      4. Added new DCMDs
 *                              MR_DCMD_SAS_CONFIG_MUX_MODE_GET_LIST
 *                              MR_DCMD_SAS_CONFIG_MUX_MODE_SET_LIST
 *                      5. Added new macros to support API versioning
 *                              MR_API_VERSION
 *                              MR_API_FWD_COMPATIBILITY
 *                              MR_API_BKWD_COMPATIBILITY
 *                      6. Added new fields
 *                              MR_CTRL_INFO.mrAPIVersion
 *                              MR_CTRL_INFO.mrAPIFwdCompatibility
 *                              MR_CTRL_INFO.mrAPIBkwdCompatibility
 *                      7. Added MR_CTRL_INFO.adapterOperations.fwAndEventTimeInGMT
 *
 * 07/13/06 BH          1. Added disablePuncturing in MR_CTRL_PROP
 *                      2. Bump API version to 1
 *
 * 08/10/06 NN          1. Added MFI_CONTROL_REG.killAdapter
 *                      2. Added new fields
 *                              MR_CTRL_INFO.adapterOperations.supportEnhForeignImport
 *                              MR_CTRL_INFO.adapterOperations.supportEnclEnumeration
 *                      3. Added comments to document usage of ALLOWED_OPS structures.
 *
 * 08/21/06 NN          1. Added capability bit MR_CTRL_INFO.adapterOperations.supportAllowedOps
 *                      2. Added byte argument to MR_DCMD_CFG_MAKE_SPARE.  Allows Applicaiton to
 *                         test validity of PD to be used as a spare, before making it one.
 *                         This option is to be used in conjunction with supportAllowedOps capability.
 *                      3. Added following enum, define, structures and DCMD to support SAS connector information
 *                              MAX_SAS_CONNECTORS_PER_CONTROLLER
 *                              MR_SAS_CONNECTOR_NONE
 *                              MR_SAS_CONN_LOCATION_TYPE
 *                              MR_SAS_CONNECTOR_INFO
 *                              MR_SAS_CONNECTORS
 *                              MR_DCMD_SAS_CONFIG_GET_CONNECTORS
 *                      4. Added new fields to MR_PD_INFO, MR_PD_ADDRESS and MR_ENCL_INFO
 *                              enclPosition
 *                              enclConnectorIndex
 *                      5. Added new field MR_SAS_PHY_CONNECTION.connectorIndex
 *                      6. Added a union to MR_EVT_ARG_PD and new fields
 *                              MR_EVT_ARG_PD.enclDeviceId
 *                              MR_EVT_ARG_PD.enclPosition
 *                              MR_EVT_ARG_PD.enclConnectorIndex
 *                      7. Added MR_MFC_DEFAULTS.zeroBasedEnclEnum
 *                      8. Changed char* MR_EVT_DESCRIPTION.desc to const char*.
 *                      9. Corrected typo in event description of MR_EVT_PD_REBUILD_MEDIUM_ERROR.
 *
 * 09/13/06 BH          1. Removed disablePuncturing from MR_CTRL_PROP
 *
 * 09/21/06 NN          1. Added new DCMD MR_DCMD_SAS_CONFIG_PAGE_PASSTHROUGH.
 *                      2. Added new structure MR_SAS_CONFIG_PAGE
 *                      3. Fixed comments related to usage of MR_PD_INVALID and single byte deviceId fields in
 *                              MR_ARRAY.encl
 *                              MR_EVT_ARG_PD.enclDeviceId
 *
 * 10/18/06 NN          1. Added new field MR_SAS_CONNECTOR_INFO.sasMuxIndex
 *                      2. Added define MR_SAS_MUX_NONE
 *                      3. Added MR_IBBU_STATE.iTBBU3chargerStatus
 *                      4. Added MR_BBU_TYPE_iTBBU3
 *
 * 11/06/06 NN          1. Added new define MR_FLASH_NAME_NVDATA
 *                      2. Corrected incorrect field name in MR_IBBU_STATE
 *
 * 11/16/06 NN          1. Added DCMD MR_DCMD_CTRL_PD_FW_DOWNLOAD_ENABLE
 *                      2. Added new struct MR_CTRL_INFO.adapterStatus
 *
 * 12/06/06 BH          1. Added pdsForRaidLevels structure in MR_CTRL_INFO that tells the min and max pd's used in a raid level
 *                      2. Added fields maxPds, maxDedHSPs, maxGlobalHSPs, ddfSize, maxLdsPerArray in MR_CTRL_INFO.
 *                      3. Comment changes in #defines for MAX_PD_PER_ENCLOSURE and MAX_LDS_PER_ARRAY as these can be also got from
 *                         MR_CTRL_INFO now due to addition of certain fields in #2.
 *                      4. Added field enableBootCLI in MR_MFC_DEFAULTS
 *
 * 12/12/06 NN          1. Added new field MR_LD_PARAMETERS.isMorphing
 *                      2. Updated comments to allow foreign configuration operation on all GUIDs for the following
 *                              MR_DCMD_CFG_FOREIGN_READ_AFTER_IMPORT
 *                              MR_DCMD_CFG_FOREIGN_IMPORT
 *                              MR_DCMD_CFG_FOREIGN_CLEAR
 *                      3. Added helper macro MR_LD_CACHE_POLICY_WRITE_CACHE_BAD_BBU
 *
 * 01/09/07 NN          1. Added following names for unnamed structs if compiled with NO_UNNAMED_STRUCT defined.
 *                                   mrProgress, mrPdRef, mrPdAddress, mrEnclAddress,
 *                                   mrLdRef, mrLdCachePolicy, mrSpareType, mrAppExecResults,
 *                                   mrBbuGasGaugeStatus, mrIbbuCharger, mrItbbu3Charger, and mrBbuFwStatus
 *                          mfi.h:   mfiControlReg and mfiMsgFlags
 *                          event.h: mrEventClassLocale, mrEvtArgLd and mrEvtArgPd
 *                      2. Added MR_CTRL_INFO.raidLevels.raidLevel_00_unsupported
 *                      3. Added new event MR_EVT_PD_SPARE_AFFINITY_IGNORED
 *
 * 01/27/07 NN          1. Fixed DCMD description for MR_DCMD_CTRL_TIME_SECS_GET and MR_DCMD_CTRL_TIME_SECS_SET
 *                      2. Added new fields
 *                              MR_MFC_DEFAULTS.quadPortConnectorMap
 *                              MR_MFC_DEFAULTS.driveActivityLed
 *                      3. Added new event arguments
 *                              MR_EVT_ARGS_BITMAP
 *                              MR_EVT_ARGS_CONNECTOR
 *                      4. Added new events
 *                              MR_EVT_FOREIGN_CFG_TABLE_OVERFLOW
 *                              MR_EVT_FOREIGN_CFG_PARTIAL_IMPORT
 *                              MR_EVT_SAS_MUX_ACTIVATED_CONNECTOR
 *                      5. Corrected comments for MR_SAS_CONFIG_PAGE
 *
 * 02/27/07 BH          1. Added field forceReboot in MR_BOOT_MESSAGE_DATA
 *
 * 03/12/07 NN          1. Added MR_CTRL_PROP.autoEnhancedImport
 *                      2. Added MR_MFC_DEFAULTS.autoEnhancedImport
 *                      3. Fixed description string for MR_EVT_FOREIGN_CFG_PARTIAL_IMPORT
 *
 * 03/20/07 NN          1. Removed MR_CTRL_PROP.autoEnhancedImport
 *                                 MR_MFC_DEFAULTS.autoEnhancedImport
 *                      2. Renamed field enableBootCLI in MR_MFC_DEFAULTS to disableBootCLI
 *
 * 03/26/07 BH          1. Removed field forceReboot (reverted) in MR_BOOT_MESSAGE_DATA and added a new input
 *                         parameter to MR_DCMD_CTRL_BIOS_BOOT_MSG_SEND_KEYSTROKE that has values
 *                         defined in MR_BOOT_PARAMS enum. This was done to follow the force boot action
 *                         design as opposed to having a forecereboot parameter in boot message.
 *                      2. Added enums and structures to support CC Scheduling
 *                              MR_CC_SCHEDULE_STATE
 *                              MR_CC_SCHEDULE_MODE
 *                              MR_CC_SCHEDULE_PROPERTIES
 *                              MR_CC_SCHEDULE_STATUS
 *                      3. Added MR_DCMD_CTRL_CC_SCHEDULE commands
 *                              MR_DCMD_CTRL_CC_SCHEDULE_GET_STATUS
 *                              MR_DCMD_CTRL_CC_SCHEDULE_GET_PROPERTIES
 *                              MR_DCMD_CTRL_CC_SCHEDULE_SET_PROPERTIES
 *                              MR_DCMD_CTRL_CC_SCHEDULE_START
 *                              MR_DCMD_CTRL_CC_SCHEDULE_STOP
 *                      4. Added MFI_STAT_CC_SCHEDULE_DISABLED.
 *
 * 03/30/07 NN          1. Incorrectly added uint64 members, fixed to use U64 in MR_CC_SCHEDULE_PROPERTIES
 *
 * 04/03/07 PM          1. Added new events
 *                              MR_EVT_CTRL_FRU
 *                              MR_EVT_PD_COMMAND_TIMEOUT
 *                              MR_EVT_PD_RESET
 *
 * 04/05/07 NN          1. Added MR_CTRL_PROP.abortCCOnError
 *                      2. Added MR_CTRL_INFO.adapterOperations.abortCCOnError
 *
 * 04/09/07 NN          1. Added define MR_SAS_ADDRESS_INVALID
 *                      2. Enhanced event arguments
 *                              MR_EVT_ARGS_CDB_SENSE
 *                              MR_EVT_ARGS_PD_ERR
 *
 * 04/11/07 NN          1. Modified MR_CC_SCHEDULE_MODE
 *                      2. Removed DCMDs MR_DCMD_CTRL_CC_SCHEDULE_START and MR_DCMD_CTRL_CC_SCHEDULE_STOP
 *                      3. Added new field MR_SAS_PHY_CONNECTION.disabled
 *
 * 04/26/07 NN          1. Added MR_PD_ALLOWED_OPS.foreignImportNotAllowed
 *                      2. Added events
 *                              MR_EVT_LD_BBM_LOG_80_PERCENT_FULL
 *                              MR_EVT_LD_BBM_LOG_FULL
 *                              MR_EVT_LD_LOG_MEDIUM_ERROR
 *                              MR_EVT_LD_CORRECTED_MEDIUM_ERROR
 *
 * 07/19/07 BH          1. Added events
 *                              MR_EVT_PD_BBM_LOG_100_PERCENT_FULL
 *                              MR_EVT_LD_BBM_LOG_100_PERCENT_FULL
 *                              MR_EVT_DIAG_FAULTY_IOP_DETECTED
 *                              MR_EVT_PD_COPYBACK_START
 *                              MR_EVT_PD_COPYBACK_ABORT
 *                              MR_EVT_PD_COPYBACK_DONE
 *                              MR_EVT_PD_COPYBACK_PROGRESS
 *                              MR_EVT_PD_COPYBACK_RESUME
 *                              MR_EVT_PD_COPYBACK_AUTO
 *                              MR_EVT_PD_COPYBACK_FAILED
 *                              MR_EVT_CTRL_EPOW_UNSUCCESSFUL
 *                              MR_EVT_BBU_FRU
 *                              MR_EVT_PD_FRU
 *                              MR_EVT_CTRL_HW_REVISION
 *                              MR_EVT_FOREIGN_CFG_UPGRADE_REQUIRED
 *                      2. Added MSI/MSI-X MFI structure changes
 *                      3. Added error code MFI_STAT_PD_COPYBACK_IN_PROGRESS
 *                      4. Added DCMDs
 *                              MR_DCMD_CTRL_MFC_MFG_2_GET
 *                              MR_DCMD_PD_COPYBACK_START
 *                              MR_DCMD_PD_COPYBACK_ABORT
 *                      5. Added structure in MR_CTRL_PROP.OnOffProperties
 *                              copyBackEnabled,
 *                              SMARTerEnabled
 *                              prCorrectUnconfiguredAreas
 *                      6. Changed comments for
 *                              MR_CTRL_INFO.pdsForRaidLevels.raidLevel_1E
 *                              DCMD desc MR_DCMD_PD_MISSING_REPLACE
 *                              DCMD desc MR_DCMD_BBU_MFG_SLEEP
 *                      7. Added new MR_PD_STATE MR_PD_STATE_COPYBACK,
 *                              MR_PD_PROGRESS.active.copyBack
 *                              MR_PD_ALLOWED_OPS.startCopyBack
 *                              MR_PD_ALLOWED_OPS.stopCopyBack
 *                              MR_PD_INFO.copyBackPartnerId
 *                      8. Added MR_MFC_MFG_2
 *                      9. MR_PD_INFO.enclConnectorIndex is deprecated
 *                         Added MR_PD_INFO.connectorIndex
 *                     10. Added MR_CTRL_INFO.partitionsInDDF
 *                     11. Added msgId to MFI_HDR and MFI Frames
 *
 * 08/14/07 NN          1. Added enum MR_ENCL_TYPE
 *                      2. Added new fields
 *                              MR_CTRL_INFO.adapterOperations.supportMultipath
 *                              MR_CTRL_PROP.loadBalance
 *                              MR_PD_INFO.enclPartnerDeviceId
 *                              MR_ENCL_INFO.type
 *                              MR_PD_INFO.pathInfo.connectorIndex
 *                      3. Updated comments for
 *                              MR_PD_INFO.pathInfo - changed number of supported paths from 4 to 2.
 *                              MR_ENCL_INFO.enclConnectorIndex
 *                      4. Moved recently added field MR_PD_INFO.connectorIndex
 *                              to MR_PD_INFO.pathInfo.connectorIndex
 *                      5. Added new event argument MR_EVT_ARGS_PD_PATHINFO
 *                      6. Added new events
 *                              MR_EVT_PD_SAS_REDUNDANT_PATH_RESTORED
 *                              MR_EVT_PD_SAS_REDUNDANT_PATH_BROKEN
 *                              MR_EVT_ENCL_REDUNDANT_ENCL_MODULE_INSERTED
 *                              MR_EVT_ENCL_REDUNDANT_ENCL_MODULE_REMOVED
 *                      7. Deprecated events
 *                              MR_EVT_PD_REDUNDANT_PATH_BROKEN
 *                              MR_EVT_PD_REDUNDANT_PATH_RESTORED
 *                      8. Updated event description for
 *                              MR_EVT_PD_COPYBACK_START
 *                              MR_EVT_PD_COPYBACK_ABORT
 *                              MR_EVT_PD_COPYBACK_DONE
 *                              MR_EVT_PD_FRU
 *                              MR_EVT_FOREIGN_CFG_UPGRADE_REQUIRED
 *                      9. Changed event argument for MR_EVT_PD_COPYBACK_AUTO
 *
 * 08/15/07 BH          1. Added MFC default and MR_CTRL_PROP disableAutoDetectBackplane bits
 *                      2. Added additional return codes for MR_DCMD_PD_COPYBACK_START and changed comment in
 *                         MR_DCMD_PD_COPYBACK_ABORT.
 *
 * 09/05/07 BH          1. PRL11, RLQ0 bits and its min/max fields added for capabilties in MR_CTRL_INFO
 *                      2. Added MR_BBU_STATE.autoLearnModeSupport
 *                      3. Padded MR_CC_SCHEDULE_PROPERTIES to ensure 8-byte alignment for U64 members.
 *                      4. Remove the MFC dependency on MR_CTRL_PROP.OnOffProperties.copyBackEnabled
 *
 * 09/18/07 BH          1. Added field supportOddEveninRAID1E in MR_CTRL_INFO.adapterOperations
 *
 * 10/12/07 NN          1. Changed event description for
 *                              MR_EVT_LD_CC_FAILED_UNCOR
 *                              MR_EVT_CTRL_PR_CANT_START
 *                      2. Changed event argument and description for MR_EVT_PD_COPYBACK_RESUME
 *                      3. Added new events
 *                              MR_EVT_PD_COPYBACK_ABORT_BY_USER
 *                              MR_EVT_PD_COPYBACK_ABORT_FOR_SPARE
 *                              MR_EVT_PD_COPYBACK_ABORT_FOR_REBUILD
 *                              MR_EVT_LD_CACHE_DISCARDED
 *                      4. Changed size of MR_EVT_DETAIL.pdFru.FRU
 *
 * 10/12/07 BH          1. Added MR_DCMD_CTRL_MFC_DEFAULTS_GET_RAW and MR_DCMD_CTRL_MFC_PCI_GET_RAW API.
 *
 * 11/01/07 NN          1. Added enum MR_LOAD_BALANCE_MODE
 *                      2. Renamed fields in MR_CTRL_PROP
 *                              loadBalance to loadBalanceMode
 *                              OnOffProperties.copyBackEnabled to OnOffProperties.copyBackDisabled
 *                      3. Updated comment for
 *                              MR_CTRL_INFO.adapterOperations.revertibleHotSpares
 *                              MR_SPARE_TYPE.isEnclAffinity
 *                              MR_DCMD_CTRL_PD_FW_DOWNLOAD_ENABLE
 *                      4. Added structure MR_PD_FW_DOWNLOAD_PARAMS
 *                      5. Added DCMD MR_DCMD_CTRL_PD_FW_DOWNLOAD_PREPARE
 *                      6. Added field MR_PD_ALLOWED_OPS.fwDownloadNotAllowed
 *                      7. Added error codes
 *                              MFI_STAT_MULTIPLE_PDS_IN_ARRAY
 *                              MFI_STAT_FW_DOWNLOAD_ERROR
 *                      8. Added new events
 *                              MR_EVT_PD_TOO_SMALL_FOR_COPYBACK
 *                              MR_EVT_PD_NO_COPYBACK_SAS_SATA_MIX_NOT_ALLOWED_IN_LD
 *                              MR_EVT_PD_FW_DOWNLOAD_START
 *                              MR_EVT_PD_FW_DOWNLOAD_COMPLETE
 *                              MR_EVT_PD_FW_DOWNLOAD_TIMEOUT
 *                              MR_EVT_PD_FW_DOWNLOAD_FAILED
 *
 * 11/29/07 BH          1. Added new events
 *                              MR_EVT_CTRL_PROP_CHANGED
 *                              MR_EVT_BBU_PROP_CHANGED
 *                              MR_EVT_CTRL_PR_PROP_CHANGED
 *                              MR_EVT_CTRL_CC_SCHEDULE_PROP_CHANGED
 *
 * 01/09/08 BH          1. Added new event MR_EVT_BBU_PERIODIC_RELEARN_PENDING
 *
 * 01/23/08 NN          1. Added error codes
 *                              MFI_STAT_FEATURE_SECURITY_NOT_ENABLED
 *                              MFI_STAT_LOCK_KEY_ALREADY_EXISTS
 *                              MFI_STAT_LOCK_KEY_BACKUP_NOT_ALLOWED
 *                              MFI_STAT_LOCK_KEY_VERIFY_NOT_ALLOWED
 *                              MFI_STAT_LOCK_KEY_REKEY_NOT_ALLOWED
 *                              MFI_STAT_LOCK_KEY_VERIFY_FAILED
 *                              MFI_STAT_LOCK_KEY_INVALID
 *                              MFI_STAT_LOCK_KEY_ESCROW_INVALID
 *                              MFI_STAT_LOCK_KEY_BACKUP_REQUIRED
 *                              MFI_STAT_SECURE_LD_EXISTS
 *                              MFI_STAT_LD_SECURE_NOT_ALLOWED
 *                              MFI_STAT_REPROVISION_NOT_ALLOWED
 *                              MFI_STAT_PD_SECURITY_TYPE_WRONG
 *                              MFI_STAT_LD_ENCRYPTION_TYPE_INVALID
 *                              MFI_STAT_CONFIG_FDE_NON_FDE_MIX_NOT_ALLOWED
 *                              MFI_STAT_CONFIG_LD_ENCRYPTION_TYPE_MIX_NOT_ALLOWED
 *                              MFI_STAT_SECRET_KEY_NOT_ALLOWED
 *                              MFI_STAT_PD_HW_ERRORS_DETECTED
 *                      2. Added new events
 *                              MR_EVT_CTRL_LOCK_KEY_CREATED
 *                              MR_EVT_CTRL_LOCK_KEY_BACKEDUP
 *                              MR_EVT_CTRL_LOCK_KEY_VERIFIED_ESCROW
 *                              MR_EVT_CTRL_LOCK_KEY_REKEYED
 *                              MR_EVT_CTRL_LOCK_KEY_REKEY_FAILED
 *                              MR_EVT_CTRL_LOCK_KEY_INVALID
 *                              MR_EVT_CTRL_LOCK_KEY_DESTROYED
 *                              MR_EVT_CTRL_ESCROW_KEY_INVALID
 *                              MR_EVT_LD_SECURED
 *                              MR_EVT_LD_PARTIALLY_SECURED
 *                              MR_EVT_PD_SECURED
 *                              MR_EVT_PD_UNSECURED
 *                              MR_EVT_PD_REPROVISIONED
 *                              MR_EVT_PD_LOCK_KEY_REKEYED
 *                              MR_EVT_PD_SECURITY_FAILURE
 *                      3. Added enum MR_LD_ENCRYPTION_TYPE
 *                      4. Added fields
 *                              MR_MFC_DEFAULTS.useFdeOnly
 *                              MR_CTRL_PROP.OnOffProperties.useFdeOnly
 *                              MR_CTRL_INFO.hwPresent.TPM
 *                              MR_CTRL_INFO.adapterOperations.supportSecurity
 *                              MR_CTRL_INFO.adapterStatus.lockKeyAssigned
 *                              MR_CTRL_INFO.adapterStatus.lockKeyFailed
 *                              MR_CTRL_INFO.adapterStatus.lockKeyNotBackedup
 *                              MR_CTRL_INFO.adapterStatus.biosNotDetected
 *                              MR_CTRL_INFO.ldOperations.allowCtrlEncryption
 *                              MR_BOOT_MESSAGE_DATA.options
 *                              MR_LD_PARAMETERS.encryptionType
 *                              MR_LD_ALLOWED_OPS.secure
 *                              MR_PD_ALLOWED_OPS.reprovision
 *                              MR_PD_INFO.security
 *                      5. Added new structures
 *                              MR_CTRL_ESCROW_LOCK_KEY
 *                              MR_CTRL_LOCK_KEY_FROM_ESCROW
 *                              MR_CTRL_LOCK_KEY_PARAMS
 *                      6. Added new DCMDs
 *                              MR_DCMD_CTRL_LOCK_KEY_CREATE
 *                              MR_DCMD_CTRL_LOCK_KEY_SUGGEST_PASSPHRASE
 *                              MR_DCMD_CTRL_LOCK_KEY_KEYID_GET
 *                              MR_DCMD_CTRL_LOCK_KEY_KEYID_SET
 *                              MR_DCMD_CTRL_LOCK_KEY_BACKUP
 *                              MR_DCMD_CTRL_LOCK_KEY_VERIFY_ESCROW
 *                              MR_DCMD_CTRL_LOCK_KEY_BACKUP_USER_ACK
 *                              MR_DCMD_CTRL_LOCK_KEY_REKEY
 *                              MR_DCMD_CTRL_LOCK_KEY_FROM_ESCROW
 *                              MR_DCMD_CTRL_LOCK_KEY_DESTROY
 *                              MR_DCMD_CTRL_BIOS_BOOT_MSG_SEND_STRING
 *                              MR_DCMD_PD_REPROVISION
 *                              MR_DCMD_PD_LOCK_KEY_REKEY
 *                              MR_DCMD_PD_RECOVER_LOCKED
 *                              MR_DCMD_LD_SECURE
 *
 * 01/23/08 NN          1. Added error code MFI_STAT_LD_CACHE_PINNED
 *                      2. Added new events
 *                              MR_EVT_LD_CACHE_PINNED
 *                              MR_EVT_CTRL_BOOT_LDS_CACHE_PINNED
 *                              MR_EVT_CTRL_LDS_CACHE_DISCARDED_BY_USER
 *                              MR_EVT_LD_CACHE_DESTAGED
 *                              MR_EVT_LD_CC_STARTED_ON_INCONSISTENT_LD
 *                      3. Added fields
 *                              MR_CTRL_INFO.adapterStatus.cacheIsPinned
 *                              MR_LD_PARAMETERS.isCachePinned
 *                      4. Added new DCMDs
 *                              MR_DCMD_LD_GET_PINNED_LIST
 *                              MR_DCMD_LD_DISCARD_PINNED
 *                      5. Modified comments for MR_DCMD_BBU_LEARN
 *
 * 01/31/08 BH          1. Added disableNCQ in MR_CTRL_PROP
 *
 * 02/04/08 NN          1. Added enum MR_CTRL_LOCK_KEY_BINDING
 *                      2. Added field MR_CTRL_INFO.lockKeyBinding
 *                      3. Added new events
 *                              MR_EVT_CTRL_LOCK_KEY_FAILED
 *                              MR_EVT_CTRL_SECRET_KEY_INVALID
 *
 * 02/29/08 NN          1. Added field MR_MFC_DEFAULTS.enableLedHeaders
 *
 * 03/13/08 BH          1. Added SSD support fields
 *                         MR_CTRL_PROP.OnOffProperties.SSDSMARTerEnabled
 *                         MR_CTRL_PROP.OnOffProperties.SSDPatrolReadEnabled
 *                         MR_CTRL_PROP.PdMixSupport.allowSSDMixInLD
 *                         MR_CTRL_PROP.PdMixSupport.allowMixSSDHDDInLD
 *                         MR_PD_INFO.mediaType
 *                         MR_MFC_DEFAULTS.allowSSDMixInLD
 *                         MR_MFC_DEFAULTS.allowMixSSDHDDInLD
 *
 * 03/20/08 NN          1. Added enums
 *                              MR_PD_MEDIA_TYPE
 *                              MR_PD_POWER_STATE
 *                              MR_PR_OPTIONS
 *                              MR_PD_FW_DOWNLOAD_OPTIONS
 *                      2. Added field
 *                              MR_CTRL_PROP.OnOffProperties.enableSpinDownUnconfigured
 *                              MR_CTRL_PROP.OnOffProperties.autoEnhancedImport
 *                              MR_CTRL_INFO.pdOperations.setPowerStateForCfg
 *                              MR_PD_INFO.powerState
 *                              MR_PR_PROPERTIES.options
 *                              MR_PD_FW_DOWNLOAD_PARAMS.options
 *                      3. Added structure MR_PD_DEVICEID_LIST
 *                      4. Added error codes
 *                              MFI_STAT_POWER_STATE_SET_IN_PROGRESS
 *                              MFI_STAT_POWER_STATE_SET_BUSY
 *                              MFI_STAT_POWER_STATE_WRONG
 *                      5. Added DCMDs
 *                              MR_DCMD_PD_POWER_STATE_GET
 *                              MR_DCMD_PD_POWER_STATE_SET
 *                      6. Added new event argument MR_EVT_ARGS_PD_POWER_STATE
 *                      7. Added new event MR_EVT_PD_POWER_STATE_CHANGE
 *                      8. Modified DCMD MR_DCMD_LD_DISCARD_PINNED
 *                      9. Changed enumerated values for MR_DCMD_PD_SECURITY
 *                              MR_DCMD_PD_RECOVER_LOCKED
 *                              MR_DCMD_PD_LOCK_KEY_REKEY
 *                              MR_DCMD_PD_REPROVISION
 *                     10. Renamed MR_PD_INFO.mediaType (was mediatype)
 *                     11. Updated comments for DCMD MR_DCMD_CTRL_LOCK_KEY_KEYID_SET
 *                     12. Corrected typo in event description for MR_EVT_LD_RECON_RESUME_FAILED
 *
 * 04/04/08 BH         1. Added field delayPOST in MR_MFC_DEFAULTS
 *
 * 04/23/08 BH         1. Added new field
 *                              MR_CTRL_INFO.adapterOperations.supportConfigPageModel
 *                     2. Added enum MR_SAS_INTERFACE_TYPE
 *                     3. MR_DCMD_CTRL_MFC_DEFAULTS_SET is deprecated, see DCMD description
 *                     4. Updated structure
 *                              MR_SAS_CONFIG_PAGE
 *                              MR_CTRL_INFO.deviceInterface
 *                              MR_MFC_PCI
 *                     5. Added DCMD MR_DCMD_CTRL_MFC_DEFAULTS_SASADDR_SET
 *
 * 04/28/08 NN         1. Added field MR_CTRL_INFO.pdOperations.supportPowerState
 *                     2. Added new event MR_EVT_ENCL_ELEMENT_STATUS_CHANGED
 *
 * 05/07/08 NN         1. Changed enumerated value for MR_DCMD_CTRL_MFC_DEFAULTS_SASADDR_SET
 *                     2. Added field MR_BIOS_DATA.disableCTO
 *                     3. Modified event description for MR_EVT_CTRL_LDS_CACHE_DISCARDED_BY_USER
 *
 * 08/01/08 NN         1. Added fields
 *                              MR_MFC_DEFAULTS.enableCrashDump
 *                              MR_MFC_DEFAULTS.enableLDBBM
 *                              MR_CTRL_INFO.ldOperations.ldbbm
 *                              MR_LD_PARAMETERS.badBlocksExist
 *                              MR_EVT_DETAIL.args.genericArg
 *                     2. Added DCMD MR_DCMD_LD_BBM_LOG_CLEAR
 *                     3. Added new event argument MR_EVT_ARGS_GENERIC
 *                     4. Added events
 *                              MR_EVT_LD_BBM_LOG_CLEARED
 *                              MR_EVT_SAS_TOPOLOGY_ERROR
 *                     5. Modified event description for MR_EVT_CTRL_PR_CANT_START
 *                     6. Added error code MFI_STAT_PR_NO_AVAILABLE_PD_FOUND
 *                     7. Modified comments for DCMDs
 *                              MR_DCMD_CFG_FOREIGN_READ_AFTER_IMPORT
 *                              MR_DCMD_CFG_FOREIGN_IMPORT
 *                              MR_DCMD_CFG_FOREIGN_CLEAR
 *
 * 09/19/08 NN         1. Added enumerated value MR_PD_SPEED.MR_PD_SPEED_6G
 *                     2. Added fields
 *                              MR_BBU_FW_STATUS.replacePack
 *                              MR_BBU_FW_STATUS.remainingCapacityLow
 *                     3. Added macros
 *                              MR_BBU_STATUS_BAD_FOR_IBBU
 *                              MR_BBU_STATUS_BAD_FOR_BBU
 *                     4. Added event MR_EVT_LD_CORRECTED_CLUSTER_OF_MEDIUM_ERRORS
 *
 * 10/06/08 BH         1. Added a comment for MR_RECON_OP_NONE to cover OCE within the array for the LD
 *                     2. Added percentExpansion field in MR_RECON to cover the above
 *                     3. Added supportOCEwithNoDriveAddition to have the above support
 *                     4. Updated comments for MR_DCMD_LD_RECONS_START
 *                     5. Added MR_PD_STATE.MR_PD_STATE_SYSTEM
 *                     6. Added MR_PD_ALLOWED_OPS.makeSystem
 *                     7. Added enum MR_PD_QUERY_TYPE
 *                     8. Added DCMD MR_DCMD_PD_LIST_QUERY
 *                     9. Added event MR_EVT_CTRL_HOST_BUS_SCAN_REQUESTED
 *                    10. Added field MR_BIOS_DATA.bootDeviceIsPD
 *
 * 11/19/08 NN         1. Added field MR_BBU_FW_STATUS.periodicLearnRequired
 *                     2. Modified event description for
 *                              MR_EVT_PD_BBM_LOG_80_PERCENT_FULL
 *                              MR_EVT_LD_BBM_LOG_80_PERCENT_FULL
 *                              MR_EVT_PD_BBM_LOG_100_PERCENT_FULL
 *                              MR_EVT_LD_BBM_LOG_100_PERCENT_FULL
 *                     3. Added field
 *                              MFI_REG_STATE.operational.useMfiContextRegsAndMfiAbortRegs
 *                     4. Added structures, used in MFI_FRAMEs and MFI_CMDs
 *                              mfiContextRegs
 *                              mfiAbortRegs
 *
 * 12/19/08 NN         1. Updated DCMDs
 *                              MR_DCMD_CTRL_LOCK_KEY_CREATE
 *                              MR_DCMD_CTRL_LOCK_KEY_REKEY
 *                              MR_DCMD_CTRL_LOCK_KEY_VERIFY_ESCROW
 *                     2. Added new events
 *                              MR_EVT_CTRL_FACTORY_REPURPOSED
 *                              MR_EVT_CTRL_LOCK_KEY_BINDING_UPDATED
 *                     3. Added fields
 *                              MR_CTRL_PROP.OnOffProperties.enableSecretKeyControl
 *                              MFI_REG_STATE.ready.hostMemRequired
 *                              MF_FLAGS.SGLIsIEEE
 *                      4. Added structures
 *                              IEEE_SGE_32
 *                              IEEE_SGE_64
 *
 * 01/06/09 BH         1. Updated DCMDs
 *                              MR_DCMD_CTRL_MFC_DEFAULTS_SET which was deprecated is re-instated
 *                     2. Added fields
 *                              allowUnCertifiedHDDs in MR_MFC_DEFAULTS
 *                              notCertified in MR_PD_INFO
 *                     3. Updated structure comments
 *                              Update comments of notSupported PD variable to reflect inclusion of non-certified drives
 *
 * 02/11/09 NN         1. Added support for big-endian bit field definitions to MR API
 *                        set compile flag MFI_BIG_ENDIAN in makefile when including API if compiling for
 *                        big-endian machine.
 *
 * 04/01/09 NN         1. Added error code MFI_STAT_CTRL_RESET_REQUIRED
 *                     2. Added fields
 *                              MFI_REG_STATE.fault.resetRequired
 *                              MFI_CONTROL_REG.resetAdapter
 *                              MR_PD_INFO.bridgeVendor
 *                              MR_PD_INFO.bridgeProductIdentification
 *                              MR_PD_INFO.bridgeProductRevisionLevel
 *                              MR_PD_INFO.satBridgeExists
 *
 * 05/16/09 NN         1. Added fields in MR_MFC_DEFAULTS
 *                              treatR1EAsR10
 *                              maxLdsPerArray
 *                     2. Fixed typo in description of event MR_EVT_PD_REASSIGN_WRITE_FAILED
 *
 * 07/30/09 NN         1. Added fields in MR_MFC_DEFAULTS
 *                              failPdOnSMARTer
 *                              nonRevertibleSpares
 *
 * 07/31/09 NN         1. Added fields
 *                              MR_MFC_DEFAULTS.disableOnlineCtrlReset
 *                              MR_CTRL_PROP.OnOffProperties.disableOnlineCtrlReset
 *                              MR_CTRL_PROP.OnOffProperties.allowBootWithPinnedCache
 *                              MR_CTRL_INFO.adapterOperations.supportEKM
 *                              MR_CTRL_INFO.adapterStatus.lockKeyRebootRequired
 *                              MR_PD_INFO.security.needsEKM
 *                              MR_BIOS_DATA.EKMStatus
 *                              MFI_REG_STATE.fault.resetAdapter
 *                      2. Added enumerated value
 *                              MR_CTRL_LOCK_KEY_BINDING.MR_CTRL_LOCK_KEY_BINDING
 *                              MR_PD_QUERY_TYPE.MR_PD_QUERY_TYPE_SECURITY_KEY_TYPE
 *                      3. Added enum
 *                              MR_SECURITY_KEY_TYPE
 *                              MR_SECURITY_EKM_STATUS
 *                      4. Added DCMD MR_DCMD_PD_LOCK_KEY_APPLY
 *                      5. Updated DCMD MR_DCMD_CTRL_LOCK_KEY_CREATE
 *                      6. Added event argument MR_EVT_ARGS_KEYINFO
 *                      7. Added events
 *                              MR_EVT_CTRL_LOCK_KEY_EKM_MODE
 *                              MR_EVT_CTRL_LOCK_KEY_EKM_FAILURE
 *                              MR_EVT_PD_LOCK_KEY_REQUIRED
 *                              MR_EVT_LD_SECURE_FAILED
 *                              MR_EVT_CTRL_ONLINE_RESET
 *                      8. Added error codes
 *                              MFI_STAT_LOCK_KEY_EKM_NO_BOOT_AGENT
 *                      9. Added structure
 *                              MFI_REG_STATE.bootMsgPending
 *                              MR_CTRL_LOCK_KEY
 *
 * 08/25/09 NN          1. Added field MR_REG_STAT.operational.supportIEEE
 *
 * 09/10/09 BH          1. Added error codes
 *                              MFI_STAT_SNAP_NO_SPACE
 *                              MFI_STAT_SNAP_PARTIAL_FAILURE
 *                      2. Added new events
 *                              MR_EVT_LD_SNAPSHOT_ENABLED
 *                              MR_EVT_LD_SNAPSHOT_DISABLED_BY_USER
 *                              MR_EVT_LD_SNAPSHOT_DISABLED_INTERNALLY
 *                              MR_EVT_LD_SNAPSHOT_PIT_CREATED
 *                              MR_EVT_LD_SNAPSHOT_PIT_DELETED
 *                              MR_EVT_LD_SNAPSHOT_VIEW_CREATED
 *                              MR_EVT_LD_SNAPSHOT_VIEW_DELETED
 *                              MR_EVT_LD_SNAPSHOT_ROLLBACK_STARTED
 *                              MR_EVT_LD_SNAPSHOT_ROLLBACK_ABORTED
 *                              MR_EVT_LD_SNAPSHOT_ROLLBACK_COMPLETED
 *                              MR_EVT_LD_SNAPSHOT_ROLLBACK_PROGRESS
 *                              MR_EVT_LD_SNAPSHOT_REPOSITORY_80_PERCENT_FULL
 *                              MR_EVT_LD_SNAPSHOT_REPOSITORY_FULL
 *                              MR_EVT_LD_SNAPSHOT_VIEW_80_PERCENT_FULL
 *                              MR_EVT_LD_SNAPSHOT_VIEW_FULL
 *                              MR_EVT_LD_SNAPSHOT_REPOSITORY_LOST
 *                              MR_EVT_LD_SNAPSHOT_REPOSITORY_RESTORED
 *                              MR_EVT_LD_SNAPSHOT_INTERNAL_ERROR
 *                              MR_EVT_LD_SNAPSHOT_AUTO_ENABLED
 *                              MR_EVT_LD_SNAPSHOT_AUTO_DISABLED
 *                              MR_EVT_CFG_CMD_LOST
 *                              MR_EVT_PD_COD_STALE
 *                              MR_EVT_PD_POWER_STATE_CHANGE_FAILED
 *                              MR_EVT_LD_NOT_READY
 *                              MR_EVT_LD_IS_READY
 *                              MR_EVT_LD_IS_SSC
 *                              MR_EVT_LD_IS_USING_SSC
 *                              MR_EVT_LD_IS_NOT_USING_SSC
 *                      3. Added fields
 *                              MR_MFC_DEFAULTS.snapVDSpace
 *                              MR_MFC_DEFAULTS.autoSnapVDSpace;
 *                              MR_MFC_DEFAULTS.snapRespositoryPRL;
 *                              MR_MFC_DEFAULTS.viewSpace
 *                              MR_MFC_DEFAULTS.spinDownTime
 *                              MR_MFC_DEFAULTS.disablePowerSavings
 *                              MR_MFC_DEFAULTS.enableJBOD
 *                              MR_CTRL_PROP.OnOffProperties.disableSpinDownHS
 *                              MR_CTRL_PROP.OnOffProperties.enableJBOD
 *                              MR_CTRL_PROP.snapVDSpace
 *                              MR_CTRL_PROP.autoSnapVDSpace
 *                              MR_CTRL_PROP.viewSpace
 *                              MR_CTRL_PROP.spinDownTime
 *                              MR_CTRL_INFO.adapterOperations.supportSnapshot
 *                              MR_CTRL_INFO.adapterStatus.rollbackInProgress
 *                              MR_CTRL_INFO.maxTargetId
 *                              MR_CTRL_INFO.maxBvlVdSize
 *                              MR_CTRL_INFO.maxConfigurableSSCSize
 *                              MR_CTRL_INFO.currentSSCsize
 *                              MR_LD_PROPERTIES.autoSnapshot
 *                              MR_LD_PROPERTIES.disableAutoDeleteOldestPIT
 *                              MR_LD_PARAMETERS.snapshotEnabled
 *                              MR_LD_PARAMETERS.isSSCD
 *                              MR_LD_PARAMETERS.notReady
 *                              MR_LD_ALLOWED_OPS.enableSnapshot
 *                              MR_LD_ALLOWED_OPS.disableSnapshot
 *                              MR_LD_ALLOWED_OPS.rollbackPIT
 *                              MR_LD_ALLOWED_OPS.createPITView
 *                              MR_LD_ALLOWED_OPS.deleteView
 *                              MR_LD_ALLOWED_OPS.enableAutoSnapshot
 *                              MR_LD_ALLOWED_OPS.disableAutoSnapshot
 *                              MR_LD_ALLOWED_OPS.SSCCacheable
 *                              MR_LD_INFO.snapshotRepository
 *                              MR_LD_INFO.autoSnapshotEnabled
 *                              MR_SPAN.stateValid
 *                              MR_SPAN.state
 *                      4. Added new structures
 *                              MR_LD_SNAPSHOT_ENABLE_PARAMS
 *                              MR_LD_SNAPSHOT_PIT_VIEW_PARAMS
 *                              MR_LD_SNAPSHOT_PIT_CREATE_DATA
 *                              MR_LD_PIT_CREATE_INFO,MR_LD_PIT_ROLLBACK_INFO
 *                              MR_LD_VIEW_CREATE_DATA
 *                              MR_LD_VIEW_CREATE_INFO
 *                              MR_LD_VIEW_PIT_CREATE_INFO
 *                              MR_LD_PIT_INFO
 *                              MR_LD_VIEW_INFO
 *                              MR_LD_VIEW_LIST
 *                              MR_LD_SNAPSHOT_INFO
 *                              MR_LD_SNAPSHOT_ALL_INFO
 *                              MR_LD_SNAPSHOT_REPOSITORY_HOLE_INFO
 *                              MR_LD_SNAPSHOT_REPOSITORY_SPACE
 *                      5. Added new DCMDs
 *                              MR_DCMD_LD_SNAPSHOT_ENABLE
 *                              MR_DCMD_LD_SNAPSHOT_DISABLE
 *                              MR_DCMD_LD_SNAPSHOT_PIT_CREATE
 *                              MR_DCMD_LD_SNAPSHOT_PIT_DELETE
 *                              MR_DCMD_LD_SNAPSHOT_VIEW_CREATE
 *                              MR_DCMD_LD_SNAPSHOT_VIEW_DELETE
 *                              MR_DCMD_LD_SNAPSHOT_PIT_ROLLBACK
 *                              MR_DCMD_LD_SNAPSHOT_GET_INFO
 *                              MR_DCMD_LD_SNAPSHOT_VIEW_GET_INFO
 *                              MR_DCMD_LD_SNAPSHOT_GET_VIEW_LIST
 *                              MR_DCMD_LD_SNAPSHOT_REPOSITORY_GET_LDS
 *                              MR_DCMD_LD_SNAPSHOT_REPOSITORY_GET_SPACE
 *                              MR_DCMD_LD_SNAPSHOT_GARBAGE_COLLECTION
 *                              MR_DCMD_LD_LIST_QUERY
 *                              MR_DCMD_LD_SSCD_SET_LDS
 *                              MR_DCMD_LD_SSCD_GET_LDS
 *                      6. Added enum
 *                              MR_LD_QUERY_TYPE
 *                              MR_PD_POWER_STATE.MR_PD_POWER_STATE_TRANSITIONING
 *                      7. Added event arguments
 *                              MR_EVT_ARGS_LD_LD
 *                              MR_EVT_ARGS_LD_PIT
 *                              MR_EVT_ARGS_LD_VIEW
 *                              MR_EVT_ARGS_LD_SNAP_PROG
 *
 * 09/23/09 NN          1. Added new events
 *                              MR_EVT_LD_SNAPSHOT_FREED_RESOURCE
 *                              MR_EVT_LD_SNAPSHOT_AUTO_FAILED
 *
 * 10/19/09 BH          1. Removed snapRespositoryPRL from MFC defaults
 *                      2. Padding correction
 *
 * 11/10/09 NN          1. Added fields
 *                              MR_CTRL_PROP.OnOffProperties.disableCacheBypass
 *                              MR_CTRL_INFO.hwPresent.expander
 *                              MR_CTRL_INFO.expanderFwVersion
 *                              MR_PD_INFO.interfaceType
 *                              MR_SAS_CONNECTOR_INFO.quadPhyMap
 *                              MR_MFC_DEFAULTS.disableCacheBypass
 *                      2. Added event MR_EVT_CTRL_RESET_EXPANDER
 *                      3. Re-modified for padding MR_MFC_DEFAULT.enableJBOD
 *                      4. Updated event arguments to correct for padding issues.
 *                              MR_EVT_ARGS_KEYINFO
 *                              MR_EVT_ARGS_LD_PIT
 *
 * 12/07/09 NN          1. Added fields
 *                              MR_CTRL_INFO.hwPresent.upgradeKey
 *                              MR_BIOS_DATA.autoSelectBootLd
 *                      2. Updated structure MR_MFC_DEFAULTS to be variable length
 *                              MR_MFC_DEFAULTS.length
 *                              MR_MFC_DEFAULTS.ttyLogInFlash
 *                      3. Added new event MR_EVT_LD_SSC_SIZE_CHANGED
 *                      4. Updated event MR_EVT_LD_IS_SSC
 *
 * 01/19/10 NN          1. Added enum type MR_BBU_TYPE.MR_BBU_TYPE_iBBU08 enum type
 *                      2. Added new fields
 *                              MR_BBU_FW_STATUS.transparentLearn
 *                              MR_BBU_DESIGN_INFO.bbuMode
 *                              MR_BBU_DESIGN_INFO.transparentLearns
 *                              MR_IBBU_STATE.mrIbbu08ChargerStatus
 *                      3. Added event MR_EVT_BBU_CANNOT_DO_TRANSPARENT_LEARN
 *
 * 03/08/10 NN          1. Added fields
 *                              MR_CTRL_INFO.pci.ctrlId
 *                              MR_CTRL_INFO.hwPresent.upgradeKeyNotSupported
 *                              MR_CTRL_INFO.adapterOperations.supportPFK
 *                              MR_CTRL_INFO.adapterStatus.PFKInNVRAM
 *                              MR_CTRL_INFO.PFKTrialTimeRemaining
 *                              MR_LD_SNAPSHOT_INFO.lastSnapSchedMissed
 *                              MR_LD_SNAPSHOT_INFO.schedProp
 *                              MR_MFC_DEFAULTS.detectCMETimer
 *                      2. Added enum types
 *                              MR_PF_LOCALE
 *                              MR_PF_ACTION
 *                              MR_SNAPSHOT_SCHEDULE_STATE
 *                      3. Added structures
 *                              MR_PREMIUM_FEATURES
 *                              MR_PF_INFO
 *                              MR_PF_LIST
 *                              MR_SNAPSHOT_SCHEDULE_PROPERTIES
 *                      4. Added DCMDs
 *                              MR_DCMD_CTRL_PFK_FEATURE_GET
 *                              MR_DCMD_CTRL_PFK_FEATURE_SET
 *                              MR_DCMD_CTRL_PFK_SAFEID_GET
 *                              MR_DCMD_CTRL_PFK_DEACTIVATE_TRIAL
 *                              MR_DCMD_CTRL_PFK_FEATURE_TRANSFER
 *                              MR_DCMD_LD_SNAPSHOT_SCHEDULE_GET_PROPERTIES
 *                              MR_DCMD_LD_SNAPSHOT_SCHEDULE_SET_PROPERTIES
 *                      5. Added event argument MR_EVT_ARGS_PFINFO
 *                      6. Added events
 *                              MR_EVT_CTRL_PFK_APPLIED
 *                              MR_EVT_LD_SNAPSHOT_SCHEDULE_PROP_CHANGE
 *                              MR_EVT_LD_SNAPSHOT_SCHEDULED_ACTION_DUE
 *                      7. Modified event descriptions replaced SSC with CacheCade(TM)
 *                      8. Added error codes
 *                              MFI_STAT_UPGRADE_KEY_INCOMPATIBLE
 *                              MFI_STAT_PFK_INCOMPATIBLE
 *                              MFI_STAT_PD_MAX_UNCONFIGURED
 *
 *
 * 03/18/10 NN          1. Added fields
 *                              MR_CTRL_INFO.ldOperations.supportFastPath
 *                              MR_CTRL_INFO.ldOperations.performanceMetrics
 *                              MR_CTRL_INFO.adapterOperations2
 *                              MR_CTRL_INFO.cacheMemorySize
 *                      2. Added structures
 *                              MR_IO_METRICS_SIZE
 *                              MR_IO_METRICS_RANDOMNESS
 *                              MR_IO_METRICS_LD_CACHE
 *                              MR_IO_METRICS_CACHE
 *                              MR_IO_METRICS_LD_OVERALL
 *                              MR_IO_METRICS_LD_OVERALL_LIST
 *                              MR_IO_METRICS
 *                      3. Added event  MR_EVT_CTRL_PERF_COLLECTION
 *                      4. Modified event class for MR_EVT_LD_SNAPSHOT_ROLLBACK_PROGRESS
 *                      5. Added error codes
 *                              MFI_STAT_IO_METRICS_DISABLED
 *                      6. Added DCMDs
 *                              MR_DCMD_CTRL_IO_METRICS_ENABLE
 *                              MR_DCMD_CTRL_IO_METRICS_GET
 *
 * 05/05/10 NN          1. Modified DCMD MR_DCMD_CTRL_MFC_MFG_GET
 *                      2. Added events
 *                              MR_EVT_CTRL_PFK_TRANSFERRED
 *                              MR_EVT_CTRL_PFK_SERIALNUM
 *                              MR_EVT_CTRL_PFK_SERIALNUM_MISMATCH
 *
 * 05/10/10 NN          1. Added fields
 *                              MR_CTRL_PROP.OnOffProperties.useDiskActivityForLocate
 *                              MR_BBU_PROPERTIES.bbuMode
 *
 * 06/07/10 NN          1. Added fields
 *                              MR_CTRL_PROP.defaultLdPSPolicy
 *                              MR_CTRL_PROP.disableLdPSInterval
 *                              MR_CTRL_PROP.disableLdPSTime
 *                              MR_CTRL_PROP.spinupEnclDriveCount
 *                              MR_CTRL_PROP.spinupEnclDelay
 *                              MR_CTRL_INFO.hwPresent.tempSensorROC
 *                              MR_CTRL_INFO.hwPresent.tempSensorCtrl
 *                              MR_CTRL_INFO.ldOperations.supportPowerSavings
 *                              MR_CTRL_INFO.ldOperations.supportPSMaxWithCache
 *                              MR_CTRL_INFO.ldOperations.supportBreakMirror
 *                              MR_CTRL_INFO.pdOperations.supportT10PowerState
 *                              MR_CTRL_INFO.pdOperations.supportTemperature
 *                              MR_CTRL_INFO.driverVersion
 *                              MR_CTRL_INFO.maxDAPdCountSpinup60
 *                              MR_CTRL_INFO.temperatureROC
 *                              MR_CTRL_INFO.temperatureCtrl
 *                              MR_PD_ALLOWED_OPS.T10PowerModeSupported
 *                              MR_PD_INFO.pathInfo.widePortCapable
 *                              MR_PD_INFO.temperature
 *                              MR_LD_PROPERTIES.defaultPSPolicy
 *                              MR_LD_PROPERTIES.currentPSPolicy
 *                              MR_LD_PARAMETERS.powerSavingOptions
 *                              MR_LD_ALLOWED_OPS.breakMirror
 *                              MR_LD_ALLOWED_OPS.joinMirror
 *                              MR_ENCL_INFO.pdCountSpinup60
 *                              MR_MFC_DEFAULTS.autoEnhancedImport
 *                              MR_MFC_DEFAULTS.breakMirrorRAIDSupport
 *                              MR_MFC_DEFAULTS.disableJoinMirror
 *                              MFI_CMD_INIT.driverVersion
 *                              MFI_FRAME_INIT.driverVersion
 *                      2. Updated field MR_MFC_DEFAULTS.disablePowerSavings
 *                      3. Added enum types
 *                              MR_LD_PS_PARAMS
 *                              MR_LD_PS_POLICY
 *                      4. Added DCMDs
 *                              MR_DCMD_LD_MIRROR_BREAK
 *                              MR_DCMD_LD_MIRROR_VIEW
 *                              MR_DCMD_LD_MIRROR_JOIN
 *                      5. Added event argument MR_EVT_ARGS_LD_PROP_PS
 *                      6. Added events
 *                              MR_EVT_BBU_REPLACEMENT_NEEDED_SOH_NOT_OPTIMAL
 *                              MR_EVT_LD_POWER_STATE_CHANGE
 *                              MR_EVT_LD_POWER_STATE_MAX_UNAVAILABLE
 *                              MR_EVT_CTRL_HOST_DRIVER_RELOADED
 *                              MR_EVT_LD_MIRROR_BROKEN
 *                              MR_EVT_LD_MIRROR_JOINED
 *                              MR_EVT_PD_SAS_WIDE_PORT_LINK_FAILURE
 *                              MR_EVT_PD_SAS_WIDE_PORT_LINK_RESTORED
 *                      7. Modified event description for
 *                              MR_EVT_BBU_REPLACEMENT_NEEDED_SOH_BAD
 *                              MR_EVT_LD_PROP_CHANGE
 *
 * 07/06/10 AT          1. Added DCMD MR_DCMD_SAS_CONFIG_SET_LINK_SPEED
 *                      2. Added enum MR_SAS_PHY_LINK_SPEED
 *                      3. Added fields
 *                              MR_SAS_PHY_CONNECTION.sasPortNumber
 *                              MR_SAS_PHY_CONNECTION.sasPortNumberValid
 *                              MR_SAS_PHY_CONNECTION.linkSpeedControl
 *                              MR_MFC_MFG_2.tmmFRU
 *                              MR_PREMIUM_FEATURES.CacheOffload
 *                              MR_BBU_DESIGN_INFO.moduleVersion
 *                              MR_BBU_STATUS_DETAIL.tmmc
 *                              MR_BBU_TYPE.MR_BBU_TYPE_TMMC
 *                              MR_BBU_FW_STATUS.noSpace
 *                              MR_BBU_FW_STATUS.predictiveFailure
 *                              MR_BBU_FW_STATUS.premiumFeatureReqd
 *                      4. Added structure MR_TMMC_STATE
 *                      5. Added define MR_BBU_STATUS_BAD_FOR_TMMC
 *                      6. Modified comments for DCMD MR_DCMD_BBU_GET_CAPACITY_INFO
 *                      7. Added events
 *                             MR_EVT_TMM_FRU
 *                             MR_EVT_BBU_REPLACEMENT_NEEDED
 *                             MR_EVT_FOREIGN_CFG_AUTO_IMPORT_NONE
 *
 * 07/08/10 AT          1. Added new DCMDs
 *                             MR_DCMD_CTRL_AEC_CONTROL
 *                             MR_DCMD_CTRL_AEC_GET
 *                             MR_DCMD_CTRL_AEC_GET_SETTINGS
 *                             MR_DCMD_DPM_GET_CFG
 *                             MR_DCMD_DPM_GET_DATA
 *                             MR_DCMD_DPM_GET_CTRL
 *                      2. Add new enums
 *                             MR_DPM_DATA_TYPE
 *                             MR_DPM_DATA_CTRL
 *                             MR_AEC_CAPTURE_MODE
 *                             MR_AEC_CAPTURE_CTRL
 *                             MR_AEC_GROUP_NUM
 *                             MR_AEC_HOST_IO_FM
 *                             MR_AEC_FW_IO_FM
 *                      3. Add new structures/unions
 *                             MR_DPM_DATA_CLEAR
 *                             MR_DPM_CFG
 *                             MR_DPM_DATA
 *                             MR_DPM_DATA_EX
 *                             MR_DPM_RUNAVG
 *                             MR_DPM_TOTALS
 *                             MR_DPM_SUMMARY
 *                             MR_AEC_SETTINGS
 *                      4. Add MFI_STAT_AEC_NOT_STOPPED command completion code.
 *
 * 07/20/10 NN          1. Renamed MR_CTRL_PROP.spinupEnclDriveCoun to spinupEnclDriveCount
 *                      2. Added field MR_BBU_FW_STATUS.microcodeUpdateReqd
 *                      3. Modified MR_BBU_STATUS_BAD_FOR_TMMC
 *                      4. Added event MR_EVT_BBU_MICROCODE_UPDATE_REQUIRED
 *                      5. Modified event descriptions
 *                              MR_EVT_CTRL_PFK_APPLIED
 *                              MR_EVT_CTRL_PFK_TRANSFERRED
 *                              MR_EVT_CTRL_PFK_SERIALNUM
 *                              MR_EVT_CTRL_PFK_SERIALNUM_MISMATCH
 *
 * 07/20/10 JR          1. Added new MR-MP2 structures
 *                             MR_MPI2_MFA_REQUEST_DESCRIPTOR
 *                             MPI2_SCSI_IO_VENDOR_UNIQUE
 *                             MPI25_SCSI_IO_VENDOR_UNIQUE
 *                      2. Added new enum for RAID Context region types
 *                             MR_RAID_CTX_REGION_TYPE
 *                      3. Added function codes for MR-MPI2 SCSI IO messages
 *                             MR_MPI2_FUNCTION_MFI_PASSTHRU_REQUEST
 *                             MR_MPI2_FUNCTION_LD_IO_REQUEST
 *                      4. Added descriptor types for MR-MPI2 requests
 *                             MR_MPI2_REQ_DESCRIPT_FLAGS_MPT
 *                             MR_MPI2_REQ_DESCRIPT_FLAGS_MFA
 *                             MR_MPI2_REQ_DESCRIPT_FLAGS_FP_IO
 *                             MR_MPI2_REQ_DESCRIPT_FLAGS_LD_IO
 *
 * 08/03/10 NN          1. Added struct MR_PF_FEATURE_SET_PARAMS
 *                      2. Modified comments for DCMD MR_DCMD_CTRL_PFK_FEATURE_SET
 *                      3. Renamed MR_MFC_DEFAULTS.breakMirrorRAIDsupport to MR_MFC_DEFAULTS.breakMirrorRAIDSupport
 *                      4. Added event MR_EVT_LD_SSC_SIZE_EXCEEDED
 *                      5. Modified event descriptions
 *                              MR_EVT_CTRL_LOCK_KEY_EKM_MODE
 *                              MR_EVT_CTRL_LOCK_KEY_EKM_FAILURE
 *                      6. Added enum
 *                              MR_LD_MIRROR_OP
 *                              MFI_WAIT_DATA
 *
 * 08/04/10 NN          1. Added new enums
 *                              MR_OOB_OBJ_TYPE
 *                              MFI_OOB_CONTROL_CMD
 *                              MFI_OOB_STATUS
 *                              MR_EVT_OOB_LOCALE
 *                      2. Added structures
 *                              MR_OOB_TYPE_BITMAP
 *                              MR_OOB_OBJ_RECORD
 *                              MR_OOB_OBJ_INFO
 *                              MR_OOB_OBJ_LIST
 *                              MR_OOB_PARAMS
 *                              MFI_OOB_LSI_HDR
 *                              MFI_OOB_CMD_PACKET1
 *                              MFI_OOB_CMD_PACKETX
 *                              MFI_OOB_RESP_PACKET1
 *                              MFI_OOB_RESP_PACKETX
 *                              MFI_OOB_LSI_PKT
 *                              MFI_OOB_MCTP_HDR
 *                              MFI_OOB_MCTP_PACKET
 *                      3. Updated DCMDs
 *                              MR_DCMD_CTRL_EVENT_CLEAR
 *                              MR_DCMD_CTRL_EVENT_GET
 *                      4. Added DCMDs
 *                              MR_DCMD_OOB_STATUS
 *                              MR_DCMD_OOB_PARAM_SET
 *                              MR_DCMD_OOB_STATUS_POLL
 *                      5. Added fields
 *                              MR_EVT_CLASS_LOCALE.oobLocale
 *                              MR_EVT_DESCRIPTION.oobLocale and related defines
 *                      6. Updated EventDescription initialization list with oobLocale values
 *
 * 08/12/10 NN          1. Incorporated review comments to OOB API update on 8/04/10
 *                              Added comments, fixed typos and updated OOBLocale info for events
 *                              Modified struct MR_OOB_OBJ_LIST
 *                      2. Updated argument for event MR_EVT_LD_SSC_SIZE_EXCEEDED
 *                      3. Modified event description for MR_EVT_CTRL_LOCK_KEY_EKM_MODE
 *
 * 08/21/10 JR          1. Added configSeqNum to MPI2_SCSI_IO_VENDOR_UNIQUE
 *                      2. Added MAX_RAIDMAP* defines
 *
 * 08/24/10 NN          1. Added enum MR_SSC_WINDOW_ACCESS_RATE
 *                      2. Added structures
 *                              MR_IO_METRICS_SSC_WINDOW_USAGE
 *                              MR_IO_METRICS_SSC_WINDOW_ACCESS
 *                              MR_IO_METRICS_SSC_DETAIL
 *                      3. Added fields
 *                              MR_CTRL_INFO.ldOperations.supportSSCWriteBack
 *                              MR_CTRL_INFO.ldOperations.supportSSCRaid5
 *                              MR_IO_METRICS_CACHE.countSSCDetail
 *                              MR_IO_METRICS_CACHE.SSCDetail
 *
 * 08/24/10 JR          1. Added fields
 *                              MR_CTRL_PROP.OnOffProperties.preventPIImport
 *                              MR_CTRL_PROP.OnOffProperties.disablePI(now enablePI)
 *                              MR_PD_INFO.properties.piType
 *                              MR_PD_INFO.properties.piFormatted
 *                              MR_PD_INFO.properties.piEligible
 *                              MR_PD_INFO.userDataBlockSize
 *                              MR_LD_PARAMETERS.piType
 *                              MR_MFC_DEFAULTS.disablePI(now enablePI)
 *                              MR_MFC_DEFAULTS.preventPIImport
 *                      2. Added event
 *                              MR_EVT_LD_PI_PROTECTION_WILL_BE_LOST
 *                      3. Added status types
 *                              MFI_STAT_PI_TYPE_WRONG
 *                              MFI_STAT_LD_PD_PI_INCOMPATIBLE
 *                              MFI_STAT_PI_NOT_ENABLED
 *                              MFI_STAT_LD_BLOCK_SIZE_MISMATCH
 *                      4. Added enum
 *                              MR_PROT_INFO_TYPE
 *                              MR_RAID_FLAGS_IO_SUB_TYPE
 *
 * 09/07/10 NN          1. Modified enum MR_LD_PS_POLICY
 *
 * 09/21/10 JR          1. Added fields
 *                              MR_MFC_DEFAULTS.enablePI(in place of previous disablePI)
 *                              MR_CTRL_PROP.OnOffProperties.enablePI(in place of previous disablePI)
 *                              MR_CTRL_INFO.adapterOperations2.supportPIcontroller
 *                              MR_CTRL_INFO.adapterOperations2.supportLdPIType1
 *                              MR_CTRL_INFO.adapterOperations2.supportLdPIType2
 *                              MR_CTRL_INFO.adapterOperations2.supportLdPIType3
 *                      2. Removed field
 *                              MR_CTRL_PROP.ldOperations.supportPI
 * 09/30/10 BH          1. Added DCMDs
 *                              MR_DCMD_CTRL_PR_SUSPEND_RESUME
 *                              MR_DCMD_CTRL_DIAG_SERVER_POWER_TEST
 *                              MR_DCMD_PD_REBUILD_SUSPEND_RESUME
 *                              MR_DCMD_PD_COPYBACK_SUSPEND_RESUME
 *                              MR_DCMD_PD_SECURE_ERASE_START
 *                              MR_DCMD_PD_SECURE_ERASE_ABORT
 *                              MR_DCMD_LD_BBM_LOG_GET
 *                              MR_LD_BBM_LOG_LIST_GET
 *                              MR_DCMD_LD_RECONS_SUSPEND_RESUME
 *                              MR_DCMD_LD_CC_SUSPEND_RESUME
 *                              MR_DCMD_LD_ERASE_START
 *                              MR_DCMD_LD_ERASE_ABORT
 *                      2. Added fields
 *                              MR_MFC_DEFAULTS.enablePI(in place of previous disablePI)
 *                              MR_CTRL_PROP.OnOffProperties.useGlobalSparesForEmergency
 *                              MR_CTRL_PROP.OnOffProperties.useUnconfGoodForEmergency
 *                              MR_CTRL_PROP.OnOffProperties.useEmergencySparesforSMARTer
 *                              MR_CTRL_INFO.adapterOperations2.supportLdBBMInfo
 *                              MR_CTRL_INFO.adapterOperations2.supportShieldState
 *                              MR_CTRL_INFO.adapterOperations2.blockSSDWriteCacheChange
 *                              MR_CTRL_INFO.adapterOperations2.supportSuspendResumeBGops
 *                              MR_CTRL_INFO.adapterOperations2.supportEmergencySpares
 *                              MR_PD_PROGRESS.active.erase
 *                              MR_PD_PROGRESS.erase
 *                              MR_LD_PROGRESS.active.erase
 *                              MR_LD_PROGRESS.erase
 *                              MR_PD_ALLOWED_OPS.suspendRebuild
 *                              MR_PD_ALLOWED_OPS.resumeRebuild
 *                              MR_PD_ALLOWED_OPS.suspendCopyback
 *                              MR_PD_ALLOWED_OPS.resumeCopyback
 *                              MR_PD_ALLOWED_OPS.startSecureEraseNonSED
 *                              MR_PD_ALLOWED_OPS.stopSecureEraseNonSED
 *                              MR_PD_INFO.properties.commissionedSpare
 *                              MR_PD_INFO.properties.emergencySpare
 *                              MR_PD_INFO.properties.NCQ
 *                              MR_PD_INFO.properties.WCE
 *                              MR_PD_INFO.properties.shieldDiagCompletionTime
 *                              MR_PD_INFO.properties.shieldCounter
 *                              MR_LD_ALLOWED_OPS.suspendCCBGI
 *                              MR_LD_ALLOWED_OPS.resumeCCBGI
 *                              MR_LD_ALLOWED_OPS.suspendRecon
 *                              MR_LD_ALLOWED_OPS.resumeRecon
 *                              MR_LD_ALLOWED_OPS.startSecureEraseNonSED
 *                              MR_LD_ALLOWED_OPS.stopSecureEraseNonSED
 *                              MR_MFC_DEFAULTS.enableShieldState
 *                              MR_MFC_DEFAULTS.enableDriveWCEforRebuild
 *                              MR_MFC_DEFAULTS.enableEmergencySpare
 *                              MR_MFC_DEFAULTS.useGlobalSparesForEmergency
 *                              MR_MFC_DEFAULTS.useUnconfGoodForEmergency
 *                              MR_IBBU_STATE.retentionTime
 *                      3. Added enum
 *                              MR_ERASE_TYPE
 *                              MR_PD_STATE_SHIELD_UNCONFIGURED
 *                              MR_PD_STATE_SHIELD_HOT_SPARE
 *                              MR_PD_STATE_SHIELD_CONFIGURED
 *                              MR_PD_STATE_SHIELD_COPYBACK
 *
 *                      4. Comments edited for
 *                              MR_PD_INFO.linkSpeed
 *                      5. Added structures
 *                              MR_LD_BBM_LOG
 *                              MR_LD_BBM_LOG_LIST
 *                      6. Added Events
 *                          MR_EVT_PD_SHIELD_DIAG_PASS
 *                          MR_EVT_PD_SHIELD_DIAG_FAIL
 *                          MR_EVT_CTRL_SERVER_POWER_DIAG_STARTED
 *                          MR_EVT_PD_RBLD_DRIVE_CACHE_ENABLED
 *                          MR_EVT_PD_RBLD_DRIVE_CACHE_RESTORED
 *                          MR_EVT_PD_EMERGENCY_SPARE_COMMISSIONED
 *                          MR_EVT_PD_EMERGENCY_SPARE_COMMISSIONED_REMINDER
 *                          MR_EVT_LD_CC_SUSPENDED
 *                          MR_EVT_LD_CC_RESUMED
 *                          MR_EVT_LD_BGI_SUSPENDED
 *                          MR_EVT_LD_BGI_RESUMED
 *                          MR_EVT_LD_RECON_SUSPENDED
 *                          MR_EVT_PD_RBLD_SUSPENDED
 *                          MR_EVT_PD_COPYBACK_SUSPENDED
 *                          MR_EVT_LD_CC_SUSPENDED_REMINDER
 *                          MR_EVT_LD_BGI_SUSPENDED_REMINDER
 *                          MR_EVT_LD_RECON_SUSPENDED_REMINDER
 *                          MR_EVT_PD_REBUILD_SUSPENDED_REMINDER
 *                          MR_EVT_PD_COPYBACK_SUSPENDED_REMINDER
 *                          MR_EVT_CTRL_PR_SUSPENDED_REMINDER
 *                          MR_EVT_PD_ERASE_ABORTED
 *                          MR_EVT_PD_ERASE_FAILED
 *                          MR_EVT_PD_ERASE_PROGRESS
 *                          MR_EVT_PD_ERASE_STARTED
 *                          MR_EVT_PD_ERASE_SUCCESSFUL
 *                          MR_EVT_LD_ERASE_ABORTED
 *                          MR_EVT_LD_ERASE_FAILED
 *                          MR_EVT_LD_ERASE_PROGRESS
 *                          MR_EVT_LD_ERASE_START
 *                          MR_EVT_LD_ERASE_SUCCESSFUL
 *                          MR_EVT_LD_ERASE_POTENTIAL_LEAKAGE
 *
 * 10/18/10 NN          1. Added enum MR_LD_PS_POLICY.MR_LD_PS_POLICY_T10
 *                      2. Added field MR_CTRL_INFO.pci.chipRevision
 *                      3. Added define MR_BBU_STATUS_BAD_FOR_IBBU08
 *
 * 11/10/10 NN          1. Added new fields
 *                              MR_CTRL_INFO.adapterStatus.SSCPolicyWB
 *                              MR_LD_PARAMETERS.accessPolicyStatus
 *                              MR_LD_PARAMETERS.currentAccessPolicy
 *                              MR_LD_PROGRESS.disassociate
 *                              MR_LD_PROGRESS.active.disassociate
 *                              MR_LD_ALLOWED_OPS.forceDeleteLD
 *                              MR_BBU_STATUS.subType
 *                              MR_BBU_DESIGN_INFO.appData
 *                              MR_BBU_STATE.badBlockCount
 *                      2. Updated comments for MR_BBU_STATE.gasGuageStatus
 *                      3. Added enum MR_LD_ACCESS_STATUS
 *                      4. Updated DCMD
 *                              MR_DCMD_LD_DELETE
 *                              MR_DCMD_CFG_CLEAR
 *                      5. Added error code MFI_STAT_LD_SSCD_CACHE_PRESENT
 *                      6. Added events
 *                              MR_EVT_BBU_CHARGE_DISABLED_OTC
 *                              MR_EVT_BBU_MICROCODE_UPDATED
 *                              MR_EVT_BBU_MICROCODE_UPDATE_FAILED
 *                              MR_EVT_LD_ACCESS_BLOCKED_SSC_OFFLINE
 *                              MR_EVT_LD_SSC_DISASSOCIATE_START
 *                              MR_EVT_LD_SSC_DISASSOCIATE_DONE
 *                              MR_EVT_LD_SSC_DISASSOCIATE_FAILED
 *                              MR_EVT_LD_SSC_DISASSOCIATE_PROGRESS
 *                              MR_EVT_LD_SSC_DISASSOCIATE_ABORT_BY_USER
 *
 * 12/20/10 BH          1. Added new fields
 *                              MR_LD_ALLOWED_OPS.fullFGInit
 *                              MR_LD_PROGRESS.pause
 *                              MR_PD_PROGRESS.pause
 *                              MR_CTRL_INFO.maxConfigurablePds
 *                              MR_CTRL_INFO.adapterOperations2.supportSetLinkSpeed
 *                              MR_CTRL_INFO.adapterOperations2.bootTimePFKSupport
 *                      2. Removed fields
 *                              MR_PD_STATE_SHIELD_COPYBACK
 *                      3. Renamed fields
 *                              MR_LD_ERASE to MR_LD_SECURE_ERASE
 *                      4. Updated event
 *                              EA to LD from None for LD_PI_LOST
 *                      5. Added event
 *                              MR_EVT_SAS_PHY_LINK_SPEED_UPDATED
 *                              MR_EVT_CTRL_PFK_DEACTIVATED
 *                              MR_EVT_LD_ACCESS_UNBLOCKED
 *
 * 01/07/11 NN          1. Modified MFI_FAULT, added MFI_FAULT_INIT
 *                      2. Added defines
 *                              MFI_FAULT_INIT_UNSUPPORTED_MEMORY
 *                              MFI_FAULT_INIT_BAD_MEMORY
 *                      3. Added events
 *                              MR_EVT_LD_IS_USING_SSC_2
 *                              MR_EVT_LD_IS_NOT_USING_SSC_2
 *
 * 01/14/11 BH          1. Rename fullFGInit to fullFGDisallow and updated comments
 *
 * 01/13/11 JR          1. Added fields
 *                              MR_CTRL_INFO.adapterOperations2.supportJBOD
 *                      2. Created new set of defines MAX_API_* to be used as the
 *                         API constraints for structures.  MAX_* are defined
 *                         to be equal to MAX_API_* for all compiles but FW where
 *                         the implemented max may be less than the API max
 *                      3. Removed MAX_RAIDMAP_* defines.  These are replaced with
 *                         MAX_API_* defines.
 *
 * 02/03/11 BH          1. Added new fields
 *                              MR_CTRL_INFO.adapterOperations2.disableOnlinePFKChange
 *                              MR_CTRL_INFO.adapterOperations2.supportPerfTuning
 *                              MR_CTRL_PROP.perfTuners
 *                              MR_CTRL_PROP.perfTunerValues
 *                      2. Added enum MR_PERF_TUNE_MODES
 *                      3. Renamed field: bootTimePFKSupport to supportBootTimePFKChange
 *
 * 02/24/11 BH          1. Added new DCMD
 *                         MR_DCMD_LD_CLEAR_BLOCKED
 *                      2. Added event
 *                         MR_EVT_PD_PR_ABORTED
 *
 * 03/07/11 AW          1. Added MR_CTRL_INFO.perfTunerValues[].maxPdLatencyMsBeforeIssuingOrderedTag
 *
 * 04/04/11 MI          1. Added a disassociatedSSC to ALLOWED_OPS
 *                      2. Modified MR_DCMD_LD_SSCD_GET_LDS DCMD with targetID in mbox[0].
 *
 * 04/07/11 MI          1. Addedd SSCWB bit to premium feature MR_PREMIUM_FEATURES
 *
 * 04/19/11 MI          1. Added MR_CTRL_INFO.ldOperations.supportSSCAssociation
 *
 * 05/09/11 MI          1. Added configuration support for SSCD
 *                              MR_CTRL_INFO.raidLevels.SSCraidLevel_0_unsupported
 *                              MR_CTRL_INFO.raidLevels.SSCraidLevel_1
 *                              MR_CTRL_INFO.raidLevels.SSCraidLevel_5
 *                              MR_CTRL_INFO.raidLevels.SSCraidLevel_1E
 *
 * 05/12/11 NN          1. Added new fields
 *                              MR_CTRL_PROP.OnOffProperties.forceSGPIOForQuadOnly
 *                              MR_MFC_DEFAULTS.blockSSDWriteCacheChange
 *                              MR_OOB_OBJ_RECORD.pdInfo.otherErr
 *                      2. Deprecated MR_CTRL_INFO.ldOperations.supportSSCRaid5
 *                      3. Added event MR_EVT_PD_TRANSIENT_ERROR_DETECTED
 *
 * 06/21/11 BH          1. Added new fields
 *                              MR_CTRL_INFO.adapterOperations2.supportSSDPatrolRead
 *
 * 07/21/11 JR          1. Added new field
 *                              MR_TMMC_STATE.packEnergyMSB
 *
 * 09/15/11 BH          1. Added new DCMD
 *                              MR_DCMD_LD_PI_DISABLE
 *                      2. Added new fields
 *                              MR_LD_ALLOWED_OPS.disablePI
 *                              MR_MFC_DEFAULTS.SMARTerEnabled
 *                              MR_MFC_DEFAULTS.SSDSMARTerEnabled
 *                      4. Added error code MFI_STAT_FLASH_UNSUPPORTED
 *                      3. Added events
 *                              MR_EVT_LD_PI_ERR_IN_CACHE
 *                              MR_EVT_CTRL_FLASH_IMAGE_UNSUPPORTED
 *
 * 10/13/11 NN          1. Added new structures
 *                              MR_BBU_MODE
 *                              MR_BBU_MODE_LIST
 *                      2. Added new DCMD MR_DCMD_BBU_MODES_GET
 *                      3. Added new event MR_EVT_BBU_MODE_SET
 *
 * 10/13/11 NK          1. Added new fields
 *                              MR_PD_PROGRESS.active.locate
 *                              MR_LD_PROGRESS.active.locate
 *                              MR_OOB_OBJ_RECORD.pdinfo.locate
 *
 * 01/17/12 NN          1. Added new field MR_CTRL_INFO.adapterOperations2.realTimeScheduler
 *                      2. Updated comment for
 *                              MR_BBU_PROPERTIES.nextLearnTime
 *                              MR_EVT_DETAILS.args.time
 *                      3. Added new event MR_EVT_BBU_PERIODIC_RELEARN_RESCHEDULED
 *
 * 02/16/12 NN          1. Added new field MR_CTRL_INFO.adapterOperations2.supportResetNow
 *                      2. Updated DCMDs
 *                              MR_DCMD_CTRL_FLASH_FW_FLASH
 *                              MR_DCMD_CTRL_SHUTDOWN
 *                      3. Added new tyepdef MFI_STP_FIS
 *                      4. Added support for MFI_CMD_OP_STP to MFI_OOB_LSI_PKT
 *                      5. Updated comments for MR_IBBU_STATE.gasGuageStatus
 *                      6. Updated event MR_EVT_PD_EMERGENCY_SPARE_COMMISSIONED
 *                      7. Added new events
 *                              MR_EVT_CTRL_RESETNOW_START
 *                              MR_EVT_CTRL_RESETNOW_DONE
 *                      8. Added error code MFI_STAT_RESETNOW_NOT_ALLOWED
 *
 * 02/22/12 NK          1. Added emulatedBlockSize to MR_PD_INFO
 *
 * 03/14/12 NN          1. Added enum MR_PCI_LINK_SPEED
 *                      2. Added new fields
 *                              MR_CTRL_INFO.hostInterface.maxPciLinkSpeed
 *                              MR_CTRL_INFO.hostInterface.pciLinkSpeed
 *                      3. Added new DCMD MR_DCMD_CTRL_MFC_PCI_LINK_SPEED_SET
 *
 * 03/28/12 NK          1. Added new field MR_CTRL_INFO.adapterOperations2.supportEmulatedDrives
 *
 * 04/11/12 NN          1. Added new fields
 *                              MR_CTRL_INFO.adapterOperations2.headlessMode
 *                              MR_CTRL_INFO.adapterStatus.headlessSafeMode
 *                      2. Added enum MR_BOOT_MODE
 *                      3. Updated comments for
 *                              MR_MFC_DEFAULTS.biosContinueOnError
 *                              MR_BIOS_DATA.continueOnError
 *                      4. Added events
 *                              MR_EVT_CTRL_BOOT_HEADLESS_MODE_HAD_ERRORS
 *                              MR_EVT_CTRL_BOOT_HEADLESS_SAFE_MODE_FOR_ERRORS
 *                              MR_EVT_CTRL_BOOT_HEADLESS_ERROR_WARNING
 *                              MR_EVT_CTRL_BOOT_HEADLESS_ERROR_CRITICAL
 *                              MR_EVT_CTRL_BOOT_HEADLESS_ERROR_FATAL
 *                      5. Added error code
 *                              MFI_STAT_HSM_MODE
 *                              MFI_STAT_SSC_MEMORY_NOT_AVAILABLE
 *
 * 05/15/12 NN          1. Deprecated fields
 *                              MR_CTRL_INFO.clusterPermitted
 *                              MR_CTRL_INFO.clusterActive
 *                              MR_LD_INFO.clusterOwnerDeviceId
 *                              MR_PREMIUM_FEATURES.cluster
 *                      2. Added new fields
 *                              MR_CTRL_INFO.maxHANodes
 *                              MR_CTRL_INFO.cluster
 *                              MR_CTRL_INFO.clusterID
 *                              MR_LD_PROPERTIES.exclusiveAccess
 *                              MR_LD_ALLOWED_OPS.enableHAShare
 *                              MR_LD_INFO.peerHasExclusiveAccess
 *                              MR_PREMIUM_FEATURES.maxHANodes
 *                              MR_EVT_ARG_LD.isRemote
 *                              MFI_FRAME_INIT.driverOperations
 *                      3. Added enum values
 *                              MR_LD_QUERY_TYPE_CLUSTER_ACCESS
 *                              MR_LD_QUERY_TYPE_CLUSTER_LOCALE
 *                      4. Added new structures
 *                              MR_LD_GUID
 *                              MR_LD_GUID_LIST
 *                              MFI_CAPABILITIES
 *                      5. Added DCMDs
 *                              MR_DCMD_CTRL_DOWNGRADE_MR_TO_iMR
 *                              MR_DCMD_LD_GET_GUID_LIST
 *                      6. Added events
 *                              MR_EVT_HA_NODE_JOIN
 *                              MR_EVT_HA_NODE_BREAK
 *                              MR_EVT_HA_PD_IS_REMOTE
 *                              MR_EVT_HA_PD_IS_LOCAL
 *                              MR_EVT_HA_LD_IS_REMOTE
 *                              MR_EVT_HA_LD_IS_LOCAL
 *                              MR_EVT_HA_LD_TARGET_ID_CONFLICT
 *                              MR_EVT_HA_LD_ACCESS_IS_SHARED
 *                              MR_EVT_HA_LD_ACCESS_IS_EXCLUSIVE
 *                              MR_EVT_HA_LD_INCOMPATIBLE
 *                              MR_EVT_HA_PEER_INCOMPATIBLE
 *                              MR_EVT_HA_HW_INCOMPATIBLE
 *                              MR_EVT_HA_CTRLPROP_INCOMPLATIBLE
 *                              MR_EVT_HA_FW_VERSION_MISMATCH
 *                              MR_EVT_HA_FEATURES_MISMATCH
 *                      7. Added error code MFI_STAT_PEER_INCOMPATIBLE
 *
 * 06/04/12 NN          1. Renamed field MR_CTRL_INFO.adapterStatus.headlessSafeMode
 *                         to  MR_CTRL_INFO.adapterStatus.safeModeBoot
 *                      2. Renamed events
 *                              MR_EVT_CTRL_BOOT_HEADLESS_SAFE_MODE_FOR_ERRORS to MR_EVT_CTRL_BOOT_SAFE_MODE_FOR_ERRORS
 *                              MR_EVT_CTRL_BOOT_HEADLESS_ERROR_WARNING to        MR_EVT_CTRL_BOOT_ERROR_WARNING
 *                              MR_EVT_CTRL_BOOT_HEADLESS_ERROR_CRITICAL to       MR_EVT_CTRL_BOOT_ERROR_CRITICAL
 *                              MR_EVT_CTRL_BOOT_HEADLESS_ERROR_FATAL to          MR_EVT_CTRL_BOOT_ERROR_FATAL
 *                      3. Added new enum value MR_LD_ACCESS_STATUS_PEER_UNAVAILABLE
 *                      4. Added new events
 *                              MR_EVT_HA_CACHE_MIRROR_ONLINE
 *                              MR_EVT_HA_CACHE_MIRROR_OFFLINE
 *                              MR_EVT_LD_ACCESS_BLOCKED_PEER_UNAVAILABLE
 *                      5. Updated comments for
 *                              MR_EVT_ARG_LD.ldIndex
 *
 * 06/06/12 NN          1. Fixed compile error for MR_PREMIUM_FEATURES.maxHANodes
 *                      2. Modified description for events MR_EVT_HA_xxx
 *
 * 06/22/12 NK          1. Added MR_MFC_DEFAULTs.perfTunerMode
 *
 * 07/29/12 NN          1. Added fields
 *                              MR_CTRL_INFO.hwPresent.ssmSupport
 *                              MR_CTRL_INFO.adapterOperations2.dedicatedHotSparesLimited
 *                              MR_PD_ALLOWED_OPS.SMARTSupported
 *                              MR_PD_INFO.properties.ineligibleForSSCD
 *                              MR_PD_INFO.properties.ineligibleForLd
 *                              MR_PD_INFO.properties.useSSEraseType
 *                      2. Updated comments for
 *                              MR_CTRL_INFO.cluster.peerIsIncompatible
 *                              MR_LD_PARAMETERS.currentAccessPolicy
 *                      3. Added new enum value MR_PD_MEDIA_TYPE_SSM_FLASH
 *                      4. Added enums
 *                              MR_FLASH_TYPE
 *                              MR_SS_ERASE_TYPE
 *                      5. Added structure MR_PD_SMART_INFO
 *                      6. Updated DCMDs
 *                              MR_DCMD_PD_SECURE_ERASE_START
 *                              MR_DCMD_LD_SECURE_ERASE_START
 *                      7. Added DCMD MR_DCMD_PD_GET_SMART_INFO
 *                      8. Added error code MFI_STAT_DEDICATED_SPARE_NOT_ALLOWED
 *                      9. Added event arguments
 *                              MR_EVT_ARGS_PD_TEMP
 *                              MR_EVT_ARGS_PD_LIFE
 *                     10. Added events
 *                              MR_EVT_PD_TEMP_HIGH
 *                              MR_EVT_PD_TEMP_CRITICAL
 *                              MR_EVT_PD_TEMP_NORMAL
 *                              MR_EVT_PD_IO_THROTTLED
 *                              MR_EVT_PD_IO_NOT_THROTTLED
 *                              MR_EVT_PD_LIFE
 *                              MR_EVT_PD_LIFE_NOT_OPTIMAL
 *                              MR_EVT_PD_LIFE_CRITICAL
 *
 * 08/23/12  NK         Added MR_MFC_DEFAULTS.enable512eSupport
 *
 * 08/27/12  NN         1. Added structure MR_FLASH_COMP_INFO_LIST
 *                      2. Added defines MR_FLASH_NAME_xxx
 *                      3. Added DCMD MR_DCMD_CTRL_FLASH_COMP_INFO_GET
 *
 * 09/13/12  NK         Added DCMD MR_DCMD_CTRL_NVCACHE_ERASE
 *
 * 09/28/12  NK         Added DCMD MR_DCMD_LD_GET_SPAN_LIST
 *
 * 10/09/12  PM         1. Added fields
 *                               MR_CTRL_INFO.raidLevels.SSCraidLevel_1E_EC
 *                               MR_CTRL_INFO.ldOperations.supportSSCBoost
 *                               MR_PD_SMART_INFO.status
 *                               MR_LD_PARAMETERS.SSCDType
 *                      2. Added enums
 *                              MR_LD_BOOST_TYPE
 *                              MR_LD_SSCD_TYPE
 *                      3. Added structures
 *                              MR_LD_BOOST_PRIORITY_TYPE
 *                              MR_LD_BOOST_PRIORITY_TYPE_LIST
 *                      4. Added DCMDs
 *                              MR_DCMD_LD_SSCD_BOOST_PRIORITY_TYPE_GET
 *                              MR_DCMD_LD_SSCD_BOOST_PRIORITY_TYPE_SET
 *                      5. Added event  MR_EVT_PD_FAILURE_LOCKEDUP
 *
 * 10/29/12  NK         1. Added fields
 *                              bbmErrCountSupported and bbmErrCount to MR_PD_INFO
 *
 * 11/06/12  ST         Added enum MR_LD_EMULATION_TYPE
 *
 * 11/16/12  ST         1. Added iov structure to MR_CTRL_INFO
 *                      2. Added numVFSkew to MR_PREMIUM_FEATURES
 *
 * 01/09/13  ST         1. Added field
 *                              MR_CTRL_INFO.adapterOperations2.supportPointInTimeProgress
 *                              MR_PROGRESS.elapsedSecsForLastPercent
 *                      2. Updated parameter description of DCMD MR_DCMD_CTRL_FLASH_FW_FLASH
 *                      3. Modified event arguments
 *                              MR_EVT_ARGS_LD_PROG
 *                              MR_EVT_ARGS_PD_PROG
 *                      4. Added events
 *                              HA_POSSIBLE_PEER_COMM_LOSS
 *
 * 01/16/13  PM         1. Added field MR_CTRL_INFO.adapterOperations2.supportDataLDonSSCArray
 *
 * 01/18/13  ST         1. Added field MR_BBU_STATUS.vpdSupported
 *                      2. Added DCMD MR_DCMD_BBU_VPD_GET
 *                      3. Added structures
 *                              MR_BBU_VPD_INFO_FIXED
 *                              MR_BBU_VPD_INFO
 *                      4. Added enums
 *                              MR_BBU_VPD_TYPE
 *                              MR_BBU_VPD_SUBTYPE
 *
 * 01/30/12  NK         1. Added new events
 *                              MR_EVT_CTRL_FLASH_SIGNED_COMPONENT_NOT_PRESENT
 *                              MR_EVT_CTRL_FLASH_AUTHENTICATION_FAILURE
 *                              MR_EVT_LD_SET_BOOT_DEVICE
 *                              MR_EVT_PD_SET_BOOT_DEVICE
 *                      2. Added new MFI status codes
 *                              MFI_STAT_FLASH_SIGNED_COMPONENT_NOT_PRESENT
 *                              MFI_STAT_FLASH_AUTHENTICATION_FAILURE
 *
 * 02/06/13  BH         1. Added fields
 *                              MR_CTRL_INFO.hostInterface.SRIOV
 *                              MR_CTRL_INFO.iov.maxVFsSupported
 *                              MR_CTRL_INFO.iov.numVFsEnabled
 *                              MR_CTRL_INFO.iov.requestorId
 *                      2. Added enums MR_LD_ACCESS.MR_LD_ACCESS_HIDDEN
 *                      3. Updated value of MR_LD_ACCESS_MASK
 *                      4. Added new DCMDs
 *                              MR_DCMD_LD_VF_MAP_GET
 *                              MR_DCMD_LD_VF_MAP_GET_ALL_LDS
 *                              MR_DCMD_LD_VF_MAP_SET
 *                              MR_DCMD_LD_VF_MAP_SET_MULTIPLE
 *                      5. Added new structures
 *                              MR_LD_VF_MAP
 *                              MR_LD_VF_AFFILIATION
 *
 * 02/06/13  ST/SM      1. Added fields
 *                              MR_CTRL_PROP.OnOffProperties.enableConfigAutoBalance
 *                              MR_CTRL_INFO.adapterOperations2.mpio
 *                              MR_CTRL_INFO.adapterOperations2.supportConfigAutoBalance
 *                              MR_CTRL_INFO.adapterOperations2.activePassive
 * 02/11/13  PM         1. Added fields
 *                              MR_CTRL_INFO.ldOperations.supportFastPathWB
 *                              MR_PD_SMART_INFO.ssmFlashSmart.status.serviceable
 *                              MR_LD_PARAMETERS.characteristics.fastPathWBEnabled
 *                              MR_CTRL_INFO.pdMixSupport.allowMixSSMandSFMinLD : 0=disallow mix of SSM and SFM, 1= allow mix of SSM and SFM (Serviceable SSM)
 *
 * 04/02/13  NK         1. Added new events
 *                              MR_EVT_NVCACHE_BACKUP_UNAVAILABLE
 *                              MR_EVT_NVCACHE_CONSIDER_REPLACEMENT
 *                              MR_EVT_NVCACHE_INVALID
 *                              MR_EVT_BOOT_DEVICE_INVALID
 *                      2. Added fields
 *                              MR_RAID_CTX_CONTEXT_TYPE
 * 04/22/13  BH         1. Added fields
 *                              MFI_STAT_FLASH_OK_RESTART_NOT_REQUIRED
 *                      2. Updated comments for
 *                              MR_DCMD_PD_LOCK_KEY_APPLY
 *                              MR_CTRL_LOCK_KEY.lockKeyLength
 *                              MR_MFC_DEFAULTS.enableLDBBM
 *                      3. Modified MR_LD_VF_MAP to correct a padding issue
 * 06/04/13  BH         1. Added fields
 *                              MR_CTRL_INFO.cluster.passive
 *                      2. Added new stat
 *                              MFI_STAT_RESTRICTED_MODE
 * 06/18/13  PM         1. Added fields
 *                              MR_CTRL_PROP.OnOffProperties.enableVirtualCache
 *                              MR_CTRL_PROP.defaultCtrlPowerPolicy
 *                              MR_CTRL_INFO.hwPresent.powerMonitor
 *                              MR_CTRL_INFO.pdOperations.supportSS3PassVerify
 *                              MR_CTRL_INFO.adapterOperations2.supportVirtualCache
 *                              MR_SS_ERASE_TYPE.MR_SS_ERASE_3PASS_VERIFY
 *                              MR_PD_SMART_INFO.ssmFlashSmart.status.onboard
 *                      2. Added structs and enums
 *                              MR_POWER_USAGE_OPTIONS, MR_POWER_SLAB, MR_POWER_THROTTLE_OPTIONS, MR_POWER_OPTION_SCHEME, MR_CTRL_POWER_USAGE
 *                              MR_UNMAP_STATISTICS
 *                              MR_SSC_METRICS_PARAMETER, MR_SSC_METRICS_LIST, MR_DEBUG_METRICS_LIST,MR_DEBUG_ENTRY
 *                              MR_SSC_EXHAUSTIVE_METRICS, MR_SSC_USER_METRICS, MR_LD_METRICS, MR_SSC_METRICS
 *                              MR_AUTO_CFG_PARAMETER, MR_AUTO_CFG_OPTIONS
 *                      3. Added DCMDs
 *                              MR_DCMD_CTRL_POWER_USAGE_THROTTLE
 *                              MR_DCMD_CTRL_UNMAP_STATISTICS_GET
 *                              MR_DCMD_CTRL_SSC_METRICS_ENABLE_EX
 *                              MR_DCMD_CTRL_SSC_METRICS_GET
 *                              MR_DCMD_CFG_AUTOCFG
 *                      4. Added Events
 *                              MR_EVT_HA_SSC_WB_POOL_SIZE_MISMATCH
 *                              MR_EVT_HA_SSC_NONSHARED_VD_WB_ASSOCIATION
 *                              MR_EVT_CTRL_POWER_IO_THROTTLE_START
 *                              MR_EVT_CTRL_POWER_IO_THROTTLE_STOP
 *                              MR_EVT_CTRL_TUNABLE_PARAMETERS_CHANGED
 * 07/19/13  NK         1. Added new structures
 *                              MR_ENCL_INFO_EXT
 *                              MR_ENCL_LIST_EXT
 *                      2. Added new events
 *                              MR_EVT_CTRL_TEMP_WITHIN_OPTIMAL_RANGE
 *                              MR_EVT_CTRL_TEMP_ABOVE_OPTIMAL_RANGE
 *                      3. Added fields
 *                              MR_CTRL_INFO.adapterOperations2.supportExtEnclDrvCount
 *
 * 08/01/13  NK         1. Added new structure
 *                              MR_CTRL_HA_CONTROL_MODE
 *                      2. Added new DCMDs
 *                              MR_DCMD_CTRL_CONTROL_HA_MODE_GET
 *                              MR_DCMD_CTRL_CONTROL_HA_MODE_SET
 *                      3. Added new events
 *                              MR_EVT_HA_MODE_HA_FEATURE_SET
 *                              MR_EVT_HA_MODE_SC_FEATURE_SET
 *
 * 08/02/13  NN         1. Added fields
 *                              MR_CTRL_INFO.pdOperations.supportWCE
 *                              MR_PD_INFO.properties.wceUnchanged
 *                      2. Updated comments for
 *                              MR_PD_INFO.properties.WCE
 *                      3. Deprecated MR_PD_INFO.properties.NCQ
 *
 * 09/04/13  PM         1. Added fields
 *                              MR_CTRL_PROP.OnOffProperties.enableAutoLockRecovery
 *                              MR_CTRL_INFO.ldOperations.supportAutoLockRecovery
 *                              MR_CTRL_INFO.ldOperations.supportScsiUnmap
 *                              MR_PD_INFO.properties.supportScsiUnmap
 *                              MR_PD_SMART_INFO.ssmFlashSmart.lifeLeftPrecision
 *                              MR_LD_PROPERTIES.scsiUnmapEnabled
 *                              MR_LD_ALLOWED_OPS.scsiUnmap
 *
 * 09/12/13  NK         1. Added new DCMD
 *                              MR_DCMD_CTRL_DQ_CONFIG_SET
 *                      2. Modified DCMD MR_DCMD_CTRL_TTY_READ
 *                      3. Added new event
 *                              MR_EVT_NVCACHE_COMPONENT_MISMATCH
 *                      4. Added structure
 *                              MR_DEBUG_QUEUE_CONFIG
 * 09/30/13  BH         1. Changed Name and updated comments for enum
 *                              MR_CTRL_HA_CONTROL_MODE
 *                      2. Removed unused field startOffset from MR_DEBUG_QUEUE_CONFIG and marked that field as reserved
 *
 * 10/14/13  BH         1. Added new DCMD
 *                              MR_DCMD_CTRL_MAINTENANCE_MODE
 *                      2. Added Structure
 *                              MR_DRV_SYSTEM_INFO, MR_DIAG_INFO, MR_DIAG_DETAILS
 *                      3. Added new event
 *                              MR_EVT_CTRL_ENTER_MAINTENANCE_MODE
 *                              MR_EVT_CTRL_EXIT_MAINTENANCE_MODE
 *                              MR_EVT_HA_TOPOLOGY_MODE
 *                              MR_EVT_HA_TOPOLOGY_LD_INCOMPATIBLE_SATA
 *                              MR_EVT_HA_TOPOLOGY_LD_INCOMPATIBLE_CACHECADE
 *                      4. Added field in structures MFI_CMD_INIT and MFI_FRAME_INIT
 *                              systemInformation
 *                      5. Added fields
 *                              MR_CTRL_INFO.adapterOperations2.supportMaintenanceMode
 *                              MR_CTRL_INFO.adapterOperations2.supportDiagResults
 *                              MR_CTRL_INFO.adapterStatus.maintenanceMode
 *                      6. Added enum
 *                              MR_CTRL_MAINTENANCE_MODE, MR_DIAG_TEST
 *                      7. Changed MR_ENCL_LIST_EXT structure to follow LIST structure principles
 *                      8. Updated DCMD
 *                              Added dcmd.mbox.w[1] definition for MR_DCMD_CTRL_DIAG_SELF_CHECK
 *
 * 10/30/13  NK         1. Re-enabling NCQ information for the drive
 *                              Added MR_CTLR_INFO.pdOperations.supportNCQ
 *
 * 11/04/13  BH         1. Added union-field
 *                              Added MR_BIOS_DATA.deviceExposure
 *
 * 11/12/13  PM         1. Added field
 *                              MR_CTRL_INFO.adapterOperations2.supportPowerThrottle
 *                      2. Added struct and enums
 *                              MR_CTRL_SSC_FLUSH_STATUS, MR_CTRL_CACHE_FLUSH_TYPE, MR_EXTENDED_POWER_OPTION2, MR_PR_PAUSED, MR_PR_USER_PAUSED
 *                      3. Added DCMDs
 *                              MR_DCMD_CTRL_SSC_FLUSH_STATUS
 *                              MR_DCMD_CTRL_CACHE_FLUSH - update for SSC
 *                      4. Added Events
 *                              MR_EVT_CTRL_CACHE_FLUSH_START
 *                              MR_EVT_CTRL_CACHE_FLUSH_DONE
 *                              MR_EVT_CTRL_CACHE_FLUSH_ABORT
 *                      5. Added error codes MFI_STAT_EXCEED_MAX_SUPPORTED_COUNT, MFI_STAT_SSC_FLUSH_ALREADY_RUNNING
 *
 * 11/19/13  NK         1. Added new DCMDs
 *                             MR_DCMD_CTRL_PERSONALITY_GET
 *                             MR_DCMD_CTRL_PERSONALITY_SET
 *                      2. Added enum
 *                             MR_CTRL_PERSONALITY
 *                      3. Added HDDThermalPollInterval, SSDThermalPollInterval, disableImmediateIO to MR_CTRL_PROP
 *                      4. Added support bit supportThermalPollInterval in adapterOperations3
 *                      5. Added new event MR_EVT_CTRL_PERSONALITY_CHANGE
 *                      6. Added new structure - adapterOperations3 in MR_CTRL_INFO
 *                      7. Added support bits supportPersonalityChange, disableImmediateIO in adapterOperations3
 *                      8. Added MFC default adapterPersonality, disableImmediateIO, cacheVaultOption
 *                      9. Added structure MR_CTRL_PERSONALITY_INFO
 *                      10. Added MFI_STAT_OK_REBOOT_REQUIRED
 *                      11. Added supportT10RebuildAssist to MR_PD_INFO, adapterOperations3
 *                      12. Added disableT10RebuildAssist to MR_CTRL_PROP
 *
 * 12/03/13  SM         1. MAX_API_LOGICAL_DRIVES is deprecated to MAX_API_LOGICAL_DRIVES_LEGACY
 *                      2. MAX_API_ARRAYS is deprecated to MAX_API_ARRAYS_LEGACY
 *                      3. MAX_API_TARGET_ID is deprecated to MAX_API_TARGET_ID_LEGACY
 *                      4. New MACRO MAX_LOGICAL_DRIVES_LIMIT_64 is defined for deprecated
 *                            MAX_LOGICAL_DRIVES
 *                            MAX_ARRAYS
 *                            MAX_TARGET_ID
 *                      5. New MACRO is defined for
 *                            MAX_API_LOGICAL_DRIVES_EXT
 *                            MAX_API_ARRAYS_EXT
 *                            MAX_API_TARGET_ID_EXT
 *                      8. Updated DCMD
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_CTRL_GET_INFO
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_CTRL_PR_GET_PROPERTIES
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_CTRL_PR_SET_PROPERTIES
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_CTRL_CC_SCHEDULE_GET_PROPERTIES
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_CTRL_CC_SCHEDULE_SET_PROPERTIES
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_CTRL_IO_METRICS_GET
 *                            Added dcmd.mbox.b[2] definition for MR_DCMD_CTRL_SSC_METRICS_GET
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_LD_GET_LIST
 *                            Added dcmd.mbox.b[2] definition for MR_DCMD_LD_LIST_QUERY
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_LD_GET_GUID_LIST
 *                            Added dcmd.mbox.b[4] definition for MR_DCMD_LD_CLEAR_BLOCKED
 *                            Added dcmd.mbox.b[4] definition for MR_DCMD_LD_RECONS_START
 *                            Added dcmd.mbox.b[5] definition for MR_DCMD_LD_DELETE
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_LD_GET_ALLOWED_OPS_ALL_LDS
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_LD_GET_PINNED_LIST
 *                            Added dcmd.mbox.b[2] definition for MR_DCMD_LD_DISCARD_PINNED
 *                            Added dcmd.mbox.b[0] definition for MR_LD_BBM_LOG_LIST_GET
 *                            Added dcmd.mbox.b[4] definition for MR_DCMD_LD_SSCD_GET_LDS
 *                            Added dcmd.mbox.b[4] definition for MR_DCMD_LD_SSCD_BOOST_PRIORITY_TYPE_GET
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_LD_VF_MAP_GET_ALL_LDS
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_LD_VF_MAP_SET_MULTIPLE
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_CFG_READ
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_CFG_ADD
 *                            Added dcmd.mbox.b[1] definition for MR_DCMD_CFG_CLEAR
 *                            Added dcmd.mbox.b[0] definition for MR_DCMD_CFG_GET_ALLOWED_OPS
 *                            Added new value 2 in dcmd.mbox.w[0] for MR_DCMD_CFG_AUTOCFG
 *                            Added dcmd.mbox.b[1] definition for MR_DCMD_CFG_FOREIGN_READ
 *                            Added dcmd.mbox.b[1] definition for MR_DCMD_CFG_FOREIGN_READ_AFTER_IMPORT
 *                            Added dcmd.mbox.b[1] definition for MR_DCMD_CFG_FOREIGN_IMPORT
 *                            Added dcmd.mbox.b[1] definition for MR_DCMD_CFG_FOREIGN_CLEAR
 *                            Extend the size of MR_LD_LIST for MAX_API_LOGICAL_DRIVES_EXT lds
 *                            Extend the size of MR_LD_ALLOWED_OPS_LIST for MAX_API_LOGICAL_DRIVES_EXT lds
 *                            Extend the size of MR_LD_BBM_LOG_LIST for MAX_API_LOGICAL_DRIVES_EXT lds
 *                            Extend the size of MR_UNMAP_STATISTICS for MAX_API_LOGICAL_DRIVES_EXT lds
 *                      9. Add MFI_STAT_INVALID_PARAMETER as valid exit code for
 *                            MR_DCMD_LD_DISCARD_PINNED
 *                            MR_DCMD_LD_VF_MAP_SET_MULTIPLE
 *                            MR_DCMD_LD_SSCD_BOOST_PRIORITY_TYPE_GET
 *                            MR_DCMD_CFG_CLEAR
 *                     10. Add new structures/unions
 *                            MR_LD_BITMAP
 *                            MR_PD_BITMAP
 *                            MR_PR_PROPERTIES_EXT
 *                            MR_CC_SCHEDULE_PROPERTIES_EXT
 *                     11. Added fields
 *                            MR_CTRL_PROP.OnOffProperties.ignore64ldRestriction
 *                            MR_CTRL_INFO.adapterOperations3
 *                            MR_CTRL_INFO.adapterOperations3.supportMaxExtLDs
 *                            MR_MFC_DEFAULTS.ignore64ldRestriction
 *                            MR_MFC_DEFAULTS.autoCfgOption
 *                            MR_MFC_FEATURES.maxLDs
 *                     12. Added new enum to MR_AUTO_CFG_OPTIONS
 *                            MR_CFG_SINGLE_PD_R0
 *                            MR_CFG_SINGLE_PD_R0_SSC_R0
 *                     13. Added new enum MR_LD_LIMIT
 *                     14. Added events
 *                            MR_EVT_CFG_AUTO_CREATED
 *
 * 12/12/13  NK        1. Added new DCMD MR_DCMD_CTRL_SET_CRASH_DUMP_PARAMS
 *                     2. Added enum MR_CRASH_DUMP_STATUS
 *                     3. Added support bit supportCrashDump in adapterOperations3
 *                     4. Added crashDumpDone, crashDumpDMADone in MFI_REG_STATE
 *                     5. Added triggerCrashDump in MFI_CONTROL_REG
 *                     6. Added exclusiveLDSupported in MR_CTRL_INFO.cluster
 *
 * 12/16/13  PM         1. Added fields
 *                            MR_CTRL_INFO.adapterOperations3.supportSwZone : support 2 nodes software zone
 *                            MR_CTRL_INFO.OnOffProperties.enableSwZone
 *                      2. Added DCMDs
 *                            MR_DCMD_CTRL_SWZONE_RESERVATION_PREEMPT
 *                            MR_DCMD_CTRL_SWZONE_RESERVATION_RELEASE
 *                      3. Added events
 *                            MR_EVT_CTRL_SWZONE_ENABLED
 *                            MR_EVT_CTRL_SWZONE_DISABLED
 *                      4. Added error codes
 *                            MFI_STAT_BGOP_IN_PROGRESS
 *                            MFI_STAT_OPERATION_NOT_POSSIBLE
 *
 * 12/22/13  NK         1. Added new event: MR_EVT_LD_INIT_ABORTED_DUE_TO_RESET
 *                      2. Added new MFI stat: MFI_STAT_FW_FLASH_IN_PROGRESS_ON_PEER
 *
 * 01/07/14  NK         1. Added MR_CTRL_PROP.OnOffProperties.limitMaxRateSATA3G
 *                      2. Added MR_CTRL_INFO.pdOperations.supportLimitMaxRateSATA
 *                      3. Added MR_CTRL_INFO.adapterOperations3.supportDebugQueue
 *
 * 01/22/14  NK         Added maxChainSizeUnits in MFI_REG_STATE_EXT1.ready
 *
 * 01/29/14  NK         Added requestedPersonality in MR_CTRL_PERSONALITY_INFO
 *                      Added MR_CTRL_INFO.adapterOperations3.supportNVCacheErase
 *                      Modified exclusiveLDSupported to  exclusiveLDNotSupported in MR_CTRL_INFO.cluster
 *
 * 02/12/14  BH         Added return values MFI_STAT_OPERATION_NOT_POSSIBLE for MR_DCMD_CTRL_PERSONALITY_SET
 *                      Added MR_CTRL_INFO.adapterOperations3.supportForceTo512e
 *                      Added MR_LD_PROPERTIES.forceEmulation
 *
 * 02/17/14  SM         1. Define MAX_LOGICAL_DRIVES_LIMIT_64 as a stop gap solution
 *                      2. Added fields
 *                              MR_CTRL_INFO.cluster.securityKeyMismatch
 *                              MR_LD_INFO.peerHasNoAccess
 *                              MR_PREMIUM_FEATURES.topology
 *                              MR_CTRL_INFO.cluster.currentTopology
 *                      3. Added events
 *                              MR_EVT_HA_CTRL_LOCK_KEY_MISMATCH
 *                              MR_EVT_HA_CTRL_LOCK_KEY_MATCH
 *                              MR_EVT_HA_LD_COMPATIBLE
 *
 * 03/05/14  NK         1. Removed #define MAX_LOGICAL_DRIVES_LIMIT_64
 *                      2. Added supportNDrvR1LoadBalance in struct MFI_CAPABILITIES
 *                      3. Provided correct endianness support for OOB headers
 *                      4. Changed the #define _MFI_IOCTL to INCLUDE_MFI_IOCTL in mfi_ioctl.h
 * 03/21/14  BH         1. Added Events
 *                         MR_EVT_PD_OPERATION_DELAYED
 *                         MR_EVT_LD_OPERATION_DELAYED
 *                         MR_EVT_CTRL_GENERIC_MESSAGE : Currently used for putting a generic string for specific OEM, product
 *                         MR_EVT_LD_VF_MAP
 *
 * 04/21/14  NK         1. Added new event MR_EVT_CTRL_TEMP_CRITICAL
 *                      2. Marked adaptive read ahead as deprecated
 *                      3. Added deviceExposure setting to NVDATA
 *
 * 05/02/14  BH         1. Added events
 *                          MR_EVT_CTRL_SHUTDOWN_CHASSIS
 *                          MR_EVT_CTRL_RESTART_CHASSIS
 *                          MR_EVT_CTRL_INFO_CHANGED
 *                      2. Added MR_DCMD_CTRL_SHUTDOWN.dcmd.mbox.b[2] for private use only.
 *                      3. Added new DCMD MR_DCMD_CTRL_IOV_PRIVATE_INFO_GET
 *
 * 05/05/14  NK         1. Added new DCMDs:
 *                          a. MR_DCMD_LD_SET_HIDDEN
 *                          b. MR_DCMD_LD_CLEAR_HIDDEN
 *                      2. Added the following to MR_LD_ALLOWED_OPS
 *                          a. hideLD
 *                          b. unhideLD
 *                      3. Added supportHide to MR_CTRL_INFO.ldOperations
 *                      4. Added new event: MR_EVT_LD_PARTIALLY_HIDDEN
 *                      5. Added new MFI status: MFI_STAT_LD_PARTIALLY_HIDDEN
 *                      6. Added MR_CTRL_INFO.adapterOperations3.supportHOQRebuild
 *                      7. Added MR_LD_PROPERTIES.disableHOQRebuild
 *
 * 06/16/14  NK         1. Added driveActivityLED to MR_CTRL_PROP.OnOffProperties
 *                      2. Added adapterOperations3.supportAllowedOpsforDrvRemoval,
 *                         adapterOperations3.supportDrvActivityLEDSetting
 *                      3. Added prepareForRemoval to MR_PD_ALLOWED_OPS
 *
 * 06/18/14  PM         1. Added fields and enum
 *                            MR_CTRL_INFO.ldOperations.supportAtomicity
 *                            MR_CTRL_INFO.ldOperations.supportT10Atomicity
 *                            MR_LD_PROPERTIES.enabledAtomicityType
 *                            MR_LD_ALLOWED_OPS.atomicityChangeable
 *                            MFI_CAPABILITIES.mfiCapabilities.supportCoreAffinity
 *                            MR_ATOMICITY_TYPE
 *                      2. Added DCMDs
 *                            MR_DCMD_LD_ATOMIC_WRITE_GET_LDS
 *                            MR_DCMD_LD_ATOMIC_WRITE_SET_LDS
 *
 * 07/09/14  NK         1. Added structure: MR_PCI_OEM_INFO
 *                      2. Added DCMDs: MR_DCMD_CTRL_PCI_OEM_GET, MR_DCMD_CTRL_PCI_OEM_SET
 *
 * 07/25/14  PM         1. Added field and structs
 *                            MR_CTRL_PROP.OnOffProperties.enableVirtualCache deprecated and replaced with enableVCorDLC
 *                            MR_CTRL_INFO.adapterOperations2.supportVirtualCache deprecated and replaced with supportVCorDLC
 *                            MR_CTRL_INFO.ldOperations.disallowPropChange
 *                            MR_CTRL_INFO.adapterOperations3.supportNVDRAM
 *                            MR_CTRL_INFO.adapterOperations3.supportForceFlash
 *                            MR_LD_PARAMETERS.characteristics.ldSmartEnabled
 *                            MR_CONFIG_ALLOWED_OPS.disallowForceClearConfig
 *                            MR_CFG_SINGLE_VD_R0_WB_WITH_ALL_PDS
 *                            MR_LD_SMART_INFO
 *                            MR_NVDRAM_PROPERTIES
 *                      2. Added DCMDs
 *                            MR_DCMD_LD_GET_SMART_INFO
 *                            MR_DCMD_CTRL_NVDRAM_PROPERTIES_GET
 *                      3. Added event: MR_EVT_CTRL_FORCE_FLASH
 *                      4. Added mbox option for MR_CTRL_FLASH_FW_FLASH to enable force update of controller firmware
 *                      5. Marked MAX_API_FOREIGN_CONFIGS as deprecated
 *
 * 08/06/14  NK         1. Added fields and enum
 *                            MR_CTRL_PROP.OnOffProperties.disableSESMonitoring
 *                            MR_CTRL_INFO.adapterOperations3.supportDisableSESMonitoring
 *                            MR_CTRL_INFO.adapterOperations3.supportCacheBypassModes
 *                            MR_LD_PROPERTIES.cacheBypassIOSize
 *                            MR_LD_PROPERTIES.cacheBypassMode
 *                            enum MR_CACHEBYPASS_IOSIZE
 *                            enum MR_CACHEBYPASS_MODE
 *
 * 08/12/14  NK         1. Added DCMD: MR_DCMD_PD_SECURE_JBOD
 *                      2. Added support bits: supportSecurityOnJBOD, discardCacheDuringLDDelete
 *                      3. Added enableSecurityonJBOD in MR_PD_ALLOWED_OPS
 *                      4. Marked MR_LD_PROPERTIES.disableHOQRebuild as deprecated
 *                      5. Added SecurityProtocolCmdsFW in MFI_CAPABILITIES
 *                      6. Updated event MR_EVT_LD_CACHE_DISCARDED
 *                      7. Added mbox bit for MR_DCMD_LD_DELETE
 *
 * 09/02/14  NK         1. Added structures:
 *                         MFI_OOB_PCI_MSG_HDR
 *                         MFI_OOB_PCI_MSG_PKT
 *                      2. Added enum:
 *                         MFI_OOB_MCTP_CONTROL_MSG
 *
 * 09/19/14  NK         1. Added new MFI stat: MFI_STAT_SECURE_SYSTEM_PD_EXISTS
 *                      2. Added new mbox value to DCMD MR_DCMD_CTRL_TTY_READ
 *                      3. Added a support bit MR_CTRL_INFO.adapterOperations3.supportTTYLogCompression
 *
 * 09/25/14  NK         1. Added struct MR_CTRL_INFO.cpld
 *                      2. Added bit MR_CTRL_INFO.hwPresent.upgradableCPLD
 *                      3. Added support bit MR_CTRL_INFO.adapterOperations3.supportCPLDUpdate
 *
 * 10/02/14  BH         1. Changed value of macro MR_BBU_STATUS_BAD_FOR_TMMC
 *
 * 10/20/14  NK         1. Added SATA_6G to MR_CTRL_INFO.deviceInterface
 *                      2. Added event MR_EVT_LD_ACCESS_POLICY_SET_TO_RW
 *                      3. Added numSGEExt to struct MPI2_SCSI_IO_VENDOR_UNIQUE
 *
 * 10/27/14  NK         1. Added MR_CTRL_PROP.diskCacheSettingForSysPDs
 *                      2. Updated status for DCMD MR_DCMD_CFG_FOREIGN_IMPORT
 *                      3. Added MR_OOB_PARAMS.dontWaitForCmdCompletion
 *                      4. Updated MR_MFC_DEFAULTS.enableLEDheaders
 *                      5. Added MFI_REG_STATE_EXT2
 *                      6. Updated comment for MR_RECON.newRaidLevel
 *                      7. Added support bit MR_CTRL_INFO.adapterOperations3.supportDiskCacheSettingForSysPDs
 *
 * 11/19/14  NK         1. Added secKeyNotOnTMM to EEPROM_DATA
 *                      2. Added macros SECONDARY_KEY_ON_TMM_GET, SECONDARY_KEY_ON_TMM_SET
 *
 * 11/24/14  NK         1. Added supportExtQueueDepth to MFI_CAPABILITIES
 *
 * 12/01/14  NK         1. Added maxBytesPerTransaction in MR_OOB_PARAMS
 *                      2. Added events MR_EVENT_CTRL_FLASH_TRANSIENT_ERROR, MR_EVENT_CTRL_FLASH_FAILED
 *
 * 12/11/14  SM         1. Added currentSSCSize to MR_CONFIG_DATA
 *
 * 01/19/15  NN         1. Added fields
 *                            MR_CTRL_INFO.pdOperations.supportDegradedMedia
 *                            MR_CTRL_PROP.pdDegradedMediaOptions
 *                            MR_MFC_DEFAULTS.pdDegradedMediaOptions
 *                      2. Added struct MR_EVT_ARG_PD_INFO
 *                      3. Added event argument MR_EVT_ARGS_PD_DEGRADED_MEDIA
 *                      4. Added events
 *                           MR_EVT_PD_DEGRAGED_MEDIA
 *                           MR_EVT_PD_DEGRAGED_MEDIA_POOR_PERFORMANCE
 *
 * 01/19/15  NK         1. Added fields
 *                            MFI_CAPABILITIES.supportExtIOSize
 *                            MR_CTRL_PROP.OnOffProperties.failPDOnSMARTErr
 *                      2. Added enum MFI_OOB_MCTP_CONTROL_EXCEPTION_PENDING to MFI_OOB_MCTP_CONTROL_MSG
 *                      3. Added events
 *                            MR_EVT_PD_LOCATE_START
 *                            MR_EVT_PD_LOCATE_STOP
 *                            MR_EVT_PD_PR_CONFLICT_ABORT
 *                      4. Added support bits
 *                            MR_CTRL_INFO.adapterOperations3.supportExtendedSSCSize
 *                            MR_CTRL_INFO.adapterOperations3.supportCfgSeqNumForSysPDs
 * 03/18/15  BH         1. Added fields
 *                            MR_CTRL_INFO.adapterOperations3.supportFWFlashDeviceStatus
 *                            MR_CTRL_INFO.adapterStatus.fwFlashDeviceStatus
 * 03/25/15  SM         1. Modified MAX_PHYS_PER_CONTROLLER
 *                      2. Modified MR_CTRL_INFO.deviceInterfacePortAddr2
 *                      3. Added fields
 *                            MR_CTRL_INFO.adaptercOperations3.supportMoreThan16Phys
 *                            MR_PD_INFO.pathInfo.connectedPortNumbers
 *                      4. MR_PD_ADDRESS.connectedPortNumbers is deprecated
 *                      5. MR_PD_INFO.connectedPortNumbers is deprecated
 *
 * 04/10/15  NK         1. Added MR_CTRL_PROP.OnOffProperties.enableWriteVerifyDuringCacheFlush
 *                      2. Added MR_CTRL_INFO.adaptercOperations3.supportCacheFlushWriteVerify
 *
 * 04/20/15  NK         1. Added event MR_EVT_BBU_SCAP_INFO
 *                      2. Added predictiveFailureLED in MR_MFC_DEFAULTS
 *
 * 04/27/15  NK         1. Added return value MFI_STAT_OPERATION_NOT_POSSIBLE for MR_DCMD_LD_RECONS_START
 *
 * 05/28/15  BH         1. Added return value  MFI_STAT_LD_BOOT_DEVICE for MR_DCMD_LD_SET_HIDDEN
 *
 * 06/01/15  NN         1. Added fields
 *                              MR_LD_PARAMETERS.characteristics.isEPD
 *                              MR_PD_INFO.properties.isEPD
 *                      2. Added enum values
 *                              MR_CTRL_PERSONALITY_SDS
 *                              MR_CFG_EPD
 *                      3. Added DCMD MR_DCMD_CFG_MAKE_EPD
 *                      4. Added MFI_REG_STATE_EXT1.ready.RDPQArrayMode
 *                      5. Added event argument MR_EVT_ARGS_PD_INQ
 *                      6. Added events
 *                              MR_EVT_CTRL_PERSONALITY_PCI_CHANGE
 *                              MR_EVT_PD_INQUIRY_INFO
 *                      7. Added structure MR_CFG_EPD_PARAMS
 *
 * 06/08/15  SM         1. Added fields
 *                              MR_CTRL_INFO.supportExtLDAllowedOps
 *                              MR_CTRL_INFO.maxTransportableVDs
 *                      2. Added enum values
 *                              MR_LD_ACCESS_TRANSPORT_READY
 *                      3. Added DCMD
 *                              MR_DCMD_LD_SET_TRANSPORT_READY
 *                              MR_DCMD_LD_CLEAR_TRANSPORT_READY
 *                      4. Updated DCMD
 *                            MR_DCMD_LD_GET_ALLOWED_OPS will return MR_LD_ALLOWED_OPS_EXT
 *                            Added new value in dcmd.mbox.b[0] definition for MR_DCMD_LD_GET_ALLOWED_OPS_ALL_LDS
 *                      5. Add new structures/unions
 *                            MR_LD_ALLOWED_OPS_EXT - here the snapshot related fields are deprecated and moved back as reserved
 *                            MR_LD_ALLOWED_OPS_LIST_EXT
 *                      6. Added events
 *                            MR_EVT_LD_TRANSPORT_READY
 *                            MR_EVT_LD_TRANSPORT_CLEAR
 *                      7. Added MFI_STAT
 *                            MFI_STAT_EXCEED_MAX_TRANSPORT_LD_COUNT
 *                            MFI_STAT_DHSP_MULTIPLE_ASSOCIATION
 *                            MFI_STAT_TRANSPORT_READY_INCOMAPTABLE
 *
 * 07/03/15  NK         1. Added enclLogicalID to MR_PD_INFO
 *
 * 07/09/15  NK         1. Added mfiCapabilities.supportVFIDinIOFrame
 *                      2. Added event
 *                            MR_EVT_CTRL_REBOOT_REQUIRED
 *                      3. Added mbox parameter for MR_DCMD_CTRL_PFK_SAFEID_GET
 *
 * 07/23/15  NK         1. Added mfiCapabilities.supportFPRLBypass
 *                      2. Added DCMDs
 *                         MR_DCMD_CTRL_GET_VF_QD
 *                         MR_DCMD_CTRL_SET_VF_QD
 *                      3. Added struct MR_CTRL_PROP.sriovOptions
 *                      4. Added struct: MR_IOV_QD_INFO
 *                      5. Added enum: MR_IOV_QD_OPTIONS
 *                      6. Added support bit: supportSRIOVOptions
 *
 * 07/27/15  NK         1. Added bits:
 *                         MR_CTRL_PROP.perfTuners.disableLargeIOSupport
 *                         MR_CTRL_INFO.adapterOperations3.supportLargeIO
 *
 * 08/06/15  NN         1. Added
 *                              MR_CTRL_INFO.adapterStatus.systemResetRequired
 *                              MR_AUTO_CFG_PARAMETER.cannotExecuteOnce
 *                              MR_AUTO_CFG_PARAMETER.allowedCfgOptions
 *                              MR_AUTO_CFG_PARAMETER.autoSecureSED
 *                              MR_AUTO_CFG_PARAMETER.disableSESManagement
 *                      2. Revised and updated MR_CFG_EPD_PARAMS
 *                      3. Added enum value MR_PR_OPTIONS_INCLUDE_EPD
 *                         and updated MR_PR_OPTIONS_MAX
 *                      4. Added MR_CONFIG_ALLOWED_OPS.makeEPD
 *                      5. Added enum MR_LATENCY_OPTIONS
 *                         and define MR_MAX_LATENCY_OPTIONS
 *                      6. Added  error code MFI_STAT_SCSI_DATA_UNDERRUN
 *                      7. Changed event MR_EVT_CTRL_REBOOT_REQUIRED to MR_EVT_SYSTEM_RESET_REQUIRED
 *
 *
 * 09/09/15  NK         1. Deprecated: MR_CTRL_PROP.autoSnapVDSpace, MR_CTRL_PROP.viewSpace
 *                      2. Added MR_CTRL_PROP.OnOffProperties2
 *                      3. Added MR_CTRL_PROP.OnOffProperties2.enableLargeIOSupport
 *                      3. Removed MR_CTRL_PROP.perfTuners.disableLargeIOSupport
 *                      4. Added MR_MFC_DEFAULTS.enableLargeIOSupport
 *
 * 09/10/15  SM         1. Removed fields forceTransportReadyWithDHSP from MR_LD_ALLOWED_OPS_LIST_EXT
 *                      2. Added enum MR_TRANSPORT_READY_DHSP_OPTIONS
 *                      3. Added MFI_STAT
 *                            MFI_STAT_TRANSPORT_READY_LD_EXISTS
 *                            MFI_STAT_PD_IN_TRANSPORT_READY_STATE
 *                            MFI_STAT_LD_IN_TRANSPORT_READY_STATE
 *                            MFI_STAT_LD_NOT_IN_TRANSPORT_READY_STATE
 *                      4. Removed MFI_STAT
 *                            MFI_STAT_TRANSPORT_READY_INCOMPATIBLE
 *                      5. Changed mailbox definition of MR_DCMD_LD_SET_TRANSPORT_READY
 *
 * 10/14/15  NK         1. Added MR_CTRL_PROP.OnOffProperties2.enableLargeQD
 *                      2. Added MR_CTRL_INFO.adapterOperations3.supportLargeQD
 *                      3. Added MR_MFC_DEFAULTS.enableLargeQD
 *                      4. Added supportQDThrottling to MFI_CAPABILITIES
 *
 * 11/03/15  SM         1. Added fields
 *                            MR_CTRL_INFO.adapterOperations3.supportHostDebug
 *                      2. Added DCMD
 *                            MR_DCMD_CTRL_SET_DEBUG_ATTRIBUTES
 *                      3. Add new structures/unions
 *                            MR_DEBUG_INPUTS
 *                      4. Added events
 *                            MR_EVT_NVCACHE_BLOCK_RECOVERY_SKIPPED
 *                      5. Added MFI_STAT
 *                            MFI_STAT_PD_READY_FOR_REMOVAL
 *                            MFI_STAT_WRONG_STATE for few missing DCMDs
 *
 * 01/25/16  NN/SM      1. Added fields
 *                            MR_CFG_EPD_PARAMS.exposeMultipath
 *                            MR_CFG_EPD_PARAMS.supportXCopy
 *                            MR_AUTO_CFG_PARAMETER.supportMultiBehaviorMode
 *                      2. Added enum value MR_CFG_EPD_PASSTHROUGH
 *                      3. Added mbox option w[0] = 3 for DCMD MR_DCMD_CFG_AUTOCFG
 *                      4. Added mbox option b[4] = MR_AUTO_CFG_OPTIONS for DCMD MR_DCMD_CFG_AUTOCFG
 *                      5. Added member MR_EVT_ARG_LD.isEPD
 *                      6. Added event arguments
 *                            MR_EVT_ARGS_AUTO_CFG_OPTION
 *                            MR_EVT_ARGS_LD_EPD
 *                      7. Added events
 *                            MR_EVT_CTRL_AUTO_CFG_OPTION
 *                            MR_EVT_CTRL_AUTO_CFG_PARAMETERS_CHANGED
 *                      8. Added MFI_STAT
 *                            MFI_STAT_HOST_REBOOT_REQUIRED
 *
 *
 * 01/29/16  NK         1. Added MFI stat
 *                            MFI_STAT_MICROCODE_UPDATE_PENDING
 *                            MFI_STAT_MICROCODE_UPDATE_IN_PROGRESS
 *                      2. Added mbox parameter for MR_DCMD_CTRL_PD_FW_DOWNLOAD_PREPARE
 *                      3. Added DCMD MR_DCMD_CTRL_PD_FW_DOWNLOAD_PROGRESS
 *                      4. Added struct MR_PD_FW_DOWNLOAD_STATUS
 *                      5. Added MR_CTRL_INFO.pdOperations.supportParallelPDFwUpdate
 *                      6. Added enableTransportReady to MR_MFC_DEFAULTS
 *
 * 03/23/16  NK         1. Added fields
 *                            MR_PD_INFO.properties.ISECapable
 *                            MR_ERASE_TYPE.MR_ERASE_CRYPTOGRAPHIC
 *                            MR_CTRL_INFO.pdOperations.supportCryptoErase
 *                            MR_SS_ERASE_TYPE.MR_SS_ERASE_CRYPTOGRAPHIC
 *                      2. Added MFI stat: MFI_STAT_PD_ERASE_TYPE_WRONG
 *
 * 04/29/16  SM         1. Added fields
 *                            MR_CFG_EPD_PARAMS.multiInitSupport
 *                      2. Added MFI stat
 *                            MFI_STAT_AUTOCONFIG_EXISTS
 *                            MFI_STAT_SECURE_DEVICE_EXISTS
 *                      3. Updated return status for DCMD MR_DCMD_CTRL_LOCK_KEY_DESTROY
 *
 * 05/06/16  SH/SM      -- Merge of Ventura Dev stream API --
 *                      1. Changes in event.h
 *                            Added field ldIndexMsb to MR_EVT_ARG_LD
 *                            Added field enclDeviceIdMsb to MR_EVT_DETAIL
 *                            Added field enclDeviceIdMsb to pdPair structure in MR_EVT_DETAIL
 *                            Added events:
 *                                  MR_EVT_CTRL_PROFILE_UPDATE_SUCCESSFUL
 *                                  MR_EVT_CTRL_PROFILE_AUTOSELECTED
 *                                  MR_EVT_CTRL_PROFILE_PHYSICAL_DEVICEID_CHANGE
 *                      2. Changes in eventmsg.h
 *                            Added events:
 *                                  CTRL_PROFILE_UPDATE_SUCCESSFUL
 *                                  CTRL_PROFILE_AUTOSELECTED
 *                                  CTRL_PROFILE_PHYSICAL_DEVICEID_CHANGE
 *                      3. Changes in mfi.h
 *                            Added fields
 *                                  MFI_REG_STATE.scan.devIDMsb
 *                                  MFI_HDR.idInStatusOut.targetIDMsb
 *                                  MFI_FRAME_LD_READ_WRITE.idInStatusOut.targetIDMsb
 *                                  MFI_FRAME_SCSI.idInStatusOut.targetIDMsb
 *                                  MFI_FRAME_STP.targetIdMsb
 *                            Added MR_RAID_CTX_G35 and MR_RAID_CTX_G2 structures and associated defines
 *                                  to use proper RAID context based on product generation
 *                      4. Changes in mr.h
 *                            Added defines MAX_API_*_EXT2 to support up to 8192 devices
 *                            Added defines for profile support
 *                            Added/modified parameters to indicate support for > 256 devices to DCMDs:
 *                                  CTRL_EVENT_GET, CTRL_EVENT_WAIT, CTRL_PR_GET_PROPERTIES, CTRL_CC_SCHEDULE_GET_STATUS,
 *                                  CTRL_CC_SCHEDULE_GET_PROPERTIES, CTRL_CC_SCHEDULE_SET_PROPERTIES,
 *                                  CTRL_IO_METRICS_GET, CTRL_UNMAP_STATISTICS_GET, CTRL_SSC_METRICS_GET,
 *                                  CTRL_SWZONE_RESERVATION_RELEASE, PD_GET_ALLOWED_OPS_ALL_PDS, LD_GET_LIST,
 *                                  LD_LIST_QUERY, LD_GUID_LIST_GET, LD_GET_INFO, LD_GET_PROPERTIES,
 *                                  LD_GET_SMART_INFO, LD_RECONS_START, LD_RECONS_GET_INFO, LD_PROGRESS, LD_DELETE,
 *                                  LD_LOCATE_START, LD_LOCATE_STOP, LD_GET_ALLOWED_OPS, LD_GET_PINNED_LIST,
 *                                  LD_DISCARD_PINNED, LD_BBM_LOG_GET, LD_BBM_LOG_LIST_GET, LD_SSCD_GET_LDS,
 *                                  LD_SSCD_BOOST_PRIORITY_TYPE_GET, LD_VF_MAP_GET, LD_VF_MAP_GET_ALL_LDS,
 *                                  LD_VF_MAP_SET_MULTIPLE, CFG_READ, CFG_ADD, CFG_CLEAR, CFG_GET_ALLOWED_OPS,
 *                                  CFG_FOREIGN_READ, CFG_FOREIGN_READ_AFTER_IMPORT, CFG_FOREIGN_CLEAR
 *                            Added new profile Id parameter to MR_DCMD_CTRL_PERSONALITY_SET
 *                            Added new MR_BOOT_ENV parameter to MR_DCMD_CTRL_BIOS_BOOT_MSG_GET
 *                            Added new MR_DCMD_CTRL_PROFILE_LIST_GET
 *                            Added structures
 *                                  MR_LD_BITMAP_EXT, MR_PD_BITMAP_EXT, MR_ARRAY_EXT, MR_PR_PROPERTIES_EXT2,
 *                                  MR_CC_SCHEDULE_PROPERTIES_EXT2, MR_CTRL_PROFILE_ENTRY, MR_CTRL_PROFILE_LIST
 *                            Added fields
 *                                  MR_CTRL_INFO.maxTransportableVDsMsb
 *                                  MR_LD_REF.targetIdMsb
 *                                  MR_IO_METRICS_LD_CACHE.targetIdMsb
 *                                  MR_IO_METRICS_SSC_DETAIL.targetIdMsb
 *                                  MR_IO_METRICS_LD_OVERALL.targetIdMsb
 *                                  MR_LD_METRICS.targetIdMsb
 *                                  MR_LD_VF_AFFILIATION.ldCountMsb
 *                                  MR_PR_STATUS.numPdDoneMsb
 *                                  MR_CC_SCHEDULE_STATUS.numLdDoneMsb
 *                                  MR_NVCACHE_ERASE_INFO.blockSize
 *                            Added structure adapterOperations4 and fields ctrlInfoExtSupported, supportIbuttonLess, and
 *                                  supportedEncAlgo in MR_CTRL_INFO
 *                            Added fields size, pad1 and structures supportedProfileConfig, supportedFWConfig,
 *                                  and adapterOperationsExt as extension of MR_CTRL_INFO
 *                            Changed size of pdFwDownloadMap array to MAX_API_PHYSICAL_DEVICES_EXT2/8
 *                            Added NVMe to intf field comment in pdType structure in MR_PD_DDF_TYPE
 *                            Changed size of allowedOpsList array to MAX_API_PHYSICAL_DEVICES_EXT2
 *                            Added union for U8 targetId[1] and new U16 targetIdExt[1] to MR_LD_TARGETID_LIST
 *                            Changed size of bbmLog array to MAX_API_LOGICAL_DRIVES_EXT2
 *                            Sturcture encl in MR_ARRAY was marked as DEPRICATED for configurations > 240 PDs or LDs
 *                            Added union of array[1] and arrayExt[1] in MR_CONFIG_DATA
 *                            Added values for 256 and 512 LDs to enum MR_LD_LIMIT
 *                            Changed size of unmappedSectorCount array to MAX_API_LOGICAL_DRIVES_EXT2
 *                            Added enum
 *                                  MR_BOOT_ENV
 *                                  MR_BLOCK_SIZE
 *
 * 05/14/16  SH         1. Changes in mfi.h
 *                            Added field maxRaidMapSize to MFI_REG_STATE_EXT2
 *                      2. Changes in mr.h
 *                            Added field MR_CTRL_INFO.adapterOperations4.supportEncryptedMfc
 *
 * 05/20/16  SH/NK      1. Changes in mfistat.h
 *                            Added MFI stats
 *                               MFI_STAT_FRU_INVALID_HOST_DATA
 *                               MFI_STAT_FRU_INVALID_CTRL_DATA
 *                               MFI_STAT_IMAGE_NOT_FOUND
 *                      2. Changes in mr.h
 *                            Added DCMDs
 *                               MR_DCMD_CTRL_SYNC_FRU_STATE
 *                               MR_DCMD_CTRL_GET_FRU_ID
 *                               MR_DCMD_CTRL_FLASH_FW_UPLOAD
 *                            Added enum MR_IOV_CTRL_FRU_STATE_DIRECTION
 *                            Added structure MR_IOV_CTRL_FRU_STATE
 *                            Added field MR_CTRL_INFO.adapterOperations4.imageUploadSupported
 *                            In MR_LD_TARGETID_LIST, changed element targetIdExt[1] to targetIdExt[2]
 *                              to maintain overall structure U32 alignment
 *
 * 05/23/16  NK         1. Added SESVPDAssociationTypeInMultiPathCfg in MR_MFC_DEFAULTS, MR_CTRL_PROP.OnOffProperties2
 *                      2. Added field MR_CTRL_INFO.adapterOperations4.supportSESCtrlInMultipathCfg
 *
 * 06/24/16  NK         1. Added MFI_REG_STATE_EXT1.ready.canHandleSyncCache
 *                      2. Added MFI_REG_STATE_EXT1.operational.canHandleSyncCache
 *
 * 07/11/16  NK         1. Added MFI stat MFI_STAT_NVCACHE_ERROR
 *                      2. Updated error status for DCMDs:
 *                             MR_DCMD_CTRL_TTY_BBU_SET_ON
 *                             MR_DCMD_CTRL_TTY_BBU_SET_TEMP_ON
 *                             MR_DCMD_CTRL_TTY_BBU_SET_OFF
 *
 * 07/20/16  NK/SH      1. Added MR_RAID_FLAGS_IO_SUB_TYPE_LDIO_BW_LIMIT = 7 to MR_RAID_FLAGS_IO_SUB_TYPE (mfi.h)
 *                      2. Added MR_DEV_HANDLE_INFO.interfaceType
 *                      3. Added MR_CTRL_INFO.adapterOperations4.supportPdMapTargetId
 *                      4. Added MFI_CAPABILITIES.mfiCapabilities.supportPdMapTargetId (mfi.h)
 *                      5. Updated event MR_EVT_PD_NOT_ENTERPRISE_SELF_ENCRYPTING_DRIVE (eventmsg.h)
 *                      6. Added mbox parameter for DCMD MR_DCMD_BBU_VPD_GET
 *                      7. Added support bit MR_CTRL_INFO.adapterOperations4.FWSwapsBBUVPDInfo
 *                      8. Updated comments in struct MR_BBU_VPD_INFO_FIXED
 *                      9. Added OOB control command MFI_OOB_CONTROL_GENERIC_INFO
 *                     10. Added enum MFI_OOB_CTRL_INFO_PARAMS
 *                     11. Added union MFI_OOB_CTRL_GENERIC_INFO
 *                     12. Added enum MFI_OOB_METHOD
 *
 * 07/28/16  SH        Added MR_CTRL_PERSONALITY_JBOD to MR_CTRL_PERSONALITY enum
 *
 * 08/17/16  NK        1. Added DCMDs: MR_DCMD_BBU_SBAT_INFO_GET, MR_DCMD_BBU_SBAT_INFO_SET
 *                     2. Added struct MR_SBAT_INFO
 *                     3. Added enum MR_SBAT_POWER_STATUS
 *
 * 11/14/16  SH        1. Added MR_CTRL_INFO.adapterOperations4.supportSSCrev3
 *                     2. Added MR_CTRL_INFO.maxSSCDs
 *                     3. Added MR_CTRL_INFO.minConfigurableSSCSize
 *                     4. Added MFI stat MFI_STAT_SIZE_LESS_THAN_MINIMUM_LIMIT
 *                     5. Added MR_LD_PROPERTIES.SSCWriteBufferMode
 *                     6. Added MR_MFC_DEFAULTS.SSCWriteBufferMode
 *                     7. Added MFI_REG_STATE_EXT3.operational.CurrentFwNvmePageSize
 *                     8. Added MR_DCMD_PD_ERROR_STATS
 *                     9. Added MR_PD_ERROR_STATS structure
 *                    10. Added MR_EVT_PD_ERROR_STATS to MR_EVT in event.h
 *                    11. Added PD_ERROR_STATS to eventmsg.h
 *                    12. Added MR_CTRL_INFO.adapterOperations5.supportCVHealthInfo
 *                    13. Added MR_BBU_FW_STATUS.mrBbuFwStatus.cacheVaultDisabled
 *                    14. Added MR_DCMD_SC_BKPLN_GET_DATA
 *                    15. Added MR_PD_INFO.name[20] in union with inquiryData[96]
 *                    16. Added MR_PD_INFO.totalSlots in union with slotNumber, enclConnectorIndex
 *                    17. Added MR_PD_INFO.boxNumber
 *                    18. Added enum MR_ENCL_TYPE_SC to MR_ENCL_TYPE
 *                    19. Added MR_SC_BKPLN_DATA structure
 *
 * 11/15/16  SM        1. Added backend PCI configuration DCMDs
 *                            MR_DCMD_PCIE_CONFIG_GET_LANE_CONNECTIONS
 *                            MR_DCMD_PCIE_LINK_CONFIG_GET
 *                            MR_DCMD_PCIE_LINK_CONFIG_SET
 *                            MR_DCMD_PCIE_CONFIG_SET_LANE_SPEED
 *                     2. Added define
 *                            MAX_LANES_PER_CONTROLLER
 *                            MAX_CSW_PER_CONTROLLER
 *                            MAX_PCIE_LANE_PER_CSW
 *                            MR_WWID_INVALID
 *                     3. Added fields
 *                            MR_CTRL_INFO.deviceInterface.maxPciLinkSpeed
 *                            MR_CTRL_INFO.deviceInterface.pciLinkSpeed
 *                            MR_CTRL_INFO.adapterOperations5.supportPCIe
 *                     4. Added enum
 *                            MR_PCIE_LANE_SPEED
 *                            MR_PCIE_LINK_CONFIG_OPTIONS
 *                     5. Added structures
 *                            MR_PCIE_LANE_CONNECTIONS
 *                            MR_PCIE_LANE_CONNECTION
 *                            MR_PCIE_LINK_CONFIG
 *                            MR_PCIE_LINK_CONFIG_INFO
 *
 * 11/18/16  SM        1. Added MFI_STAT_INVALID_DRIVE_COUNT as valid exit code for MR_DCMD_LD_GET_SPAN_LIST
 *                     2. Added enum MR_PCI_LINK_SPEED_GEN4
 *                     3. Marked supportUnevenSpans as deprecated
 *                     4. Renamed supportUnevenSpans as supportAutoSelectSpans
 *                     5. Extended enum MR_PD_SPEED for NVMe drive speed support
 *                     6. Some unnecessary space remomal from end of line
 *
 * 11/25/16  NK        1. Removed bit supportAutoSelectSpans. Corrected comment for supportUnevenSpans
 *                     2. Corrected padding for struct MR_CTRL_INFO
 *
 * 12/06/16  SH        1. Added MR_CTRL_PROFILE_ENTRY.isNVMeOnlyProfile
 *
 * 01/03/17  NK        1. Added errorCode, msgSeverity to struct MR_BOOT_MESSAGE_DATA
 *                     2. Added bit doubleDip to struct MFI_REG_STATE.bootMsgPending
 *
 * 01/17/17  SM        1. Added DCMDs
 *                            MR_DCMD_PD_SC_READ_NVRAM
 *                            MR_DCMD_PD_SC_CLEAR_NVRAM
 *                     2. Updated DCMD MR_DCMD_SC_BKPLN_GET_DATA to get all NVRAM data in one call
 *                     3. Added Event information
 *                            New event parameter MR_EVT_ARG_PD_EXT is being added for event locale
 *                            MR_EVT_LOCALE_PD and MR_EVT_LOCALE_ENCL. This structure will always go
 *                            along with MR_EVT_ARG_PD as an event argument.
 *                     4. Event Message string change
 *                            For the following events the string "LD" changed to "VD" in event messages
 *                                LD_PI_LOST
 *                                LD_PI_ERR_IN_CACHE
 *                                HA_TOPOLOGY_LD_INCOMPATIBLE
 *                                LD_OPERATION_DELAYED
 *                                LD_PARTIALLY_HIDDEN
 *                     5. Added structures
 *                            MR_SC_BKPLN_NVRAM_DATA
 *                            MR_SC_NVRAM_DATA
 *                     6. Added MFI status
 *                            MFI_STAT_SC_AUTH_FAILURE
 *                            MFI_STAT_SC_NOT_FOUND
 *
 * 01/25/17 AK         1. Modified MFI_FAULT, added MFI_FAULT_INTERNAL_USE
 *
 * 02/28/17 MT         1. Event Message string changes to eliminate duplicate "VD VD" displayed in some events
 *                     2. Added new events corresponding to some boot message cases
 *                     3. Return pendingBootMsgCount in a previously-reserved field of MR_DCMD_CTRL_BIOS_BOOT_MSG_GET return data
 *                     4. Dual FW changes to MR_DCMD_CTRL_FLASH_FW_FLASH, mbox.b[1] and mbox.b[2]
 *                     5. New flags in MR_CTRL_INFO for Dual FW
 *
 * 04/13/17 MT         1. Added DCMDs:
 *                          MR_DCMD_CTRL_HOST_INFO_SET
 *                          MR_DCMD_CTRL_HOST_INFO_GET
 *                     2. Added structure
 *                          MR_HOST_INFO
 *                     3. Revised meaning of mbox.b[2] field in MR_DCMD_CTRL_FLASH_FW_FLASH
 *                     4. Revised meaning of mbox.b[0] field in MR_DCMD_CTRL_FLASH_COMP_INFO_GET, to select which set of components to report
 *                     5. Modified structure
 *                          MR_FLASH_COMP_INFO -- added union so that package version string length can be up to 64 bytes
 *                     6. Added new MR_FLASH_NAME_* strings for additional modules possibly in flash
 *                     7. Added new support bits:
 *                          MR_CTRL_INFO.adapterOperations4.supportFlashCompInfo
 *                          MR_CTRL_INFO.adapterOperations4.supportHostInfo
 *                     8. New events:
 *                          MR_EVT_CTRL_DUAL_FW_VERSION_MISMATCH
 *                          MR_EVT_CTRL_PACKAGE_VERSION_BACKUP
 *                          MR_EVT_CTRL_CACHE_RESTORE_ERROR
 *
 * 04/25/17  NK        1. Added event MR_EVT_PD_LOCK_KEY_UNLOCKED_ESCROW
 *
 * 04/27/17  SM        1. Added enum MR_ENCL_TYPE_MC to MR_ENCL_TYPE
 *                     2. Added MR_CTRL_INFO.adapterOperations5.supportExtMfgVPD
 *
 * 05/08/17  SM        1. Added support for strict 64 bit DMA mask to handle the use case of SWTLB DMA buffer above 4GB
 *                        by adding MFI_REG_STATE_EXT1.canHandle64BitMode and MFI_CAPABILITIES.support64BitMode
 *                     2. Remove all unwanted end of line spaces
 *
 * 05/25/17  SM        1. Added new event MR_EVT_PD_TEMP_FATAL
 *                     2. Added MFI status MFI_STAT_FLASH_SAME_IMAGE
 *
 * 06/01/17  SM        1. Change enum definition for deviceInterface.pciLinkSpeed
 *                     2. Added new enum MR_ENCL_SUBTYPE
 *                     3. Added a enclosure subtype in MR_ENCL_INFO_EXT
 *
 * 06/08/17  SM        1. Added new enum MR_ENCL_SUBTYPE_IOM_INT and MR_ENCL_SUBTYPE_IOM_EXT in MR_ENCL_SUBTYPE
 *                     2. Removed enum MR_ENCL_TYPE_MC from MR_ENCL_TYPE
 *
 * 07/05/17  NK        1. Updated DCMD MR_DCMD_CTRL_TTY_CLEAR
 *                     2. Updated DCMD MR_DCMD_CTRL_TTY_READ. The DCMD now supports retrieval of PL logs
 *                     3. Added support bit MR_CTRL_INFO.adapterOperations4.supportPLDebugInfo
 *
 * 07/25/17  NK        1. Added event MR_EVT_CTRL_MEMORY_ERROR
 *
 * 07/26/17  SM        1. Added support bit MFI_CAPABILITIES.supportNVMePassthru
 *                     2. Added support bit MR_CTRL_INFO.adapterOperations4.supportNVMePassthru. Driver should check
 *                        this bit and should not send NVMe passthru to FW if not supported by FW
 *
 * 08/13/17  SM        1. Added support bit MR_CTRL_INFO.adapterOperations4.supportOOBCtrlFwDownload
 *
 * 08/18/17  AK        1. Added isWBSupported to MR_CTRL_PROFILE_ENTRY
 *
 * 08/18/17  AK        1. Added MR_EVT_CTRL_FW_INCOMPATIBLE_WITH_PROFILE event
 *
 * 09/20/17  NK        1. Added MFI stat MFI_STAT_UNMAP_NOT_SUPPORTED
 *                     2. Added enableUnmapSupport to MR_MFC_DEFAULTS
 *                     Note: Unmap feature is controlled with the following knobs:
 *                     NVDATA - Global switch to enable/disable Unmap support
 *                            - If disabled, the feature can be enabled through MR_LD_PROPERTIES.scsiUnmapEnabled
 *                            - If enabled, can be disabled for individual LDs through MR_LD_PROPERTIES.scsiUnmapEnabled
 *                     Apps need to check MR_CTRL_INFO.ldOperations.supportScsiUnmap to know whether Unmap is supported for LDs by FW
 *                     For those apps that can check allowedOps, MR_LD_ALLOWED_OPS.scsiUnmap provides information about the Unmap support for the LD
 *
 * 09/27/17  AK        1. Added MR_DCMD_OOB_MODE_SET and MR_DCMD_OOB_MODE_GET commands
 *                     2. Added MFI_OOB_BINDING_LSI_SMBUS to MFI_OOB_METHOD.
 *                     3. Added MFI_OOB_METHOD_INFO structure
 *
 * 10/05/17  AK        1. Modified comment for MR_DCMD_CTRL_PERSONALITY_SET about status MFI_STAT_OPERATION_NOT_POSSIBLE to
 *                        include when an unsupported personality is requested.
 *
 * 11/27/17  NK        1.  Added new enum MR_ENCL_SUBTYPE_SPADE in MR_ENCL_SUBTYPE
 *                     2.  Added metaStrActiveColumn in MR_DEBUG_QUEUE_CONFIG
 *                     3.  Added mbox options for DCMD MR_DCMD_CTRL_TTY_READ
 *                     4.  Added MFI stat MFI_STAT_PD_CLEAR_IN_PROGRESS as a possible return value for DCMD MR_DCMD_PD_STATE_SET
 *                     5.  Added/Updated mbox options to DCMD  MFI_DCMD_PD_SECURE_ERASE_START
 *                     6.  Added possible return values for DCMD MR_DCMD_PD_SECURE_ERASE_START
 *                     7.  Added possible return values for DCMD MR_DCMD_PD_SECURE_ERASE_ABORT
 *                     8.  Added supportSanitize in MR_CTRL_INFO.pdOperations
 *                     9.  Deprecated MR_ERASE_CRYPTOGRAPHIC, MR_SS_ERASE_CRYPTOGRAPHIC options
 *                     10. Added enum MR_SANITIZE_TYPE
 *                     11. Added sanitize in MR_PD_PROGRESS
 *                     12. Added sanitize in MR_PD_ALLOWED_OPS
 *                     13. Added MR_PD_INFO.properties.SanitizeType
 *                     14. Added MFI stat MFI_STAT_PD_SANITIZE_TYPE_UNSUPPORTED
 *                     15. Deprecated MR_CTRL_INFO.pdOperations.supportCryptoErase, MR_PD_INFO.properties.ISECapable
 *                     16. Added MR_BBU_TYPE_SMARTBATTERY in MR_BBU_TYPE
 *                     17. Added batteryCableFailure in MR_BBU_FW_STATUS
 *                     18. Added MR_RAID_FLAGS_IO_SUB_TYPE_R56_DIV_OFFLOAD in MR_RAID_FLAGS_IO_SUB_TYPE
 *                     19. Added r56ArmMap in MR_RAID_CTX_G35
 *
 * 12/01/17  SM        1. Added support bit MR_CTRL_INFO.adapterOperations5.supportOCEOnly
 *
 * 12/04/17  AK        1. Added MR_DIAG_TEST_LINK to MR_DIAG_TEST and incremented MR_MAX_DIAG_TEST.
 *
 * 01/09/18  AK        1. Added MR_CTRL_INFO.NVMeTaskAbortTO and MR_CTRL_INFO.NVMeMaxControllerResetTO
 *                     2. Added MR_CTRL_INFO.adapterOperations5.supportNVMeTM
 *
 * 01/12/18  NK        1. Added enum MR_CTRL_WRITE_CACHING_STATUS
 *                     2. Added MR_CTRL_INFO.writeCachingStatus
 *                     3. Added MR_PD_STATE_SANITIZE in MR_PD_STATE
 *                     4. Added mask value for Smart Battery
 *                     5. Added event MR_EVT_CTRL_WRITE_CACHING_STATUS_CHANGED
 *
 * 01/19/18  SM        1. Removed enum MR_CTRL_WRITE_CACHING_STATUS
 *                     2. Removed MR_CTRL_INFO.writeCachingStatus
 *                     3. Added extEventInfo and extArgs in MR_EVT_DETAIL
 *                     4. Added pdErase extEventInfo in MR_EVT_DETAIL
 *                     5. Changed MR_PD_INFO.properties.SanitizeType to MR_PD_INFO.properties.sanitizeType
 *                     6. Changed MR_SANITIZE_TYPE to bitmap
 *                     7. Removed event MR_EVT_CTRL_WRITE_CACHING_STATUS_CHANGED
 *                     8. Added the following new events: MR_EVT_PD_ARRAY_DEVICE_SLOT_FAILED,
 *                                                        MR_EVT_ENCL_TEMPERATURE_ABOVE_CRITICAL,
 *                                                        MR_EVT_ENCL_SERVICE_CTRL_EMM_CRITICAL,
 *                                                        MR_EVT_ENCL_SERVICE_CTRL_EMM_WARNING,
 *                                                        MR_EVT_ENCL_ELEMENT_CRITICAL,
 *                                                        MR_EVT_ENCL_ELEMENT_WARNING,
 *                                                        MR_EVT_ENCL_SAS_CONNECTOR_ERROR
 *                     9. Added string replacement so FW will put appropriate string (Erase or Sanitize) for PD_ERASE events
 *
 * 02/08/18  NK        1. Added MR_CTRL_INFO.adapterOperations4.DivertIOWithChainFrame
 *                     2. Added flags divertedFPIO, IOPInitiated in MR_RAID_CTX_G2
 *
 * 02/14/18  DM        1. Added event MR_EVT_CTRL_FW_VALID_SNAPDUMP_AVAILABLE
 *                     2. Added following structures
 *                          MR_SNAPDUMP_PROPERTIES
 *                          MR_SNAPDUMP_HDR
 *                          MR_SNAPDUMP_INFO
 *                          MR_SNAPDUMP_DATA
 *                     3. Added Snapdump management DCMDS
 *                          MR_DCMD_CTRL_SNAPDUMP_GET_PROPERTIES
 *                          MR_DCMD_CTRL_SNAPDUMP_SET_PROPERTIES
 *                          MR_DCMD_CTRL_SNAPDUMP_GET
 *                     4. Added following fields
 *                          MR_CTRL_INFO.adapterOperations5.supportSnapDump
 *                          MR_CTRL_PROP.OnOffProperties2.enableSnapDump
 *                     5. Added two entries in MFI_STAT
 *                          MFI_STAT_SNAPDUMP_NOT_AVAILABLE
 *                          MFI_STAT_SNAPDUMP_NOT_ENABLED
 *                     6. Added triggerSnapDump in MFI_CONTROL_REG
 *
 * 02/23/18   BJH      1. Added MFI status MFI_STAT_INVALID_POLICY.
 *                     2. Added details to ldOperations.ioPolicy.
 *
 * 02/28/18  PN        1. Added MR_MFC_DEFAULTS.enableFDETypeMix
 *                     2. Added MR_CTRL_INFO.adapterOperationExt.adapterOperations5.supportFDETypeMix
 *                     3. Added MR_PD_INFO.security.fdeType
 *                     4. Added enum MR_PD_FDE_TYPE
 *
 * 2/28/18   SS        1. Added enableParallelPDFwUpdate to MR_MFC_DEFAULTS.
 *
 * 03/15/18  SS/VV     1. Added events
 *                          MR_EVT_CTRL_BOOT_LDS_WILL_GO_OFFLINE_EXT
 *                          MR_EVT_CTRL_BOOT_LDS_MISSING_EXT
 *
 * 03/20/18  VV        1. Added event MR_EVT_CTRL_LDS_CACHE_DISCARDED_AUTO
 *                     2. Added possible return values for DCMD MR_DCMD_CFG_AUTOCFG
 *                     3. Added enum MR_PINNED_CACHE_DISCARD_TIMER_OPTIONS
 *                     4. Added MR_CFG_EPD_PARAMS.disableRaidConfig
 *                     5. Added MR_CFG_EPD_PARAMS.pinnedCacheDiscardTimerOptions
 *                     6. Added MR_CFG_EPD_PARAMS.failPDOnReadME
 *                     7. Added MR_CFG_EPD_PARAMS.enableWB
 *
 * 03/23/18  PR        1. Added return MFI_STAT_WRONG_STATE for MR_DCMD_PD_GET_INFO.
 *
 * 03/22/18  GK/VV     1. Added MR_PD_INFO.security.fdeNotSupported
 *
 * 03/23/18  AS/VV     1. Added MFI Status MFI_STAT_CONFIG_PRESENT_ERROR
 *
 * 04/09/18  VT        1. Added MR_ENCL_SUBTYPE_VSES
 *                     2. Added MR_ENCL_SUBTYPE_I2C
 *
 * 04/16/18  AK/SM     1. Revert PD_ERASE event text to only say "Erase".
 *                     2. Remove extended event info from MR_EVT_DETAIL
 *                     3. Create new events for sanitize:
 *                         MR_EVT_PD_SANITIZE_FAILED
 *                         MR_EVT_PD_SANITIZE_PROGRESS
 *                         MR_EVT_PD_SANITIZE_STARTED
 *                         MR_EVT_PD_SANITIZE_SUCCESSFUL
 *
 * 04/18/18  VV        1. Added force option to MR_DCMD_CTRL_PERSONALITY_SET
 *                     2. Added supportEPDWithAutoConfigOff flag to MR_AUTO_CFG_PARAMETER
 *
 * 05/10/18  RK        1. Added bit supportForcePersonalityChange. This bit indicates
 *                         whether Forced personality change operation is supported or not.
 *
 * 05/15/18  HNR/VV    1. Added event MR_EVT_LD_CANNOT_BE_SECURED_IN_FUTURE
 *
 * 05/15/18  VV        Added the following for enhanced unmap support:
 *                     1. MR_CTRL_PROP.OnOffProperties2.disableScsiUnmap
 *                     2. MR_PD_INFO.properties.unmapCapableForLDs
 *                     3. MR_LD_PARAMETER.characteristics.scsiUnmapEnabled
 *
 * 05/15/18  VB/VV     1. Remove enableWB from MR_CFG_EPD_PARAMS
 *                     2. Remove disableRaidConfig from MR_CFG_EPD_PARAMS
 *
 * 05/29/18  VM        1. Added MFI Status MFI_STAT_OK_SHUTDOWN_REQUIRED
 *                     2. Added bit MR_CTRL_INFO.hwPresent.upgradablePSOC
 *                     3. Added MR_CTRL_INFO.adapterOperationExt.adapterOperations5.supportPSOCUpdate
 *                     4. Added new struct MR_CTRL_INFO.adapterStatus2
 *                     5. Added bit MR_CTRL_INFO.adapterStatus2.systemShutdownRequired
 *                     6. Added MR_IMAGE_PSOC to enum MR_IMAGE_TYPE
 *
 * 06/12/2018 AJ       1. DEPRECATE the curNumSupported and maxNumSupported of MR_SNAPDUMP_PROPERTIES
 *
 * 06/13/2018 AJ       1. Added  snapDumpTime to MR_SNAPDUMP_HDR
 *
 * 06/14/2018 VV       Added the following events for enhanced unmap support:
 *                     1. MR_EVT_LD_NON_UNMAP_CAPABLE_PD_USED
 *                     2. MR_EVT_LD_UNMAP_NOT_ENABLED
 *
 * 06/19/2018 VV       1. Added MFI status MFI_STAT_PD_NOT_ELIGIBLE_FOR_LD
 *
 * 06/20/2018 AJ       1. Added DCMD MR_DCMD_CTRL_SNAPDUMP_CLEAR
 *
 * 06/01/2018 SL       1. Added MR_EVT MR_EVT_PD_SSD_WEAR_GAUGE_CHANGED
 *                     2. MR_CTRL_INFO.pdOperations.supportWearGauge
 *                     3. Added MR_PD_INFO.properties.pdSupportsSSDWearGauge
 *                     4. Added MR_PD_INFO.ssdPercentUsed
 *                     5. Added MR_PD_INFO.ssdLifeRemainingInDays
 *
 * 07/13/2018 NT       1. Added MR_BBU_PROPERTIES.lastSuccessfulLearnTime
 *
 * 07/17/2018 BJH      1. Added supportSecureBoot to adapterOperations5
 *                     2. Added secureBootHashSlotsRemaining and secureBootDetails
 *                        to controller info
 *                     3. Added MFI_STAT_SECURE_BOOT_KEY_SLOTS_FULL
 *                     4. Added CTRL_SB_KEY_UPDATE_POWER_CYCLE event
 *
 * 07/18/2018 SH       1. Added DCMD MR_DCMD_DRIVER_RTTTRACE_BUFFER
 *
 * 07/25/2018 KR       1. Added MFI status MFI_STAT_SNAPDUMP_CLEAR_REQUIRED
 *
 * 07/31/2018 VB       1. Added DCMD MR_DCMD_CTRL_SNAPDUMP_ONDEMAND
 *                     2. Added following fields
 *                        MR_CTRL_INFO.adapterOperations5.supportOnDemandSnapDump
 *                        MR_CTRL_INFO.adapterOperations5.supportClearSnapDump
 *
 * 08/08/2018 GK       1. Deprecated MR_LD_PARAMETER.characteristics.scsiUnmapEnabled
 *                     2. Added new error code - MFI_STAT_CONFIG_UNMAP_AND_NON_UNMAP_MIX_NOT_ALLOWED
 *                     3. Added the below two events
 *                        MR_EVT_PD_DEDICATED_NON_UNMAP_SPARE_NO_LONGER_USEFUL
 *                        MR_EVT_PD_NO_UNMAP_CAPABLE_PD_FOR_REBUILD
 *                        MR_EVT_PD_NO_UNMAP_CAPABLE_PD_FOR_COPYBACK
 *
 * 08/20/2018 VV       Corrected the definition of DCMD MR_DCMD_CTRL_SNAPDUMP_ONDEMAND to avoid the conflict
 *                     with DCMD MR_DCMD_CTRL_RTTRACE_BUFFER
 *
 * 08/20/2018 SS       1. Added field MR_MFC_MFG_2.customSN
 *
 * 08/24/2018 AKJ      1. Updated DCMD
 *                         Added dcmd.mbox.b[0] and dcmd.mbox.b[1] definition for MR_DCMD_OOB_STATUS
 *                     2. Added bit needsEKMOverOOB to struct MFI_REG_STATE.bootMsgPending
 *
 * 08/27/2018 DC       Added MR_CTRL_INFO.adapterOperationExt.adapterOperations5.disableLeastLatencyMode
 *
 * 09/14/2018 SWT      1. Added MFI_REG_STATE_EXT1.ready.intCoalescing
 *                     2. Added MFI_FRAME_INIT.replyQueueMask
 *
 * 09/25/2018 RS       1. Added MR_CTRL_INFO.adapterOperationExt.adapterOperations5.supportSMPOverOOB
 *                     2. Updated MFI_OOB_CMD_PACKET1 to include structure smp
 *
 */


#ifndef INCLUDE_MR
#define INCLUDE_MR

/*
 * define macros for maximum parameters supported by the API. These are not necessarily the maximum currently available
 * or the maximum implemented on a given controller(ie iMR).  Host applications should continue to use CtrlInfo to determine
 * MAX_ suported/populated constraints.
 */
#define MAX_API_LOGICAL_DRIVES_LEGACY           64      // 64. But if all 64 are configured then reconstruction is not possible.
                                                    // Reconstruction is possible if configured logical drives are <=63.
#define MAX_API_PHYSICAL_DEVICES                256     // only 255 physical devices may be used. Also can be got from MR_CTRL_INFO.maxPds
#define MAX_API_PD_PER_ENCLOSURE                64      // Deprecated. FW can support >64 drives in an enclosure.
#define MAX_API_ROW_SIZE                        32      // maximum disks per array
#define MAX_API_ARRAYS_DEDICATED                16      // maximum number of arrays a hot spare may be dedicated to
#define MAX_API_ARRAYS_LEGACY                   128     // maximum number of arrays which may exist
#define MAX_API_FOREIGN_CONFIGS                 8       // Deprecated. Maximum number of foreign configs that may ha managed at once
#define MAX_API_SPARES_FOR_THE_CONTROLLER       MAX_API_PHYSICAL_DEVICES    // maximum spares (global and dedicated combined)
#define MAX_API_TARGET_ID_LEGACY                63      // maximum possible Target IDs (i.e. 0 to 63)
#define MAX_API_ENCLOSURES                      32      // maximum number of supported enclosures

#define MAX_SPAN_DEPTH                          8       // maximum spans per logical drive
#define MAX_PHYS_PER_CONTROLLER                 24      // maximum number of PHYs per controller
#define MAX_LANES_PER_CONTROLLER                MAX_PHYS_PER_CONTROLLER      // maximum number of LANES per controller
#define MAX_LDS_PER_ARRAY                       16      // maximum number of LDs per array (due to DDF limitations).
                                                        // Also can be got from MR_CTRL_INFO.maxLdsPerArray
#define MAX_SAS_QUADS_PER_CONTROLLER            4       // maximum number of SAS QUADs per controller
#define MAX_SAS_CONNECTORS_PER_CONTROLLER       8       // maximum number of SAS connectors (internal or external) per controller
#define MAX_CSW_PER_CONTROLLER                  8       // maximum number CSW supported by controller
#define MAX_PCIE_LANE_PER_CSW                   16      // maximum number of PCIe lanes managed by per CSW

#define MAX_API_VIEWS                           (MAX_API_LOGICAL_DRIVES_LEGACY)

#define MAX_API_LOGICAL_DRIVES_EXT              256     // 240/255. But if all are configured then reconstruction is not possible.
#define MAX_API_ARRAYS_EXT                      256     // maximum number of arrays which may exist
#define MAX_API_TARGET_ID_EXT                   255     // maximum possible Target IDs (i.e. 0 to 255)

#if defined(MR_IMPLEMENTED_MAX_API_EXT2)
#define MAX_API_PHYSICAL_DEVICES_EXT2           8192    // only 8192 physical devices may be used. Also can be got from MR_CTRL_INFO.maxPds
#define MAX_API_LOGICAL_DRIVES_EXT2             8192    // 8192. But if all are configured then reconstruction is not possible.
#define MAX_API_ARRAYS_EXT2                     8192    // maximum number of arrays which may exist
#define MAX_API_TARGET_ID_EXT2                  8192    // maximum possible Target IDs (i.e. 0 to 8191)
#else
#define MAX_API_PHYSICAL_DEVICES_EXT2           MAX_API_PHYSICAL_DEVICES
#define MAX_API_LOGICAL_DRIVES_EXT2             MAX_API_LOGICAL_DRIVES_EXT
#define MAX_API_ARRAYS_EXT2                     MAX_API_ARRAYS_EXT
#define MAX_API_TARGET_ID_EXT2                  MAX_API_TARGET_ID_EXT
#endif

#if defined(MAX_LOGICAL_DRIVES_LIMIT_64)
#define MAX_API_LOGICAL_DRIVES              (MAX_API_LOGICAL_DRIVES_LEGACY)
#define MAX_API_TARGET_ID                   (MAX_API_TARGET_ID_LEGACY)
#define MAX_API_ARRAYS                      (MAX_API_ARRAYS_LEGACY)
#endif

//These defines below will be used by API files for applications to preserve the original defines.
#if !defined(MR_IMPLEMENTED_MAX_NOT_EQUAL_API_MAX)
#if defined(MAX_LOGICAL_DRIVES_LIMIT_64)
#define MAX_LOGICAL_DRIVES                  (MAX_API_LOGICAL_DRIVES)
#define MAX_TARGET_ID                       (MAX_API_TARGET_ID)
#define MAX_ARRAYS                          (MAX_API_ARRAYS)
#endif
#define MAX_PHYSICAL_DEVICES                (MAX_API_PHYSICAL_DEVICES)
#define MAX_PD_PER_ENCLOSURE                (MAX_API_PD_PER_ENCLOSURE)
#define MAX_ARRAYS_DEDICATED                (MAX_API_ARRAYS_DEDICATED)
#define MAX_FOREIGN_CONFIGS                 (MAX_API_FOREIGN_CONFIGS)
#define MAX_SPARES_FOR_THE_CONTROLLER       (MAX_API_SPARES_FOR_THE_CONTROLLER)
#define MAX_ROW_SIZE                        (MAX_API_ROW_SIZE)
#define MAX_ENCLOSURES                      (MAX_API_ENCLOSURES)
#endif
/*
 * define macros for MR API versioning
*/
#define  MR_API_VERSION             3     // Incremented every time API is updated.
                                          // Allows application to know API is updated, some fields may not be
                                          // available.  No action needed, no need to inform user.
#define  MR_API_FWD_COMPATIBILITY   0     // Incremented when API won't be compatible with existing Apps.
                                          // If FW reports a greater value than application,
                                          // then application needs to be upgraded.
#define  MR_API_BKWD_COMPATIBILITY  0     // Incremented, to ensure application won't support older FW.
                                          // If this value in Application is greater than reported by
                                          // FW, then application cannot support FW, and needs to be downgraded.

/*
 * define macros to convert unnamed structs to named structs
 *
 * Provides compatbility with compilers that do no allow unnamed structs.
 * Consumer application must compile with NO_UNNAMED_STRUCT defined.
 * search for "unnamed" to find usage in this file.
 * If NO_UNNAMED_STRUCT is defined, use structure names to access members.
 * If unnamed structs are allowed, do not use these structure names.
 *
 * If possible ensure all new structures are named.
 */
#ifdef NO_UNNAMED_STRUCT        /* unnamed struct are not acceptable */

#define mrProgress              mrProgress
#define mrPdRef                 mrPdRef
#define mrPdAddress             mrPdAddress
#define mrEnclAddress           mrEnclAddress
#define mrLdRef                 mrLdRef
#define mrLdCachePolicy         mrLdCachePolicy
#define mrSpareType             mrSpareType
#define mrAppExecResults        mrAppExecResults
#define mrBbuGasGaugeStatus     mrBbuGasGaugeStatus
#define mrIbbuCharger           mrIbbuCharger
#define mrItbbu3Charger         mrItbbu3Charger
#define mrBbuFwStatus           mrBbuFwStatus
#define mrDpmDataClear          mrDpmDataClear

#else                           /* unnamed struct are acceptable */

#define mrProgress
#define mrPdRef
#define mrPdAddress
#define mrEnclAddress
#define mrLdRef
#define mrLdCachePolicy
#define mrSpareType
#define mrAppExecResults
#define mrBbuGasGaugeStatus
#define mrIbbuCharger
#define mrItbbu3Charger
#define mrBbuFwStatus
#define mrDpmDataClear

#endif  /* NO_UNNAMED_STRUCT */

/*
 * define the MegaRAID DCMD opcodes
 *
 * DCMD (or Direct CMDs) are used to manage the MegaRAID adapter.  They may be used for configuration
 * control, and retrieving status information.  Each DCMD opcode may contain multiple sub opcodes
 * that are used for further command descriptions.
 *
 * The following enum defines the primary, secondary, and tertiary opcodes for all commands.  The details
 * of each command and its opcodes are described.
 *
 * The input/output parameters are described using the MFI structures and fields.  For all the following
 * descriptions, "dcmd" is assumed to be an instance of an MFI_CMD_DCMD structure.  All reserved fields
 * must be initialized with zero data.  The "IN" and "OUT" directions below are from the perspective of
 * the application (i.e. "IN" implies APP<--FW, and "OUT" implies APP-->FW).  For the "Status" field,
 * only unsuccessful results are listed. All undefined mbox fields are reserved and must be initialized to
 * zeroes.
 *
 * The MFI hdr.length field is used for input as well as output. The application will set it while sending
 * the command and then firmware will change this to the actual transferred bytes. If the data structure
 * (in the SGE IN) has a count field, then it will be updated by the firmware to let the application know
 * about the total number of entries following the count field. In this way the application can re-send
 * the call with hdr.length set to the correct total size.
 *
 * Unless otherwise stated, all commands working with a 32-bit TIME field, the time is measured in seconds
 * since January 1, 2000.
 */
typedef enum _MR_DCMD {
    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * Controller commands
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_CTRL                            = 0x01000000,   // controller opcodes

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_GET_INFO
     *          dcmd.mbox           - reserved
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds and MAX_API_ARRAYS_LEGACY
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds MAX_API_ARRAYS_EXT
     *          dcmd.sge IN         - ptr to returned MR_CTRL_INFO structure
     * Desc:    Return the controller info structure.
     * Status:  No error
     */
    MR_DCMD_CTRL_GET_INFO                   = 0x01010000,   // get controller info structure

    /*
     *---------------------------------------------------------------------------------------------------
     * Controller Properties commands
     */
    MR_DCMD_CTRL_PROPERTIES                 = 0x01020000,   // manage controller properties

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PROPERTIES_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to returned MR_CTRL_PROP structure
     * Desc:    Return the controller properties structure.
     * Status:  No error
     */
    MR_DCMD_CTRL_PROPERTIES_GET             = 0x01020100,   // get only the controller properties structure

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PROPERTIES_SET
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - ptr to MR_CTRL_PROP structure
     * Desc:    Update the controller properties structure. Some of the fields will take into effect after reboot.
     *          Please refer to the structure defintions.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is invalid.
     *          MFI_STAT_INVALID_PARAMETER          - Any i/p parameter is invalid
     *          MFI_STAT_HOST_REBOOT_REQUIRED       - Host reboot required
     */
    MR_DCMD_CTRL_PROPERTIES_SET             = 0x01020200,   // set the controller properties structure

    /*
     *---------------------------------------------------------------------------------------------------
     * ALARM commands
     */
    MR_DCMD_CTRL_ALARM                      = 0x01030000,   // alarm control operatoins

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_ALARM_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned alarm status byte (0=disabled, 1=enabled)
     * Desc:    Return the current alarm setting
     * Status:  MFI_STAT_NO_HW_PRESENT  - No alarm HW present
     */
    MR_DCMD_CTRL_ALARM_GET                  = 0x01030100,   // return current alarm setting

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_ALARM_ENABLE
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Enable the alarm
     * Status:  MFI_STAT_NO_HW_PRESENT  - No alarm HW present
     */
    MR_DCMD_CTRL_ALARM_ENABLE               = 0x01030200,   // enable alarm operations

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_ALARM_DISABLE
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Disable the alarm
     * Status:  MFI_STAT_NO_HW_PRESENT  - No alarm HW present
     */
    MR_DCMD_CTRL_ALARM_DISABLE              = 0x01030300,   // disable alarm operations

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_ALARM_SILENCE
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Silence an active alarm
     * Status:  MFI_STAT_NO_HW_PRESENT  - No alarm HW present
     */
    MR_DCMD_CTRL_ALARM_SILENCE              = 0x01030400,   // temporarily silence the alarm

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_ALARM_TEST
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Start the alarm TEST pattern
     * Status:  MFI_STAT_NO_HW_PRESENT  - No alarm HW present
     */
    MR_DCMD_CTRL_ALARM_TEST                 = 0x01030500,   // start the alarm TEST sequence


    /*
     *---------------------------------------------------------------------------------------------------
     * EVENT commands
     */
    MR_DCMD_CTRL_EVENT                      = 0x01040000,   // event opcodes

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_EVENT_GET_INFO
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned MR_EVT_LOG_INFO structure
     * Desc:    Return the filled-in MR_EVT_LOG_INFO structure with the event sequence
     *          information.
     * Status:  No error
     */
    MR_DCMD_CTRL_EVENT_GET_INFO             = 0x01040100,   // get event sequence info structure

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_EVENT_CLEAR
     *          dcmd.mbox.b[0]      - types of events to clear
     *                                0=default, Clear the event log
     *                                1=set marker for OOB events, only for OOB interface use
     *                                3=set both markers
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Clear the event log.  The log is not actually cleared, but a "marker" is recorded to note
     *          the last point at which the log was cleared.  The purpose of this behavior is to maintain
     *          as much historical data as possible, but allow applications to only show events that may
     *          have occured after the last "clear" (presumably during manufacturing).
     *          A separate marker is used to track events already retrieved via OOB interface using
     *          MR_DCMD_OOB_STATUS_POLL.  When cleared a marker is set and only events posted after
     *          the marker are sent via next POLL.  The OOB marker is reset on each boot.
     * Status:  No error
     */
    MR_DCMD_CTRL_EVENT_CLEAR                = 0x01040200,   // clear the evet log (logical clear only)

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_EVENT_GET
     *          dcmd.mbox.w[0]      - starting sequence number
     *          dcmd.mbox.w[1]      - MR_EVT_CLASS_LOCALE desired class/locale event types wanted
     *          dcmd.mbox.b[8]      - 0=return MR_EVT_LIST structure
     *                                1=return MR_EVT_PACKED_LIST structure, for use via OOB interface
     *          dcmd.mbox.b[9]      - 0 - supports MR_EVT_DETAILS with < 255 PD/LD support
     *                                1 - supports MR_EVT_DETAILS_EXT with 8K PD/LD support
     *          dcmd.sge IN         - Ptr to returned MR_EVT_LIST or MR_EVT_PACKED_LIST structure
     * Desc:    Return the first 'N' events, starting with the given sequence number, that meet the class/locale
     *          mask.  'N' is limited by the size of the application buffer (in dcmd.hdr.length).  If no event
     *          meets those requirements, then MFI_STAT_NOT_FOUND is returned.  The "class" field of the
     *          MR_EVT_CLASS_LOCALE structure define the minimum class error that will be returned.  Any events
     *          with a class less than specified will not be returned.  Events may also be filtered by the
     *          "locale" field of the MR_EVT_CLASS_LOCALE structure.  These bits are used to specify the locale
     *          of events that will be returned.  If a locale bit is clear, then those events will NOT be
     *          returned.  For exapmple, if only the LD and PD locale bits are set, then only LD or PD events
     *          will be returned.  As a special case, is all bits i nthe locale field are zero, then all
     *          events will match and will be returned.
     * Status:  MFI_STAT_NOT_FOUND  - No events found matching criteria.
     */
    MR_DCMD_CTRL_EVENT_GET                  = 0x01040300,   // get specific event details

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_EVENT_COUNT
     *          dcmd.mbox.w[0]      - starting event sequence number
     *          dcmd.mbox.w[1]      - MR_EVT_CLASS_LOCALE desired class/locale event types wanted
     *          dcmd.sge IN         - Ptr to returned U32 event count
     * Desc:    Return the count of the number of events that meet the class/locale mask.  See the
     *          MR_DCMD_CTRL_EVENT_GET command for a description of the MR_EVT_CLASS_LOCALE field.
     * Status:  MFI_STAT_NOT_FOUND  - No events found matching criteria.
     */
    MR_DCMD_CTRL_EVENT_COUNT                = 0x01040400,   // return count of events meeting criteria

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_EVENT_WAIT
     *          dcmd.hdr.timeout    - wait timeout (0 for indefinite)
     *          dcmd.mbox.w[0]      - starting sequence number
     *          dcmd.mbox.w[1]      - MR_EVT_CLASS_LOCALE desired class/locale event types wanted
     *          dcmd.mbox.b[9]      - 0 - supports MR_EVT_DETAILS with < 255 PD/LD support
     *                                1 - supports MR_EVT_DETAILS_EXT with 8K PD/LD support
     *          dcmd.sge IN         - Ptr to returned MR_EVT_DETAIL/MR_EVT_DETAIL_EXT structure
     * Desc:    Wait for the next event, starting with the given sequence number, that meets the class/locale
     *          mask.  See the MR_DCMD_CTRL_EVENT_GET command for a description of the MR_EVT_CLASS_LOCALE field.
     *          If no event matching the class/locale requirements is received within the given timeout period,
     *          then MFI_STAT_NOT_FOUND is returned.  If an event already exists meeting the requirements, it
     *          is returned immediately.  A WAIT command may be aborted by issuing an MFI_CMD_ABORT frame; in
     *          this case, the original command is completed with a MFI_STAT_NOT_FOUND error.
     * Status:  MFI_STAT_NOT_FOUND  - Timeout waiting for event.
     */
    MR_DCMD_CTRL_EVENT_WAIT                 = 0x01040500,   // wait for an event (possibly of a specific type)


    /*
     *---------------------------------------------------------------------------------------------------
     * Shutdown/hibernate commands
     */

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_SHUTDOWN
     *          dcmd.mbox.b[0]      - Enable drive spindown (0=don't spin down drives, 1=spin down per MFC setting)
     *          dcmd.mbox.b[1]      - reset controller now (1=resetNow, 0=don't reset controller)
     *                                MR_CTRL_INFO.adapterOperations2.supportResetNow must be set
     *                                resetNow cannot be set with spin down.
     *          dcmd.mbox.b[2]      - Private use Only
     *          dcmd.sge            - reserved
     * Desc:    Shutdown the controller.  All background operations are put on hold for resume. The controller
     *          cache is flushed, all disk drive caches are flushed, and the on-disk configuration is "closed"
     *          to indicate redundancy data is consistent.  Further writes will cause the system to "reopen"
     *          the configuration, thus undoing the effects of the shutdown command.  The "Enable drive spindown"
     *          field is used to differentiate between SHUTDOWN, RESTART, and HIBERNATE operations.  On a SHUTDOWN
     *          operation, this field should be set TRUE to enable drive spin-down per the controller's settings.
     * Status:  MFI_STAT_SHUTDOWN_FAILED        - One or more flush operations failed
     *          MFI_STAT_RESETNOW_NOT_ALLOWED   - resetNow is not allowed
     *          MFI_STAT_INVALID_PARAMETER      - Parameters are not valid
     */
    MR_DCMD_CTRL_SHUTDOWN                   = 0x01050000,   // shutdown controller

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_HIBERNATE_STANDBY
     *          dcmd.mbox.b[0]      - Enable drive spindown (0=don't spin down drives, 1=spin down per MFC setting)
     *          dcmd.sge            - reserved
     * Desc:    Hibernate/Standby the controller. All background operations are put on hold for resume. The controller
     *          cache is flushed, all disk drive caches are flushed, and the on-disk configuration is "closed"
     *          to indicate redundancy data is consistent.  After completing this command, FW will "hang"
     *          waiting for a new command to be posted to the request queue. The driver should ensure that,
     *          after issuing the hibernate command, no further commands are issued to FW until resuming from
     *          hibernate.
     * Status:  MFI_STAT_SHUTDWON_FAILED        - One or more flush operations failed
     */
    MR_DCMD_CTRL_HIBERNATE_STANDBY          = 0x01060000,   // hibernate controller


    /*
     *---------------------------------------------------------------------------------------------------
     * Patrol Read commands
     */
    MR_DCMD_CTRL_PR                         = 0x01070000,   // Patrol Read commands

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PR_GET_STATUS
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned MR_PR_STATUS structure
     * Desc:    Return the Patrol Read status structure.
     * Status:  No error
     */
    MR_DCMD_CTRL_PR_GET_STATUS              = 0x01070100,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PR_GET_PROPERTIES
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds and uses legacy MR_PR_PROPERTIES
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds and uses extended struct MR_PR_PROPERTIES_EXT
     *                              - 2 - supports struct MR_PR_PROPERTIES_EXT2
     *          dcmd.sge IN         - Ptr to returned MR_PR_PROPERTIES/MR_PR_PROPERTIES_EXT/MR_PR_PROPERTIES_EXT2 structure
     * Desc:    Get the PR_MODE
     * Status:  No error
     */
    MR_DCMD_CTRL_PR_GET_PROPERTIES          = 0x01070200,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PR_SET_PROPERTIES
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds and uses legacy MR_PR_PROPERTIES
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds and uses extended struct MR_PR_PROPERTIES_EXT
     *          dcmd.sge OUT        - Ptr to new MR_PR_PROPERTIES/MR_PR_PROPERTIES_EXT structure
     * Desc:    Set the PR_MODE
     * Status:  MFI_STAT_INVALID_PARAMETER  - Some input parameters are invalid
     */
    MR_DCMD_CTRL_PR_SET_PROPERTIES          = 0x01070300,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PR_START
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Start the Patrol Read Iteration
     * Status:  MFI_STAT_PR_DISABLED            - Patrol mode is disabled
     *          MFI_STAT_WRONG_STATE            - Patrol Read is already active
     *          MFI_STAT_LD_RECON_IN_PROGRESS   - CE/RLM is going on in the controller
     */
    MR_DCMD_CTRL_PR_START                   = 0x01070400,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PR_STOP
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Stop the current patrol read iteration
     * Status:  MFI_STAT_PR_DISABLED    - Patrol mode is disabled
     */
    MR_DCMD_CTRL_PR_STOP                    = 0x01070500,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PR_SUSPEND_RESUME
     *          dcmd.mbox.b[0]      - 0 = suspend, 1 = resume
     *          dcmd.sge            - reserved
     * Desc:    Suspend/resume the Patrol Read Iteration
     * Status:  MFI_STAT_PR_DISABLED            - Patrol mode is disabled
     *          MFI_STAT_WRONG_STATE            - Patrol Read is already active (for resume) and
     *                                            Patrol Read is not running (for suspend)
     *          MFI_STAT_LD_RECON_IN_PROGRESS   - CE/RLM is going on in the controller
     *          MFI_STAT_INVALID_PARAMETER          - Parameters are not valid
     */
    MR_DCMD_CTRL_PR_SUSPEND_RESUME          = 0x01070600,


    /*
     *---------------------------------------------------------------------------------------------------
     * Controller Time management commands
     */
    MR_DCMD_CTRL_TIME                       = 0x01080000,   // time management opcodes

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_TIME_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returning MR_TIME_STAMP structure
     * Desc:    Return the current time stamp of the firmware
     * Status:  MFI_STAT_TIME_NOT_SET   - Firmware time was not set by the host
     */
    MR_DCMD_CTRL_TIME_GET                   = 0x01080101,   // reurn current FW time

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_TIME_SET
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - Ptr to MR_TIME_STAMP structure
     * Desc:    Set the current time stamp of the firmware
     * Status:  MFI_STAT_INVALID_PARAMETER  - time structure contains invalid values
     */
    MR_DCMD_CTRL_TIME_SET                   = 0x01080102,   // set FW time

    /* Input    dcmd.opcode         - MR_DCMD_CTRL_TIME_SECS_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned U32
     * Desc:    Return new FW time in seconds past January 1, 2000.
     * Status:  MFI_STAT_TIME_NOT_SET   - Firmware time was not set by the host
     */
    MR_DCMD_CTRL_TIME_SECS_GET              = 0x01080201,   // get FW time in seconds

    /* Input    dcmd.opcode         - MR_DCMD_CTRL_TIME_SECS_SET
     *          dcmd.mbox.w[0]      - new FW time in seconds past January 1, 2000
     *          dcmd.sge            - reserved
     * Desc:    Set the current FW time in seconds past January 1, 2000.
     * Status:  MFI_STAT_INVALID_PARAMETER  - time is invalid (time greater than 0xFEFFFFFF is invalid)
     */
    MR_DCMD_CTRL_TIME_SECS_SET              = 0x01080202,   // set FW time in seconds


    /*
     *---------------------------------------------------------------------------------------------------
     * Controller TTY history buffer management and DQ configuration
     */
    MR_DCMD_CTRL_TTY                        = 0x01090000,   // TTY history buffer management

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_TTY_CLEAR
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Clear the TTY history buffer and PL debug information
     * Status:  No error
     */
    MR_DCMD_CTRL_TTY_CLEAR                  = 0x01090100,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_TTY_READ
     *          dcmd.hdr.length     - number of bytes to read
     *          dcmd.mbox.w[0]      - starting virtual offset
     *          dcmd.mbox.w[1]        To retrieve DebugQ (DQ)/PL debug information
     *                                1 - DQ header
     *                                2 - DQ data
     *                                    MR_CTRL_INFO.adapterOperations3.supportDebugQueue must be set
     *                                3 - PL Ring Buffer
     *                                    MR_CTRL_INFO.adapterOperations4.supportPLDebugInfo must be set
     *                                4 - DQ metadata
     *                                    MR_DEBUG_QUEUE_CONFIG.metaStrActiveColumn must be non-zero
     *                                5 - DQ buffer
     *                                    MR_DEBUG_QUEUE_CONFIG.metaStrActiveColumn must be non-zero
     *          dcmd.mbox.b[9]        1 - TTY logs compressed
     *          dcmd.sge IN         - Ptr to returned MR_TTY_DATA buffer OR DQ header OR DQ buffer
     * Output:  dcmd.hdr.length     - total number of bytes transferred (including MR_TTY_DATA fields)
     * Desc:    Returns the requested TTY history buffer or DQ/PL debug information
     * Status:  MFI_STAT_MEMORY_NOT_AVAILABLE   - FW can't allocate sufficient memory - try again later
     */
    MR_DCMD_CTRL_TTY_READ                   = 0x01090200,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_DQ_CONFIG_SET
     *          dcmd.mbox           - Reserved
     *          dcmd.sge OUT        - Ptr to user buffer with information about DQ configuration
     * Desc:    Will set the DQ configuration
     * Status:  No error
     */
    MR_DCMD_CTRL_DQ_CONFIG_SET              = 0x01090300,

    /*
     *---------------------------------------------------------------------------------------------------
     * Controller TTY history battery management
     */
    MR_DCMD_CTRL_TTY_BBU                    = 0x010a0000,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_TTY_BBU_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to a U8 returning the MR_TTY_BBU_STATE state
     * Desc:    Returns the current BBU state
     * Status:  No error
     */
    MR_DCMD_CTRL_TTY_BBU_GET                = 0x010a0100,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_TTY_BBU_SET_OFF
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Set the TTY BBU buffering off
     * Status:
     * MFI_STAT_INVALID_PARAMETER   - Some input parameters are invalid
     * MFI_STAT_OK_REBOOT_REQUIRED  - Ok but reboot required
     * MFI_STAT_NVCACHE_ERROR       - Command failed due to NVCACHE error
     */
    MR_DCMD_CTRL_TTY_BBU_SET_OFF            = 0x010a0200,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_TTY_BBU_SET_ON
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Set the TTY BBU buffering on
     * Status:
     * MFI_STAT_INVALID_PARAMETER   - Some input parameters are invalid
     * MFI_STAT_OK_REBOOT_REQUIRED  - Ok but reboot required
     * MFI_STAT_NVCACHE_ERROR       - Command failed due to NVCACHE error
     */
    MR_DCMD_CTRL_TTY_BBU_SET_ON             = 0x010a0300,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_TTY_BBU_SET_TEMP_ON
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Set the TTY BBU buffering off, but turn on at next boot
     * Status:
     * MFI_STAT_INVALID_PARAMETER   - Some input parameters are invalid
     * MFI_STAT_OK_REBOOT_REQUIRED  - Ok but reboot required
     * MFI_STAT_NVCACHE_ERROR       - Command failed due to NVCACHE error
     */
    MR_DCMD_CTRL_TTY_BBU_SET_TEMP_ON        = 0x010a0400,


    /*
     *---------------------------------------------------------------------------------------------------
     * Controller environment area information
     */
    MR_DCMD_CTRL_ENV                        = 0x010b0000,   // get FW environment area info

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_ENV_GET_DATA
     *          dcmd.hdr.length     - buffer size (typically 1K) for the environment area
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned MR_ENV_DATA environment area
     * Output   dcmd.hdr.length     - Actual number of bytes returned
     * Desc:    Returns the current environment variable data
     * Status:  No error
     */
    MR_DCMD_CTRL_ENV_GET_DATA               = 0x010b0100,


    /*
     *---------------------------------------------------------------------------------------------------
     * BIOS specific commands
     */
    MR_DCMD_CTRL_BIOS                       = 0x010c0000,   //BIOS specific commands

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_BIOS_DATA_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned MR_BIOS_DATA structure
     * Desc:    Return the BIOS private data
     * Status:  No error
     */
    MR_DCMD_CTRL_BIOS_DATA_GET              = 0x010c0100,   // Return BIOS private data

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_BIOS_DATA_SET
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - Ptr to MR_BIOS_DATA structure
     * Desc:    Store the BIOS private data in a non-volatile area
     * Status:  No error
     */
    MR_DCMD_CTRL_BIOS_DATA_SET              = 0x010c0200,   // Set BIOS private data

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_BIOS_BOOT_MSG_GET
     *          dcmd.mbox.s[0]      - Index of the message, or MR_BOOT_MSG_INDEX_INVALID to request pending message count only
     *          dcmd.mbox.b[2]      - MR_BOOT_ENV
     *          dcmd.sge IN         - Ptr to returned MR_BOOT_MESSAGE_DATA structure
     * Desc:    Return boot messages if any.  BIOS should start retrieving messages at index
     *          0 until no more messages are available.
     * Status:  MFI_STAT_NOT_FOUND  - If firmware doesn't have any messages for the index.  MFI_STAT_NOT_FOUND is also returned if
     *                                  message index value of MR_BOOT_MSG_INDEX_INVALID was passed to a version of FW that doesn't
     *                                  support the bootMsgPendingCount field or this command option to retrieve it.
     *                                NOTE: If message index value MR_BOOT_MSG_INDEX_INVALID was passed and no MFI_STAT_NOT_FOUND
     *                                  error occurs, then BIOS will know that bootMsgPendingCount field is valid.
     */
    MR_DCMD_CTRL_BIOS_BOOT_MSG_GET          = 0x010c0300,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_BIOS_BOOT_MSG_SEND_KEYSTROKE
     *          dcmd.mbox.s[0]      - message index
     *          dcmd.mbox.s[1]      - 16bit value encoded in BIOS key format
     *          dcmd.mbox.b[4]      - value defined by MR_BOOT_PARAMS enum
     *          dcmd.sge            - Ptr to U32 returning a MR_BOOT_ACTION enum.  The host should
     *                                take the action specified by the contents of the enum.
     * Desc:    The bios passes the keystroke to the firmware.
     * Status:  MFI_STAT_NOT_FOUND  - If firmware doesn't have any messages for the index
     */
    MR_DCMD_CTRL_BIOS_BOOT_MSG_SEND_KEYSTROKE   = 0x010c0400,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_BIOS_BOOT_MSG_SEND_STRING
     *          dcmd.mbox.s[0]      - message index
     *          dcmd.sge[0] IN      - Ptr to U32 returning a MR_BOOT_ACTION enum.  The host should
     *                                take the action specified by the contents of the enum.
     *          dcmd.sge[1] OUT     - Ptr to a null terminated string (max 80 bytes).
     * Desc:    The bios passes the string to the firmware.
     * Status:  MFI_STAT_NOT_FOUND  - If firmware doesn't have any messages for the index
     */
    MR_DCMD_CTRL_BIOS_BOOT_MSG_SEND_STRING   = 0x010c0500,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_HOST_INFO_SET
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - Ptr to MR_HOST_INFO structure
     * Desc:    Store the MR_HOST_INFO in a non-volatile area.
     *          This DCMD is expected to be issued by the UEFI driver and/or Legacy BIOS during boot.
     *          Check MR_CTLR_INFO.supportHostInfo to determine if this DCMD is supported.
     *          This DCMD should be supported in MFI_STATE_BOOT_MSG_PENDING or higher.
     *          (If caller can only provide part of the information, a MR_DCMD_CTRL_HOST_INFO_GET should be
     *          issued first to pre-load the other fields of the data buffer.)
     * Status:  No error
     */
    MR_DCMD_CTRL_HOST_INFO_SET              = 0x010c0600,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_HOST_INFO_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to MR_HOST_INFO structure
     * Desc:    Return the MR_HOST_INFO data.
     *          Check MR_CTLR_INFO.supportHostInfo to determine if this DCMD is supported.
     *          This DCMD should be supported in MFI_STATE_BOOT_MSG_PENDING or higher.
     * Status:  No error
     */
    MR_DCMD_CTRL_HOST_INFO_GET              = 0x010c0700,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_SET_FACTORY_DEFAULTS
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Set the controller settings to factory defaults
     * Status:  No error
     */
    MR_DCMD_CTRL_SET_FACTORY_DEFAULTS       = 0x010d0000,   // Set Factory defaults


    /*
     *---------------------------------------------------------------------------------------------------
     * MFC-specific commands
     */
    MR_DCMD_CTRL_MFC                        = 0x010e0000,   // MFC specific commands

    /*
     * Input:   dcmd.opcode         - MR_DCMD_CTRL_MFC_GET_PCI
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned MR_MFC_PCI structure with current PCI IDs
     * Desc:    Get the current cached PCI device and vendor IDs from the MFC data
     * Status:  No error
     */
    MR_DCMD_CTRL_MFC_PCI_GET                = 0x010e0100,

    /*
     * Input:   dcmd.opcode         - MR_DCMD_CTRL_MFC_GET_PCI_RAW
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned MR_MFC_PCI structure with current PCI IDs
     * Desc:    Get the current PCI device and vendor IDs from the SEEPROM device.
     * Status:  MFI_STAT_MFC_HW_ERROR   - HW error reading the SEEPROM
     */
    MR_DCMD_CTRL_MFC_PCI_GET_RAW            = 0x010e0101,

    /*
     * Input:   dcmd.opcode         - MR_DCMD_CTRL_MFC_PCI_LINK_SPEED_SET
     *          dcmd.mbox.b[0]      - PCIe link speed value (MR_PCI_LINK_SPEED)
     *          dcmd.sge            - reserved
     * Desc:    Set the controller's advertised max PCIe link speed
     *          Note - DCMD is supported if MR_CTRL_INFO.hostInterface.maxPciLinkSpeed != MR_PCI_LINK_SPEED_DEFAULT
     * Status:  MFI_STAT_CTRL_RESET_REQUIRED  - changes shall take effect after a reset
     *          MFI_STAT_INVALID_PARAMETER    - Invalid parameter, or unsupported link speed value
     *          MFI_STAT_MFC_HW_ERROR         - HW error reading/writing the SEEPROM
     */
    MR_DCMD_CTRL_MFC_PCI_LINK_SPEED_SET          = 0x010e0103,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_MFC_DEFAULTS_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned MR_MFC_DEFAULTS structure
     * Desc:    Get cached MFC Default structure data
     * Status:  No error
     */
    MR_DCMD_CTRL_MFC_DEFAULTS_GET           = 0x010e0201,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_MFC_DEFAULTS_SET
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - Ptr to MR_MFC_DEFAULTS structure with new MFC Default data
     * Desc:    Set new MFC Default structure data.
     * Status:  MFI_STAT_MFC_HW_ERROR   - HW error writing to SEEPROM
     */
    MR_DCMD_CTRL_MFC_DEFAULTS_SET           = 0x010e0202,                       // DEPRECATED

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_MFC_DEFAULTS_GET_RAW
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned MR_MFC_DEFAULTS structure
     * Desc:    Get MFC Default structure data from the SEEPROM device.
     * Status:  MFI_STAT_MFC_HW_ERROR   - HW error reading the SEEPROM
     */
    MR_DCMD_CTRL_MFC_DEFAULTS_GET_RAW       = 0x010e0203,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_MFC_DEFAULTS_SASADDR_SET
     *          dcmd.mbox.w[0]      - lower word of sas addr
     *          dcmd.mbox.w[1]      - upper word of sas addr
     *          dcmd.sge            - reserved
     * Desc:    Set SAS addr in MFC. Firmware writes this value to SBR where sas
     *          address is stored.
     *          This DCMD is only supported if MR_CTRL_INFO.adpaterOperations.supportConfigPageModel
     *          is set.
     * Status:  MFI_STAT_MFC_HW_ERROR   - HW error in programming sas address
     */
    MR_DCMD_CTRL_MFC_DEFAULTS_SASADDR_SET   = 0x010e0204,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_MFC_MFG_GET
     *          dcmd.mbox.b[0]      - 1=from secondary Raid Key
     *          dcmd.sge IN         - Ptr to returned MR_MFC_MFG structure
     * Desc:    Get MFC manufacturing information structure
     * Status:  No error
     * Status:  MFI_STAT_DEVICE_NOT_FOUND - Secondary key is not present
     *          MFI_STAT_NOT_FOUND        - MFG info is not available

     */
    MR_DCMD_CTRL_MFC_MFG_GET                = 0x010e0301,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_MFC_MFG_2_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned MR_MFC_MFG_2 structure
     * Desc:    Get MFC manufacturing 2 information structure
     * Status:  No error
     */
    MR_DCMD_CTRL_MFC_MFG_2_GET              = 0x010e0302,

     /*
     * Input:   dcmd.opcode         - MR_DCMD_CTRL_PCI_OEM_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Ptr to returned MR_PCI_OEM_INFO structure with current PCI IDs,
     *                                OEM, subOEM IDs
     * Desc:    Get the controller PCI device, vendor IDs and OEM, subOEM IDs
     * Status:  No error
     */
    MR_DCMD_CTRL_PCI_OEM_GET                = 0x010e0400,

    /*
     * Input:   dcmd.opcode         - MR_DCMD_CTRL_PCI_OEM_SET
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - Ptr to MR_PCI_OEM_INFO structure with the new subOEM ID.
     *                                Note that the other fields in the structure MR_PCI_OEM_INFO are
     *                                read-only and cannot be modified
     * Desc:    Set the controller subOEM ID. Other fields in the structure are read-only.
     * Status:  MFI_STAT_INVALID_PARAMETER - subOEM ID is invalid
     */
    MR_DCMD_CTRL_PCI_OEM_SET                = 0x010e0401,

    /*
     *---------------------------------------------------------------------------------------------------
     * Firmware Flash Commands
     */
    MR_DCMD_CTRL_FLASH_FW                   = 0x010f0000,   // Flash ctrl firmware commands

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_FLASH_FW_OPEN
     *          dcmd.mbox.w[0]      - Size of the whole flash component (BIOS, BB etc or whole) in Kb
     *          dcmd.sge            - reserved
     * Desc:    Allows firmware to allocate memory for incoming flash image.
     * Status:  MFI_STAT_FLASH_ALLOC_FAIL   - Couldn't allocate the needed memory
     *          MFI_STAT_FLASH_BUSY         - Flash download already in progress
     */
    MR_DCMD_CTRL_FLASH_FW_OPEN              = 0x010f0100,   // Firmware flash init command

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_FLASH_FW_DOWNLOAD
     *          dcmd.hdr.length     - Number of bytes of the flash buffer
     *          dcmd.mbox.w[0]      - starting offset within the flash buffer
     *          dcmd.sge OUT        - ptr to flash buffer image of specified size
     * Desc:    Copies the user flash buffer into already allocated firmware memory at the specified offset.
     *          User can repeat this call to download the entire image.
     * Status:  MFI_STAT_INVALID_PARAMETER  - User gave a size (accumulated) more than the whole image size
     *          MFI_STAT_FLASH_NOT_OPEN     - Firmware OPEN was not done by the user application
     *          MFI_STAT_FLASH_NOT_ALLOWED  - FW Flash not allowed
     */
    MR_DCMD_CTRL_FLASH_FW_DOWNLOAD          = 0x010f0200,   // Firmware flash download

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_FLASH_FW_FLASH
     *          dcmd.mbox.b[0]      - reset controller after flash is done (1=resetNow, 0=don't reset controller)
     *                                MR_CTRL_INFO.adapterOperations2.supportResetNow must be set
     *          dcmd.mbox.b[1]      - force flash (2=private use only, 1=force flash, 0=dont force flash)
     *                                MR_CTRL_INFO.adapterOperations3.supportForceFlash must be set
     *          dcmd.mbox.b[2]      - flash program method (2=backup flash, 1=active flash, 0=dual flash)
     *                                MR_CTRL_INFO.adapterOperations4.supportDualFWUpdate must be set.
     *                                Last image flashed will become the active image on next boot unless overridden.
     *          dcmd.sge IN         - Pointer to MR_PROGRESS structure that will be periodically filled-in with the flash
     *                                         operation progress. MR_PROGRESS.elapsedSecs will contain always "Total Elapsed Seconds"
     *                                         irrespective of supportPointInTimeProgress value.
     * Desc:    Actually flashes the firmware into the flash
     * Status:  MFI_STAT_FLASH_NOT_OPEN         - Firmware OPEN was not done by the user application
     *          MFI_STAT_FLASH_IMAGE_INCOMPLETE - Firmware Image download is not complete
     *          MFI_STAT_FLASH_IMAGE_BAD        - The image is corrupt
     *          MFI_STAT_FLASH_ERROR            - Flash operation failed; see event log for details
     *          MFI_STAT_RESETNOW_NOT_ALLOWED   - resetNow is not allowed
     *          MFI_STAT_OK_SHUTDOWN_REQUIRED   - Flash ok but shutdown is required to take effect
     */
    MR_DCMD_CTRL_FLASH_FW_FLASH             = 0x010f0300,   // Firmware flash flash

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_FLASH_FW_CLOSE
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Indication to release the firmware flash memory.  A CLOSE operatoin is implicitly
     *          done by firmware after a FLASH operation completes.  This command is provided for
     *          completeness to enable an application to gracefully abort a previously started download.
     * Status:  MFI_STAT_FLASH_NOT_OPEN     - Firmware OPEN was not done by the user application
     *          MFI_STAT_FLASH_BUSY         - Flash operation is in progress
     */
    MR_DCMD_CTRL_FLASH_FW_CLOSE             = 0x010f0400,   // Firmware flash command

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_FLASH_COMP_INFO_GET
     *          dcmd.mbox.b[0]      - 0 for currently running FW
     *                                1 for pending FW (will be activated after reset)
     *                                2 for backup FW
     *          dcmd.sge IN         - Pointer to MR_FLASH_COMP_INFO_LIST
     * Desc:    Return a variable list of component version details managed by firmware.
     *          This component detail is not limited to controller flash, and may include firmware for
     *          on board expander, driver version, bbu module version etc., if available and present.
     *          See support bit - MR_CTRL_INFO.adapterOperations4.supportFlashCompInfo.
     * Status:  MFI_STAT_OK                     - Command successful
     *          MFI_STAT_MEMORY_NOT_AVAILABLE   - FW can't allocate sufficient memory - try again later
     */
    MR_DCMD_CTRL_FLASH_COMP_INFO_GET        = 0x010f0500,   // Get info on Firmware components

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_NVCACHE_ERASE
     *          dcmd.mbox.b[0]      - 0 for ONFI flash erase and get bad block count
     *                                1 for getting just the bad block count
     * Desc:    Erases entire contents from NV Cache (ex: ONFI Flash) and gets the bad block count.
     * Status:  MFI_STAT_OK                     - NVCache Erase completed successfully
     *          MFI_STAT_FLASH_ERROR            - NVCache Erase failed due to time out
     *          MFI_STAT_UNSUPPORTED_HW         - If the controller does not support the NVCache part
     */
    MR_DCMD_CTRL_NVCACHE_ERASE              = 0x010f0600,    //NVCache erase command (ONFI on Invader)

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_FLASH_FW_UPLOAD
     *          dcmd.hdr.length     - Number of bytes of the host buffer
     *          dcmd.mbox.b[0]      - Image type from MR_IMAGE_TYPE enum
     *          dcmd.sge IN         - ptr to buffer of specified size
     * Desc:    Copies the specified firmware image into the buffer.
     *
     * Status:  MFI_STAT_INVALID_PARAMETER      - User gave a size (accumulated) more than the whole image size
     *          MFI_STAT_FLASH_ERROR            - Not able to read flash
     *          MFI_STAT_IMAGE_NOT_FOUND        - Not able to find the requested image
     */
    MR_DCMD_CTRL_FLASH_FW_UPLOAD            = 0x010f0800,   // Firmware flash download

    /*
     *---------------------------------------------------------------------------------------------------
     * CACHE commands
     */
    MR_DCMD_CTRL_CACHE                      = 0x01100000,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_FLUSH_CACHE
     *          dcmd.mbox.b[0]      - This is a bit controlled setting (enum MR_CTRL_CACHE_FLUSH_TYPE)
     *                              - set bit 0 to flush controller cache
     *                              - set bit 1 to flush all disk caches
     *                              - set bit 2 to flush ssc current number of dirty windows. Not recommended for OS drivers.
     *          dcmd.mbox.b[1]      - This is a bit for stop flush operation. The corresponding dcmd.mbox.b[0] bit have to be set. Currently only for SSC cache.
     *                              - set bit 0 to stop flush cache
     *          dcmd.sge            - reserved
     * Desc:    Flushes the adapter cache and/or disk caches, ssc cache
     * Status:
     *          MFI_STAT_FLUSH_FAILED   -             If the above operations failed
     *          MFI_STAT_INVALID_PARAMETER            Invalid parameter
     *          MFI_STAT_SSC_FLUSH_ALREADY_RUNNING    Already SSC flush is running
     */
    MR_DCMD_CTRL_CACHE_FLUSH                = 0x01101000,   // Flush the ctrl/drive/ssc caches

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_SSC_FLUSH_STATUS
     *          dcmd.sge IN         - ptr to returned MR_CTRL_SSC_FLUSH_STATUS structure
     * Desc:    Return the status of 'user initiated ssc cache flush'
     * Status:  MFI_STAT_INVALID_DCMD            - if f/w does not support this DCMD
     */
    MR_DCMD_CTRL_SSC_FLUSH_STATUS          = 0x01102000,   // Status for ssc flush

    /*
     *---------------------------------------------------------------------------------------------------
     * Diagnostics commands
     */
    MR_DCMD_CTRL_DIAG                       = 0x01120000,   // diagnostics commands

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_DIAG_SELF_CHECK
     *          dcmd.mbox.w[0]      - timeinSecs
     *          dcmd.mbox.w[1]      - 0 - legacy where detailed information/status about the tests is not returned (no sge OUT),
     *                                1 - where detailed information/status is returned in sge OUT as MR_DIAG_INFO
     *          dcmd.mbox.w[2]      - reserved
     *          dcmd.sge IN/OUT     - IN: ptr to minimum 200k buffer provided by host; larger buffers may
     *                                supplied and will be used.
     *                                OUT: Ptr to MR_DIAG_INFO, applicable with w[1] being non-zero
     * Desc:    Run the self-check diagnostics for the given number of seconds and use the supplied
     *          (200K minimum) host buffer.  Firmware uses the host memory to run DMA and IO patterns
     *          at given buffer area testing the DMA and the host interconnect pathways.  Firmware
     *          also runs tests on various hardware components and interconnects for the given time.
     *          The completion status and/or MR_DIAG_INFO identifies the diagnostic results.
     * Status:  MFI_STAT_INVALID_PARAMETER  -   input parameter is not correct
     *          MFI_STAT_DIAG_FAILED        -   if any of the test fails
     *          MFI_STAT_OK                 -   if test completes successfully
     */
    MR_DCMD_CTRL_DIAG_SELF_CHECK            = 0x01120100,   // start self check diagnostics

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_DIAG_BATTERY_RETENTION_TEST
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Host issues this command to start a battery retention test.  When received, this command
     *          will flush any dirty data in the cache and the disk drives.  Then, FW will fill cache memory
     *          with a known pattern.  At this point, the system should be powered off for the desired test
     *          time.  When power is restored, FW will verify the test pattern and log an event indicating
     *          the success or failure of the test.  The test application should query the log for the
     *          appropriate MR_EVT_DIAG_BBU_RETENTION_PASSED or MR_EVT_DIAG_BBU_RETENTION_FAILED events
     *          to determine the test results.
     * Status:  MFI_STAT_NO_HW_PRESENT      -   if no battery present on the controller.
     *          MFI_STAT_OK                 -   if test starts successfully
     */
    MR_DCMD_CTRL_DIAG_BATTERY_RETENTION_TEST= 0x01120200,   // start battery retention test

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_DIAG_SERVER_POWER_TEST
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Host issues this command to test the server's power supply capability to withstand all drives
     *          doing a fill stroke seek. This is a offline tool command. Firmware would stop all its activites
     *          seek all drives to cylinder 0, wait for 5 seconds, go to next cylinder till the end and repeat this
     *          forever until user power downs the server.
     * Status:
     *          MFI_STAT_OK                 -   if test started successfully
     *          MFI_STAT_INVALID_CMD        -   can't start as there are no drives to test
     */
    MR_DCMD_CTRL_DIAG_SERVER_POWER_TEST     = 0x01120300,   // start server power test


    /*
     *---------------------------------------------------------------------------------------------------
     * PD Firmware Download commands
     */
    MR_DCMD_CTRL_PD_FW_DOWNLOAD             = 0x01130000,   // Firmware download commands

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PD_FW_DOWNLOAD_ENABLE
     *          dcmd.mbox.b[0]      - enableFwDownload
     * Desc:    Non zero value for enableFwDownload, directs the controller to snoop for appropriate
     *          microcode update pass-through commands for its targets (SAS/SATA drives, enclosure modules, etc.),
     *          and quiesce the system to allow a microcode update of the target without disrupting the controller
     *          and its configuration or data. Same parameter can be used to disable this behavior as well.
     *          This setting is not maintained across reboots.
     *          Setting enableFwDownload, indicates that the application shall ensure that target microcode update
     *          shall occur in a controlled environment with NO HOST I/Os.
     * Status:  MFI_STAT_WRONG_STATE            - Controller cannot allow/disallow microcode update
     *          MFI_STAT_PD_READY_FOR_REMOVAL   - PD is in removal ready state
     */
    MR_DCMD_CTRL_PD_FW_DOWNLOAD_ENABLE      = 0x01130100,   // Allow/disallow microcde update with no host I/Os

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PD_FW_DOWNLOAD_PREPARE
     *          dcmd.mbox.s[0]      - deviceId for microcode update
     *          dcmd.mbox.b[2]      - 0=Do not perform parallel PD microcode update
     *                                1=Perform parallel PD microcode update
     *                                MR_CTRL_INFO.pdOperations.supportParallelPDFwUpdate must be set
     *          dcmd.mbox.b[3]      - force parallel PD microcode update irrespective of VD tolerance level
     *                                1=force parallel update, 0=don't force parallel update
     *          dcmd.sge OUT        - ptr to MR_PD_FW_DOWNLOAD_PARAMS
     * Desc:    Host should issue this command to prepare controller for microcode update to one or more physical
     *          devices.  When issued independant of MR_DCMD_CTRL_PD_FW_DOWNLOAD_ENABLE, microcode update can be
     *          done with active host I/Os.
     *          When received, this command directs the controller to snoop for appropriate microcode update
     *          pass-through commands for its targets (SAS/SATA drives, enclosure modules, etc.).
     *          FW shall quiesce the relevant arrays, and update microcode of the device(s) without disrupting the
     *          controller, its configuration, data, or active host I/Os.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND       - deviceId is invalid
     *          MFI_STAT_PD_READY_FOR_REMOVAL   - PD is in removal ready state
     *          MFI_STAT_WRONG_STATE            - Microcode update already in progress
     *          MFI_STAT_MEMORY_NOT_AVAILABLE   - FW can't allocate sufficient memory - try again later
     *          MFI_STAT_MULTIPLE_PDS_IN_ARRAY  - Simultaneous updates to multiple PDs in same array is not allowed
     *          MFI_STAT_INVALID_PARAMETER      - One or more input parameters is invalid
     *          MFI_STAT_INVALID_CMD            - FW doesn't support microcode update to multiple PDs
     *          MFI_STAT_PD_TYPE_WRONG          - Wrong device type selected for parallel microcode update
     *          MFI_STAT_FW_DOWNLOAD_ERROR      - Devices are not eligible for parallel microcode update
     */
    MR_DCMD_CTRL_PD_FW_DOWNLOAD_PREPARE     = 0x01130200,   // Prepare for microcode update to physical device(s)

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PD_FW_DOWNLOAD_PROGRESS (to be used only when parallel PD microcode update is in progress)
     *          dcmd.mbox.s[0]      - PD reference (this will be used as a reference to return the FW update
     *                                status for the next set of MAX_API_PHYSICAL_DEVICES following this device)
     *          dcmd.sge IN         - ptr returned to MR_PD_FW_DOWNLOAD_STATUS structure
     *
     * Desc:    Return the physical drive microcode update status to host
     * Status:  MFI_STAT_OK                           - Microcode updated successfully on the SCSI device
     *          MFI_STAT_DEVICE_NOT_FOUND             - Device ID is invalid
     *          MFI_STAT_PD_READY_FOR_REMOVAL         - PD is in removal ready state
     *          MFI_STAT_PD_TYPE_WRONG                - Wrong device type selected for parallel microcode update
     *          MFI_STAT_MICROCODE_UPDATE_PENDING     - Microcode update pending on the device
     *          MFI_STAT_MICROCODE_UPDATE_IN_PROGRESS - Microcode update is in progress on the device
     *          MFI_STAT_SCSI_DONE_WITH_ERROR         - Microcode update command done with error(s)
     */
    MR_DCMD_CTRL_PD_FW_DOWNLOAD_PROGRESS     = 0x01130300,   // Return microcode update status information for all physical devices

    /*
     *---------------------------------------------------------------------------------------------------
     * CC Schedule commands
     */
    MR_DCMD_CTRL_CC_SCHEDULE                = 0x01140000,   // CC schedule commands

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_CC_SCHEDULE_GET_STATUS
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_LOGICAL_DRIVES/MAX_API_LOGICAL_DRIVES_EXT
     *                                2 - supports MAX_API_LOGICAL_DRIVES_EXT2
     *          dcmd.sge IN         - Ptr to returned MR_CC_SCHEDULE_STATUS structure
     * Desc:    Return the CC schedule status structure.
     * Status:  No error
     */
    MR_DCMD_CTRL_CC_SCHEDULE_GET_STATUS     = 0x01140100,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_CC_SCHEDULE_GET_PROPERTIES
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds and uses legacy MR_CC_SCHEDULE_PROPERTIES
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds and uses extended struct MR_CC_SCHEDULE_PROPERTIES_EXT
     *                              - 2 - supports MAX_API_LOGICAL_DRIVES_EXT2 lds and uses extended struct MR_CC_SCHEDULE_PROPERTIES_EXT2
     *          dcmd.sge IN         - Ptr to returned MR_CC_SCHEDULE_PROPERTIES/MR_CC_SCHEDULE_PROPERTIES_EXT/MR_CC_SCHEDULE_PROPERTIES_EXT2 structure
     * Desc:    Get the CC schedule properties
     * Status:  No error
     */
    MR_DCMD_CTRL_CC_SCHEDULE_GET_PROPERTIES  = 0x01140200,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_CC_SCHEDULE_SET_PROPERTIES
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds and uses legacy MR_CC_SCHEDULE_PROPERTIES
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds and uses extended struct MR_CC_SCHEDULE_PROPERTIES_EXT
     *                              - 2 - supports MAX_API_LOGICAL_DRIVES_EXT2 lds and uses extended struct MR_CC_SCHEDULE_PROPERTIES_EXT2
     *          dcmd.sge OUT        - Ptr to new MR_CC_SCHEDULE_PROPERTIES/MR_CC_SCHEDULE_PROPERTIES_EXT/MR_CC_SCHEDULE_PROPERTIES_EXT2 structure
     * Desc:    Set the CC schedule properties
     * Status:  MFI_STAT_INVALID_PARAMETER  - Some input parameters are invalid
     */
    MR_DCMD_CTRL_CC_SCHEDULE_SET_PROPERTIES  = 0x01140300,


    /*
     *---------------------------------------------------------------------------------------------------
     * Key Management commands
     */
    MR_DCMD_CTRL_LOCK_KEY                   = 0x01150000,   // Key Management commands

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_LOCK_KEY_CREATE
     *          dcmd.mbox.b[0]      - This is a bit controlled setting
     *                              - set bit 0 to ensure passPhrase is not hashed
     *                              - set bit 1 to bind lock key to TPM
     *                              - set bit 2 to create the rekey (replacement key)
     *                                          This is the first step in a rekey operation
     *                                          must be followed by MR_DCMD_CTRL_LOCK_KEY_REKEY
     *                              - set bit 3 to indicate only binding and secret key is being
     *                                          updated. enableSecretKeyControl must be set to allow this.
     *                                          bits 2 and 3 are mutually exclusive.
     *                              - set bit 4 to bind enable EKM
     *                                          MR_CTRL_INFO.adapterOperations.supportEKM must be set.
     *          dcmd.sge OUT        - ptr to MR_CTRL_LOCK_KEY_PARAMS
     * Desc:    Create a controller lock key.  Required for rekey as well.
     *          For rekey, this DCMD is used to initiate the process to change the lock key, or
     *          update the lock key binding.
     * Status:  MFI_STAT_INVALID_PARAMETER          - Some input parameters are invalid
     *          MFI_STAT_INVALID_CMD                - Some input parameters are not supported
     *          MFI_STAT_LOCK_KEY_REKEY_NOT_ALLOWED - Lock key must be created before it can be rekeyed
     *          MFI_STAT_LOCK_KEY_ALREADY_EXISTS    - Lock key can only be created once
     *          MFI_STAT_SECRET_KEY_NOT_ALLOWED     - Cannot bind to secret key
     *          MFI_STAT_LOCK_KEY_BACKUP_NOT_ALLOWED- This type of lock key cannot be backedup
     *          MFI_STAT_LOCK_KEY_BACKUP_REQUIRED   - This type of lock key requires backupPassPhrase for backup
     */
    MR_DCMD_CTRL_LOCK_KEY_CREATE            = 0x01150100,   // create lock key

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_LOCK_KEY_SUGGEST_PASSPHRASE
     *          dcmd.mbox           - None
     *          dcmd.sge IN         - ptr to null terminated string
     * Desc:    FW generates a random alphanumeric password as a suggestion for a lock key
     * Status:  MFI_STAT_NO_HW_PRESENT  - No HW available to generate pass-phrase
     */
    MR_DCMD_CTRL_LOCK_KEY_SUGGEST_PASSPHRASE= 0x01150200,

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_LOCK_KEY_KEYID_GET
     *          dcmd.mbox.s[0]      - deviceId or MR_PD_INVALID for controller
     *          dcmd.mbox.b[2]      - suggestKeyId
     *          dcmd.sge IN         - ptr to null terminated string (max 256 bytes)
     * Desc:    Return the requested keyId, for a physical device, or the controller.  If
     *          suggestKeyId is set, FW shall generate a keyId, instead of obtaining the keyId
     *          from the device.
     * Status:  MFI_STAT_WRONG_STATE     - deviceId must be secured to retrieve keyId
     */
    MR_DCMD_CTRL_LOCK_KEY_KEYID             = 0x01150300,   // keyId commands
    MR_DCMD_CTRL_LOCK_KEY_KEYID_GET         = 0x01150301,   // get keyId

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_LOCK_KEY_KEYID_SET
     *          dcmd.mbox.s[0]      - deviceId or MR_PD_INVALID for controller
     *          dcmd.sge OUT        - ptr to null terminated string (max 256 bytes)
     * Desc:    Change the keyId, for the device
     * Status:  MFI_STAT_INVALID_CMD - FW only supports command for controller
     *          MFI_STAT_WRONG_STATE - deviceId must be secured to set keyId
     *          MFI_STAT_TRANSPORT_READY_LD_EXISTS - configuration has some LD in transport ready state
     */
    MR_DCMD_CTRL_LOCK_KEY_KEYID_SET         = 0x01150302,   // set keyId

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_LOCK_KEY_BACKUP
     *          dcmd.mbox.b[0]      - lock key to backup (0=controller lock key, 1=reKey)
     *          dcmd.sge IN         - ptr to MR_CTRL_ESCROW_LOCK_KEY
     * Desc:    Provide an encrypted lock key to be escrowed for backup
     * Status:  MFI_STAT_INVALID_PARAMETER           - Some input parameters are invalid
     *          MFI_STAT_INVALID_CMD                 - Some input parameters are not supported
     *          MFI_STAT_LOCK_KEY_BACKUP_NOT_ALLOWED - If lock key exists, backup is not allowed for it
     */
    MR_DCMD_CTRL_LOCK_KEY_BACKUP            = 0x01150400,   // backup lock key

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_LOCK_KEY_VERIFY_ESCROW
     *          dcmd.mbox.b[0]      - key to verify (0=controller lock key, 1=reKey, 2=secret key)
     *          dcmd.sge OUT        - ptr to MR_CTRL_LOCK_KEY_FROM_ESCROW
     * Desc:    Provide a lock key from escrow, to verify that backup was successful,
     *          and escrow lock key is valid.
     *          Also used to verify secret key, if one is present.
     * Status:  MFI_STAT_INVALID_PARAMETER           - Some input parameters are invalid
     *          MFI_STAT_LOCK_KEY_VERIFY_NOT_ALLOWED - If lock key exists, and backup/verify is not allowed for it
     */
    MR_DCMD_CTRL_LOCK_KEY_VERIFY_ESCROW     = 0x01150500,   // verify lock key backup

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_LOCK_KEY_BACKUP_USER_ACK
     *          dcmd.mbox           - None
     * Desc:    Acknowledge successfull backup (and verify) of lock key to escrow
     * Status:  No error
     */
    MR_DCMD_CTRL_LOCK_KEY_BACKUP_USER_ACK   = 0x01150600,   // acknowledge successful lock key backup

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_LOCK_KEY_REKEY
     *          dcmd.mbox.b[0]      - This is a bit controlled setting
     *                              - set bit 0 to indicate that passPhrase is a secretKey
     *          dcmd.sge OUT        - ptr to MR_CTRL_LOCK_KEY_FROM_ESCROW
     * Desc:    Second step of changing the controller lock key or update its binding.
     *          First step is to create the replacement key.
     *          This step authenticates the host, by providing the current lock key (to be changed) or,
     *          the current secret key and initiates the actual rekey/update lock key-binding operation.
     * Status:  MFI_STAT_LOCK_KEY_REKEY_NOT_ALLOWED  - Step 1 must be completed first
     *          MFI_STAT_INVALID_LOCK_KEY            - Host authentication failed
     */
    MR_DCMD_CTRL_LOCK_KEY_REKEY             = 0x01150700,   // Change the lock key (re-key)

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_LOCK_KEY_FROM_ESCROW
     *          dcmd.mbox.b[0]      - Don't hash pass-phrase (0=hash pass-phrase, 1=don't hash)
     *          dcmd.sge OUT        - ptr to MR_CTRL_LOCK_KEY_FROM_ESCROW
     * Desc:    Provide a lock key from escrow, to unlock foreign secured configurations
     * Status:  MFI_STAT_INVALID_PARAMETER       - Some input parameters are invalid
     *          MFI_STAT_INVALID_CMD             - Some input parameters are not supported
     *          MFI_STAT_LOCK_KEY_ESCROW_INVALID - Escrow lock key could not be used
     *          MFI_STAT_TRANSPORT_READY_LD_EXISTS - configuration has some LD in transport ready state
     */
    MR_DCMD_CTRL_LOCK_KEY_FROM_ESCROW       = 0x01150800,   // provide lock key from escrow

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_LOCK_KEY_DESTROY
     *          dcmd.mbox           - None
     * Desc:    Destroy the controller lock key
     * Status:  MFI_STAT_SECURE_LD_EXISTS          - some secure LD exists
     *          MFI_STAT_SECURE_SYSTEM_PD_EXISTS   - some secure SYSTEM_PD exists
     *          MFI_STAT_SECURE_DEVICE_EXISTS      - some secure EPD or EPD-PASSTHRU DEVICE exists
     *          MFI_STAT_TRANSPORT_READY_LD_EXISTS - configuration has some LD in transport ready state
     */
    MR_DCMD_CTRL_LOCK_KEY_DESTROY           = 0x01150900,


    /*
     *---------------------------------------------------------------------------------------------------
     * Premium Feature Key Management commands
     */
    MR_DCMD_CTRL_PFK                        = 0x01160000,   // PFK Management commands

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PFK_FEATURE_GET
     *          dcmd.mbox.b[0]      - locale (MR_PF_LOCALE), 0xff for all locales
     *          dcmd.sge IN         - If locale!=0xff, ptr to MR_PF_INFO
     *                                if locale==0xff, ptr to MR_PF_LIST
     * Desc:    Return premium features stored in various locations per the request.
     *          If locale=0xff, then FW shall return data only for those locales that have features.
     * Status:  MFI_STAT_INVALID_PARAMETER       - query not supported
     */
    MR_DCMD_CTRL_PFK_FEATURE_GET            = 0x01160100,   // Provide list of features based on location

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PFK_FEATURE_SET
     *          dcmd.mbox.b[0]      - testOnly, 1=Only test the validity of the PFK, 0=apply PFK
     *          dcmd.sge[0] IN/OUT  - Ptr to MR_PF_FEATURE_SET_PARAMS
     * Desc:    Activate premium features indicate by the provided PFK string,
     *          and return the premium features activated by PFK and its locale.
     *          In testOnly mode, FW processes the PFK to report its validity and data, but does not actually
     *          activate any features.
     * Status:  MFI_STAT_OK                      - PFK applied and features activated successfully
     *          MFI_STAT_CTRL_RESET_REQUIRED     - PRK applied, features shall be avaiilable after a reset
     *          MFI_STAT_INVALID_PARAMETER       - PFK string is invalid
     *          MFI_STAT_PFK_INCOMPATIBLE        - PFK cannot be applied to this controller
     *          MFI_STAT_INVALID_SEQUENCE_NUMBER - PFK points to an invalid sequence number
     */
    MR_DCMD_CTRL_PFK_FEATURE_SET            = 0x01160200,   // Activate features based on PFK

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PFK_SAFEID_GET
     *          dcmd.mbox.b[0]      - 0: use EEPROM ID based generator for safeID
     *                              - 1: use random number based generator. The safeID value in this case
     *                                   will not be derived from iButton
     *          dcmd.sge IN         - ptr to null terminated SAFEID string
     * Desc:    Return the SAFEID string.
     * Status:  No error
     */
    MR_DCMD_CTRL_PFK_SAFEID_GET             = 0x01160300,   // Deactivate trial features

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PFK_DEACTIVATE_TRIAL
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Deactivated the Trial key.
     * Status:  MFI_STAT_OK                      - Trial features deactivated
     *          MFI_STAT_CTRL_RESET_REQUIRED     - Trial features deactivated, shall take effect after reset
     *          MFI_STAT_WRONG_STATE             - No trial features enabled
     */
    MR_DCMD_CTRL_PFK_DEACTIVATE_TRIAL       = 0x01160400,   // Deactivate trial features

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_PFK_FEATURE_TRANSFER
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Transfer activated features to key-vault.
     * Status:  MFI_STAT_UPGRADE_KEY_INCOMPATIBLE   - upgrade key is not compatible with controller
     *          MFI_STAT_MFC_HW_ERROR               - HW error writing to key-vault
     */
    MR_DCMD_CTRL_PFK_FEATURE_TRANSFER       = 0x01160500,   // Transfer activated features to key-vault

    /*
     *---------------------------------------------------------------------------------------------------
     * IO metrics collection commands
     */
    MR_DCMD_CTRL_IO_METRICS                 = 0x01170000,   // IO metrics commands

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_IO_METRICS_ENABLE
     *          dcmd.mbox.s[0]      - collectionPeriod in minutes. 0=stop collection
     *          dcmd.sge            - reserved
     * Desc:    Start or stop collection of IO metrics.  Start also specifies the collection period
     * Status:  MFI_STAT_WRONG_STATE    - IO metrics collection already in the state as specified by mbox
     */
    MR_DCMD_CTRL_IO_METRICS_ENABLE          = 0x01170100,   // enable/disable IO metrics collection

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_IO_METRICS_GET
     *          dcmd.mbox.b[0]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to MR_IO_METRICS
     * Desc:    Return the IO metrics for the controller
     * Status:  MFI_STAT_IO_METRICS_DISABLED
     */
    MR_DCMD_CTRL_IO_METRICS_GET             = 0x01170200,   // get IO metrics

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_POWER_USAGE_THROTTLE
     *          dcmd.mbox.w[0]      - (MR_POWER_USAGE_OPTIONS)
     *                                0 : Get power usage,
     *                                1 : Set Power Throttle,
     *                                2 : Reset Power Throttle
     *          dcmd.mbox.w[1]      - sequence number for Set or Reset
     *          dcmd.sge IN         - ptr to MR_CTRL_POWER_USAGE (Get Power Usage)
     *          dcmd.sge OUT        - ptr to MR_CTRL_POWER_USAGE (Set or Reset Power Usage)
     * Desc:    Power usage base IO throttle
     * Status:  MFI_STAT_INVALID_DCMD            - if f/w does not support this DCMD
     *          MFI_STAT_INVALID_PARAMETER       - if power and voltage are invalid or out of range
     *          MFI_STAT_INVALID_SEQUENCE_NUMBER - sequence number is out of sync
     */
    MR_DCMD_CTRL_POWER_USAGE_THROTTLE      = 0x01170300,   // get/set/reset power throttle

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_UNMAP_STATISTICS_GET
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_LOGICAL_DRIVES
     *                                2 - supports MAX_API_LOGICAL_DRIVES_EXT2
     *          dcmd.sge IN         - ptr to MR_UNMAP_STATISTICS
     * Desc:    Return the UNMAP statistics for all the LDs
     * Status:  MFI_STAT_INVALID_DCMD            - if f/w does not support this DCMD
     */
    MR_DCMD_CTRL_UNMAP_STATISTICS_GET       = 0x01170400,   // get unmap statistics


    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_SSC_METRICS_ENABLE_EX
     *          dcmd.mbox.w[0]      -  0: Enable, 1: Disable, 2:Get Parameter
     *          dcmd.mbox.w[1]      -  MR_SSC_METRICS_PARAMETER
     *                                  :type of parameters
     *                                    01 - user
     *                                    02 - exhaustive
     *                                    04 - ld specific
     *                                    80 - debug - not available in Release f/w and it is exclusive
     *          dcmd.sge IN         - ptr to MR_SSC_METRICS_PARAMETER for get parameters
     * Desc:    Set/get MR_SSC_METRICS_PARAMETER for current SSC Metric collection
     * Status:  MFI_STAT_IO_METRICS_DISABLED    - In case IO metrics collection is not enabled already.
     */
    MR_DCMD_CTRL_SSC_METRICS_ENABLE_EX       = 0x01170500,   // Extended enable/disable advanced level SSC metrics collection


    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_SSC_METRICS_GET
     *          dcmd.mbox.s[0]      - type of parameters( 1:user, 2:exhaustive, 4:ld specific) or (0x80:debug) or (0x7F for everything except Debug)
     *          dcmd.mbox.b[2]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to MR_SSC_METRICS_LIST
     * Desc:    Return the SSC metrics for the controller
     * Status:  MFI_STAT_IO_METRICS_DISABLED
     */
    MR_DCMD_CTRL_SSC_METRICS_GET          = 0x01170600,   // get SSC statistics




    /*
     *---------------------------------------------------------------------------------------------------
     * Advanced Event Capture (AEC) commands
     */

    MR_DCMD_CTRL_AEC                        = 0x01180000,   // Advanced Event Capture

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_AEC_CONTROL
     *          dcmd.mbox.w[0]      - 0: free resources (stops capture if started)
     *                                1: stop capture
     *                                2: start capture (mbox.w[1] specifies buffer size)
     *                                3: set capture mode (mbox.w[1] specifies mode)
     *                                4: set filter mask (mbox.w[1] specifies
     *                                    mask group. mbox.w[2] specifies mask bits to set.
     *                                5: clear filter mask (mbox.w[1] specifies
     *                                    mask group. mbox.w[2] specifies mask bits to clear.
     *          dcmd.mbox.w[1]      - (mbox.w[0]==2)
     *                                0: default or retain buffer size from
     *                                   previously specified value
     *                                N: buffer size in terms of number of events
     *
     *                                (mbox.w[0]==3)
     *                                0: capture until stop (default)
     *                                1: capture until buffer full
     *
     *                                (mbox.w[0]==4 or 5)
     *                                  mask group number
     *
     *          dcmd.mbox.w[2]      - (mbox.w[0]==4 or 5)
     *                                bit mask to set or clear the associating
     *                                  bits in the mask group as specified in
     *                                  mbox.w[1].
     *
     * Output:
     *          dcmd.mbox.w[2]      - (mbox.w[0]==4 or 5)
     *                                    resultant mask value after applying
     *                                    set/clear from mbox.w[2]
     * Desc:    Controls AEC (start/stop/free and set capture mode/mask)
     * Status:  MFI_STAT_INVALID_PARAMETER
     */
    MR_DCMD_CTRL_AEC_CONTROL                = 0x01180100,   // AEC control

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_AEC_GET
     *          dcmd.mbox.w[0]      - segment number (starting from 0)
     *          dcmd.sge IN         - ptr to return segment data
     *
     * Output:
     *          dcmd.mbox.w[1]      - Number of segments remaining. Value is valid
     *                                  even if status is MFI_STAT_AEC_NOT_STOPPED
     *
     * Desc: Returns one segment worth of data from AEC buffer. Each segment is MR_AEC_SEGMENT_BUFFER_SIZE bytes.
     *
     * Status: MFI_STAT_INVALID_PARAMETER - Unsupported or incorrect parameters.
     *         MFI_STAT_AEC_NOT_STOPPED - AEC capture is not stopped.
     */
    MR_DCMD_CTRL_AEC_GET                    = 0x01180200,   // get AEC capture data

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_AEC_GET_SETTINGS
     *          dcmd.mbox.w[0]      - None
     *          dcmd.sge IN         - ptr to return MR_AEC_SETTINGS
     *
     * Desc: Returns current AEC setting data (MR_AEC_SETTINGS)
     *
     * Status: No error
    */
    MR_DCMD_CTRL_AEC_GET_SETTINGS           = 0x01180300,   // get AEC capture settings (MR_AEC_SETTINGS)

    /*
     *---------------------------------------------------------------------------------------------------
     * Misc controller commands
     */

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_DOWNGRADE_MR_TO_iMR
     *          dcmd.mbox.w[0]      - None
     *          dcmd.sge IN         - reserved
     *
     * Desc: Downgrades MR controller to iMR mode on next reboot.
     *      If controller has iMR firmware in flash,
     *      and no memory is found on next reboot.
     *
     * Status: MFI_STAT_INVALID_DCMD - if iMR image is not present.
     */
    MR_DCMD_CTRL_DOWNGRADE_MR_TO_iMR        = 0x01190000,   // Downgrade controller from MR to iMR

    /* Input:
     *           dcmd.opcode         -  MR_DCMD_CTRL_SET_CRASH_DUMP_PARAMS
     *           dcmd.mbox.b[0]      -  0 = Turn off (MR_CRASH_DUMP_STATUS.MR_CRASH_DUMP_TURN_OFF)
     *                               -  1 = Turn on  (MR_CRASH_DUMP_STATUS.MR_CRASH_DUMP_TURN_ON)
     *           dcmd.sge OUT        -  N/A
     *           dcmd.sge IN         -  address, length to the crash dump buffer, not applicable if mbox.b[0] is 0
     *
     * Desc: This DCMD will be used to provide the address of the DMA buffer to firmware.
     *       It will also be used to turn on/off the crash dump feature in FW. When the feature
     *       is turned on, DMA buffer address need to be provided by the driver.
     *
     * Status: MFI_STAT_INVALID_PARAMETER - Invalid Parameter
     */

    MR_DCMD_CTRL_SET_CRASH_DUMP_PARAMS       = 0x01190100, // Set crash dump parameters

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_IOV_PRIVATE_INFO_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - ptr to MR_CTRL_IOV_PRIVATE_INFO
     * Desc:    Intended exclusively for certain private IOV information purposes. Not meant for general solutions
     * Status:  MFI_STAT_INVALID_CMD - command not supported
     *
     */
    MR_DCMD_CTRL_IOV_PRIVATE_INFO_GET       =  0x01190200,   // Return private info in IOV cases

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_GET_VF_QD
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to struct MR_IOV_QD_INFO
     * Desc:    Used to get the QD value assigned to a VF.
     * Status:  MFI_STAT_INVALID_CMD - command not supported
     *
     */
    MR_DCMD_CTRL_GET_VF_QD                  =  0x01190300,   // Return ptr to MR_IOV_QD_INFO

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_SET_VF_QD
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - ptr to struct MR_IOV_QD_INFO
     * Desc:    Used to set the QD value assigned to a VF. Note that this is applicable only when the VFQDMode is
     *          set to custom mode (MR_IOV_QD_CUSTOM_SETTING).
     * Status:  MFI_STAT_INVALID_CMD - command not supported
     *
     */
    MR_DCMD_CTRL_SET_VF_QD                  =  0x01190400,   // Set VF QD

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_SET_DEBUG_ATTRIBUTES
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - ptr to MR_DEBUG_INPUTS structure
     * Desc:    This DCMD is used to set/clear debug variables in FW. It can also be used to
     *          execute certain set of debug commands and print the corresponding output to
     *          serial log.
     * Status:  MFI_STAT_INVALID_CMD - command not supported
     */
    MR_DCMD_CTRL_SET_DEBUG_ATTRIBUTES       = 0x01190500,   // set/clear debug variables in FW

    /*
     *---------------------------------------------------------------------------------------------------
     * Commands for control HA modes intended for SPECIAL purposes ONLY
     */
    MR_DCMD_CTRL_CONTROL_HA_MODE            = 0x011a0000,   // Control HA mode specific commands

    /* Input:   dcmd.opcode         - MR_DCMD_CTRL_CONTROL_HA_MODE_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - enum byte value MR_CTRL_HA_CONTROL_MODE
     * Desc:    Return the MR_CTRL_HA_CONTROL_MODE
     * Status:  MFI_STAT_INVALID_CMD - command not supported
     *
     */
    MR_DCMD_CTRL_CONTROL_HA_MODE_GET        =  0x011a0100,   // Return current MR_CTRL_HA_CONTROL_MODE

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_CONTROL_HA_MODE_SET
     *          dcmd.mbox.b[0]      - enum value of MR_CTRL_HA_CONTROL_MODE
     *          dcmd.sge IN         - reserved
     *
     * Desc: This is a command that a sepcialized application is intended to use.
     *       If the controller is geared (via license or other methods) for the destination mode of HA then user can change
     *       the funtioning of this controller to that mode. Whether all controllers change to this mode is not dictated by
     *       the API itself but by the implementation behavior of the solution. If the soltuion choses gearing itself to operate
     *       in the desired mode then the user need not use this command.
     *       Example1: There are solutions where user wants to allow moving to say a 2 controller HA mode
     *       from single controller mode only via a tool even though the controller(s) are capable of multiple controller HA
     *       (via say a license enablement already).
     *       Example2: There is a single controller in the solution and the FW is upgraded to multiple controller HA and also the
     *       the license (or any other method) is set to multiple controller HA mode, but user wants to continue the same feature set
     *       of single controller only till a tool issues the above DCMD with value 2.
     *
     * Status:
     *         MFI_STAT_INVALID_PARAMETER      - one or more input parameters is invalid
     *         MFI_STAT_WRONG_STATE            - current state doesn't allow this change
     *         MFI_STAT_INVALID_CMD            - command not supported
     */
    MR_DCMD_CTRL_CONTROL_HA_MODE_SET     =     0x011a0200,   // Control HA mode

    /*
     *---------------------------------------------------------------------------------------------------
     * Commands for Maintenance modes. To be used for the special purposes only.
     */
    MR_DCMD_CTRL_MAINTENANCE_MODE            =     0x011b0000,   // Maintenance Mode commands

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_MAINTENANCE_MODE_SET
     *          dcmd.mbox.b[0]      - enum value of MR_CTRL_MAINTENANCE_MODE
     *          dcmd.sge IN         - reserved
     *
     * Desc: This is a command that will place the controller into maintenance mode or return it to normal operating
     *       mode.  For the mode to take effect, the controller may require a system reboot or controller restart.
     *       This mode will be persistent until maintenance mode is changed. Please refer to the enum MR_CTRL_MAINTENANCE_MODE
     *       for details of the modes.
     * Status:
     *         MFI_STAT_INVALID_PARAMETER      - one or more input parameters is invalid
     *         MFI_STAT_INVALID_CMD            - command not supported
     */
    MR_DCMD_CTRL_MAINTENANCE_MODE_SET     =     0x011b0100,   // Set maintenance mode

    /*
     *---------------------------------------------------------------------------------------------------
     * Commands for personality management of the controller. To be used for the special purposes only.
     */
    MR_DCMD_CTRL_PERSONALITY_MANAGEMENT          =  0x011c0000,   //Personality management commands

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_PERSONALITY_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to structure MR_CTRL_PERSONALITY_INFO
     *
     * Desc: Return the current mode of the controller through the structure MR_CTRL_PERSONALITY_INFO
     *
     * Status: MFI_STAT_INVALID_CMD - command not supported
     */
    MR_DCMD_CTRL_PERSONALITY_GET                 =  0x011c0100,      // Get the current personality of the controller

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_PERSONALITY_SET
     *          dcmd.mbox.s[0]      - bitmap of the enum values defined in MR_CTRL_PERSONALITY
     *          dcmd.mbox.s[1]      - New profile Id
     *          dcmd.mbox.b[4]      - Force personality change. This is applicable only when
     *                                adapterOperations5.supportForcePersonalityChange is set.
     *          dcmd.sge OUT        - reserved
     *
     * Desc: This is a command to set the personalities of the controller. Applications should first query the
     *       current mode of the controller and then use this command for the personality change request. For the change
     *       to take effect, the controller may require a system reboot or controller restart. The personality will be
     *       persistent until it is changed. Using the force option with this DCMD skips all the compatibility checks
     *       performed in the firmware during a personality switch.
     *
     * Status:
     *         MFI_STAT_INVALID_PARAMETER      - one or more input parameters is invalid
     *         MFI_STAT_OK_REBOOT_REQUIRED     - Ok but reboot required
     *         MFI_STAT_INVALID_CMD            - command not supported
     *         MFI_STAT_OPERATION_NOT_POSSIBLE - switching mode is not possible (e.g., current mode is RAID
     *                                           and there is VD or foreign config and user attempting to switch to HBA,
     *                                           or apps requeseted a switch to an unsupported personality)
     *         MFI_STAT_SNAPDUMP_CLEAR_REQUIRED- Existing snapdump needs to be cleared before attempting this cmd
     */
    MR_DCMD_CTRL_PERSONALITY_SET                 =  0x011c0200,   // Set the mode of the controller

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_PROFILE_LIST_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - Pointer to the MR_CTRL_PROFILE_LIST structure
     *
     * Desc: This command returns the list of all valid profiles entries for the controller through the structure
     *       MR_CTRL_PROFILE_LIST
     *
     * Status: MFI_STAT_INVALID_CMD - command not supported
     */
    MR_DCMD_CTRL_PROFILE_LIST_GET                 =  0x011c0300,      // Get the current personality of the controller

    /*
     *---------------------------------------------------------------------------------------------------
     * Commands for Software Zone management
     */
    MR_DCMD_CTRL_SWZONE                    =  0x011d0000,   //Software Zone management commands

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_SWZONE_RESERVATION_PREEMPT
     *          dcmd.mbox.b[0]      -  0: 2 nodes SWZONE
     *
     * Desc: This command is issued to surviving controller node to take over all the VDs which are failed in other node/controller
     *
     * Status: MFI_STAT_INVALID_CMD               - command not supported
     *         MFI_STAT_INVALID_PARAMETER         - controller is disabled Software Zone
     *         MFI_STAT_OPERATION_NOT_POSSIBLE    - unable to perform preempt operation
     *
     */
    MR_DCMD_CTRL_SWZONE_RESERVATION_PREEMPT=  0x011d0100,      // Preempt orphaned VDs/PDs

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_SWZONE_RESERVATION_RELEASE
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_TARGET_ID/MAX_API_TARGET_ID_EXT
     *                                2 - supports MAX_API_TARGET_ID_EXT2
     *          dcmd.sge IN         - ptr to returned MR_LD_TARGETID_LIST structure
     *
     * Desc: This command is issued to running controller/node to release its owned VDs.
     *
     * Status: MFI_STAT_INVALID_CMD               - command not supported
     *         MFI_STAT_INVALID_PARAMETER         - software Zone is disabled
     *         MFI_STAT_LD_SSCD_CACHE_PRESENT     - unable to release due to SSC cache
     *         MFI_STAT_BGOP_IN_PROGRESS          - unable to release due to conflict with background operation
     *         MFI_STAT_OPERATION_NOT_POSSIBLE    - unable to perform release operation
     */
    MR_DCMD_CTRL_SWZONE_RESERVATION_RELEASE=  0x011d0200,   // Release ownership of specified VDs/PDs

    /*
     *---------------------------------------------------------------------------------------------------
     * Commands for NVDRAM management
     */
    MR_DCMD_CTRL_NVDRAM                    =  0x011e0000,   //NVDRAM management commands

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_NVDRAM_SIZE_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to returned MR_NVDRAM_PROPERTIES structure
     *
     * Desc: Returns NVDRAM size and its properties
     *
     * Status: MFI_STAT_INVALID_CMD               - command not supported
     *
     */
    MR_DCMD_CTRL_NVDRAM_PROPERTIES_GET           =  0x011e0100,

    /*
     *---------------------------------------------------------------------------------------------------
     * Commands for Controller FRU state management
     */
    MR_DCMD_CTRL_FRU_STATE                  = 0x011f0000,   // FRU state management commands

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_SYNC_FRU_STATE
     *          dcmd.mbox.w[0]      - FruID (lower 4 bytes)
     *          dcmd.mbox.w[1]      - FruID (upper 4 bytes)
     *          dcmd.mbox.b[8]      - Direction
     *                                MR_IOV_FRU_HOST_TO_CONTROLLER (0) - from host to controller
     *                                MR_IOV_FRU_CONTROLLER_TO_HOST (1) - from controller to host
     *          dcmd.sge IN/OUT     - Pointer to FRU data
     * Desc: Syncs FRU state between the controller and the host
     * Status:  MFI_STAT_OK                     - Command successful
     *          MFI_STAT_FRU_INVALID_HOST_DATA  - FRU data provided by host does not match that of controller.
     *          MFI_STAT_FRU_INVALID_CTRL_DATA  - Controller's FRU data is invalid.
     *          MFI_STAT_INVALID_DCMD           - If DCMD called in non-IOV mode
     */
     MR_DCMD_CTRL_SYNC_FRU_STATE            = 0x011f0100,   // Save controller's FRU state.

    /* Input:
     *          dcmd.opcode         - MR_DCMD_CTRL_GET_FRU_ID
     *          dcmd.mbox           - Reserved
     *          dcmd.sge IN         - Ptr to struct MR_IOV_CTRL_FRU_STATE
     * Desc:    Get controller's FRU ID
     * Status:  MFI_STAT_OK                     - Command successful
     *          MFI_STAT_INVALID_DCMD           - If DCMD called in non-IOV mode
     */
     MR_DCMD_CTRL_GET_FRU_ID                = 0x011f0200,   // Get controller's FRU ID

    /*
     *---------------------------------------------------------------------------------------------------
     * Commands for Controller context based snapdump management
     */
     MR_DCMD_CTRL_SNAPDUMP                   = 0x01200000,   // snapdump management


    /* Input:
     *          dcmd.opcode                - MR_DCMD_CTRL_SNAPDUMP_GET_PROPERTIES
     *          dcmd.hdr.length            - number of bytes to read
     *          dcmd.sge                   - Ptr to MR_SNAPDUMP_PROPERTIES
     * Desc:    Fill in snapdump properties
     * Status:  MFI_STAT_OK                           - Command successful
     *          MFI_STAT_INVALID_PARAMETER            - Parameters are not valid
     *          MFI_STAT_SNAPDUMP_NOT_ENABLED         - snapdump features is not enabled
     */
     MR_DCMD_CTRL_SNAPDUMP_GET_PROPERTIES    = 0x01200100,   // Get snapdump properties.


    /* Input:
     *          dcmd.opcode                - MR_DCMD_CTRL_SNAPDUMP_SET_PROPERTIES
     *          dcmd.hdr.length            - number of bytes to send
     *          dcmd.sge                   - Ptr to MR_SNAPDUMP_PROPERTIES
     * Desc:    Update snapdump properties
     * Status:  MFI_STAT_OK                           - Command successful
     *          MFI_STAT_INVALID_PARAMETER            - Parameters are not valid
     *          MFI_STAT_SNAPDUMP_NOT_ENABLED         - snapdump features is not enabled
     */
     MR_DCMD_CTRL_SNAPDUMP_SET_PROPERTIES    = 0x01200200,   // set snapdump properties

    /* Input:   dcmd.opcode                - MR_DCMD_CTRL_SNAPDUMP_GET
     *          dcmd.hdr.length            - number of bytes in data buffer
     *          dcmd.mbox.b[0]             - 1 - retrieve snap dump info(MR_SNAPDUMP_INFO)
     *                                     - 2 - retrieve snap dump data(MR_SNAPDUMP_DATA)
     *          dcmd.mbox.b[1]             - snapshot number (valid if b[0] is 2)
     *          dcmd.mbox.w[1]             - starting offset (valid if b[0] is 2)
     *          dcmd.sge IN                 - Ptr to returned MR_SNAPDUMP_INFO OR MR_SNAPDUMP_DATA
     *          Output:  dcmd.hdr.length    - total number of bytes transferred
     * Desc:    Returns the requested snapdump info or snapdump data
     * Status:  MFI_STAT_OK                           - Command successful
     *          MFI_STAT_INVALID_PARAMETER            - Parameters are not valid
     *          MFI_STAT_SNAPDUMP_NOT_ENABLED         - snapdump features is not enabled
     *          MFI_STAT_SNAPDUMP_NOT_AVAILABLE       - no valid snapdump is available
     */
     MR_DCMD_CTRL_SNAPDUMP_GET               = 0x01200300,   // get snap dump info or read snapdump data

    /* Input:   dcmd.opcode                 - MR_DCMD_CTRL_SNAPDUMP_CLEAR
     *          dcmd.mbox                   - reserved
     *          dcmd.sge IN                 - reserved
     * Desc:    Clears all the available snap dump images from controller memory
     * Status:  MFI_STAT_OK                 - Command successful
     *          MFI_STAT_SNAPDUMP_NOT_ENABLED snapdump features is not enabled
     *          MFI_STAT_INVALID_DCMD         SnapDump Clear is not valid command
     */
    MR_DCMD_CTRL_SNAPDUMP_CLEAR              = 0x01200400,   // Clear all snap dump images from ctrl memory

    /* Input:  dcmd.opcode                  - MR_DCMD_CTRL_RTTRACE_BUFFER
    *          dcmd.mbox.w[0]               - lower word of RTTrace start address
    *          dcmd.mbox.w[1]               - upper word of RTTrace start address
    *          dcmd.mbox.w[2]               - length of the RTTrace in bytes
    *          dcmd.sge  IN                 - reserved
    * Desc:    Register the driver RT Trace buffer address/length with FW
    * Status:
    *          MFI_STAT_OK                  - Command successful
    *          MFI_STAT_INVALID_CMD         - Invalid command
    */
    MR_DCMD_CTRL_RTTRACE_BUFFER             = 0x01200500,   //Register driver rttrace buffer address and length with FW

    /* Input:   dcmd.opcode                 - MR_DCMD_CTRL_SNAPDUMP_ONDEMAND
     *          dcmd.mbox                   - reserved
     *          dcmd.sge IN                 - reserved
     * Desc:    Triggers on demand snap dump data collection. FW completes the command only after collecting
     *          the snapdump data.
     * Status:  MFI_STAT_OK                    Command successful
     *          MFI_STAT_SNAPDUMP_NOT_ENABLED  SnapDump features is not enabled
     */
    MR_DCMD_CTRL_SNAPDUMP_ONDEMAND           = 0x01200600, // OnDemand SnapDump generation

    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * Physical Device commands
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_PD                              = 0x02000000,   // Physical Device (PD) opcodes

    /* Input:   dcmd.opcode         - MR_DCMD_PD_GET_LIST
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to returned MR_PD_LIST structure
     * Desc:    Return the physical drive list (configured and unconfigured) structure.
     * Status:  No error
     */
    MR_DCMD_PD_GET_LIST                     = 0x02010000,   // get PD list (abbreviated list)

    /* Input:   dcmd.opcode         - MR_DCMD_PD_LIST_QUERY
     *          dcmd.mbox.b[0]      - query attribute (MR_PD_QUERY_TYPE)
     *          dcmd.mbox.b[1]      - query value
     *          dcmd.sge IN         - ptr to returned MR_PD_LIST structure
     * Desc:    Return a list of physical devices satisfying the supported query. The query matches
     *          device attribute to specified value.
     * Status:  MFI_STAT_INVALID_PARAMETER          - query not supported
     */
    MR_DCMD_PD_LIST_QUERY                   = 0x02010100,   // get PD list (abbreviated list)

    /* Input:   dcmd.opcode         - MR_DCMD_PD_GET_INFO
     *          dcmd.mbox.s[0]      - deviceId for this physical drive
     *          dcmd.sge IN         - ptr to returned MR_PD_INFO structure
     *                                If supportPointInTimeProgress is set, then MR_PROGRESS.elapsedSecsForLastPercent is returned
     *                                Otherwise MR_PROGRESS.elapsedSecs is returned
     * Desc:    Return the physical drive info structure
     * Status:  MFI_STAT_DEVICE_NOT_FOUND   - deviceId is invalid
     *          MFI_STAT_WRONG_STATE        - device is in wrong state (DDF read in progress)
     */
    MR_DCMD_PD_GET_INFO                     = 0x02020000,   // get PD info (complete PD info)

    /* Input:   dcmd.opcode         - MR_DCMD_PD_STATE_SET
     *          dcmd.mbox.w[0]      - MR_PD_REF
     *          dcmd.mbox.s[2]      - MR_PD_STATE (new state)
     * Desc:    Change the PD state to new state. For details refer to the state diagram.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_INVALID_PARAMETER          - new state is invalid
     *          MFI_STAT_WRONG_STATE                - new state not allowed. Refer to state diagram
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - new state not allowed. PD is in removal ready state
     *          MFI_STAT_SCSI_RESERVATION_CONFLICT  - drive owned by peer
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     *          MFI_STAT_PD_CLEAR_IN_PROGRESS       - Sanitize in progress on the PD
     */
    MR_DCMD_PD_STATE                        = 0x02030000,   // change PD state commands (only for LD member drives)
    MR_DCMD_PD_STATE_SET                    = 0x02030100,   // change to new state

    /* Input:   dcmd.opcode         - MR_DCMD_PD_REBUILD_START
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Rebuild this PD
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - device is in wrong state.  Refer to state diagram
     *          MFI_STAT_LD_INIT_IN_PROGRESS        - Init is in progress on an LD using this PD
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     *          MFI_STAT_PD_NOT_ELIGIBLE_FOR_LD       device is not eligible for RAID configuration
     */
    MR_DCMD_PD_REBUILD                      = 0x02040000,   // Rebuild commands
    MR_DCMD_PD_REBUILD_START                = 0x02040100,   // start a REBUILD

    /* Input:   dcmd.opcode         - MR_DCMD_PD_REBUILD_ABORT
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Abort the rebuild of this PD
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - device is in wrong state (device must be in rebuilding state)
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_REBUILD_ABORT                = 0x02040200,   // abort a REBUILD

    /* Input:   dcmd.opcode         - MR_DCMD_PD_CLEAR_START
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Clear the entire PD by writing 0's
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - device is in wrong state (must be an unconfigured good drive)
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_PD_CLEAR_IN_PROGRESS       - clear operation already in progress
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_CLEAR                        = 0x02050000,   // PD clear commands
    MR_DCMD_PD_CLEAR_START                  = 0x02050100,   // start a write 0's on this PD

    /* Input:   dcmd.opcode         - MR_DCMD_PD_CLEAR_ABORT
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Abort the PD clear operation
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - device is in wrong state (must be initializing)
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_CLEAR_ABORT                  = 0x02050200,   // abort the PD init

    /* Input:   dcmd.opcode         - MR_DCMD_PD_GET_PROGRESS
     *          dcmd.mbox.s[0]      - deviceId for this physical drive
     *          dcmd.sge IN         - ptr to returned MR_PD_PROGRESS structure
     *                                If supportPointInTimeProgress is set, then MR_PROGRESS.elapsedSecsForLastPercent is returned
     *                                Otherwise MR_PROGRESS.elapsedSecs is returned
     * Desc:    Return the physical drive progress structure
     * Status:  MFI_STAT_DEVICE_NOT_FOUND   - deviceId is invalid
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_GET_PROGRESS                 = 0x02060000,   // get PD progress info

    /* Input:   dcmd.opcode         - MR_DCMD_PD_LOCATE_START
     *          dcmd.mbox.s[0]      - deviceId for this physical drive
     *          dcmd.mbox.s[1]      - duraiton of identify (in seconds; 0 for indefinite)
     * Desc:    Flash the LED's of the drive that are in a enclosure that supports LOCATE
     * Status:  MFI_STAT_DEVICE_NOT_FOUND   - deviceId is invalid
     *          MFI_STAT_NOT_IN_ENCL        - one or more drives are not in an enclosure
     */
    MR_DCMD_PD_LOCATE                       = 0x02070000,   // locate PD commands
    MR_DCMD_PD_LOCATE_START                 = 0x02070100,   // locate the given PD (illuminate LEDs)

    /* Input:   dcmd.opcode         - MR_DCMD_PD_LOCATE_STOP
     *          dcmd.mbox.s[0]      - deviceId for this physical drive
     * Desc:    Undo the locate start for this PD
     * Status:  MFI_STAT_DEVICE_NOT_FOUND   - targetId is invalid
     *          MFI_STAT_NOT_IN_ENCL        - none of the drives are in an enclosure
     */
    MR_DCMD_PD_LOCATE_STOP                  = 0x02070200,    // stop locate the given PD

    /* Input:   dcmd.opcode         - MR_DCMD_PD_MISSING_MARK
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Mark the configured PD as missing. Kick in the HSP if present.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - Drive should be FAILED or OFFLINE and part of the logical drive.
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_MISSING                      = 0x02080000,   // mark a PD missing operations
    MR_DCMD_PD_MISSING_MARK                 = 0x02080100,   // mark a PD in a ARRAY as 'missing'

    /* Input:   dcmd.opcode         - MR_DCMD_PD_MISSING_REPLACE
     *          dcmd.mbox.w[0]      - MR_PD_REF
     *          dcmd.mbox.s[2]      - array reference (from MR_ARRAY)
     *          dcmd.mbox.b[6]      - array row number
     * Desc:    Replace the configured PD. Specified arrayIndex/row must be a missing drive. Automatic rebuild
     *          will not start, user must explicitly rebuild this drive.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_ARRAY_ROW_NOT_EMPTY        - row number is not empty
     *          MFI_STAT_ARRAY_INDEX_INVALID        - array index invalid
     *          MFI_STAT_ROW_INDEX_INVALID          - row index invalid
     *          MFI_STAT_DRIVE_TOO_SMALL            - drive too small to rebuild
     *          MFI_STAT_PD_TYPE_WRONG              - replacing with a wrong drive type (like SAS with SATA)
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_PD_CLEAR_IN_PROGRESS       - CLEAR operation in progress
     *          MFI_STAT_WRONG_STATE                - PD in wrong state - must be UNCONFIGURED_GOOD
     *          MFI_STAT_CONFIG_RESOURCE_CONFLICT   - PD already exists in this array
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_MISSING_REPLACE              = 0x02080200,   // replace a 'missing' PD in an ARRAY

    /* Input:   dcmd.opcode         - MR_DCMD_PD_REMOVE_PREP
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Prepare for removal of this unconfigured physical drive. Firmware will spin down this drive. The drive
     *          state is unchanged. But MR_PD_INFO disabledForRemoval will be set. No DDF updates.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - deviceId is part of configuration
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is already in removal ready state
     */
    MR_DCMD_PD_REMOVE                       = 0x02090000,   // Prepare for Removal operations
    MR_DCMD_PD_REMOVE_PREP                  = 0x02090100,   // prepare unconfigured PD for removal

    /* Input:   dcmd.opcode         - MR_DCMD_PD_REMOVE_UNDO
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Undo the prepare for removal. Firmware will spin back this drive. Drive state is unchanged.
     *          But MR_PD_INFO disabledForRemoval will be cleared. No DDF updates.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - deviceId was not prepared for removal
     */
    MR_DCMD_PD_REMOVE_UNDO                  = 0x02090200,   // undo 'prepare PD for removal'

    /* Input:   dcmd.opcode         - MR_DCMD_PD_GET_ALLOWED_OPS
     *          dcmd.mbox.s[0]      - deviceId for this physical drive
     *          dcmd.sge IN         - ptr to returned MR_PD_ALLOWED_OPS structure
     * Desc:    Return the allowed configuration operations structure.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     */
    MR_DCMD_PD_GET_ALLOWED_OPS              = 0x020a0000,   // get the allowed operations for a particular PD.

    /* Input:   dcmd.opcode         - MR_DCMD_PD_GET_ALLOWED_OPS_ALL_PDS
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_PHYSICAL_DEVICES
     *                                2 - supports MAX_API_PHYSICAL_DEVICES_EXT2
     *          dcmd.sge IN         - ptr to returned MR_PD_ALLOWED_OPS_LIST structure
     * Desc:    Return the allowed configuration operations structure.
     * Status:  No error
     */
    MR_DCMD_PD_GET_ALLOWED_OPS_ALL_PDS      = 0x020a0100,   // get the allowed operations for all PDs.

    /*
     *---------------------------------------------------------------------------------------------------
     * COPYBACK commands
     */
    MR_DCMD_PD_COPYBACK                     = 0x020b0000,   // COPYBACK commands

    /* Input:   dcmd.opcode         - MR_DCMD_PD_COPYBACK_START
     *          dcmd.mbox.w[0]      - MR_PD_REF     - destination
     *          dcmd.mbox.w[1]      - MR_PD_REF     - source
     * Desc:    Copy the data from source PD to destination PD
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - device is in wrong state.  Refer to state diagram
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_PD_COPYBACK_IN_PROGRESS    - CopyBack is in progress on this deviceId
     *          MFI_STAT_LD_OFFLINE                 - LD/array is offline
     *          MFI_STAT_LD_RBLD_IN_PROGRESS        - Rebuild(s) is in progress on this array
     *          MFI_STAT_INVALID_CMD                - Copyback feature is not enabled or no resources to start
     *          MFI_STAT_LD_INIT_IN_PROGRESS        - Init is in progress on this array
     *          MFI_STAT_LD_RECON_IN_PROGRESS       - Recon is in progress on this array
     *          MFI_STAT_PD_TYPE_WRONG              - Wrong drive type for destination
     *          MFI_STAT_DRIVE_TOO_SMALL            - Destination drive size too small
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     *          MFI_STAT_PD_NOT_ELIGIBLE_FOR_LD       device is not eligible for RAID configuration
     */
    MR_DCMD_PD_COPYBACK_START               = 0x020b0100,

    /* Input:   dcmd.opcode         - MR_DCMD_PD_COPYBACK_ABORT
     *          dcmd.mbox.w[0]      - MR_PD_REF     - copyBack
     * Desc:    Abort the copyBack on the copyBack PD
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - device is in wrong state (device must be in copyback state)
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_COPYBACK_ABORT               = 0x020b0200,

    /*
     *---------------------------------------------------------------------------------------------------
     * Security commands
     */

    MR_DCMD_PD_SECURITY                     = 0x020c0000,   // PD Security commands

    /* Input:   dcmd.opcode         - MR_DCMD_PD_RECOVER_LOCKED
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Transition a locked drive to unsecured and unlocked. Used to recover drives, when lock key is lost or
     *          unavailble.
     *          Drive shall be cryptographically erased, as part of this operation.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - deviceId is part of configuration
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_RECOVER_LOCKED               = 0x020c0100,   // Recover a locked foreign drive

    /* Input:   dcmd.opcode         - MR_DCMD_PD_LOCK_KEY_REKEY
     *          dcmd.mbox.w[0]      - MR_PD_REF
     *          dcmd.sge            - reserved
     * Desc:    Change the lock key of a drive. Only an unlocked drive can be rekeyed.  The current lock
     *          key of the drive must be known.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - deviceId is part of configuration
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_LOCK_KEY_ESCROW_INVALID    - Escrow lock key is not present, or applies to the given PD
     *          MFI_STAT_LOCK_KEY_INVALID           - Controller doesn't have an assigned lock key
     *          MFI_STAT_LOCK_KEY_REKEY_NOT_ALLOWED - deviceId cannot be re-keyed
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_LOCK_KEY_REKEY               = 0x020c0200,   // Recover a locked foreign drive

    /* Input:   dcmd.opcode         - MR_DCMD_PD_REPROVISION
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Transition an FDE drive from secured to unsecured.  Drive shall be cryptographically
     *          erased, as part of this operation.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - deviceId is part of configuration
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_REPROVISION_NOT_ALLOWED    - devideId is a non-FDE drive
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_REPROVISION                  = 0x020c0300,   // Reprovision drive

    /* Input:   dcmd.opcode         - MR_DCMD_PD_LOCK_KEY_APPLY
     *          dcmd.mbox.w[0]      - MR_PD_REF (MR_PD_REF.seqNum is required if controller is in MFI_STATE_READY or later)
     *          dcmd.sge OUT        - ptr to MR_CTRL_LOCK_KEY
     * Desc:    Provide a lock key from EKMS, to unlock or rekey configuration
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_INVALID_PARAMETER          - some input parameters are invalid
     *          MFI_STAT_FEATURE_SECURITY_NOT_ENABLED - Drive security feature is not enabled
     *          MFI_STAT_LOCK_KEY_EKM_NO_BOOT_AGENT - EKM boot agent was not detected
     *          MFI_STAT_LOCK_KEY_REKEY_NOT_ALLOWED - Rekey is not allowed, as device doesn't need to be rekeyed
     *          MFI_STAT_LOCK_KEY_INVALID           - key could not be applied
     *          MFI_STAT_PD_SECURITY_TYPE_WRONG     - device doesn't support this operation
     *          MFI_STAT_PD_HW_ERRORS_DETECTED      - there was a problem in applying the key to the device
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_LOCK_KEY_APPLY                = 0x020c0400,   // apply lock key to device

    /* Input:   dcmd.opcode         -  MR_DCMD_PD_SECURE_JBOD
     *          dcmd.mbox.w[0]      -  MR_PD_REF
     * Desc:    Enable security on JBOD/system drive.
     *          MR_PD_ALLOWED_OPS.enableSecurityonJBOD must be TRUE
     * Status:  MFI_STAT_WRONG_STATE                - PD is not in JBOD/System drive state
     *          MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_INVALID_PARAMETER          - some input parameters are invalid
     *          MFI_STAT_FEATURE_SECURITY_NOT_ENABLED - Drive security feature is not enabled
     *          MFI_STAT_LOCK_KEY_EKM_NO_BOOT_AGENT - EKM boot agent was not detected
     *          MFI_STAT_LOCK_KEY_INVALID           - key could not be applied
     *          MFI_STAT_PD_SECURITY_TYPE_WRONG     - device doesn't support this operation
     *          MFI_STAT_PD_HW_ERRORS_DETECTED      - there was a problem in applying the key to the device
     */
    MR_DCMD_PD_SECURE_JBOD                     = 0x020c0500, // support security on JBOD


    /*
     *---------------------------------------------------------------------------------------------------
     * POWER commands
     */
    MR_DCMD_PD_POWER                        = 0x020d0000,   // POWER commands

    /* Input:   dcmd.opcode         - MR_DCMD_PD_POWER_STATE_GET
     *          dcmd.mbox.b[0]      - power state (MR_PD_POWER_STATE)
     *          dcmd.sge IN         - Pointer to returned MR_PD_DEVICEID_LIST structure
     * Desc:    Returns list of deviceId that have the specified power state
     * Status:  No error
     */
    MR_DCMD_PD_POWER_STATE_GET              = 0x020d0100,   // get all drives with specificied power state

    /* Input:   dcmd.opcode         - MR_DCMD_PD_POWER_STATE_SET
     *          dcmd.mbox.b[0]      - power state (MR_PD_POWER_STATE)
     *          dcmd.sge IN/OUT     - Pointer to MR_PD_DEVICEID_LIST structure
     * Desc:    Sets specified power set on list of provided deviceIds.  FW may take some time to actually
     *          transition the devices to the specified power state. If the command is re-issued, while
     *          power state transition is in progress, with the same parameters, FW shall return status
     *          MFI_STAT_POWER_STATE_SET_IN_PROGRESS and the list of devices that have successfully
     *          transitioned to the new power state.  This mechanism may be used to poll the progress of
     *          the power state transition.
     * Status:  MFI_STAT_INVALID_PARAMETER           - power state is invalid
     *          MFI_STAT_DEVICE_NOT_FOUND            - one or more of the deviceId is invalid
     *          MFI_STAT_WRONG_STATE                 - PD is in wrong state (cannot be UNCONFIGURED_BAD or FAILED)
     *          MFI_STAT_PD_READY_FOR_REMOVAL        - PD is in removal ready state
     *          MFI_STAT_INVALID_CMD                 - Some input parameters are not supported
     *          MFI_STAT_POWER_STATE_SET_IN_PROGRESS - this DCMD is already in progress, see Desc.
     *          MFI_STAT_POWER_STATE_SET_BUSY        - FW is busy processing another request
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE - device is in transport ready state
     */
    MR_DCMD_PD_POWER_STATE_SET              = 0x020d0200,   // set drives to specificied power state

    /*
     *---------------------------------------------------------------------------------------------------
     * Suspend/Resume PD commands
     */
    /* Input:   dcmd.opcode         - MR_DCMD_PD_REBUILD_SUSPEND_RESUME
     *          dcmd.mbox.w[0]      - MR_PD_REF
     *          dcmd.mbox.b[4]      - 0 = suspend, 1 = resume
     * Desc:    Suspend/Resume the rebuild of this PD
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - device is in wrong state (device must be in rebuilding state)
     *          MFI_STAT_INVALID_PARAMETER          - Parameters are not valid
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_SUSPEND_RESUME               = 0x020e0000,   // suspend/resume for PD operations
    MR_DCMD_PD_REBUILD_SUSPEND_RESUME       = 0x020e0100,   // suspend/resume a REBUILD

    /* Input:   dcmd.opcode         - MR_DCMD_PD_COPYBACK_SUSPEND_RESUME
     *          dcmd.mbox.w[0]      - MR_PD_REF     - copyBack
     *          dcmd.mbox.b[4]      - 0 = suspend, 1 = resume
     * Desc:    Suspend/Resume the copyBack on the copyBack PD
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - device is in wrong state
     *          MFI_STAT_INVALID_PARAMETER          - Parameters are not valid
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_COPYBACK_SUSPEND_RESUME      = 0x020e0200,

    /*
     *---------------------------------------------------------------------------------------------------
     * PD erase commands
     */
    /* Input:   dcmd.opcode         - MR_DCMD_PD_SECURE_ERASE_START
     *          dcmd.mbox.w[0]      - MR_PD_REF
     *          dcmd.mbox.b[4]      - This is a bit controlled setting
     *                              - bit (7,6) - 00=MR_ERASE_TYPE, (1,0)=MR_SS_ERASE_TYPE, (0,1)=MR_SANITIZE_TYPE
     *                              - bits 0..5 - erase/Sanitize type
     *          dcmd.mbox.b[5]      - Erase Pattern A; not applicable for MR_SANITIZE_TYPE
     *          dcmd.mbox.b[6]      - Erase Pattern B; not applicable for MR_SANITIZE_TYPE
     *          dcmd.mbox.b[7]      - This is a bit controlled setting applicable only for MR_SANITIZE_TYPE
     *                              - bits 0   - Allow Unrestricted Sanitize Exit (AUSE)
     *                              - bits 1-7 - Reserved
     * Desc:    Erases the PD's data via MR_ERASE_TYPE or MR_SS_ERASE_TYPE or MR_SANITIZE_TYPE
     *          see MR_PD_INFO.properties.useSSEraseType;
     *          see MR_PD_INFO.properties.sanitizeType in order to issue Sanitize.
     *          Erase Patterns are optional and based on erase type; no pattern for Sanitize type.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - device is in wrong state
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_INVALID_PARAMETER          - Parameters are not valid
     *          MFI_STAT_UNSUPPORTED_HW             - device doesn't support this command
     *          MFI_STAT_PD_CLEAR_IN_PROGRESS       - Sanitize/Clear already in progress on the device and host tries to issue non allowed operations on that PD
     *          MFI_STAT_PD_SANITIZE_TYPE_UNSUPPORTED  - device doesn't support the Sanitize type
     *          MFI_STAT_INVALID_CMD                - device doesn't support Sanitize or other operation is in progress
     */
    MR_DCMD_PD_SECURE_ERASE                 = 0x020f0000,   // PD Erase commands
    MR_DCMD_PD_SECURE_ERASE_START           = 0x020f0100,   // start the erase (MR_ERASE_TYPE)

    /* Input:   dcmd.opcode         - MR_DCMD_PD_SECURE_ERASE_ABORT
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Abort the PD erase operation
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     *          MFI_STAT_WRONG_STATE                - device is in wrong state
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_OPERATION_NOT_POSSIBLE     - Cannot stop Sanitize
     */
    MR_DCMD_PD_SECURE_ERASE_ABORT           = 0x020f0200,   // abort the erase

    /*
     *---------------------------------------------------------------------------------------------------
     * Misc PD commands
     */

    /* Input:   dcmd.opcode         -  MR_DCMD_PD_GET_SMART_INFO
     *          dcmd.mbox.s[0]      -  device id
     *          dcmd.mbox.s[1]      -  reserved for future
     *          dcmd.sge IN         -  ptr to MR_PD_SMART_INFO structure
     * Desc:    Read SMART attributes from device and return interpreted or raw data
     *          MR_PD_ALLOWED_OPS.SMARTSupported must be 1
     * Status:  MFI_STAT_DEVICE_NOT_FOUND - deviceId is invalid
     *          MFI_STAT_UNSUPPORTED_HW   - device doesn't support this command
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- device is in transport ready state
     */
    MR_DCMD_PD_GET_SMART_INFO               = 0x02020100,   // gets SMART attributes info

    /* Input:   dcmd.opcode         - MR_DCMD_PD_ERROR_STATS
     *          dcmd.mbox.b[0]      - Get/Reset the error counter values. 0 to Get error counters, 1 to Reset error counters
     *          dcmd.mbox.b[1]      - Device/Slot error counters. 1 = Device error counters. 2 = slot error counters
     *          dcmd.mbox.s[1]      - DeviceId of a physical device or SlotId within the given enclosure
     *                                0xFFFF to indicate all devices or all slots for the enclosure
     *          dcmd.mbox.s[2]      - DeviceId of enclosure. Applicable only when b[1] is set to slot
     *          dcmd.sge IN         - ptr to MR_PD_ERROR_STATS
     * Desc:    Get or Reset the cable or drive error counters
     * Status:  MFI_STAT_INVALID_PARAMETER  - Unsupported or incorrect parameters
     *          MFI_STAT_DEVICE_NOT_FOUND   - deviceId is invalid
     */
    MR_DCMD_PD_ERROR_STATS                 = 0x02020200,   // get/reset cable and drive error counters

    /* Input:   dcmd.opcode         - MR_DCMD_PD_SC_READ_NVRAM
     *          dcmd.mbox.s[0]      - deviceId for this physical drive
     *          dcmd.sge IN         - ptr to MR_SC_NVRAM_DATA structure
     * Desc:    Return the drive specific NVRAM data
     * Status:  MFI_STAT_DEVICE_NOT_FOUND    - deviceId is invalid
     *          MFI_STAT_I2C_ERRORS_DETECTED - I2C communication to the device failed
     *          MFI_STAT_UNSUPPORTED_HW      - device doesn't support this command
     */
    MR_DCMD_PD_SC_READ_NVRAM                = 0x02020300,   // get the SC NVRAM data for a PD

    /* Input:   dcmd.opcode         - MR_DCMD_PD_CLEAR_NVRAM
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Clear the device specific NVRAM data
     * Status:  MFI_STAT_DEVICE_NOT_FOUND    - deviceId is invalid
     *          MFI_STAT_I2C_ERRORS_DETECTED - I2C communication to the device failed
     *          MFI_STAT_UNSUPPORTED_HW      - device doesn't support this command
     */
    MR_DCMD_PD_SC_CLEAR_NVRAM               = 0x02020400,   // clear the SC NVRAM data for a PD

    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * Logical Drive commands
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_LD                             = 0x03000000,   // Logical Device (LD) opcodes

    /* Input:   dcmd.opcode         - MR_DCMD_LD_GET_LIST
     *          dcmd.mbox.b[0]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to returned MR_LD_LIST structure
     * Desc:    Return the logical drive list structure
     * Status:  No error
     */
    MR_DCMD_LD_GET_LIST                     = 0x03010000,   // read LD list (abbreviated LD info)

    /* Input:   dcmd.opcode         - MR_DCMD_LD_LIST_QUERY
     *          dcmd.mbox.b[0]      - query attribute (MR_LD_QUERY_TYPE)
     *          dcmd.mbox.b[1]      - query value
     *          dcmd.mbox.b[2]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to returned MR_LD_TARGETID_LIST structure
     * Desc:    Return a list of logical drives satisfying the supported query. The query matches
     *          device attribute to specified value.
     * Status:  MFI_STAT_INVALID_PARAMETER          - query not supported
     */
    MR_DCMD_LD_LIST_QUERY                   = 0x03010100,   // get LD targetId list

    /* Input:   dcmd.opcode         - MR_DCMD_LD_GUID_LIST_GET
     *          dcmd.mbox.b[0]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to returned MR_LD_GUID_LIST
     * Desc:    Return list of GUID for each LD
     * Status:  no errors
     */
    MR_DCMD_LD_GET_GUID_LIST                = 0x03010200,   // Get LD GUID list

    /* Input:   dcmd.opcode         - MR_DCMD_LD_GET_INFO
     *          dcmd.mbox.b[0]      - targetId for this logical drive (deprecated)
     *          dcmd.mbox.s[0]      - targetId for this logical drive (0 to MAX_API_TARGET_ID_EXT2)
     *          dcmd.sge IN         - ptr to returned MR_LD_INFO structure
     *                                If supportPointInTimeProgress is set, then MR_PROGRESS.elapsedSecsForLastPercent is returned
     *                                Otherwise MR_PROGRESS.elapsedSecs is returned
     * Desc:    Return the logical drive info structure
     * Status:  MFI_STAT_DEVICE_NOT_FOUND   - targetId is invalid
     */
    MR_DCMD_LD_GET_INFO                     = 0x03020000,   // read LD info (complete LD info)

    /* Input:   dcmd.opcode         - MR_DCMD_LD_GET_SPAN_LIST
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN/OUT     - ptr to MR_CONFIG_SPAN structure
     * Desc:    Return the span list info structure
     * Status:  MFI_STAT_INVALID_DRIVE_COUNT         - drive count is invalid
     */
    MR_DCMD_LD_GET_SPAN_LIST                = 0x03020200,

    /* Input:   dcmd.opcode         - MR_DCMD_LD_GET_PROPERTIES
     *          dcmd.mbox.b[0]      - targetId for this logical drive (deprecated)
     *          dcmd.mbox.s[0]      - targetId for this logical drive (0 to MAX_API_TARGET_ID_EXT2)
     *          dcmd.sge IN         - ptr to returned MR_LD_PROPERTIES structure
     * Desc:    Return the logical drive properties structure
     * Status:  MFI_STAT_DEVICE_NOT_FOUND   - targetId is invalid
     */
    MR_DCMD_LD_GET_PROPERTIES               = 0x03030000,   // read LD properties structures

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SET_PROPERTIES
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.sge OUT        - ptr to MR_LD_PROPERTIES structure
     * Desc:    Set the LD properties (more than one property set is also a valid set)
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_INVALID_PARAMETER          - One or more values of set parameters are invalid
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_PEER_NOTIFICATION_REJECTED - peer controller rejected request (possibly due to resource conflict)
     *          MFI_STAT_PEER_NOTIFICATION_FAILED   - unable to inform peer of communication changes (retry may be appropriate)
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     *          MFI_STAT_UNMAP_NOT_SUPPORTED        - Unmap operation is not supported on the LD
     */
    MR_DCMD_LD_SET_PROPERTIES               = 0x03040000,   // write LD properties structures

    /* Input:   dcmd.opcode         - MR_DCMD_LD_CLEAR_BLOCKED
     *          dcmd.mbox.w[0]      - MR_LD_REF or 0xFF (for targetId)  = for all the logical drives
     *          dcmd.mbox.b[4]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    and mbox.w[0] is 0xFF will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds
     * Desc:    Clear CurrentAccessPolicy on given LD or all LDs to its' DefaultAccessPolicy state
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_INVALID_PARAMETER          - One or more values of set parameters are invalid
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_PEER_NOTIFICATION_REJECTED - peer controller rejected request (possibly due to resource conflict)
     *          MFI_STAT_PEER_NOTIFICATION_FAILED   - unable to inform peer of communication changes (retry may be appropriate)
     */
    MR_DCMD_LD_CLEAR_BLOCKED                = 0x03040010,

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SET_HIDDEN
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - LD/DG level operation
     *                              - 0 - LD level operation
     *                              - 1 - DG level operation
     * Desc:    Hide the specified LD OR hide all the LDs within the same disk group. Hidden LD will not be visible to the OS.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_INVALID_PARAMETER          - One or more values of set parameters are invalid
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_LD_PARTIALLY_HIDDEN        - not all LDs of the disk group could be hidden (only applicable to DG level operation)
     *          MFI_STAT_OPERATION_NOT_POSSIBLE     - Operation is not possible on the LD (for the LD level operation)
     *                                              - Operation is not possible of any LD of the DG (for the DG level operation)
     *          MFI_STAT_LD_RECON_IN_PROGRESS       - CE/RLM in progress on the LD
     *          MFI_STAT_LD_BOOT_DEVICE             - Boot LD can't be hidden
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_SET_HIDDEN                   = 0x03040020,

    /* Input:   dcmd.opcode         - MR_DCMD_LD_CLEAR_HIDDEN
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - LD/DG level operation
     *                              - 0 - LD level operation
     *                              - 1 - DG level operation
     * Desc:    Un-hide the specified LD OR un-hide all the LDs within the same disk group. LD will be visible to the OS.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_INVALID_PARAMETER          - One or more values of set parameters are invalid
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_CLEAR_HIDDEN                 = 0x03040030,

    /* Input:   dcmd.opcode         - MR_DCMD_LD_ATOMIC_WRITE_GET_LDS
     *          dcmd.mbox.b[0]      - MR_ATOMIC_WRITE_TYPE or 0xFF for all types
     *          dcmd.sge IN         - ptr to returned MR_LD_TARGETID_LIST structure
     * Desc:    Get list of LDs with atomicity enabled
     * Status:  MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     */
    MR_DCMD_LD_ATOMIC_WRITE_GET_LDS         = 0x03040040,

    /* Input:   dcmd.opcode         - MR_DCMD_LD_ATOMIC_WRITE_SET_LDS
     *          dcmd.mbox.b[0]      - MR_ATOMIC_WRITE_TYPE
     *          dcmd.mbox.b[1]      - Update Type (0 = disable atomicity, 1 = enable atomicity)
     *          dcmd.sge OUT        - ptr to MR_LD_TARGETID_LIST structure
     * Desc:    Set list of LDs with atomicty enable/disable.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER        - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     *          MFI_STAT_INVALID_PARAMETER              - targetId list is not valid
     *          MFI_STAT_INVALID_CMD                    - AW command does not support by fw
     */

    MR_DCMD_LD_ATOMIC_WRITE_SET_LDS         = 0x03040050,

    /* Input:   dcmd.opcode         - MR_DCMD_LD_GET_SMART_INFO
     *          dcmd.mbox.b[0]      - targetId of LD (deprecated)
     *          dcmd.mbox.s[0]      - targetId for this logical drive (0 to MAX_API_TARGET_ID_EXT2)
     *          dcmd.sge IN         - ptr to returned MR_LD_SMART_INFO structure
     * Desc:    Get SMART LD INFO for specified targetId.
     *
     * Status:  MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     *          MFI_STAT_INVALID_CMD                    - command does not support by fw
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE    - LD is in transport ready state
     */

    MR_DCMD_LD_GET_SMART_INFO             = 0x03040060,

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SET_TRANSPORT_READY
     *          dcmd.mbox.w[0]      - MR_LD_REF (any LD ref from DG, applicable all VDs from DG)
     *          dcmd.mbox.b[4]      - Dedicated Hot Spare options (MR_TRANSPORT_READY_DHSP_OPTIONS)
     *                                Shared DHSP    - Dedicated hot spare drive shared by more than one array
     *                                Exclusive DHSP - Hot spare drive exclusive to an array
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_INVALID_PARAMETER          - One or more values of set parameters are invalid
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_OPERATION_NOT_POSSIBLE     - Operation is not possible on the LD
     *          MFI_STAT_LD_RECON_IN_PROGRESS       - CE/RLM in progress on the LD
     *          MFI_STAT_EXCEED_MAX_TRANSPORT_LD_COUNT - Indicates that the LD count > max transportable LD count
     *          MFI_STAT_DHSP_MULTIPLE_ASSOCIATION     - DHSP is associated with more than one disk group
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE   - LD is in transport ready state
     */
    MR_DCMD_LD_SET_TRANSPORT_READY       = 0x03040070,

    /* Input:   dcmd.opcode           - MR_DCMD_LD_CLEAR_TRANSPORT_READY
     *          dcmd.mbox.w[0]        - MR_LD_REF (any LD ref from DG, applicable to all VDs from DG)
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_INVALID_PARAMETER          - One or more values of set parameters are invalid
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_LD_NOT_IN_TRANSPORT_READY_STATE- LD is not in transport ready state
     */
    MR_DCMD_LD_CLEAR_TRANSPORT_READY     = 0x03040080,


    /* Input:   dcmd.opcode         - MR_DCMD_LD_CC_START
     *          dcmd.mbox.w[0]      - MR_LD_REF
     * Desc:    Start a check consistency (CC) on this logical drive. If already a CC/BGI is going
     *          on then also success will be returned.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_LD_WRONG_RAID_LEVEL        - if LD is a RAID 0
     *          MFI_STAT_SCSI_RESERVATION_CONFLICT  - LD is reserved by peer
     *          MFI_STAT_LD_NOT_OPTIMAL             - LD is not optimal
     *          MFI_STAT_LD_RECON_IN_PROGRESS       - CE/RLM is going on for this logical drive
     *          MFI_STAT_LD_INIT_IN_PROGRESS        - FGInit is going on for this LD
     *          MFI_STAT_LD_CC_IN_PROGRESS          - CC/BGI is already in progress
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_CC                           = 0x03050000,   // CC operations
    MR_DCMD_LD_CC_START                     = 0x03050100,   // start a CC

    /* Input:   dcmd.opcode         - MR_DCMD_LD_CC_ABORT
     *          dcmd.mbox.w[0]      - MR_LD_REF
     * Desc:    Abort a check consistency (or BGI) on this logical drive.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_SCSI_RESERVATION_CONFLICT  - LD is reserved by peer
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_CC_ABORT                     = 0x03050200,   // abort a CC

    /* Input:   dcmd.opcode         - MR_DCMD_LD_INIT_START
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - Fast(0) or Full(1) Init
     * Desc:    Start a Init on this logical drive.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_SCSI_RESERVATION_CONFLICT  - LD is reserved by peer
     *          MFI_STAT_LD_RECON_IN_PROGRESS       - CE/RLM is going on for this logical drive
     *          MFI_STAT_LD_INIT_IN_PROGRESS        - FGInit is going on for this LD
     *          MFI_STAT_LD_CC_IN_PROGRESS          - CC/BGI in progress
     *          MFI_STAT_LD_NOT_OPTIMAL             - LD is not optimal
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_INIT                         = 0x03060000,   // Init operations
    MR_DCMD_LD_INIT_START                   = 0x03060100,   // start a Init

    /* Input:   dcmd.opcode         - MR_DCMD_LD_INIT_ABORT
     *          dcmd.mbox.w[0]      - MR_LD_REF
     * Desc:    Abort a init on this logical drive..
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_SCSI_RESERVATION_CONFLICT  - LD is reserved by peer
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_INIT_ABORT                   = 0x03060200,   // abort a Init

    /* Input:   dcmd.opcode         - MR_DCMD_LD_RECONS_START
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds
     *                              - 2 - supports MAX_API_LOGICAL_DRIVES_EXT2 lds
     *          dcmd.sge OUT        - ptr to MR_RECON structure
     * Desc:    Perform a CE/RLM on this logical drive.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_LD_MAX_CONFIGURED          - maximum LD's exhausted
     *          MFI_STAT_LD_RECON_IN_PROGRESS       - CE/RLM is going on in the controller
     *          MFI_STAT_LD_RBLD_IN_PROGRESS        - Rebuild is going on for this LD
     *          MFI_STAT_LD_INIT_IN_PROGRESS        - FGInit is going on for this LD
     *          MFI_STAT_LD_CC_IN_PROGRESS          - CC/BGI is going on for this LD
     *          MFI_STAT_INVALID_PARAMETER          - Parameters are not valid
     *          MFI_STAT_OPERATION_NOT_POSSIBLE     - For cases where recon is not possible
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     *          MFI_STAT_WRONG_STATE                - PD is in wrong state
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_PD_NOT_ELIGIBLE_FOR_LD       device is not eligible for RAID configuration
     */
    MR_DCMD_LD_RECONS                       = 0x03070000,   // Reconstruct (RLM/OCE) commands on an ld
    MR_DCMD_LD_RECONS_START                 = 0x03070100,   // start a reconstruct (RLM/OCE) on the given LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_RECONS_GET_INFO
     *          dcmd.mbox.b[0]      - targetId for this logical drive (deprecated)
     *          dcmd.mbox.s[0]      - targetId for this logical drive (0 to MAX_API_TARGET_ID_EXT2)
     *          dcmd.sge IN         - ptr to returned MR_RECON structure
     * Desc:    Returns the MR_RECON structure
     * Status:  MFI_STAT_DEVICE_NOT_FOUND       - targetId is invalid
     *          MFI_STAT_WRONG_STATE            - CE/RLM is not going on in the controller
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_RECONS_GET_INFO              = 0x03070200,   // start a reconstruct (RLM/OCE) on the given LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_PROGRESS
     *          dcmd.mbox.b[0]      - targetId for this logical drive (deprecated)
     *          dcmd.mbox.s[0]      - targetId for this logical drive (0 to MAX_API_TARGET_ID_EXT2)
     *          dcmd.sge IN         - ptr to returned MR_LD_PROGRESS structure
     *                                If supportPointInTimeProgress is set, then MR_PROGRESS.elapsedSecsForLastPercent is returned
     *                                Otherwise MR_PROGRESS.elapsedSecs is returned
     * Desc:    Return the logical drive operation in progress structure
     * Status:  MFI_STAT_DEVICE_NOT_FOUND   - targetId is invalid
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_PROGRESS                     = 0x03080000,   // return current LD progress information

    /* Input:   dcmd.opcode         - MR_DCMD_LD_DELETE
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - force, 1=force LD delete, don't wait to flush cache etc.
     *          dcmd.mbox.b[5]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds
     *                              - 2 - supports MAX_API_LOGICAL_DRIVES_EXT2 lds
     *          dcmd.mbox.b[6]      - 1 - purge the cache before deleting the LD. Do not flush its dirty cache
     * Desc:    Delete this logical drive from the configuration.  If this logical drive is the last to reference
     *          an MR_ARRAY, then the array is automatically deleted.  The remaining MR_ARRAY references are
     *          adjusted to maintain sequential numbering of arrays.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER        - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     *          MFI_STAT_LD_INIT_IN_PROGRESS            - INIT is going on for this LD
     *          MFI_STAT_PEER_NOTIFICATION_REJECTED     - peer controller rejected request (possibly due to resource conflict)
     *          MFI_STAT_PEER_NOTIFICATION_FAILED       - unable to inform peer of communication changes (retry may be appropriate)
     *          MFI_STAT_INVALID_PARAMETER              - invalid option (force not allowed for SSCD with cache etc.)
     */
    MR_DCMD_LD_DELETE                       = 0x03090000,   // delete the given LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_LOCATE_START
     *          dcmd.mbox.b[0]      - targetId for this logical drive (deprecated)
     *          dcmd.mbox.s[0]      - targetId for this logical drive (0 to MAX_API_TARGET_ID_EXT2)
     *          dcmd.mbox.s[1]      - duraiton of identify (in seconds; 0 for indefinite)
     * Desc:    Flash the LED's of the physical drives that are in this LD(in an enclosure that supports LOCATE)
     * Status:  MFI_STAT_DEVICE_NOT_FOUND   - targetId is invalid
     *          MFI_STAT_NOT_IN_ENCL        - none of the drives are in an enclosure
     */
    MR_DCMD_LD_LOCATE                       = 0x030a0000,   // locate LD commands
    MR_DCMD_LD_LOCATE_START                 = 0x030a0100,   // locate the given LD (illuminate LEDs)

    /* Input:   dcmd.opcode         - MR_DCMD_LD_LOCATE_STOP
     *          dcmd.mbox.b[0]      - targetId for this logical drive (deprecated)
     *          dcmd.mbox.s[0]      - targetId for this logical drive (0 to MAX_API_TARGET_ID_EXT2)
     * Desc:    Undo the locate start for this LD
     * Status:  MFI_STAT_DEVICE_NOT_FOUND   - targetId is invalid
     *          MFI_STAT_NOT_IN_ENCL        - none of the drives are in an enclosure
     */
    MR_DCMD_LD_LOCATE_STOP                  = 0x030a0200,    // stop locate the given LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_GET_ALLOWED_OPS
     *          dcmd.mbox.b[0]      - targetId for this logical drive (deprecated)
     *          dcmd.mbox.s[0]      - targetId for this logical drive (0 to MAX_API_TARGET_ID_EXT2)
     *          dcmd.sge IN         - ptr to returned MR_LD_ALLOWED_OPS/MR_LD_ALLOWED_OPS_EXT structure
     * Desc:    Return the allowed configuration operations structure.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - deviceId is invalid
     */
    MR_DCMD_LD_GET_ALLOWED_OPS              = 0x030b0000,   // get the allowed operations for a particular LD.

    /* Input:   dcmd.opcode         - MR_DCMD_LD_GET_ALLOWED_OPS_ALL_LDS
     *          dcmd.mbox.b[0]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                                    if ld is present with target greater than MAX_API_TARGET_ID_LEGACY then
     *                                    allowed operations will be modified accordingly e.g.
     *                                    deleteLD will be 0
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - Variable size MR_LD_ALLOWED_OPS_LIST_EXT
     *          dcmd.sge IN         - ptr to returned MR_LD_ALLOWED_OPS_LIST/MR_LD_ALLOWED_OPS_LIST_EXT structure with MAX_API_LOGICAL_DRIVES_EXT
     * Desc:    Return the allowed configuration operations structure.
     * Status:  No error
     */
    MR_DCMD_LD_GET_ALLOWED_OPS_ALL_LDS      = 0x030b0100,   // get the allowed operations for all LDs.

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SECURE
     *          dcmd.mbox.w[0]      - MR_LD_REF
     * Desc:    Enable LD security, so it must be unlocked before access is allowed
     * Status:  MFI_STAT_LD_SECURE_NOT_ALLOWED              - LD cannot be secured, security feature is not enabled
     *          MFI_STAT_CONFIG_FDE_NON_FDE_MIX_NOT_ALLOWED - If VD consists of non-FDE drives
     *          MFI_STAT_LD_NOT_OPTIMAL                     - LD must be in optimal state to be secured
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE        - LD is in transport ready state
     */
    MR_DCMD_LD_SECURE                       = 0x030c0000,   // Secure an LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_GET_PINNED_LIST
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds and uses legacy U64 bitmap
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds and uses extended MR_LD_BITMAP
     *                              - 2 - supports MAX_API_LOGICAL_DRIVES_EXT2 lds and uses extended MR_LD_BITMAP_EXT
     *          dcmd.sge IN         - ptr to returned U64/MR_LD_BITMAP/MR_LD_BITMAP_EXT, a 64-bit/256-bit/8K-bit bitmap of
     *                                LD targetIds with pinned cache
     * Desc:    Return the logical drive targetId list with pinned cache.
     * Status:  No error
     */
    MR_DCMD_LD_GET_PINNED_LIST              = 0x030d0000,   // get LD list with pinned cache

    /* Input:   dcmd.opcode         - MR_DCMD_LD_DISCARD_PINNED
     *          dcmd.mbox.b[0]      - targetId for this logical drive
     *                              - use 0xFF to discard cache for all missing LDs with pinned cache.
     *          dcmd.mbox.b[1]      - discard all pinned cache on controller
     *                                (1=discard for all LDs, 0=discard only for missing LDs)
     *          dcmd.mbox.b[2]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    and mbox.b[0] is 0xFF will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds
     *                              - 2 - supports MAX_API_LOGICAL_DRIVES_EXT2 lds
     *          dcmd.mbox.b[3]      - targetIdMsb for this logical drive
     *                              - use 0xFF to discard cache for all missing LDs with pinned cache.
     * Desc:    Discard pinned controller cache for the specified LD targetIds. This DCMD should be
     *          used to discard cache for LDs that are missing (already deleted). For offline LDs use
     *          MR_DCMD_LD_DELETE.
     * Status:  MFI_STAT_INVALID_PARAMETER          - Parameters are not valid
     */
    MR_DCMD_LD_DISCARD_PINNED               = 0x030d0100,   // discard LD pinned cache


    /* Input:   dcmd.opcode         - MR_DCMD_LD_BBM_LOG_CLEAR
     *          dcmd.mbox.w[0]      - MR_LD_REF
     * Desc:    Clear the LD bad block table.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER        - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     *          MFI_STAT_INVALID_CMD                    - LD bad block management is not enabled
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE    - LD is in transport ready state
     */
    MR_DCMD_LD_BBM_LOG_CLEAR                = 0x030e0000,   // clear LD bad block table

    /* Input:   dcmd.opcode         - MR_DCMD_LD_BBM_LOG_GET
     *          dcmd.mbox.b[0]      - targetId for this logical drive (deprecated)
     *          dcmd.mbox.s[0]      - targetId for this logical drive (0 to MAX_API_TARGET_ID_EXT2)
     *          dcmd.sge IN         - ptr to returned MR_LD_BBM_LOG structure
     * Desc:    Return the LD bad block table.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND   - targetId is invalid
     *          MFI_STAT_INVALID_CMD        - LD bad block management is not enabled
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_BBM_LOG_GET                  = 0x030e0100,   // Get LD bad block table

    /* Input:   dcmd.opcode         - MR_LD_BBM_LOG_LIST_GET
     *          dcmd.mbox.b[0]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to returned MR_LD_BBM_LOG_LIST structure
     * Desc:    Return the LD bad block table for all LDs
     *          MFI_STAT_INVALID_CMD        - LD bad block management is not enabled
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_LD_BBM_LOG_LIST_GET                  = 0x030e0200,   // Get LD bad block list


    /*
     *---------------------------------------------------------------------------------------------------
     * SNAPSHOT commands
     */

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_ENABLE
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.sge OUT        - ptr to MR_LD_SNAPSHOT_ENABLE_PARAMS structure
     * Desc:    Enable Snapshot on this LD.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId's are invalid
     *          MFI_STAT_SNAP_NO_SPACE              - No space on the snapshot VD
     *          MFI_STAT_INVALID_PARAMETER          - invalid size/SnapVD targetId or other reasons
     */
    MR_DCMD_LD_SNAPSHOT                     = 0x030f0000,    // LD snapshot commands
    MR_DCMD_LD_SNAPSHOT_ENABLE              = 0x030f0100,    // Enable snapshot on this LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_DISABLE
     *          dcmd.mbox.w[0]      - MR_LD_REF
     * Desc:    Disable Snapshot on this LD.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     */
    MR_DCMD_LD_SNAPSHOT_DISABLE             = 0x030f0200,    // Disable snapshot on this LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_PIT_CREATE
     *          dcmd.mbox.b[0]      - numVDs, number of VDs in this set
     *          dcmd.mbox.b[1]      - createView, 1=Create VIEWs for each created PIT
     *          dcmd.sge IN/OUT     - ptr to array of numVDs of MR_LD_SNAPSHOT_PIT_VIEW_PARAMS
     * Desc:    Create Snapshot PIT and/or VIEW on a set of LDs
     *          FW shall update the the PARAMs with PITCreationTime and if VIEW is also
     *          created - viewCreationTime and viewTargetId.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_INVALID_PARAMETER          - snapshot is not enabled
     *          MFI_STAT_SNAPSHOT_NO_SPACE          - No space on the snapshot VD
     *          MFI_STAT_MEMORY_NOT_AVAILABLE       - Memory is unavailable to perform this operation
     */
    MR_DCMD_LD_SNAPSHOT_PIT_CREATE          = 0x030f0300, // Create snapshot PIT/VIEW on this set of LDs

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_PIT_DELETE
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - deleteAllPITs, 1=delete all PITs and VIEWs
     *          dcmd.sge OUT        - Ptr to MR_TIME_STAMP structure - PIT's creation time, for single PIT deletion
     *                                Required if not deleting all PITs and Views for the LD.
     * Desc:    Delete the Snapshot PITs/VIEWS on this LD
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_INVALID_PARAMETER          - snapshot is not enabled or time has invalid values
     *          MFI_STAT_MEMORY_NOT_AVAILABLE       - Memroy is unavailable to perform this operation
     */
    MR_DCMD_LD_SNAPSHOT_PIT_DELETE          = 0x030f0400, // Delete snapshot PIT/VIEW on this set of LDs

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_VIEW_CREATE
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.sge OUT        - Ptr to MR_LD_SNAPSHOT_PIT_VIEW_PARAMS structure.
     * Desc:    Create a VIEW on this PIT of the LD
     *          FW shall update the viewCreationTime and viewTargetId
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_SNAP_NO_SPACE              - No space on the snapshot VD
     *          MFI_STAT_INVALID_PARAMETER          - snapshot is not enabled or Invalid PIT timestamps
     *          MFI_STAT_MEMORY_NOT_AVAILABLE       - Memroy is unavailable to perform this operation
     */
    MR_DCMD_LD_SNAPSHOT_VIEW_CREATE         = 0x030f0500, // Create snapshot PIT VIEW on this LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_VIEW_DELETE
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.sge OUT        - Ptr to MR_TIME_STAMP structure with associated PIT's creation time.
     * Desc:    Delete a VIEW on this PIT of the LD
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_INVALID_PARAMETER          - snapshot is not enabled or time has invalid values
     *          MFI_STAT_MEMORY_NOT_AVAILABLE       - Memroy is unavailable to perform this operation
     */
    MR_DCMD_LD_SNAPSHOT_VIEW_DELETE         = 0x030f0600, // Delete a snapshot PIT VIEW on this LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_PIT_ROLLBACK
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - 0 - indicates rollback is performed from offline applications/tools
     *                                1 - indicates rollback is performed from online applications/tools
     *          dcmd.sge OUT        - Ptr to MR_TIME_STAMP structure - PIT's creation time
     * Desc:    Rollback the Snapshot PIT on this LD.
     *          This PIT and any newer PITs shall be removed upon completion
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_INVALID_PARAMETER          - snapshot is not enabled or Invalid PIT timestamps,
     *          MFI_STAT_MEMORY_NOT_AVAILABLE       - Memory is unavailable to perform this operation
     */
    MR_DCMD_LD_SNAPSHOT_PIT_ROLLBACK        = 0x030f0700, // Rollback snapshot PIT of this LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_GET_INFO
     *          dcmd.mbox.b[0]      - targetId for this logical drive, 0xFF = for all the logical drives
     *          dcmd.sge IN         - ptr to returned MR_LD_SNAPSHOT_ALL_INFO structure
     * Desc:    Return the Snapshot Information of the LDs
     * Status:  MFI_STAT_INVALID_PARAMETER          - Invalid Parameter
     */
    MR_DCMD_LD_SNAPSHOT_GET_INFO            = 0x030f0800,   // get the Snapshot information of this LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_VIEW_GET_INFO
     *          dcmd.mbox.b[0]      - targetId of the VIEW
     *          dcmd.sge IN         - ptr to returned MR_LD_VIEW_INFO structure
     * Desc:    Return the VIEW information
     * Status:  MFI_STAT_INVALID_PARAMETER          - Invalid Parameter
     */
    MR_DCMD_LD_SNAPSHOT_VIEW_GET_INFO       = 0x030f0900,   // get the VIEW Information of the VIEW targetId

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_VIEW_GET
     *          dcmd.sge IN         - ptr to returned MR_LD_VIEW_LIST structure
     * Desc:    Return the VIEW LIST for all the Snapshot LDs
     */
    MR_DCMD_LD_SNAPSHOT_GET_VIEW_LIST       = 0x030f0a00,   // get the VIEW list for all the LDs

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_REPOSITORY_GET_LDS
     *          dcmd.mbox.b[0]      - targetId of a snapshot respository
     *          dcmd.sge IN         - ptr to returned MR_LD_TARGETID_LIST structure
     * Desc:    Return the source VDs for this snapshot repository
     * Status:  MFI_STAT_INVALID_PARAMETER          - Invalid Parameter
     */
    MR_DCMD_LD_SNAPSHOT_REPOSITORY_GET_LDS  = 0x030f0b00,   // get all the LDs belonging to this repository

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_REPOSITORY_GET_SPACE
     *          dcmd.mbox.b[0]      - snapshot repository targetId
     *          dcmd.sge IN         - ptr to returned MR_LD_SNAPSHOT_REPOSITORY_SPACE structure
     * Desc:    Return information on all recoverable space in the snapshot respository
     * Status:  MFI_STAT_INVALID_PARAMETER          - Invalid Parameter
     */
    MR_DCMD_LD_SNAPSHOT_REPOSITORY_GET_SPACE = 0x030f0c00,   // get all the space left in the snapshot targetId

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_GARBAGE_COLLECTION
     *          dcmd.mbox.w[0]      - MR_LD_REF of the snapshot repository
     *          dcmd.mbox.b[4]      - source targetId for orphaned PITs and views
     *                                Use 0xFF for to recover space from all orphaned PITs and views
     *          dcmd.sge            - reserved
     * Desc:    Recover the space used by orphaned PITs and views, to be used by other snapshot operations.
     *          Recover space associated with specified source targetId, or all recoverable space.
     *          PITs and views are orphaned, if their source LD is missing or offline.
     * Status:  MFI_STAT_INVALID_PARAMETER           - Invalid Parameter
     */
    MR_DCMD_LD_SNAPSHOT_GARBAGE_COLLECTION = 0x030f0d00,   // Relieve garbage space to usable

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_SCHEDULE_GET_PROPERTIES
     *          dcmd.mbox[0]        - targetId
     *          dcmd.sge OUT        - Ptr to new MR_SNAPSHOT_SCHEDULE_PROPERTIES structure. Also returned with MR_DCMD_LD_SNAPSHOT_GET_INFO
     * Desc:    Set the Snapshot schedule properties
     * Status:  MFI_STAT_INVALID_PARAMETER  - Some input parameters are invalid
     */
    MR_DCMD_LD_SNAPSHOT_SCHEDULE_GET_PROPERTIES  = 0x030f0e00,

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SNAPSHOT_SCHEDULE_SET_PROPERTIES
     *          dcmd.mbox[0]        - MR_LD_REF of LD or 0xFF for all LDs
     *          dcmd.sge OUT        - Ptr to new MR_SNAPSHOT_SCHEDULE_PROPERTIES structure
     * Desc:    Set the Snapshot schedule properties
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER        - sequence number is out of sync. Applicable only when single LD's snapshot is scheduled
     *          MFI_STAT_INVALID_PARAMETER              - Some input parameters are invalid
     */
    MR_DCMD_LD_SNAPSHOT_SCHEDULE_SET_PROPERTIES  = 0x030f0f00,

    /*
     *---------------------------------------------------------------------------------------------------
     * CacheCade commands
     */

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SSCD_SET_LDS
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.sge OUT        - ptr to MR_LD_TARGETID_LIST structure
     * Desc:    Associate list of LDs to be cached on this SSCD.
     *          This DCMD is also used to update the associated list of LDs, i.e. add or remove associated LDs.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER        - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     *          MFI_STAT_WRONG_STATE                    - LD is not a super-size cache LD
     *          MFI_STAT_INVALID_PARAMETER              - targetId list is not valid
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE    - LD is in transport ready state
     */
    MR_DCMD_LD_SSCD                         = 0x03100000,  // LD SSCD commands
    MR_DCMD_LD_SSCD_SET_LDS                 = 0x03100100,  // Mark an LD as SSCD and assign LDs to it

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SSCD_GET_LDS
     *          dcmd.mbox.w[0]      - targetID of the created VDs or SSCDs
     *          dcmd.mbox.b[4]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to returned MR_LD_TARGETID_LIST structure
     * Desc:    Get list of LDs associated with SSC
     * Status:  MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     *          MFI_STAT_WRONG_STATE                    - LD is not a super-size cache LD
     */
    MR_DCMD_LD_SSCD_GET_LDS                 = 0x03100200,  // Get list of associated LDs

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SSCD_BOOST_PRIORITY_TYPE_GET
     *          dcmd.mbox.w[0]      - targetID of the LD or 0xFF for all LDs
     *          dcmd.mbox.b[4]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    and mbox.b[0] is 0xFF will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to returned MR_LD_BOOST_PRIORITY_TYPE_LIST structure
     * Desc:    Get boost priority type of VD associated with SSC
     * Status:  MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     *          MFI_STAT_INVALID_DCMD                   - if f/w does not support this DCMD
     *          MFI_STAT_INVALID_PARAMETER              - Parameters are not valid
     */
    MR_DCMD_LD_SSCD_BOOST_PRIORITY_TYPE_GET = 0x03100300,  // Get list of associated LDs and its boost priority type


    /* Input:   dcmd.opcode         - MR_DCMD_LD_SSCD_BOOST_PRIORITY_TYPE_SET
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.sge IN         - ptr to returned MR_LD_BOOST_PRIORITY_TYPE structure
     * Desc:    Set boost priority type of VD associated with SSC
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER        - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     *          MFI_STAT_WRONG_STATE                    - LD is not part of SSCD LD cache
     *          MFI_STAT_INVALID_PARAMETER              - targetId list is not valid
     *          MFI_STAT_INVALID_DCMD                   - if f/w does not support this DCMD
     */
    MR_DCMD_LD_SSCD_BOOST_PRIORITY_TYPE_SET = 0x03100400,  // Set list of associated LDs and its boost priority type

    /*
     *---------------------------------------------------------------------------------------------------
     * Break Mirror commands
     */

    /* Input:   dcmd.opcode         - MR_DCMD_LD_MIRROR_BREAK
     *          dcmd.mbox.w[0]      - MR_LD_REF
     * Desc:    Break the LD mirror
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_LD_WRONG_RAID_LEVEL        - LD raid level doesn't support break mirror
     *          MFI_STAT_LD_NOT_OPTIMAL             - LD is not optimal
     *          MFI_STAT_LD_RECON_IN_PROGRESS       - CE/RLM is going on for this logical drive
     *          MFI_STAT_LD_INIT_IN_PROGRESS        - FGInit is going on for this LD
     *          MFI_STAT_LD_CC_IN_PROGRESS          - CC/BGI is already in progress
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_MIRROR                       = 0x03110000,   // LD break mirror commands
    MR_DCMD_LD_MIRROR_BREAK                 = 0x03110100,   // break LD mirror

    /* Input:   dcmd.opcode         - MR_DCMD_LD_MIRROR_VIEW
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.sge IN         - ptr to returned MR_CONFIG_DATA structure
     * Desc:    View the LD and its available mirror. MR_CONFIG_DATA for LD is returned.
     *          The PDs in the broken part of the mirror are in MR_PD_STATE_UNCONFIGURED_GOOD.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_LD_WRONG_RAID_LEVEL        - LD raid level doesn't support break mirror
     *          MFI_STAT_WRONG_STATE                - LD mirror is not broken
     *          MFI_STAT_LD_OFFLINE                 - LD/array is offline
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_MIRROR_VIEW                  = 0x03110200,   // view the LD and its broken mirror

    /* Input:   dcmd.opcode         - MR_DCMD_LD_MIRROR_JOIN
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - MR_LD_MIRROR_OP
     * Desc:    Join the LD with its broken mirror.  By default the mirror is rebuilt on joining.
     *          See MR_LD_MIRROR_OP for possible operations.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_LD_WRONG_RAID_LEVEL        - LD raid level doesn't support break mirror
     *          MFI_STAT_WRONG_STATE                - LD mirror is not broken
     *          MFI_STAT_LD_OFFLINE                 - LD/array is offline
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_MIRROR_JOIN                  = 0x03110300,   // join the LD and its broken mirror


    /*
     *---------------------------------------------------------------------------------------------------
     * LD suspend/resume commands
     */

    /* Input:   dcmd.opcode         - MR_DCMD_LD_RECONS_SUSPEND_RESUME
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - 0 = suspend, 1 = resume
     * Desc:    Suspend/Resume a CE/RLM on this logical drive.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_INVALID_PARAMETER          - Parameters are not valid
     *          MFI_STAT_WRONG_STATE                - LD state is not correct for this operation
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_SUSPEND_RESUME               = 0x03120000,   // suspend/resume LD commands
    MR_DCMD_LD_RECONS_SUSPEND_RESUME        = 0x03120100,   // suspend/resume a reconstruct (RLM/OCE) on the given LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_CC_SUSPEND_RESUME
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - 0 = suspend, 1 = resume
     * Desc:    Suspend a check consistency (or BGI) on this logical drive.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_WRONG_STATE                - LD state is not correct for this operation
     *          MFI_STAT_SCSI_RESERVATION_CONFLICT  - LD is reserved by peer
     *          MFI_STAT_INVALID_PARAMETER          - Parameters are not valid
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_CC_SUSPEND_RESUME            = 0x03120200,   // suspend/resume a CC

    /*
     *---------------------------------------------------------------------------------------------------
     * LD secure erase commands
     */
    /* Input:   dcmd.opcode         - MR_DCMD_LD_SECURE_ERASE_START
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.mbox.b[4]      - This is a bit controlled setting
     *                              - bits 0..6 - erase type
     *                              - bit 7     - 0=MR_ERASE_TYPE, 1=MR_SS_ERASE_TYPE
     *          dcmd.mbox.b[5]      - Erase Pattern A
     *          dcmd.mbox.b[6]      - Erase Pattern B
     *          dcmd.mbox.b[7]      - Bit 0 (if 1) indicates if LD should be deleted after the erase operation
     * Desc:    Start a Erase on this logical drive, via MR_ERASE_TYPE or MR_SS_ERASE_TYPE.
     *          Erase Patterns are optional and based on erase type.
     *          mbox.b[7]:0 must be set for certain erase types, see erase types.
     *          Applications must check the LD's underlying PDs' MR_PD_INFO.properties.useSSEraseType
     *          to choose the appropriate erase type.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_SCSI_RESERVATION_CONFLICT  - LD is reserved by peer
     *          MFI_STAT_WRONG_STATE                - LD state is not correct for this operation
     *          MFI_STAT_INVALID_PARAMETER          - Parameters are not valid
     *          MFI_STAT_UNSUPPORTED_HW             - LD's constituent drives don't support this command
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_SECURE_ERASE                         = 0x03130000,   // LD erase operations
    MR_DCMD_LD_SECURE_ERASE_START                   = 0x03130100,   // start a Erase

    /* Input:   dcmd.opcode         - MR_DCMD_LD_SECURE_ERASE_ABORT
     *          dcmd.mbox.w[0]      - MR_LD_REF
     * Desc:    Abort a Erase on this logical drive.
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_SCSI_RESERVATION_CONFLICT  - LD is reserved by peer
     *          MFI_STAT_WRONG_STATE                - LD state is not correct for this operation
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_LD_SECURE_ERASE_ABORT                   = 0x03130200,   // Abort a Erase

    /*
     *---------------------------------------------------------------------------------------------------
     * LD PI commands
     */

    /* Input:   dcmd.opcode         - MR_DCMD_LD_PI_DISABLE
     *          dcmd.mbox.w[0]      - MR_LD_REF
     * Desc:    Disable the PI for this LD and all other LDs in the disk groups associated to this LD.
     *          The user data is available but PI data is irretrievably lost.
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER        - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     *          MFI_STAT_INVALID_CMD                    - LD is not PI enabled
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE    - LD is in transport ready state
     */
    MR_DCMD_LD_PI                                   = 0x03140000,   // LD PI commands
    MR_DCMD_LD_PI_DISABLE                           = 0x03140100,   // disable PI on the LDs

    /*
     *---------------------------------------------------------------------------------------------------
     * Multinode Configuration commands (e.g SR-IOV, Multinode HA)
     * In SR-IOV only PF can manage and in Multinode HA any node can manage.
     * Solution involving both IOV and HA, IOV takes precedence.
     * MR_DCMD_CTRL_SHUTDOWN.dcmd.mbox.b[2]      - Private use Only: bit0 : if 1 then chassis shutdown.
     *                                             Other bits not yet defined
     */

    /* Input:   dcmd.opcode         - MR_DCMD_LD_VF_MAP_GET
     *          dcmd.mbox.b[0]      - targetId for this logical drive (deprecated)
     *          dcmd.mbox.s[0]      - targetId for this logical drive (0 to MAX_API_TARGET_ID_EXT2)
     *          dcmd.sge OUT        - ptr to returned MR_LD_VF_MAP
     * Desc:    Get node access policy map associated with given LD
     *             For SR-IOV, PF has full access
     * Status:  MFI_STAT_DEVICE_NOT_FOUND               - targetId is invalid
     */
    MR_DCMD_LD_VF_MAP                        = 0x03150000,  // LD node MAP Commands
    MR_DCMD_LD_VF_MAP_GET                    = 0x03150100,  // get node access policy map for LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_VF_MAP_GET_ALL_LDS
     *          dcmd.mbox.b[0]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge OUT        - ptr to returned MR_LD_VF_AFFILIATION
     * Desc:    Get node access policy map for all LDs
     *             For SR-IOV, PF has full access
     * Status:
     */
    MR_DCMD_LD_VF_MAP_GET_ALL_LDS            = 0x03150200,  // get node access policy map for all LDs

    /* Input:   dcmd.opcode         - MR_DCMD_LD_VF_MAP_SET
     *          dcmd.mbox.w[0]      - MR_LD_REF
     *          dcmd.sge IN         - ptr to MR_LD_VF_MAP structure
     * Desc:    Set node access policy map for given LD.
     *             For SR-IOV, PF has full access
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - targetId is invalid
     *          MFI_STAT_INVALID_SEQUENCE_NUMBER    - incorrect sequence number
     *          MFI_STAT_INVALID_CMD                - returned when invoked from anywhere outside of managment node
     */
    MR_DCMD_LD_VF_MAP_SET                    = 0x03150300,  // Set node access policy map for LD

    /* Input:   dcmd.opcode         - MR_DCMD_LD_VF_MAP_SET_MULTIPLE
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds
     *                              - 2 - supports MAX_API_LOGICAL_DRIVES_EXT2 lds
     *          dcmd.sge IN         - ptr to MR_LD_VF_AFFILIATION structure
     * Desc:    Set node access policy map for one or more VDs. If you want to change the existing VD affilation
     *          policies of one or more, do the MR_DCMD_LD_VF_MAP_GET_ALL_LDS, update only those VD related information
     *          and use that to issue this call
     *             For SR-IOV, PF has full access
     * Status:  MFI_STAT_DEVICE_NOT_FOUND           - one or more targetId(s) is invalid
     *          MFI_STAT_INVALID_SEQUENCE_NUMBER    - incorrect sequence number
     *          MFI_STAT_INVALID_CMD                - returned when invoked from anywhere outside of managment node
     *          MFI_STAT_INVALID_PARAMETER          - if greater than MAX_API_TARGET_ID_LEGACY ld is present and mbox.b[0] set to 0
     */
    MR_DCMD_LD_VF_MAP_SET_MULTIPLE            = 0x03150400,  // Set node access policy map one or more LDs

    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * Configuration commands
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_CFG                             = 0x04000000,   // Configuration commands

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_READ
     *          dcmd.mbox.b[0]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW
     *                                    any ld with target id greater than MAX_API_TARGET_ID_LEGACY will not be returned
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to returned MR_CONFIG_DATA structure
     * Desc:    Return the existing RAID configuration
     * Status:  MFI_STAT_MEMORY_NOT_AVAILABLE           - FW can't allocate sufficient memory - try again later
     */
    MR_DCMD_CFG_READ                        = 0x04010000,   // read current configuration

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_ADD
     *          dcmd.mbox.b[0]      - 0 - Legacy apps capable of supporting MAX_API_LOGICAL_DRIVES_LEGACY lds
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - New apps capable of supporting MAX_API_LOGICAL_DRIVES_EXT lds
     *                              - 2 - New apps capable of supporting MAX_API_LOGICAL_DRIVES_EXT2 lds
     *          dcmd.sge OUT        - ptr to MR_CONFIG_DATA structure
     * Desc:    Add these set of logical drive(s) and spares to the existing configuration.  If an LD is being created
     *          that uses an existing array (MR_ARRAY), then the arrayRef for that array MUST match the arrayRef used
     *          by firmware.  Newly created arrays MUST use a unique arrayRef from current firmware arrayRef values.
     *          If a conflict is detected with the array definitions, then the command will be rejected.  The MF_CONFIG_DATA
     *          structure must contain all arrays referenced within the configuration.
     * Status:  MFI_STAT_INVALID_PARAMETER              - one or more parameters are invalid
     *          MFI_STAT_INVALID_SEQUENCE_NUMBER        - PD sequence number is invalid
     *          MFI_STAT_CONFIG_RESOURCE_CONFLICT       - some of the config resources conflict
     *          MFI_STAT_LD_WRONG_RAID_LEVEL            - the raid level is invalid
     *          MFI_STAT_LD_MAX_CONFIGURED              - already max LD's reached (or max per array reached)
     *          MFI_STAT_ARRAY_INDEX_INVALID            - array index invalid
     *          MFI_STAT_ROW_INDEX_INVALID              - row index invalid
     *          MFI_STAT_PD_TYPE_WRONG                  - replacing with a wrong drive type (like SAS with SATA)
     *          MFI_STAT_LD_RECON_IN_PROGRESS           - CE/RLM is going on in the controller
     *          MFI_STAT_PD_CLEAR_IN_PROGRESS           - PD clear operation in progress on one of the drives
     *          MFI_STAT_MEMORY_NOT_AVAILABLE           - FW can't allocate sufficient memory - try again later
     *          MFI_STAT_WRONG_STATE                    - PD is in wrong state (must be UNCONFIGURED_GOOD for newly created arrays)
     *          MFI_STAT_PD_READY_FOR_REMOVAL           - PD is in removal ready state
     *          MFI_STAT_DEVICE_NOT_FOUND               - PD in newly created array is missing
     *          MFI_STAT_DRIVE_TOO_SMALL                - PD too small for requested array size
     *          MFI_STAT_PEER_NOTIFICATION_REJECTED     - peer controller rejected request (possibly due to resource conflict)
     *          MFI_STAT_PEER_NOTIFICATION_FAILED       - unable to inform peer of communication changes (retry may be appropriate)
     *          MFI_STAT_PD_NOT_ELIGIBLE_FOR_LD       device is not eligible for RAID configuration
     */
    MR_DCMD_CFG_ADD                         = 0x04020000,   // create the given (set of) LDs

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_CLEAR
     *          dcmd.mbox.b[0]      - force, 1=force config clear, don't flush cache etc.
     *          dcmd.mbox.b[1]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds
     *                              - 2 - supports MAX_API_LOGICAL_DRIVES_EXT2 lds
     * Desc:    Clear the RAID configuration. Firmware will abort the background operations and
     *          accept the clear configuration. There is a difference in behavior between deleting
     *          last LD and clear configuration.
     * Status:  MFI_STAT_PEER_NOTIFICATION_REJECTED - peer controller rejected request (possibly due to resource conflict)
     *          MFI_STAT_PEER_NOTIFICATION_FAILED   - unable to inform peer of communication changes (retry may be appropriate)
     *          MFI_STAT_LD_SSCD_CACHE_PRESENT      - secondary cache is present
     *          MFI_STAT_INVALID_PARAMETER          - ld with target id greater than MAX_API_TARGET_ID_LEGACY is present and mbox.b[1] set to 0
     *          MFI_STAT_TRANSPORT_READY_LD_EXISTS  - configuration has some LD in transport ready state
     */
    MR_DCMD_CFG_CLEAR                       = 0x04030000,   // clear the current configuration

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_MAKE_SPARE
     *          dcmd.mbox.b[0]      - testOnly, 1=Only test the validity of the PD to be used as a spare, 0=make Spare
     *          dcmd.sge OUT        - ptr to MR_SPARE structure
     * Desc:    Assign a hot spare
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER        - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND               - PD id is invalid
     *          MFI_STAT_INVALID_PARAMETER              - spare type not supported
     *          MFI_STAT_CONFIG_RESOURCE_CONFLICT       - PD is part of the config
     *          MFI_STAT_MAX_SPARES_EXCEEDED            - global hot spares exceeded MAX_SPARES_FOR_THE_CONTROLLER
     *          MFI_STAT_PD_CLEAR_IN_PROGRESS           - PD clear operation is in progress on one of the spare PDs
     *          MFI_STAT_PEER_NOTIFICATION_REJECTED     - peer controller rejected request (possibly due to resource conflict)
     *          MFI_STAT_PEER_NOTIFICATION_FAILED       - unable to inform peer of communication changes (retry may be appropriate)
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE    - LD is in transport ready state
     *          MFI_STAT_WRONG_STATE                    - PD is in wrong state
     *          MFI_STAT_PD_READY_FOR_REMOVAL           - PD is in removal ready state
     *          MFI_STAT_PD_NOT_ELIGIBLE_FOR_LD       device is not eligible for RAID configuration
     */
    MR_DCMD_CFG_MAKE_SPARE                  = 0x04040000,   // make a single PD a HS (hot spare)

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_REMOVE_SPARE
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    Remove a hot spare
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - the PD id is invalid
     *          MFI_STAT_WRONG_STATE                - PD not a hot spare
     *          MFI_STAT_PEER_NOTIFICATION_REJECTED - peer controller rejected request (possibly due to resource conflict)
     *          MFI_STAT_PEER_NOTIFICATION_FAILED   - unable to inform peer of communication changes (retry may be appropriate)
     *          MFI_STAT_PD_IN_TRANSPORT_READY_STATE- configuration has some LD in transport ready state
     */
    MR_DCMD_CFG_REMOVE_SPARE                = 0x04050000,   // remove a single PD from being a HS

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_RESIZE_ARRAY
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - ptr to a single MR_ARRAY structure containing desired new size
     * Desc:    Change the size of the given array.  The supplied array must match FW's data for this array.
     *          In other words, the drive counts and states must match FW's current state.  The new drive
     *          size must be large enough to handle any configured LDs.  It must also be smaller than the
     *          smallest member disk.
     * Status:  MFI_STAT_INVALID_PARAMETER          - one or more parameters are invalid
     *          MFI_STAT_DRIVE_TOO_SMALL            - PD too small for requested array size
     *          MFI_STAT_ARRAY_INDEX_INVALID        - array index invalid
     *          MFI_STAT_INVALID_SEQUENCE_NUMBER    - Supplied array configuration doesn't match FW's state
     *          MFI_STAT_PEER_NOTIFICATION_REJECTED - peer controller rejected request (possibly due to resource conflict)
     *          MFI_STAT_PEER_NOTIFICATION_FAILED   - unable to inform peer of communication changes (retry may be appropriate)
     *          MFI_STAT_LD_IN_TRANSPORT_READY_STATE- LD is in transport ready state
     */
    MR_DCMD_CFG_RESIZE_ARRAY                = 0x04060000,   // create the given (set of) LDs

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_GET_ALLOWED_OPS
     *          dcmd.mbox.b[0]      - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds
     *                              - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds
     *                              - 2 - supports MAX_API_LOGICAL_DRIVES_EXT2 lds
     *          dcmd.sge IN         - ptr to returned MR_CONFIG_ALLOWED_OPS structure
     * Desc:    Return the allowed configuration operations structure.
     * Status:  No error
     */
    MR_DCMD_CFG_GET_ALLOWED_OPS             = 0x04070000,   // get allowed configuration operations

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_AUTOCFG
     *          dcmd.mbox.w[0]      - 0 : get,
     *                                1 : set,
     *                                2 : execute once. With the execute once option FW will configure
     *                                    unconfigured-pds once and will not configure any newly inserted UG drives.
     *                                3 : supported fields under that behavior mode
     *                                         0 for each field or bit value indicates that field is not supported
     *                                         1 for each field or bit value indicates that field is supported.
     *          dcmd.mbox.b[4]      - MR_AUTO_CFG_OPTIONS, 0 for current behavior mode. Only applicable for get(0) and supported(3)
     *          dcmd.sge IN         - ptr to returned  MR_AUTO_CFG_PARAMETER structure
     *          dcmd.sge OUT        - ptr to set  MR_AUTO_CFG_PARAMETER structure
     * Desc:    Return or set auto configuration parameter structure.
     * Status:  MFI_STAT_INVALID_DCMD            - if f/w does not support this DCMD
     *          MFI_STAT_INVALID_PARAMETER       - one or more parameters are invalid
     *          MFI_STAT_LD_CACHE_PINNED         - controller has pinned cache
     *          MFI_STAT_OK_REBOOT_REQUIRED      - Ok but reboot required
     */
    MR_DCMD_CFG_AUTOCFG                     = 0x04070100,   // get/set auto configuration

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_MAKE_EPD
     *          dcmd.mbox.w[0]      - MR_PD_REF
     * Desc:    configure an Enhanced PD
     * Status:  MFI_STAT_INVALID_SEQUENCE_NUMBER    - sequence number is out of sync
     *          MFI_STAT_DEVICE_NOT_FOUND           - the PD id is invalid
     *          MFI_STAT_PD_READY_FOR_REMOVAL       - PD is in removal ready state
     *          MFI_STAT_WRONG_STATE                - PD cannot be configured
     *          MFI_STAT_PEER_NOTIFICATION_REJECTED - peer controller rejected request (possibly due to resource conflict)
     *          MFI_STAT_PEER_NOTIFICATION_FAILED   - unable to inform peer of communication changes (retry may be appropriate)
     */
    MR_DCMD_CFG_MAKE_EPD                    = 0x04070200,   // Configure a PD as an Enhanced PD


    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * Foreign DDF Configuration commands
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_CFG_FOREIGN                     = 0x04060000,   // foreign configuration management

    /* Input:   dcmd.opcode         - MR_DCMD_SCAN_FOR_FOREIGN_CONFIGS
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to returned MR_FOREIGN_CFG_GUIDS structure
     * Desc:    Return the foreign configuration DDF header GUID's
     * Status:  No error
     */
    MR_DCMD_SCAN_FOR_FOREIGN_CONFIGS        = 0x04060100,    // scan for foreign GUID's

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_FOREIGN_READ
     *          dcmd.mbox.b[0]      - Index of the DDF header GUID returned in MR_DCMD_SCAN_FOR_FOREIGN_CONFIGS call
     *          dcmd.mbox.b[1]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW.
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to returned MR_CONFIG_DATA structure
     * Desc:    Return the foreign configuration related the DDF header GUID
     * Status:  MFI_STAT_INVALID_PARAMETER - invalid DDF header GUID index etc.
     *          MFI_STAT_FOREIGN_CONFIG_INCOMPLETE - foreign configuration is incomplete
     */
    MR_DCMD_CFG_FOREIGN_READ                = 0x04060200,   // read a foreign configuration

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_FOREIGN_READ_AFTER_IMPORT
     *          dcmd.mbox.b[0]      - Index of the DDF header GUID returned in MR_DCMD_SCAN_FOR_FOREIGN_CONFIGS call
     *                                Or 0xFF to preview configuration as if all foreign GUIDs are imported.
     *          dcmd.mbox.b[1]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW.
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge IN         - ptr to returned MR_CONFIG_DATA structure
     * Desc:    Return the foreign configuration that would be after import is done for this GUID
     *          DCMD MR_DCMD_SCAN_FOR_FOREIGN_CONFIGS should be issued prior to this DCMD.
     * Status:  MFI_STAT_INVALID_PARAMETER - invalid DDF header GUID index etc.
     *          MFI_STAT_FOREIGN_CONFIG_INCOMPLETE - foreign configuration is incomplete
     */
    MR_DCMD_CFG_FOREIGN_READ_AFTER_IMPORT   = 0x04060300,   // read a foreign configuration as it would be imported

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_FOREIGN_IMPORT
     *          dcmd.mbox.b[0]      - Index of the DDF header GUID returned in MR_DCMD_SCAN_FOR_FOREIGN_CONFIGS call
     *                                Or 0xFF to import all foreign GUIDs.
     *          dcmd.mbox.b[1]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW.
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     * Desc:    Add this foreign configuration to the existing native configuration
     *          DCMD MR_DCMD_SCAN_FOR_FOREIGN_CONFIGS should be issued prior to this DCMD.
     * Status:  MFI_STAT_INVALID_PARAMETER      - invalid DDF header GUID index etc.
     *          MFI_STAT_LD_RECON_IN_PROGRESS   - CE/RLM is going on in the controller
     *          MFI_STAT_FOREIGN_CONFIG_INCOMPLETE - foreign configuration is incomplete
     *          MFI_STAT_LD_MAX_CONFIGURED         - maximum LDs are already configured
     *          MFI_STAT_PD_NOT_ELIGIBLE_FOR_LD       device is not eligible for RAID configuration
     */
    MR_DCMD_CFG_FOREIGN_IMPORT              = 0x04060400,   // import a foreign configuration and merge with current configuration

    /* Input:   dcmd.opcode         - MR_DCMD_CFG_FOREIGN_CLEAR
     *          dcmd.mbox.b[0]      - Index of the DDF header GUID returned in MR_DCMD_SCAN_FOR_FOREIGN_CONFIGS call
     *                                Or 0xFF to clear all foreign GUIDs.
     *          dcmd.mbox.b[1]      - 0 - MAX_API_LOGICAL_DRIVES_LEGACY lds will be returned by FW.
     *                                    If any ld with target id greater than MAX_API_TARGET_ID_LEGACY is present
     *                                    will return MFI_STAT_INVALID_PARAMETER
     *                              - 1 - MAX_API_LOGICAL_DRIVES_EXT lds will be returned by FW
     *                              - 2 - MAX_API_LOGICAL_DRIVES_EXT2 lds will be returned by FW
     *          dcmd.sge            - reserved
     * Desc:    Clear this foreign configuration
     *          DCMD MR_DCMD_SCAN_FOR_FOREIGN_CONFIGS should be issued prior to this DCMD.
     * Status:  MFI_STAT_INVALID_PARAMETER  - Invalid DDF header GUID Index etc.
     */
    MR_DCMD_CFG_FOREIGN_CLEAR               = 0x04060500,   // clear the config from a set of foreign drives


    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * BBU Management commands
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_BBU                             = 0x05000000,   // BBU commands

    /* Input:   dcmd.opcode         - MR_DCMD_BBU_GET_STATUS
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to returned MR_BBU_STATUS structure
     * Desc:    Return the current status information for the BBU
     * Status:  MFI_STAT_NO_HW_PRESENT  - No BBU HW present
     */
    MR_DCMD_BBU_GET_STATUS                  = 0x05010000,   // read BBU design/status information

    /* Input:   dcmd.opcode         - MR_DCMD_BBU_GET_CAPACITY_INFO
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to returned MR_BBU_CAPACITY_INFO structure
     * Desc:    Return the capacity of the BBU
     * Status:  MFI_STAT_NO_HW_PRESENT  - No BBU HW present
     *          MFI_STAT_UNSUPPORTED_HW - pack doesn't support this command
     */
    MR_DCMD_BBU_GET_CAPACITY_INFO           = 0x05020000,   // read BBU capacity

    /* Input:   dcmd.opcode         - MR_DCMD_BBU_GET_DESIGN_INFO
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to returned MR_BBU_DESIGN_INFO structure
     * Desc:    Return the design parameters for the BBU.
     * Status:  MFI_STAT_NO_HW_PRESENT  - No BBU HW present
     */
    MR_DCMD_BBU_GET_DESIGN_INFO             = 0x05030000,   // read BBU capacity

    /* Input:   dcmd.opcode         - MR_DCMD_BBU_LEARN
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Start a Learning cycle on the battery.
     * Status:  MFI_STAT_NO_HW_PRESENT  - No BBU HW present
     *          MFI_STAT_WRONG_STATE    - BBU is not in a state to start a learn cycle.
     */
    MR_DCMD_BBU_LEARN                       = 0x05040000,   // start a Learning cycle

    /* Input:   dcmd.opcode         - MR_DCMD_BBU_PROPERTIES_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to returned MR_BBU_PROPERTIES structure
     * Desc:    Return the current BBU properties structure.
     * Status:  MFI_STAT_NO_HW_PRESENT  - No BBU HW Present
     */
    MR_DCMD_BBU_PROPERTIES_GET              = 0x05050100,   // get current BBU properties

    /* Input:   dcmd.opcode         - MR_DCMD_BBU_PROPERTIES_SET
     *          dcmd.mbox           - reserved
     *          dcmd.sge OUT        - ptr to new MR_BBU_PROPERTIES structure
     * Desc:    Update the BBU properties structure with the provided structure.
     * Status:  MFI_STAT_NO_HW_PRESENT  - No BBU HW Present
     *          MFI_STAT_INVALID_PARAMETER  - One or more of the parameters are invalid
     */
    MR_DCMD_BBU_PROPERTIES_SET              = 0x05050200,   // set current BBU properties

    /* Input:   dcmd.opcode         - MR_DCMD_BBU_MFG_SLEEP
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Place the battery into low-power storage mode.
     *          To exit this state issue DCMD MR_DCMD_BBU_GET_CAPACITY_INFO or
     *          MR_DCMD_BBU_GET_DESIGN_INFO.
     * Status:  MFI_STAT_NO_HW_PRESENT  - No BBU HW Present
     */
    MR_DCMD_BBU_MFG_SLEEP                   = 0x05060100,   // enable sleep mode

    /* Input:   dcmd.opcode         - MR_DCMD_BBU_MFG_SEAL
     *          dcmd.mbox           - reserved
     *          dcmd.sge            - reserved
     * Desc:    Seal the gas guage EEPROM write access.  This opcode is should only be used at
     *          manufacturing time.
     * Status:  MFI_STAT_NO_HW_PRESENT  - No BBU HW Present
     */
    MR_DCMD_BBU_MFG_SEAL                    = 0x05060200,   // seal the gas guage eeprom write access

    /* Input:   dcmd.opcode         - MR_DCMD_BBU_MODES_GET
     *          dcmd.mbox           - reserved
     *          dcmd.sge IN         - ptr to returned MR_BBU_MODE_LIST structure
     * Desc:    Return the current BBU modes structure.
     * Status:  MFI_STAT_NO_HW_PRESENT  - No BBU HW Present
     */
    MR_DCMD_BBU_MODES_GET                   = 0x05070100,   // get supported BBU Modes

    /* Input:   dcmd.opcode         - MR_DCMD_BBU_VPD_GET
     *          dcmd.mbox.b[0]      - MR_BBU_VPD_TYPE
     *          dcmd.mbox.b[1]      - MR_BBU_VPD_SUBTYPE
     *          dcmd.mbox.b[2]      0 - Apps will swap the values of relevant fields in struct MR_BBU_VPD_INFO_FIXED
     *                              1 - FW will swap the values of relevant fields in struct MR_BBU_VPD_INFO_FIXED
     *          dcmd.sge IN         - ptr to MR_BBU_VPD_INFO
     * Desc:    Get Battery VPD data
     * Status:  MFI_STAT_NO_HW_PRESENT       - HW device not Present
     *          MFI_STAT_UNSUPPORTED_HW      - Command not supported for device
     *          MFI_STAT_INVALID_PARAMETER   - One or more of the parameters are invalid
     */
    MR_DCMD_BBU_VPD_GET                     = 0x05070200,

   /* Input:   dcmd.opcode          - MR_DCMD_BBU_SBAT_INFO_GET
    *           dcmd.mbox           - reserved
    *           dcmd.sge IN         - ptr to returned MR_SBAT_INFO structure
    * Desc:     Return the properties of Smart Battery
    * Status:   MFI_STAT_UNSUPPORTED_HW  - Command not supported for device
    */
    MR_DCMD_BBU_SBAT_INFO_GET       = 0x05080100,

   /* Input:   dcmd.opcode          - MR_DCMD_BBU_SBAT_INFO_SET
    *           dcmd.mbox           - reserved
    *           dcmd.sge OUT        - ptr to supplied MR_SBAT_INFO structure
    * Desc:     Set the properties of Smart Battery
    * Status:   MFI_STAT_UNSUPPORTED_HW  - Command not supported for device
    */
    MR_DCMD_BBU_SBAT_INFO_SET       = 0x05080200,

    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * FW-Resident Application Control
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_APP                             = 0x06000000,

    /* Input:   dcmd.opcode         - MR_DCMD_APP_OPEN
     *          dcmd.mbox.w[0]      - Application name to open (4-character ROM file name)
     *          dcmd.sge IN         - Ptr to returned MR_DCMD_APP_INFO structure
     * Desc:    Open the FW-application specified in mbox.w[0].  FW will fill-in the INFO structure to inform the host application
     *          of the FW-app handle, and the number of input/output buffers available for application use.  After opening
     *          a FW-app, the host should complete its use then "close" the application.  The host application should issue
     *          FW-app requests within the returned "timeout" number of seconds.  If no command is issued within that timeout,
     *          AND another host application requests to open a FW-app, the first FW-app will be automatically closed.  Only one
     *          FW-app may be opened at one point in time.
     * Status:  MFI_STAT_NOT_FOUND          - no application found
     *          MFI_STAT_APP_IN_USE         - application already in use - try again in a few seconds
     */
    MR_DCMD_APP_OPEN                        = 0x06010000,   // open FW application

    /* Input:   dcmd.opcode         - MR_DCMD_APP_SEND_INPUT
     *          dcmd.mbox.w[0]      - appHandle returned from OPEN call
     *          dcmd.mbox.w[1]      - Buffer index to send data to (0=stdin)
     *          dcmd.hdr.length     - Number of bytes to send (must be <= bufferSize returned at OPEN)
     *          dcmd.sge OUT        - Ptr to data to be placed in FW-app input buffer
     * Desc:    Provide input data to the FW-app.  The actual number of bytes copied to the FW-app buffer will be returned in
     *          the hdr.length field of the frame.
     * Status:  MFI_STAT_INVALID_PARAMETER  - invalid buffer number
     *          MFI_STAT_APP_IN_USE         - bad appHandle - app already in use
     *          MFI_STAT_APP_NOT_INITIALIZED- application not initialized
     */
    MR_DCMD_APP_SEND_INPUT                  = 0x06020000,   // send input data to FW-app

    /* Input:   dcmd.opcode         - MR_DCMD_APP_GET_OUTPUT
     *          dcmd.mbox.w[0]      - appHandle returned from OPEN call
     *          dcmd.mbox.w[1]      - Buffer index to get data from (1=stdout)
     *          dcmd.hdr.length     - Number of bytes to get (must be <= bufferSize returned at OPEN)
     *          dcmd.sge IN         - Ptr to data to be retrieved from FW-app output buffer
     * Desc:    Retrieve output data from the FW-app.  The actual number of bytes copied from the FW-app buffer will be returned in
     *          the hdr.length field of the frame.
     * Status:  MFI_STAT_INVALID_PARAMETER  - invalid buffer number
     *          MFI_STAT_APP_IN_USE         - bad appHandle - app already in use
     *          MFI_STAT_APP_NOT_INITIALIZED- application not initialized
     */
    MR_DCMD_APP_GET_OUTPUT                  = 0x06030000,   // get output data from FW-app

    /* Input:   dcmd.opcode         - MR_DCMD_APP_EXEC
     *          dcmd.mbox.w[0]      - appHandle returned from OPEN call
     *          dcmd.sge IN         - Ptr to returned exit results structure (MR_DCMD_APP_EXEC_RESULTS)
     * Desc:    Execute FW-app until it stops.  The FW-app may stop because more input data is need, the output buffer
     *          is full, or the application has completed its task.
     * Status:  MFI_STAT_INVALID_PARAMETER  - invalid buffer number
     *          MFI_STAT_APP_IN_USE         - bad appHandle - app already in use
     *          MFI_STAT_APP_NOT_INITIALIZED- application not initialized
     */
    MR_DCMD_APP_EXEC                        = 0x06040000,   // get output data from FW-app

    /* Input:   dcmd.opcode         - MR_DCMD_APP_CLOSE
     *          dcmd.mbox.w[0]      - appHandle returned from OPEN call
     * Desc:    Close the FW-application and allow other host application to open the FW-app.  No further operations
     *          on the appHandle are permitted.
     * Status:  MFI_STAT_APP_IN_USE         - bad appHandle - app already in use
     *          MFI_STAT_APP_NOT_INITIALIZED- application not initialized
     */
    MR_DCMD_APP_CLOSE                       = 0x06050000,   // close FW-app

    /* Input:   dcmd.opcode         - MR_DCMD_APP_LOAD
     *          dcmd.mbox.w[0]      - Application name to load (4-character ROM file name)
     *          dcmd.mbox.b[4]      - host image number (0 for first, 1 for second, etc.  This is application-specific)
     *          dcmd.hdr.length     - size of the user buffer
     *          dcmd.sge IN         - Ptr to host memory to receive the host program
     * Desc:    Download the host code for the specified application into the host memory.
     * Status:  MFI_STAT_NOT_FOUND              - no application found
     *          MFI_STAT_APP_HOST_CODE_NOT_FOUND- Host code is not present in the image
     */
    MR_DCMD_APP_LOAD                        = 0x06060000,   // Load host side application code into the host memory

    /* Input:   dcmd.opcode         - MR_DCMD_APP_GET_MEM_REQUIREMENTS
     *          dcmd.mbox.w[0]      - Application name to get memory requirements for
     *          dcmd.sge IN         - Ptr to returned U32 that will contain the number of memory bytes this APP requires.
     * Desc:    Get the amount of memory (in bytes) this application requires in order to run.
     * Status:  MFI_STAT_NOT_FOUND              - no application found
     *          MFI_STAT_APP_HOST_CODE_NOT_FOUND- Host code is not present in the image
     */
    MR_DCMD_APP_GET_MEM_REQUIREMENTS        = 0x06070000,   // get application memory requirements

    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * Backend SAS configuration commands
     *
     * These commands may use the MPT config pages for communication.
     *  1. SAS configuration pass through read only commands
     * Please refer to appropriate sections of the MPI Specification for details.
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_SAS_CONFIG                      = 0x07000000,   // Backend SAS configuration pages

    /* Input:   dcmd.opcode         - MR_DCMD_SAS_CONFIG_GET_PHY_CONNECTIONS
     *          dcmd.mbox.b[0]      - reserved
     *          dcmd.sge IN         - Pointer to returned MR_SAS_PHY_CONNECTIONS structure
     * Desc:    Get PHY connection information for each physical PHY on the controller.
     * Status:  No error
     */
    MR_DCMD_SAS_CONFIG_GET_PHY_CONNECTIONS  = 0x07010000,   // read PHY direct connection information

    /* Input:   dcmd.opcode         - MR_DCMD_SAS_CONFIG_GET_MUX_MODE_LIST
     *          dcmd.mbox.b[0]      - reserved
     *          dcmd.sge IN         - Pointer to returned MR_SAS_MUX_LIST structure
     * Desc:    Get MR_SAS_MUX_MODE for each QUAD MUX.
     * Status:  No error
     */
    MR_DCMD_SAS_CONFIG_MUX_MODE             = 0x07020000,   // SAS MUX Mode commands
    MR_DCMD_SAS_CONFIG_MUX_MODE_GET_LIST    = 0x07020100,   // read MUX mode settings for all QUAD MUX

    /* Input:   dcmd.opcode         - MR_DCMD_SAS_CONFIG_GET_MUX_MODE_LIST
     *          dcmd.mbox.b[0]      - reserved
     *          dcmd.sge OUT        - Pointer to MR_SAS_MUX_LIST structure
     * Desc:    Set MR_SAS_MUX_MODE for each QUAD MUX.
     * Status:  No error
     */
    MR_DCMD_SAS_CONFIG_MUX_MODE_SET_LIST    = 0x07020200,   // set MUX mode settings for all QUAD MUXs

    /* Input:   dcmd.opcode         - MR_DCMD_SAS_CONFIG_GET_CONNECTORS
     *          dcmd.mbox.b[0]      - reserved
     *          dcmd.sge IN         - Pointer to returned MR_SAS_CONNECTORS structure
     * Desc:    Get connector information for each physical connector on the controller.
     * Status:  No error
     */
    MR_DCMD_SAS_CONFIG_GET_CONNECTORS       = 0x07030000,   // read connector information

    /* Input:   dcmd.opcode         - MR_DCMD_SAS_CONFIG_PAGE_PASSTHROUGH
     *          dcmd.mbox.b[0]      - reserved
     *          dcmd.sge IN/OUT     - Pointer to MR_SAS_CONFIG_PAGE structure
     * Desc:    Read and update sas config pages directly from SAS Core.
     *          This DCMD is for EXPERT USE ONLY.  It shouldn't be exposed to end-user.
     * Status:  MFI_STAT_INVALID_PARAMETER  -  Unsupported or incorrect parameters.
     */
    MR_DCMD_SAS_CONFIG_PAGE_PASSTHROUGH     = 0x07040000,   // Issue sas configuration request

    /* Input:   dcmd.opcode         - MR_DCMD_SAS_CONFIG_SET_LINK_SPEED
     *          dcmd.mbox.b[0]      - phy index
     *          dcmd.mbox.b[1]      - Link speed value (MR_SAS_PHY_LINK_SPEED)
     *          dcmd.sge IN/OUT     - reserved
     * Desc:    Limit link speed of a phy. (takes affect after controller re-boot)
     * Status:  MFI_STAT_INVALID_PARAMETER  -  Unsupported or incorrect parameters.
     */
    MR_DCMD_SAS_CONFIG_SET_LINK_SPEED       = 0x07050000, // update sas link speed of a phy

    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * Clustering commands TBD
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_CLUSTER                         = 0x08000000,   // TBD cluster management commands

    /* Input:   dcmd.opcode         - MR_DCMD_CLUSTER_RESET_ALL
     *          dcmd.mbox           - reserved
     * Desc:    Reset all LD reservations.
     * Status:  No error
     */
    MR_DCMD_CLUSTER_RESET_ALL               = 0x08010100,

    /* Input:   dcmd.opcode         - MR_DCMD_CLUSTER_RESET_LD
     *          dcmd.mbox.b[0]      - targetId for this logical drive
     * Desc:    Reset this LD's reservation.
     * Status:  No error
     */
    MR_DCMD_CLUSTER_RESET_LD                = 0x08010200,

    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * Enclosure management commands
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_ENCL                            = 0x09000000,   // enclosure management commands

    /* Input:   dcmd.opcode         - MR_DCMD_ENCL_GET_LIST
     *          dcmd.mbox.b[0]      - 0 - FW supports max 64 drives in an enclosure and uses legacy MR_ENCL_INFO
     * -                              1 - FW supports >64 drives in an enclosure and uses extended struct MR_ENCL_INFO_EXT
     *          dcmd.sge IN         - ptr to returned MR_ENCL_LIST/MR_ENCL_LIST_EXT structure (depending on the value of mbox[0])
     * Desc:    Return the enclosure list structure.
     * Status:  MFI_STAT_MEMORY_NOT_AVAILABLE   Memory not available
     *          MFI_STAT_INVALID_PARAMETER      Input parameters are incorrect
     */
    MR_DCMD_ENCL_GET_LIST                   = 0x09010000,   // get enclosure list

    /* Input:   dcmd.opcode         - MR_DCMD_ENCL_LOCATE
     *          dcmd.mbox.s[0]      - deviceId of enclosure
     *          dcmd.sge            - reserved
     * Desc:    Locate the enclosure.  The IDENTIFY command will be issued to the enclosure.
     * Status:  No error
     */
    MR_DCMD_ENCL_LOCATE                     = 0x09020000,   // locate enclosure

    /* Input:   dcmd.opcode         - MR_DCMD_SC_BKPLN_GET_DATA
     *          dcmd.sge.IN         - pointer to MR_SC_BKPLN_NVRAM_DATA
     * Desc:    Return all the backplane data
     * Status:  MFI_STAT_INVALID_STATUS   - Data not available try again
     *          MFI_STAT_OK
     */
    MR_DCMD_SC_BKPLN_GET_DATA               = 0x09030000,

    /*
     *---------------------------------------------------------------------------------------------------
     * Drive Performance Monitor (DPM) commands
     */

    MR_DCMD_DPM                             = 0x0A000000,   // Drive Performance Monitoring commands

    /* Input:   dcmd.opcode         - MR_DCMD_DPM_GET_CFG
     *          dcmd.sge IN         - ptr to returned MR_DPM_CFG structure
     * Desc:    Return the physical drive DPM configuration structure
     * Status:  No error
     */
    MR_DCMD_DPM_GET_CFG                     = 0x0A010000,   // get DPM configuration info

    /* Input:   dcmd.opcode         - MR_DCMD_DPM_GET_DATA
     *          dcmd.mbox.w[0]      - MR_PD_REF
     *          dcmd.mbox.w[1]      - parameter to return (see MR_PDM_DATA_TYPE enum)
     *          dcmd.sge IN         - ptr to data returned.  Structure of data returned depends on dcmd.mbox.w[1]
     * Desc:    Returns the DPM data requested
     * Status:  MFI_STAT_DEVICE_NOT_FOUND        - deviceId is invalid
     *          MFI_STAT_INVALID_SEQUENCE_NUMBER - sequence number is out of sync
     *          MFI_STAT_INVALID_PARAMETER       - dcmd.mbox.w[1] is invalid/reserved
     */
    MR_DCMD_DPM_GET_DATA                    = 0x0A020000,   // get DPM statistics for a PD

    /* Input:   dcmd.opcode         - MR_DCMD_DPM_CTRL
     *          dcmd.mbox.w[0]      - control param (see MR_DPM_DATA_CTRL enum)
     *          dcmd.mbox w[1]      - bitmap for clearing if w[0] == MR_DPM_DATA_CTRL_CLEAR (see MR_DPM_DATA_CLEAR structure)
     *          dcmd.sge            - reserved
     * Desc:    Controls DPM enabling/disabling/clearing
     * Status:  MFI_STAT_INVALID_PARAMETER - dcmd.mbox.w[0] is invalid or any reserved bits are set in dcmd.mbox w[1]
     */
    MR_DCMD_DPM_CTRL                        = 0x0A030000,   // Control DPM behavior (e.g. start, stop, clear)

    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * OOB management commands
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_OOB                             = 0x0B000000,   // OOB management related opcodes

    /* Input:   dcmd.opcode         - MR_DCMD_OOB_STATUS
     *          dcmd.mbox.b[0]      - 1 - send whole U32 MFI_REG_STATE value.  Also OOB/Sideband supports EKMS
     *                                0 - send only MFI_STATE value, OOB/Sideband doesn't support EKMS
     *          dcmd.mbox.b[1]      - 1 - OOB/Sideband EKMS key exchange done
     *          packet payload IN   - MFI_REG_STATE
     * Desc:    Respond to OOB Manager with status of MFI_REG_STATE, to indicate if FW
     *          is READY, or requires attention.  Will also indicate that OOB/sideband supports EKMS if mbox.b[0]
     *          is set.  mbox.b[1] indicates OOB/Sideband key exchange is done and FW can proceed.
     *          May also be issued as a control cmd MFI_OOB_CONTROL_OOB_STATUS
     * Status:  MFI_STAT_OK
     *          MFI_STAT_INVALID_STATUS   - Data not available try again
     */
    MR_DCMD_OOB_STATUS                      = 0x0B010000,   // Check status of OOB interface

    /* Input:   dcmd.opcode         - MR_DCMD_OOB_PARAM_GET
     *          dcmd.mbox.b[1]      - 0 - get current parameters
     *                                1 - get default parameters
     *          packet payload IN   - MR_OOB_PARAMS
     * Desc:    OOB Manager sets the types of events and MR_OOB_OBJ_TYPEs to be included in the STATUS POLL packet
     * Status:  MFI_STAT_OK
     */
    MR_DCMD_OOB_PARAM_GET                   = 0x0B020000,   // Get parameters for FW OOB service

    /* Input:   dcmd.opcode         - MR_DCMD_OOB_PARAM_SET
     *          dcmd.mbox           - MR_OOB_PARAMS
     *          packet payload OUT  - reserved
     * Desc:    OOB Manager sets the types of events and MR_OOB_OBJ_TYPEs to be included in the STATUS POLL packet.
     *          Note - As bytes are at a premium over the OOB interface,
     *          MR_OOB_PARAMS shall be used in place of mbox arguments.
     * Status:  MFI_STAT_OK
     */
    MR_DCMD_OOB_PARAM_SET                   = 0x0B030000,   // Set parameters for FW OOB service

    /* Input:   dcmd.opcode         - MR_DCMD_OOB_STATUS_POLL
     *          dcmd.mbox.b[0]      - max number of response packets
     *                                0x00=indicates no limit
     *          dcmd.mbox.b[1]      - 1 - send all information
     *                                0 - send incremental information
     *          packet payload IN   - MR_OOB_OBJ_LIST structure
     * Desc:    Respond to OOB Manager with list of objects and related data that have
     *          changed since last poll.
     *          May also be issued as a control cmd MFI_OOB_CONTROL_STATUS_POLL
     * Status:  MFI_STAT_INVALID_STATUS   - Data not available try again
     */
    MR_DCMD_OOB_STATUS_POLL                 = 0x0B040000,   // Poll FW for status/temp changes

    /* Input: dcmd.opcode              - MR_DCMD_OOB_MODE_SET
     *        dcmd.sge OUT             - ptr to MFI_OOB_METHOD_INFO structure with values to be updated.
     * Desc:  OOB Manager sets the needed Communication method, max payload and max packet size.
     *
     * Status:  MFI_STAT_OK_REBOOT_REQUIRED - If parameters are correct and update was successful
     *          MFI_STAT_MFI_STAT_INVALID_PARAMETER - If any of the parameters are incorrect.
     */
     MR_DCMD_OOB_METHOD_SET                  = 0x0B050000,

    /* Input: dcmd.opcode              - MR_DCMD_OOB_MODE_GET
     *        dcmd.mbox[0]             - 0 - get current parameters
     *                                 - 1 - get default parameters
     *        dcmd.sge IN              - ptr to returned MFI_OOB_METHOD_INFO structure
     * Status:  MFI_STAT_OK
     */
     MR_DCMD_OOB_METHOD_GET                  = 0x0B060000,

    /*
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     *
     * Backend PCIe configuration commands
     *
     *---------------------------------------------------------------------------------------------------
     *---------------------------------------------------------------------------------------------------
     */
    MR_DCMD_PCIE_CONFIG                     = 0x0C000000,   // Backend PCIe configuration pages

    /* Input:   dcmd.opcode         - MR_DCMD_PCIE_CONFIG_GET_LANE_CONNECTIONS
     *          dcmd.mbox.b[0]      - reserved
     *          dcmd.sge IN         - Pointer to returned MR_PCIE_LANE_CONNECTIONS structure
     * Desc:    Get LANE connection information for each physical LANE on the controller.
     * Status:  No error
     */
    MR_DCMD_PCIE_CONFIG_GET_LANE_CONNECTIONS= 0x0C010000,   // read LANE direct connection information

    /* Input:   dcmd.opcode         - MR_DCMD_PCIE_LINK_CONFIG_GET
     *          dcmd.mbox.b[0]      - reserved
     *          dcmd.sge IN         - Pointer to returned MR_PCIE_LINK_CONFIG structure
     * Desc:    Get PCIe link configuration
     * Status:  MFI_STAT_INVALID_CMD  -  invalid command
     */
    MR_DCMD_PCIE_LINK_CONFIG_GET            = 0x0C020000, // read pcie link configuration

    /* Input:   dcmd.opcode         - MR_DCMD_PCIE_LINK_CONFIG_SET
     *          dcmd.mbox.b[0]      - CSW number, use 0xFF to set all CSWs when supportLinkConfig=1
     *          dcmd.mbox.b[1]      - linkConfigIndex
     *          dcmd.sge IN/OUT     - reserved
     * Desc:    Set PCIe link configuration. (takes affect after controller re-boot)
     * Status:
     * MFI_STAT_INVALID_PARAMETER   - Some input parameters are invalid
     * MFI_STAT_OK_REBOOT_REQUIRED  - Ok but reboot required
     */
    MR_DCMD_PCIE_LINK_CONFIG_SET            = 0x0C030000, // update pcie link configuration

    /* Input:   dcmd.opcode         - MR_DCMD_PCIE_CONFIG_SET_LANE_SPEED
     *          dcmd.mbox.b[0]      - lane index
     *          dcmd.mbox.b[1]      - speed value (MR_PCIE_LANE_SPEED)
     *          dcmd.sge IN/OUT     - reserved
     * Desc:    Limit speed of a lane. User has to set this for all lanes participating in a pci-link
     *          (takes affect after controller re-boot)
     * Status:
     * MFI_STAT_INVALID_PARAMETER   - Some input parameters are invalid
     * MFI_STAT_OK_REBOOT_REQUIRED  - Ok but reboot required
     */
    MR_DCMD_PCIE_CONFIG_SET_LANE_SPEED      = 0x0C040000, // update pci speed of a lane

} MR_DCMD;

/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to the controller
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * define the controller environment structure
 */
typedef struct _MR_ENV_DATA {
    U32     size;                           // size of ENV area (including this SIZE field)
    char    data[1020];                     // variable length environment area (NULL-terminated strings, NULL-terminated list)
} MR_ENV_DATA;

/*
 * defines the ld bitmap structure for MAX_API_LOGICAL_DRIVES_EXT lds, 0xFF is reserved for special use
 */
typedef union _MR_LD_BITMAP {
    U8                      b[32];
    U16                     s[16];
    U32                     w[8];
    U64                     d[4];
} MR_LD_BITMAP;

/*
 * defines the ld bitmap structure for MAX_API_LOGICAL_DRIVES_EXT2 lds, all 0xFs are reserved for special use
 */
typedef union _MR_LD_BITMAP_EXT {
    U8                      b[MAX_API_LOGICAL_DRIVES_EXT2/8];
    U16                     s[MAX_API_LOGICAL_DRIVES_EXT2/16];
    U32                     w[MAX_API_LOGICAL_DRIVES_EXT2/32];
    U64                     d[MAX_API_LOGICAL_DRIVES_EXT2/64];
} MR_LD_BITMAP_EXT;

/*
 * defines the pd bitmap structure for MAX_API_PHYSICAL_DEVICES pds, 0xFF is reserved for special use
 */
typedef union _MR_PD_BITMAP {
    U8                      b[32];
    U16                     s[16];
    U32                     w[8];
    U64                     d[4];
} MR_PD_BITMAP;

/*
 * defines the pd bitmap structure for MAX_API_PHYSICAL_DEVICES_EXT2 pds, all 0xFs are reserved for special use
 */
typedef union _MR_PD_BITMAP_EXT {
    U8                      b[MAX_API_PHYSICAL_DEVICES_EXT2/8];
    U16                     s[MAX_API_PHYSICAL_DEVICES_EXT2/16];
    U32                     w[MAX_API_PHYSICAL_DEVICES_EXT2/32];
    U64                     d[MAX_API_PHYSICAL_DEVICES_EXT2/64];
} MR_PD_BITMAP_EXT;

/*
 * defines the enum for type of load balance
 */

typedef enum _MR_LOAD_BALANCE_MODE {
    MR_LOAD_BALANCE_MODE_AUTO           = 0,    // system chooses mode of load balancing
    MR_LOAD_BALANCE_MODE_NONE           = 1,    // load balancing disabled
} MR_LOAD_BALANCE_MODE;
#define MR_LOAD_BALANCE_MODE_MAX          1     // maximum permissible value

typedef struct _MR_CTRL_PROP {
    U16     seqNum;                         // seq # to control concurrent updates
    U16     predFailPollInterval;           // # of seconds between pred fail polls
    U16     intThrottleCount;               // interrupt throttle active count
    U16     intThrottleTimeUs;              // interrupt throttle completion hold time
    U8      rebuildRate;                    // Rebuild rate
    U8      patrolReadRate;                 // Patrol Read Rate
    U8      bgiRate;                        // back ground init rate
    U8      ccRate;                         // consistency check rate
    U8      reconRate;                      // reconstruction rate
    U8      cacheFlushInterval;             // cache flush interval in seconds
    U8      spinupDriveCount;               // max # of drives to spin up at one time
    U8      spinupDelay;                    // # of seconds to delay among spinup groups
    U8      clusterEnable;                  // TBD cluster is enabled or disabled
    U8      coercionMode;                   // drive capacity coercion mode. 0->None; 1->128MB; 2->1GB
    U8      alarmEnable;                    // set TRUE to enable alarm (if supported by HW)
    U8      disableAutoRebuild;             // set TRUE to disable auto-rebuild and hot-spare commissioning
    U8      disableBatteryWarning;          // set TRUE to disable warnings for missing battery or missing HW
    U8      eccBucketSize;                  // 'size' of ECC single-bit-error bucket (logs event when full)
    U16     eccBucketLeakRate;              // 'leak rate' (in minutes) of the single-bit bucket (one entry removed per leak-rate)
    U8      restoreHotSpareOnInsertion;     // TRUE to enable auto-restoration of HS drives on insertion into same slot
    U8      exposeEnclosureDevices;         // set TRUE to enable device drivers to expose enclosure devices, FALSE=hide enclosure devices
    U8      maintainPdFailHistory;          // set TRUE to enable tracking of bad PDs across reboot; will also show failed LED status for missing bad drives
    U8      disallowHostRequestReordering;  // set FALSE to enable shared region locking, better performance, TRUE=exclusive region locking, strict
    U8      abortCCOnError;                 // set TRUE to abort CC on detecting an inconsistency
    U8      loadBalanceMode;                // load balance mode (MR_LOAD_BALANCE_MODE)
    U8      disableAutoDetectBackplane;     // 0 - use auto detect logic of backplanes like SGPIO, i2c SEP using h/w mechansim like GPIO pins
                                            // 1 - disable auto detect SGPIO,
                                            // 2 - disable i2c SEP auto detect
                                            // 3 - disable both auto detect
    U8      snapVDSpace;                    // % of source LD to be reserved for a VDs snapshot in snapshot repository, for metadata and user data
                                            // 1=5%, 2=10%, 3=15% and so on

    /*
     * Add properties that can be controlled by a bit in the following structure.
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     copyBackDisabled            : 1;     // set TRUE to disable copyBack (0=copback enabled)
        U32     SMARTerEnabled              : 1;     // set TRUE to enable SMARTer (0=Disable copyback on SMART error)
        U32     prCorrectUnconfiguredAreas  : 1;     // Correct media errors during PR by writing 0's.
        U32     useFdeOnly                  : 1;     // 1=Only allow FDE drive based encryption
        U32     disableNCQ                  : 1;     // 0 - enabled, 1 - disabled
        U32     SSDSMARTerEnabled           : 1;     // 1 = Copyback to SSD on SMART error Enabled 0 = Disabled
        U32     SSDPatrolReadEnabled        : 1;     // 0 = SSD Patrol Read Disabled, 1 = SSD Patrol Read Enabled
        U32     enableSpinDownUnconfigured  : 1;     // 0 = do not spin down unconfigured drives, 1 = spin down unconfigured drives
        U32     autoEnhancedImport          : 1;     // 0 = do not import foreign configuration automatically, at boot
        U32     enableSecretKeyControl      : 1;     // 1 = allow lock key binding update without validating with lock key passphrase
        U32     disableOnlineCtrlReset      : 1;     // 0 = allow online controller resets, 1 = disallow online controller resets
        U32     allowBootWithPinnedCache    : 1;     // 1 = allow controller to boot with pinned cache
        U32     disableSpinDownHS           : 1;     // 1 = do not spin down hot spares, 0 = spin down hot spares
        U32     enableJBOD                  : 1;     // 1 = enable JBOD mode, by default drives shall be system drives
        U32     disableCacheBypass          : 1;     // 1 = disable cache-bypass-performance-improvement feature
        U32     useDiskActivityForLocate    : 1;     // 1 = drive activity LED is toggled for LOCATE
        U32     enablePI                    : 1;     // 0 = Disable SCSI PI for controller.  Remove any active protection information
        U32     preventPIImport             : 1;     // 1 = Prevent import of SCSI DIF protected logical disks
        U32     useGlobalSparesForEmergency : 1;     // 1 = Use global spares for Emergency (if spare is incompatible without Emergency)
        U32     useUnconfGoodForEmergency   : 1;     // 1 = Use uncofgured good drives for Emergency
        U32     useEmergencySparesforSMARTer: 1;     // 1 = Use Emergency spares for SMARTer
        U32     forceSGPIOForQuadOnly       : 1;     // 1 = Force SGPIO status per port only for four drives, affects HPC controllers
        U32     enableConfigAutoBalance     : 1;     // 0 = Configuration auto balance disabled, 1 = Configuration auto balance enabled
//      U32     enableVirtualCache          : 1;     // Deprecated and replaced with enableVCorDLC
        U32     failPDOnSMARTErr            : 1;     // 1=Fail a PD that's reached SMART failure threshold, if it doesn't take an LD offline
        U32     enableWriteVerifyDuringCacheFlush  : 1;     // FW uses write-verify during the cache flush operation
        U32     disableImmediateIO          : 1;     // 1 = Disable Legacy Immediate IO, 0 = Enable
        U32     disableT10RebuildAssist     : 1;     // 1 = Disable T10 Rebuild Assist, use legacy rebuild method
        U32     ignore64ldRestriction       : 1;     // 0 - limit LD to 64 even if more LD support exists, 1 - support more than 64 ld with new DDF format
        U32     enableSwZone                : 1;     // 1 = enable Software Zone
        U32     limitMaxRateSATA3G          : 1;     // 1 = negotiated link rates to direct attached SATA devices shall be limited to 3Gbps
        U32     driveActivityLED            : 1;     // 0 = No LED for drive activity, 1 = Show all drive activity
        U32     disableSESMonitoring        : 1;     // 1 = Disables SES Monitoring on the controller. Doesnt get preserved over controller reset
#else
        U32     disableSESMonitoring        : 1;     // 1 = Disables SES Monitoring on the controller. Doesnt get preserved over controller reset
        U32     driveActivityLED            : 1;     // 0 = No LED for drive activity, 1 = Show all drive activity
        U32     limitMaxRateSATA3G          : 1;     // 1 = negotiated link rates to direct attached SATA devices shall be limited to 3Gbps
        U32     enableSwZone                : 1;     // 1 = enable Software Zone
        U32     ignore64ldRestriction       : 1;     // 0 - limit LD to 64 even if more LD support exists, 1 - support more than 64 ld with new DDF format
        U32     disableT10RebuildAssist     : 1;     // 1 = Disable T10 Rebuild Assist, use legacy rebuild method
        U32     disableImmediateIO          : 1;     // 1 = Disable Legacy Immediate IO, 0 = Enable
        U32     enableWriteVerifyDuringCacheFlush  : 1;     // FW uses write-verify during the cache flush operation
        U32     failPDOnSMARTErr            : 1;     // 1=Fail a PD that's reached SMART failure threshold, if it doesn't take an LD offline
//      U32     enableVirtualCache          : 1;     // Deprecated and replaced with enableVCorDLC
        U32     enableConfigAutoBalance     : 1;     // 0 = Configuration auto balance disabled, 1 = Configuration auto balance enabled
        U32     forceSGPIOForQuadOnly       : 1;     // 1 = Force SGPIO status per port only for four drives, affects HPC controllers
        U32     useEmergencySparesforSMARTer: 1;     // 1 = Use Emergency spares for SMARTer
        U32     useUnconfGoodForEmergency   : 1;     // 1 = Use uncofgured good drives for Emergency
        U32     useGlobalSparesForEmergency : 1;     // 1 = Use global spares for Emergency (if spare is incompatible without Emergency)
        U32     preventPIImport             : 1;     // 1 = Prevent import of SCSI DIF protected logical disks
        U32     enablePI                    : 1;     // 0 = Disable SCSI PI for controller.  Remove any active protection information
        U32     useDiskActivityForLocate    : 1;     // 1 = drive activity LED is toggled for LOCATE
        U32     disableCacheBypass          : 1;     // 1 = disable cache-bypass-performance-improvement feature
        U32     enableJBOD                  : 1;     // 1 = enable JBOD mode, by default drives shall be system drives
        U32     disableSpinDownHS           : 1;     // 1 = do not spin down hot spares, 0 = spin down hot spares
        U32     allowBootWithPinnedCache    : 1;     // 1 = allow controller to boot with pinned cache
        U32     disableOnlineCtrlReset      : 1;     // 0 = allow online controller resets, 1 = disallow online controller resets
        U32     enableSecretKeyControl      : 1;     // 1 = allow lock key binding update without validating with lock key passphrase
        U32     autoEnhancedImport          : 1;     // 0 = do not import foreign configuration automatically, at boot
        U32     enableSpinDownUnconfigured  : 1;     // 0 = do not spin down unconfigured drives, 1 = spin down unconfigured drives
        U32     SSDPatrolReadEnabled        : 1;     // 0 = SSD Patrol Read Disabled, 1 = SSD Patrol Read Enabled
        U32     SSDSMARTerEnabled           : 1;     // 1 = Copyback to SSD on SMART error Enabled 0 = Disabled
        U32     disableNCQ                  : 1;     // 0 - enabled, 1 - disabled
        U32     useFdeOnly                  : 1;     // 1=Only allow FDE drive based encryption
        U32     prCorrectUnconfiguredAreas  : 1;     // Correct media errors during PR by writing 0's.
        U32     SMARTerEnabled              : 1;     // set TRUE to enable SMARTer (0=Disable copyback on SMART error)
        U32     copyBackDisabled            : 1;     // set TRUE to disable copyBack (0=copback enabled)
#endif  // MFI_BIG_ENDIAN
    } OnOffProperties;

    union {
        U8      autoSnapVDSpace;                // Deprecated % of source LD to be reserved for auto snapshot in snapshot repository, for metadata and user data
        U8      viewSpace;                      // Deprecated snapshot writeable VIEWs capacity as a % of source LD capacity. 0=READ only
        struct {
#ifndef MFI_BIG_ENDIAN
            U16 enableLargeIOSupport              : 1;    // 0 = LargeIOSupport (>256K) Disabled, 1 = LargeIOSupport Enabled
            U16 enableLargeQD                     : 1;    // 0 = Large QD (>256 for iMR) is disabled, 1 = Large QD support is enabled
            U16 SESVPDAssociationTypeInMultiPathCfg : 1;  // Applicable only for the SES devivce
                                                          // 0 = VPD association type based on LUN
                                                          // 1 = VPD association type based on target port
            U16 activeFWImageSelect               : 1;    // 0 = controller booted from FW image located at primary flash region,
                                                          // 1 = controller booted from FW image located at secondary flash region(Dual FW support)
            U16 enableSnapDump                    : 1;    // 0 = snapdump feature is not enable
                                                          // 1 = snapdump feature is enable
            U16 disableScsiUnmap                  : 1;    // 1 = Disable Unmap, 0 = Enable Unmap
            U16 reserved                          : 10;   // Reserved for future use.
#else
            U16 reserved                          : 10;   // Reserved for future use.
            U16 disableScsiUnmap                  : 1;    // 1 = Disable Unmap, 0 = Enable Unmap
            U16 enableSnapDump                    : 1;    // 0 = snapdump feature is not enable
                                                          // 1 = snapdump feature is enable
            U16 activeFWImageSelect               : 1;    // 0 = next boot from FW image located at primary flash region,
                                                          // 1 = next boot from FW image located at secondary flash region(Dual FW support)
            U16 SESVPDAssociationTypeInMultiPathCfg : 1;  // Applicable only for the SES devivce
                                                          // 0 = VPD association type based on LUN
                                                          // 1 = VPD association type based on target port
            U16 enableLargeQD                     : 1;    // 0 = Large QD (>256 for iMR) is disabled, 1 = Large QD support is enabled
            U16 enableLargeIOSupport              : 1;    // 0 = LargeIOSupport Disabled, 1 = LargeIOSupport Enabled.
#endif
        } OnOffProperties2;
    };

    U16     spinDownTime;                   // # of idle minutes before device is spun down (0=use FW defaults)
    U8      defaultLdPSPolicy;              // default LD power savings policy (bitmap of MR_LD_PS_POLICY).
    U8      disableLdPSInterval;            // LD power savings are disabled for yy hours beginning at disableLdPSTime
    U16     disableLdPSTime;                // LD power savings shall be disabled at xx minutes from 12:00am
    U8      spinupEnclDriveCount;           // max # of drives within an enclosure to spin up at one time
    U8      spinupEnclDelay;                // # of seconds to delay among spinup groups within an enclosure

    U8      defaultCtrlPowerPolicy;         //Default Controller Power Policy (Basic power options of MR_POWER_THROTTLE_OPTIONS)
    struct {                                // Performance Tuning settings
#ifndef MFI_BIG_ENDIAN
        U8      mode        : 2;            // currently 2 modes supported: 0 - BestIOPS, 1 - Least Latency
                                            // All other modes should be defined later. Auto mode when defined
                                            // will have no values to set but get would be supported
        U8      reserved0   : 6;
#else
        U8      reserved0   : 6;
        U8      mode        : 2;
#endif  // MFI_BIG_ENDIAN
    } perfTuners;

    struct {                                // Two set of values per mode, tied to perfTuners.
        U8  maxFlushLines;                  // controller-wide max-flush scaling value
        union {                             // The ordered tag forces the drive to complete previous IOs before moving on to subsequent IOs.
            U8  numOrderIOs;                            // orig implementation, used for select releases. Num physical IOs (value multiplied by 10 = number of I/Os
            U8  maxPdLatencyMsBeforeIssuingOrderedTag;  // max latency in milliseconds before ordered tag issued. Value is in increments of 25ms (0=disable, 1=25ms, 2=50ms, etc...)
        };
    } perfTunerValues[4];                   // Index the mode value to access these. Refer to MR_PERF_TUNE_MODES_COUNT for supported indices

    U16     HDDThermalPollInterval;         // Thermal polling interval for HDDs in seconds
    U16     SSDThermalPollInterval;         // Thermal polling interval for SSDs in seconds
    U8      diskCacheSettingForSysPDs;      // Value from MR_PD_CACHE. When set, it is applicable to all system PDs in the system

    union {
        struct {
#ifndef MFI_BIG_ENDIAN
            U8      detectionType    : 2;   // 00b = detection disabled
                                            // 01b = detection enabled, high latency for reads is ok
                                            // 10b = detection enabled, aggressive (high latency for reads is not OK)
                                            // 11b = detection enabled, use NVDATA specfied value see recoveryTimeLimit, and writeRetryCount
            U8      correctiveAction : 1;   // 0 = only log events, 1 = log events, take corrective action based on SMARTer
            U8      reserved0        : 1;
            U8      errorThreshold   : 2;   // 00b = 1 i.e 1 error every 8 hours (least tolerant)
                                            // 01b = 8 i.e 1 error every 1 hour
                                            // 10b = 32 i.e. 1 error every 15 minutes
                                            // 11b = 90 i.e. 1 error every 5 minutes (most tolerant of drive with degraded media)
            U8      reserved1        : 2;
#else
            U8      reserved1        : 2;
            U8      errorThreshold   : 2;
            U8      reserved0        : 1;
            U8      correctiveAction : 1;
            U8      detectionType    : 2;
#endif
        } pdDegradedMediaOptions;
        U8  pdDegradedMediaProperties;      // Shorthand reference to pdDegradedMediaOptions
    };

    struct {
#ifndef MFI_BIG_ENDIAN
        U8        sriovEnabled: 1;
        U8        VFQDMode:4;              // Check enum MR_IOV_QD_OPTIONS
        U8        rsvd:3;
#else
        U8        rsvd:3;
        U8        VFQDMode:4;              // Check enum MR_IOV_QD_OPTIONS
        U8        sriovEnabled: 1;
#endif
    } sriovOptions;

    U8      reserved;
} MR_CTRL_PROP; // 64 bytes


/*
 * define the flash component detail structure
 */
typedef struct _MR_FLASH_COMP_INFO {
    char    name[8];                            // component name (null terminated) ("BIOS", "APP", ...)
    union {
        char pkgVer[64];                        // package version string (needs all 64 bytes)
        struct {
            char    version[32];                // image version string
            char    buildDate[16];              // image build date
            char    buildTime[16];              // image build time
        };
    };
} MR_FLASH_COMP_INFO;

#define MR_FLASH_NAME_APP           "APP "      // name of RAID application code
#define MR_FLASH_NAME_BBU           "BBU"       // name of BBU component
#define MR_FLASH_NAME_BIOS          "BIOS"      // name of BIOS module
#define MR_FLASH_NAME_BOOT          "BOOT"      // another name for Boot Block module
#define MR_FLASH_NAME_BQMI          "BQMI"      // name of BQMI module
#define MR_FLASH_NAME_BTBL          "BTBL"      // name of Boot Block module
#define MR_FLASH_NAME_CPLD          "CPLD"      // name of CPLD module
#define MR_FLASH_NAME_CTLR          "CTLR "     // name of CTLR module
#define MR_FLASH_NAME_DBGR          "DBGR"      // name of Debug module
#define MR_FLASH_NAME_DIAG          "DIAG"      // name of Diag module
#define MR_FLASH_NAME_DRIVER        "DRIVER"    // name of Driver component
#define MR_FLASH_NAME_HIIA          "HIIA"      // name of HII module for Arm
#define MR_FLASH_NAME_HIIM          "HIIM"      // name of HII module
#define MR_FLASH_NAME_MBBL          "MBBL"      // name of Master Boot Block module
#define MR_FLASH_NAME_NVDATA        "NVDATA"    // name of NVDATA module, typically for IR application
#define MR_FLASH_NAME_NVDT          "NVDT"      // another name for NVDATA module in flash
#define MR_FLASH_NAME_PACKAGE       "PACKAGE"   // name of whole ROM package
#define MR_FLASH_NAME_PCLI          "PCLI"      // name of PCLI module
#define MR_FLASH_NAME_PSOC          "PBLP"      // name of PSOC module

typedef struct _MR_FLASH_COMP_INFO_LIST {
    U32                  size;             // size of total data structure (including size/count fields)
    U32                  count;            // count of components with details available
    MR_FLASH_COMP_INFO   components[1];    // variable list of component info details
                                           // Note - if certain fields are not available they shall be set to null strings.
                                           //        this list is not exclusive to components stored in controller flash
} MR_FLASH_COMP_INFO_LIST;

/*
 * define constants for PCI link speeds
 */
typedef enum _MR_PCI_LINK_SPEED {
    MR_PCI_LINK_SPEED_DEFAULT       = 0,    // Max supported by hardware
    MR_PCI_LINK_SPEED_GEN1          = 1,    // Gen1 2.5GT/s
    MR_PCI_LINK_SPEED_GEN2          = 2,    // Gen2 5GT/s
    MR_PCI_LINK_SPEED_GEN3          = 3,    // Gen3 8GT/s
    MR_PCI_LINK_SPEED_GEN4          = 4,    // Gen4 16GT/s
} MR_PCI_LINK_SPEED;
#define MR_PCI_LINK_SPEED_MAX       MR_PCI_LINK_SPEED_GEN4

/*
 * define the controller information structure
 */
typedef struct _MR_CTRL_INFO {
    /*
     * PCI device information
     */
    struct {                                //0x000
        U16     vendorId;                           // PCI Vendor ID
        U16     devId;                              // PCI Device ID
        U16     subVendorId;                        // PCI Sub-Vendor ID
        U16     subDevId;                           // PCI Sub-Device ID
        U16     ctrlId;                             // mapped to PnPID
        U8      chipRevision[2];                    // Ascii revision of the chip "A0", "B4" etc.
        U8      reserved[20];
    } pci;

    /*
     * host interface information
     */
    struct {                                //0x020
#ifndef MFI_BIG_ENDIAN
        U8      PCIX        : 1;
        U8      PCIE        : 1;
        U8      iSCSI       : 1;
        U8      SAS_3G      : 1;
        U8      SRIOV       : 1;
        U8      reserved0   : 3;
#else
        U8      reserved0   : 3;
        U8      SRIOV       : 1;
        U8      SAS_3G      : 1;
        U8      iSCSI       : 1;
        U8      PCIE        : 1;
        U8      PCIX        : 1;
#endif  // MFI_BIG_ENDIAN
        U8      maxPciLinkSpeed;                    // MR_PCI_LINK_SPEED, max supported PCI link speed
        U8      pciLinkSpeed;                       // MR_PCI_LINK_SPEED, advertised max PCI link speed
        U8      reserved1[4];
        U8      portCount;                          // typically 1 for PCIX/PCIE - count of ports for SAS/iSCSI
        U64     portAddr[8];                        // frontend addresses (i.e. SAS Addr) - N/A for PCIX/PCIE
    } hostInterface;

    /*
     * device (backend) interface information
     */
    struct {                                //0x068
#ifndef MFI_BIG_ENDIAN
        U8      SPI         : 1;
        U8      SAS_3G      : 1;
        U8      SATA_1_5G   : 1;
        U8      SATA_3G     : 1;
        U8      SAS_6G      : 1;
        U8      SAS_12G     : 1;
        U8      SATA_6G     : 1;
        U8      reserved0   : 1;
#else
        U8      reserved0   : 1;
        U8      SATA_6G     : 1;
        U8      SAS_12G     : 1;
        U8      SAS_6G      : 1;
        U8      SATA_3G     : 1;
        U8      SATA_1_5G   : 1;
        U8      SAS_3G      : 1;
        U8      SPI         : 1;
#endif  // MFI_BIG_ENDIAN
        U8      type;                               // interface type (MR_SAS_INTERFACE_TYPE)
        U8      maxPciLinkSpeed;                    // MR_PCI_LINK_SPEED, max supported PCI link speed
        U8      pciLinkSpeed;                       // MR_PCIE_LANE_SPEED, bitmask for HW supported PCI link speeds
        U8      reserved1[3];
        U8      portCount;                          // If port count is greater than 8, see deviceInterfacePortAddr2
        U64     portAddr[8];                        // backend address (i.e. SAS Addr)
                                                    // If port count is greater than 8, first 8 SAS Addr are populated here
                                                    // and rest are populated in deviceInterfacePortAddr2
    } deviceInterface;

    /*
     * List of components residing in flash.  All strings are null-terminated.
     */
    U32         imageCheckWord;             //0x0b0 // FW Image check word
    U32         imageComponentCount;        //0x0b4 // active FW image component count (0 if none found, or not supported)
    MR_FLASH_COMP_INFO  imageComponent[8];  //0x0b8 // running flash component detail

    /*
     * List of flash components that have been flashed on the card, but aren't
     * in use, pending reset of the adapter.  This list will be empty if a flash
     * operation has not occurred.  All strings are null-terminated.
     */
    U32         pendingImageComponentCount; //0x2f8 // Pending in Flash FW image component count (0 if none pending)
    MR_FLASH_COMP_INFO  pendingImageComponent[8];   //0x2fc

    U8          maxArms;                    //0x53c // max arms per LD
    U8          maxSpans;                           // max spans per LD
    U8          maxArrays;                          // max arrays
    U8          maxLDs;                             // max number of LDs FW will support this value may change
                                                    // depending upon ignore64ldRestriction

    char        productName[80];            //0x540 // product name ("MegaRAID SAS-8E, MegaRAID SAS-4X, etc.)
    char        serialNo[32];               //0x590 // Controller serial number string

    /*
     * Other physical / controller / operation information
     * indicates presence of hardware
     */
    struct {                                //0x5b0
#ifndef MFI_BIG_ENDIAN
        U32     bbu         : 1;
        U32     alarm       : 1;
        U32     nvram       : 1;
        U32     uart        : 1;                    // serial debugger
        U32     memory      : 1;                    // local memory exists
        U32     flash       : 1;                    // flash memory exists
        U32     TPM         : 1;                    // 1=TPM is available and configured.
        U32     expander    : 1;                    // 1=on board expander is present
        U32     upgradeKey  : 1;                    // 1=an upgrade feature key is present
        U32     upgradeKeyNotSupported : 1;         // 1=upgradeKey is not supported by HW
        U32     tempSensorROC  : 1;                 // 1=temperature sensor for ROC is present
        U32     tempSensorCtrl : 1;                 // 1=temperature sensor for controller is present
        U32     ssmSupport  : 1;                    // 1=solid state modules supported by HW
        U32     powerMonitor: 1;                    // 1=I2C Power monitor/regulator is present
        U32     upgradableCPLD:1;                   // CPLD present and can be updated
        U32     upgradablePSOC:1;                   // PSOC present and can be updated
        U32     reserved    :16;
#else
        U32     reserved    :16;
        U32     upgradablePSOC:1;                   // PSOC present and can be updated
        U32     upgradableCPLD:1;                   // CPLD present and can be updated
        U32     powerMonitor: 1;                    // 1=I2C Power monitor/regulator is present
        U32     ssmSupport  : 1;                    // 1=solid state modules supported by HW
        U32     tempSensorCtrl : 1;                 // 1=temperature sensor for controller is present
        U32     tempSensorROC  : 1;                 // 1=temperature sensor for ROC is present
        U32     upgradeKeyNotSupported : 1;         // 1=upgradeKey is not supported by HW
        U32     upgradeKey  : 1;                    // 1=an upgrade feature key is present
        U32     expander    : 1;                    // 1=on board expander is present
        U32     TPM         : 1;                    // 1=TPM is available and configured.
        U32     flash       : 1;                    // flash memory exists
        U32     memory      : 1;                    // local memory exists
        U32     uart        : 1;                    // serial debugger
        U32     nvram       : 1;
        U32     alarm       : 1;
        U32     bbu         : 1;
#endif  // MFI_BIG_ENDIAN
    } hwPresent;

    U32         currentFwTime;              //0x5b4 // current FW time (seconds since January 1, 2000 - if FF in bits 24-32, then bits 0-23 are seconds since boot)

    /*
     * maximum data transfer sizes
     */
    U16         maxConcurrentCmds;          //0x5b8 // max parallel commands supported
    U16         maxSGECount;                        // maximum Scatter Gather Element count allowed
    U32         maxRequestSize;                     // maximum data transfer size in sectors
                // also see maxStripsPerIo below for maximum IO size

    /*
     * logical and physical device counts
     */
    U16         ldPresentCount;             //0x5c0 // # logical  devices present
    U16         ldDegradedCount;            //0x5c2 // # logical  devices critical
    U16         ldOfflineCount;             //0x5c4 // # logical  devices offline
    U16         pdPresentCount;             //0x5c6 // # physical devices present
    U16         pdDiskPresentCount;         //0x5c8 // # physical disk devices present
    U16         pdDiskPredFailureCount;     //0x5ca // # physical disk devices critical
    U16         pdDiskFailedCount;          //0x5cc // # physical disk devices failed

    /*
     * memory size information
     */
    U16         nvramSize;                  //0x5ce // NVRAM size in KB
    U16         memorySize;                 //0x5d0 // Memory size in MB
    U16         flashSize;                  //0x5d2 // flash size in MB

    /*
     * error counters
     */
    U16         memCorrectableErrorCount;   //0x5d4
    U16         memUnCorrectableErrorCount; //0x5d6

    /*
     * cluster information
     */
    union {
        U8          maxHANodes;             //0x5d8 # of HA nodes this FW supports
        U8          clusterPermitted;       //0x5d8 // DEPRECATED
    };
    union {
        U8          reserved6;              //0x5d9 for future use
        U8          clusterActive;          //0x5d9 // DEPRECATED
    };

    /*
     * additional maximum data transfer sizes
     */
    U16         maxStripsPerIo;             //0x5da // maximum strips per IO (alternate limit to IO size)

    /*
     * controller capabilities structures
     */
    // supported RAID levels
    struct {                                //0x5dc
#ifndef MFI_BIG_ENDIAN
        U32     raidLevel_0 : 1;
        U32     raidLevel_1 : 1;
        U32     raidLevel_5 : 1;
        U32     raidLevel_1E: 1;                    // PRL11,RLQ1 DDF layout
        U32     raidLevel_6 : 1;
        U32     raidLevel_10: 1;                    // RAID-1 with spanning
        U32     raidLevel_50: 1;                    // RAID-5 with spanning
        U32     raidLevel_60: 1;                    // RAID-6 with spanning
        U32     SRL_03      : 1;                    // SRL==03 is supported (see DDF definitions)
        U32     raidLevel_00_unsupported : 1;       // RAID-0 with spanning, 1=not supported, 0=supported.
        U32     raidLevel_1E_RLQ0 : 1;              // PRL11, RLQ0 DDF layout with no span
        U32     raidLevel_1E0_RLQ0 : 1;             // PRL11, RLQ0 DDF layout with span
        U32     SSCraidLevel_0_unsupported : 1;     // SSCD Raid Level 0  - PRL 0, RLQ 0
        U32     SSCraidLevel_1 : 1;                 // SSCD Raid Level 1  - PRL 1, RLQ 0
        U32     SSCraidLevel_5 : 1;                 // SSCD Raid Level 5  - PRL 5, RLQ 0
        U32     SSCraidLevel_1E: 1;                 // SSCD Raid Level IE - PRL11, RLQ 1 (uneven drive support)
        U32     SSCraidLevel_1E_EC: 1;              // 1=Elastic Cache SSCD supports raid 1E for caching write IOs
                                                    //      Use SSCraidLevel_1E, minPdRaidLevel_1E, and maxPdRaidLevel_1E
                                                    //      set MR_LD_PARAMETERS.SSCDType appropriately
        U32     reserved    : 15;                   // remaining bits
#else
        U32     reserved    : 15;                   // remaining bits
        U32     SSCraidLevel_1E_EC: 1;
        U32     SSCraidLevel_1E: 1;
        U32     SSCraidLevel_5 : 1;
        U32     SSCraidLevel_1 : 1;
        U32     SSCraidLevel_0_unsupported : 1;     // SSCD Raid Level 0
        U32     raidLevel_1E0_RLQ0 : 1;             // PRL11, RLQ0 DDF layout with span
        U32     raidLevel_1E_RLQ0 : 1;              // PRL11, RLQ0 DDF layout with no span
        U32     raidLevel_00_unsupported : 1;       // RAID-0 with spanning, 1=not supported, 0=supported.
        U32     SRL_03      : 1;                    // SRL==03 is supported (see DDF definitions)
        U32     raidLevel_60: 1;                    // RAID-6 with spanning
        U32     raidLevel_50: 1;                    // RAID-5 with spanning
        U32     raidLevel_10: 1;                    // RAID-1 with spanning
        U32     raidLevel_6 : 1;
        U32     raidLevel_1E: 1;                    // PRL11,RLQ1 DDF layout
        U32     raidLevel_5 : 1;
        U32     raidLevel_1 : 1;
        U32     raidLevel_0 : 1;
#endif  // MFI_BIG_ENDIAN
    } raidLevels;

    // supported Adapter operations, also see adapterOperations2
    struct {                                //0x5e0
#ifndef MFI_BIG_ENDIAN
        U32     rbldRate            : 1;
        U32     ccRate              : 1;
        U32     bgiRate             : 1;
        U32     reconRate           : 1;
        U32     patrolRate          : 1;
        U32     alarmControl        : 1;
        U32     clusterSupported    : 1;
        U32     bbu                 : 1;            // battery API enabled
        U32     spanningAllowed     : 1;
        U32     dedicatedHotSpares  : 1;
        U32     revertibleHotSpares : 1;            // revertible spares revert back to spare status after replacement of originally failed pd
                                                    // 1=copyback is supported. All spares are revertible if copyback is enabled
        U32     foreignConfigImport : 1;            // allow foreign config import
        U32     selfDiagnostic      : 1;            // controller supports self diagnostic
        U32     allowMixedRedundancyOnArray : 1;    // allow different VD on the same array to have different redundancy levels
        U32     globalHotSpares     : 1;            // global hot spares are supported
        U32     denySCSIPassthrough : 1;            // 0=Allows SCSI passthrough, 1=not supported
        U32     denySMPPassthrough  : 1;            // 0=Allows SMP passthrough, 1=not supported
        U32     denySTPPassthrough  : 1;            // 0=Allows STP passthrough, 1=not supported
        U32     supportMoreThan8Phys : 1;           // 0=API supports only 8 phys/ports etc., 1=API supports MAX_PHYS_PER_CONTROLLER
        U32     fwAndEventTimeInGMT : 1;            // Time reported by FW and events, 0=local time, 1=GMT
        U32     supportEnhForeignImport : 1;        // Enhanced Foreign Import capability is 1=supported, 0=not supported.
        U32     supportEnclEnumeration  : 1;        // Enclosure enumeration is 1=supported, 0=not supported.
        U32     supportAllowedOps   : 1;            // 1=Provide Allowed Ops for PD, LD and CONFIG via DCMDs, 0=not supported.
        U32     abortCCOnError      : 1;            // 0=CC fix errors and continue, 1=CC shall abort on detecing inconsistency
        U32     supportMultipath    : 1;            // 0=Multipath not supported, 1=Multipath and load balancing supported
        U32     supportOddEveninRAID1E: 1;          // 0=support even drive count in RAID 1E, 1=support both odd/even
        U32     supportSecurity     : 1;            // 1=security is supported, 0=security not supported
        U32     supportConfigPageModel : 1;         // 1=manufacturing data is maintained using Config Pages
        U32     supportOCEwithNoDriveAddition : 1;  // 1=support the OCE without adding drives but to expand the LD size within the array
        U32     supportEKM          : 1;            // 1=EKM mode is supported
        U32     supportSnapshot     : 1;            // 1=snapshot is supported, 0=snapshot not supported
        U32     supportPFK          : 1;            // 1=PFK is supported
#else
        U32     supportPFK          : 1;            // 1=PFK is supported
        U32     supportSnapshot     : 1;            // 1=snapshot is supported, 0=snapshot not supported
        U32     supportEKM          : 1;            // 1=EKM mode is supported
        U32     supportOCEwithNoDriveAddition : 1;  // 1= support the OCE without adding drives but to expand the LD size within the array
        U32     supportConfigPageModel : 1;         // 1= manufacturing data is maintained using Config Pages
        U32     supportSecurity     : 1;            // 1=security is supported, 0=security not supported
        U32     supportOddEveninRAID1E: 1;          // 0=support even drive count in RAID 1E, 1=support both odd/even
        U32     supportMultipath    : 1;            // 0=Multipath not supported, 1=Multipath and load balancing supported
        U32     abortCCOnError      : 1;            // 0=CC fix errors and continue, 1=CC shall abort on detecing inconsistency
        U32     supportAllowedOps   : 1;            // 1=Provide Allowed Ops for PD, LD and CONFIG via DCMDs, 0=not supported.
        U32     supportEnclEnumeration  : 1;        // Enclosure enumeration is 1=supported, 0=not supported.
        U32     supportEnhForeignImport : 1;        // Enhanced Foreign Import capability is 1=supported, 0=not supported.
        U32     fwAndEventTimeInGMT : 1;            // Time reported by FW and events, 0=local time, 1=GMT
        U32     supportMoreThan8Phys : 1;           // 0=API supports only 8 phys/ports etc., 1=API supports MAX_PHYS_PER_CONTROLLER
        U32     denySTPPassthrough  : 1;            // 0=Allows STP passthrough, 1=not supported
        U32     denySMPPassthrough  : 1;            // 0=Allows SMP passthrough, 1=not supported
        U32     denySCSIPassthrough : 1;            // 0=Allows SCSI passthrough, 1=not supported
        U32     globalHotSpares     : 1;            // global hot spares are supported
        U32     allowMixedRedundancyOnArray : 1;    // allow different VD on the same array to have different redundancy levels
        U32     selfDiagnostic      : 1;            // controller supports self diagnostic
        U32     foreignConfigImport : 1;            // allow foreign config import
        U32     revertibleHotSpares : 1;            // revertible spares revert back to spare status after replacement of originally failed pd
                                                    // 1=copyback is supported. All spares are revertible if copyback is enabled
        U32     dedicatedHotSpares  : 1;
        U32     spanningAllowed     : 1;
        U32     bbu                 : 1;            // battery API enabled
        U32     clusterSupported    : 1;
        U32     alarmControl        : 1;
        U32     patrolRate          : 1;
        U32     reconRate           : 1;
        U32     bgiRate             : 1;
        U32     ccRate              : 1;
        U32     rbldRate            : 1;
#endif  // MFI_BIG_ENDIAN
    } adapterOperations;

    // Supported Logical Drive operations
    struct {                                //0x5e4
#ifndef MFI_BIG_ENDIAN
        U32     readPolicy              : 1;
        U32     writePolicy             : 1;
        U32     ioPolicy                : 1; // 0=IO policy cannot be changed from MR_MFC_DEFAULTS
        U32     accessPolicy            : 1;
        U32     diskCachePolicy         : 1;
        U32     reconstruction          : 1;
        U32     denyLocate              : 1; // 0=Allows MR_DCMD_LD_LOCATE, 1=not supported
        U32     denyCC                  : 1; // 0=Allows MR_DCMD_LD_CC, 1=not supported
        U32     allowCtrlEncryption     : 1; // 1=supports controller encryption
        U32     ldbbm                   : 1; // 1=LD bad block management is enabled
        U32     supportFastPath         : 1; // 1=FastPath supported, 0=feature not supported
        U32     performanceMetrics      : 1; // 1=Support LD based Host IO performance metrics
        U32     supportPowerSavings     : 1; // 1=Support power savings (for configured drives)
        U32     supportPSMaxWithCache   : 1; // 1=Support power savings option MAX with cached writes
        U32     supportBreakMirror      : 1; // 1=Support break mirror operation
        U32     supportSSCWriteBack     : 1; // 1=SSC supports caching write IOs
        U32     supportSSCAssociation   : 1; // When set MR_DCMD_LD_SSCD_SET_LDS is necessary to create association;
                                             // otherwise regular VD policy CIO will enable the read cache.
        U32     supportSSCBoost         : 1; // 1=Support Boost Priority SSC Associated VDs
        U32     supportFastPathWB       : 1; // 1=Support Fast Path WB on Raptor Type Ld
        U32     supportAutoLockRecovery : 1; // DFF auto lock recover in non-redundant raid levels
        U32     supportScsiUnmap        : 1; // 1=LD support TRIM/UNMAP
        U32     supportHide             : 1; // 1=LD hide/unhide supported. MR_DCMD_LD_SET_HIDDEN/MR_DCMD_LD_CLEAR_HIDDEN
                                             // can be used to hide/expose the LD
        U32     supportAtomicity        : 1; // 1 =Support Atomicity (atomic write)
        U32     supportT10Atomicity     : 1; // 1 =Support T10 Atomicity (T10 atomic write CDB and VPD Pages)
        U32     disallowPropChange      : 1; // 1 =LD prop change is not allowed
        U32     supportExtLDAllowedOps  : 1; // 1 =Support MR_LD_ALLOWED_OPS_EXT
        U32     reserved                : 6;
#else
        U32     reserved                : 6;
        U32     supportExtLDAllowedOps  : 1; // 1 =Support MR_LD_ALLOWED_OPS_EXT
        U32     disallowPropChange      : 1; // 1 =LD prop change is not allowed
        U32     supportT10Atomicity     : 1; // 1 =Support T10 Atomicity (T10 atomic write CDB and VPD Pages)
        U32     supportAtomicity        : 1; // 1 =Support Atomicity (atomic write)
        U32     supportHide             : 1; // 1=LD hide/unhide supported. MR_DCMD_LD_SET_HIDDEN/MR_DCMD_LD_CLEAR_HIDDEN
                                             // can be used to hide/expose the LD
        U32     supportScsiUnmap        : 1; // 1=LD support TRIM/UNMAP
        U32     supportAutoLockRecovery : 1; // DFF auto lock recover in non-redundant raid levels
        U32     supportFastPathWB       : 1; // 1=Support Fast Path WB on Raptor Type Ld
        U32     supportSSCBoost         : 1; // 1=Support Boost Priority SSC Associated VDs
        U32     supportSSCAssociation   : 1; // When set MR_DCMD_LD_SSCD_SET_LDS is necessary to create association;
                                             // otherwise regular VD policy CIO will enable the read cache.
        U32     supportSSCWriteBack     : 1; // 1=SSC supports caching write IOs
        U32     supportBreakMirror      : 1; // 1=Support break mirror operation
        U32     supportPSMaxWithCache   : 1; // 1=Support power savings option MAX with cached writes
        U32     supportPowerSavings     : 1; // 1=Support power savings (for configured drives)
        U32     performanceMetrics      : 1; // 1=Support LD based Host IO performance metrics
        U32     supportFastPath         : 1; // 1=FastPath supported, 0=feature not supported
        U32     ldbbm                   : 1; // 1=LD bad block management is enabled
        U32     allowCtrlEncryption     : 1; // 1=supports controller encryption
        U32     denyCC                  : 1; // 0=Allows MR_DCMD_LD_CC, 1=not supported
        U32     denyLocate              : 1; // 0=Allows MR_DCMD_LD_LOCATE, 1=not supported
        U32     reconstruction          : 1;
        U32     diskCachePolicy         : 1;
        U32     accessPolicy            : 1;
        U32     ioPolicy                : 1; // 0=IO policy cannot be changed from MR_MFC_DEFAULTS
        U32     writePolicy             : 1;
        U32     readPolicy              : 1;
#endif  // MFI_BIG_ENDIAN
    } ldOperations;

    // supported "STRIP" sizes
    struct {                                //0x5e8
        U8      min;                        // minimum Strip_Size per DDF (0=512, 1=1K, 2=2K, 3=4K, ..., 7=64K, ..., 11=1MB, etc.)
        U8      max;                        // maximum Strip_Size per DDF (0=512, 1=1K, 2=2K, 3=4K, ..., 7=64K, ..., 11=1MB, etc.)
        U8      reserved[2];
    } stripeSizeOptions;    // (should really be named stripSizeOptions - GES 07/12/05)

    // supported physical drive operations
    struct {                                //0x5ec
#ifndef MFI_BIG_ENDIAN
        U32     forceOnline        : 1;     // Allows Failed->Online, Offline->Online
        U32     forceOffline       : 1;     // Allows Online->Offline, Online->Failed
        U32     forceRebuild       : 1;     // Allows Failed->Rebuild, Offline->Rebuild
        U32     denyForceFailed    : 1;     // 0=Allows Online->Failed, Offline->Failed, 1=not supported
        U32     denyForceGoodBad   : 1;     // 0=Allows unconfiguredGood->unconfiguredBad, unconfiguredBad->unconfiguredGood, 1=not supported
        U32     denyMissingReplace : 1;     // 0=Allows Failed->unconfiguredBad, Offline->unconfiguredGood, unconfiguredGood->Offline, 1=not supported
        U32     denyClear          : 1;     // 0=Allows MR_DCMD_PD_CLEAR, 1=not supported
        U32     denyLocate         : 1;     // 0=Allows MR_DCMD_PD_LOCATE, 1=not supported
        U32     supportPowerState  : 1;     // 1=Power State is supported
        U32     setPowerStateForCfg : 1;    // FW requires application to set power state before configuration changes
        U32     supportT10PowerState: 1;    // 1=Power conditions per T10 are supported
        U32     supportTemperature : 1;     // 1=PD temperature monitoring is supported
        U32     supportSS3PassVerify: 1;    // 1=PD support 3 pass erase and verify (MR_SS_ERASE_3PASS_VERIFY)
        U32     supportWCE         : 1;     // 1=MR_PD_INFO.properties.WCE and MR_PD_INFO.properties.wceUnchanged are supported
        U32     supportNCQ         : 1;     // 1=MR_PD_INFO.properties.NCQ is supported
        U32     supportLimitMaxRateSATA:1;  // 1=MR_CTRL_PROP.OnOffProperties.limitMaxRateSATA3G is supported
        U32     supportDegradedMedia:1;     // 1=PD degraded media detection is supported
        U32     supportParallelPDFwUpdate:1;// 1=FW supports parallel PD microcode update
        U32     supportCryptoErase : 1;     // (Deprecated, use supportSanitize instead) 1=FW supports cryptographic erase of ISE drives
        U32     supportSanitize    : 1;     // 1=FW supports Sanitize operation for drives
        U32     supportWearGauge   : 1;     // 1=FW supports weargauge for SSD drives
        U32     reserved           :11;     // remaining bits
#else
        U32     reserved           :11;     // remaining bits
        U32     supportWearGauge   : 1;     // 1=FW supports weargauge for SSD drives
        U32     supportSanitize    : 1;     // 1=FW supports Sanitize operation for drives
        U32     supportCryptoErase : 1;     // (Deprecated, use supportSanitize instead) 1=FW supports cryptographic erase of ISE drives
        U32     supportParallelPDFwUpdate:1;// 1=FW supports parallel PD microcode update
        U32     supportDegradedMedia:1;     // 1=PD degraded media detection is supported
        U32     supportLimitMaxRateSATA:1;  // 1=MR_CTRL_PROP.OnOffProperties.limitMaxRateSATA3G is supported
        U32     supportNCQ         : 1;     // 1=MR_PD_INFO.properties.NCQ is supported
        U32     supportWCE         : 1;     // 1=MR_PD_INFO.properties.WCE and MR_PD_INFO.properties.wceUnchanged are supported
        U32     supportSS3PassVerify: 1;    // 1=PD support 3 pass erase and verify (MR_SS_ERASE_3PASS_VERIFY)
        U32     supportTemperature : 1;     // 1=PD temperature monitoring is supported
        U32     supportT10PowerState: 1;    // 1=Power conditions per T10 are supported
        U32     setPowerStateForCfg : 1;    // FW requires application to set power state before configuration changes
        U32     supportPowerState  : 1;     // 1=Power State is supported
        U32     denyLocate         : 1;     // 0=Allows MR_DCMD_PD_LOCATE, 1=not supported
        U32     denyClear          : 1;     // 0=Allows MR_DCMD_PD_CLEAR, 1=not supported
        U32     denyMissingReplace : 1;     // 0=Allows Failed->unconfiguredBad, Offline->unconfiguredGood, unconfiguredGood->Offline, 1=not supported
        U32     denyForceGoodBad   : 1;     // 0=Allows unconfiguredGood->unconfiguredBad, unconfiguredBad->unconfiguredGood, 1=not supported
        U32     denyForceFailed    : 1;     // 0=Allows Online->Failed, Offline->Failed, 1=not supported
        U32     forceRebuild       : 1;     // Allows Failed->Rebuild, Offline->Rebuild
        U32     forceOffline       : 1;     // Allows Online->Offline, Online->Failed
        U32     forceOnline        : 1;     // Allows Failed->Online, Offline->Online
#endif  // MFI_BIG_ENDIAN
    } pdOperations;

    // supported physical drive mix on controller, encl, LD
    struct {                                //0x5f0
#ifndef MFI_BIG_ENDIAN
        U32     ctrlSupportsSAS     : 1;
        U32     ctrlSupportsSATA    : 1;
        U32     allowMixInEnclosure : 1;
        U32     allowMixInLD        : 1;
        U32     allowSataInCluster  : 1;
        U32     allowSSDMixInLD     : 1;
        U32     allowMixSSDHDDInLD  : 1;
        U32     allowMixSSMandSFMinLD : 1;
        U32     reserved            : 24;            // remaining bits
#else
        U32     reserved            : 24;            // remaining bits
        U32     allowMixSSMandSFMinLD : 1;
        U32     allowMixSSDHDDInLD  : 1;
        U32     allowSSDMixInLD     : 1;
        U32     allowSataInCluster  : 1;
        U32     allowMixInLD        : 1;
        U32     allowMixInEnclosure : 1;
        U32     ctrlSupportsSATA    : 1;
        U32     ctrlSupportsSAS     : 1;
#endif  // MFI_BIG_ENDIAN
    } pdMixSupport;

    // define ECC single-bit-error bucket information
    U16         eccBucketCount;             //0x5f4 // count of single-bit ECC errors currently in the bucket
    U8          reserved2[2];               //0x5f6

    // Adapter's dynamically updated status/operation.
    // These bits indicate adapter behavior that is not static, nor sticky and may change in realtime.
    struct {                                //0x5f8
#ifndef MFI_BIG_ENDIAN
        U16     supportPdFwDownload    : 1;
        U16     lockKeyAssigned        : 1; // 1=controller is assigned a lock key
        U16     lockKeyFailed          : 1; // 1=failed to get lock key on bootup
        U16     lockKeyNotBackedup     : 1; // 1=lock key has not been backed up
        U16     biosNotDetected        : 1; // 1=bios was not detected during boot
        U16     cacheIsPinned          : 1; // 1=controller cache is pinned for missing or offline LDs
        U16     lockKeyRebootRequired  : 1; // 1=controller must be rebooted to complete security operation
        U16     rollbackInProgress     : 1; // 1=a rollback operation is in progress
        U16     PFKInNVRAM             : 1; // At least one PFK exists in NVRAM
        U16     SSCPolicyWB            : 1; // 1=SSC policy is WB, 0=if SSC exists, its read only cache
        U16     safeModeBoot           : 1; // 1=controller has booted into safe mode, 0=normal boot mode
        U16     maintenanceMode        : 2; // 0=normal boot mode, see MR_CTRL_MAINTENANCE_MODE for other values
        U16     fwFlashDeviceStatus    : 2; // 0=ok, 1=degraded, 2 = fail, 3 = unknown. supportFlashStatus has to be 1
        U16     systemResetRequired    : 1; // 1= System reset required
#else
        U16     systemResetRequired    : 1; // 1= System reset required
        U16     fwFlashDeviceStatus    : 2; // 0=ok, 1=degraded, 2 = fail, 3 = unknown. supportFlashStatus has to be 1
        U16     maintenanceMode        : 2; // 0=normal boot mode, see MR_CTRL_MAINTENANCE_MODE for other values
        U16     safeModeBoot           : 1; // 1=controller has booted into safe mode, 0=normal boot mode
        U16     SSCPolicyWB            : 1; // 1=SSC policy is WB, 0=if SSC exists, its read only cache
        U16     PFKInNVRAM             : 1; // At least one PFK exists in NVRAM
        U16     rollbackInProgress     : 1; // 1=a rollback operation is in progress
        U16     lockKeyRebootRequired  : 1; // 1=controller must be rebooted to complete security operation
        U16     cacheIsPinned          : 1; // 1=controller cache is pinned for missing or offline LDs
        U16     biosNotDetected        : 1; // 1=bios was not detected during boot
        U16     lockKeyNotBackedup     : 1; // 1=lock key has not been backed up
        U16     lockKeyFailed          : 1; // 1=failed to get lock key on bootup
        U16     lockKeyAssigned        : 1; // 1=controller is assigned a lock key
        U16     supportPdFwDownload    : 1;
#endif  // MFI_BIG_ENDIAN
    } adapterStatus;

    // define API versioning
    U16         mrAPIVersion;               //0x5fa // = MR_API_VERSION
    U16         mrAPIFwdCompatibility;      //0x5fc // = MR_API_FWD_COMPATIBILITY
    U16         mrAPIBkwdCompatibility;     //0x5fe // = MR_API_BKWD_COMPATIBILITY

    /*
     * include the controller properties (changable items)
     */
    MR_CTRL_PROP properties;                //0x600 // changeable properties

    /*
     * define FW PACKAGE version (set in FW environment variables on an OEM-basis)
     */
    char        packageVersion[0x60];       //0x640 // firmware package version (NULL if not defined)

    /*
     * If adapterOperations.supportMoreThan8Phys is set, and deviceInterface.portCount is greater than 8,
     * SAS Addrs for first 8 ports shall be populated in deviceInterface.portAddr, and the rest shall be
     * populated in deviceInterfacePortAddr2.
     */
    U64         deviceInterfacePortAddr2[16]; //0x6a0 // extended backend addresses (i.e. SAS Addr)
    U8          reserved3[64];                //0x720 // Space reserved for 8 additional SAS Addr,
                                                      // in case MAX_PHYS_PER_CONTROLLER exceeds 24.
    // Drive Min-Max for supported RAID levels
    struct {                                //0x760
#ifndef MFI_BIG_ENDIAN
        U16 minPdRaidLevel_0                : 4;
        U16 maxPdRaidLevel_0                : 12;

        U16 minPdRaidLevel_1                : 4;
        U16 maxPdRaidLevel_1                : 12;

        U16 minPdRaidLevel_5                : 4;
        U16 maxPdRaidLevel_5                : 12;

        U16 minPdRaidLevel_1E               : 4;
        U16 maxPdRaidLevel_1E               : 12;

        U16 minPdRaidLevel_6                : 4;
        U16 maxPdRaidLevel_6                : 12;

        U16 minPdRaidLevel_10               : 4;
        U16 maxPdRaidLevel_10               : 12;

        U16 minPdRaidLevel_50               : 4;
        U16 maxPdRaidLevel_50               : 12;

        U16 minPdRaidLevel_60               : 4;
        U16 maxPdRaidLevel_60               : 12;

        U16 minPdRaidLevel_1E_RLQ0          : 4;
        U16 maxPdRaidLevel_1E_RLQ0          : 12;

        U16 minPdRaidLevel_1E0_RLQ0         : 4;
        U16 maxPdRaidLevel_1E0_RLQ0         : 12;
#else
        U16 maxPdRaidLevel_0                : 12;
        U16 minPdRaidLevel_0                : 4;

        U16 maxPdRaidLevel_1                : 12;
        U16 minPdRaidLevel_1                : 4;

        U16 maxPdRaidLevel_5                : 12;
        U16 minPdRaidLevel_5                : 4;

        U16 maxPdRaidLevel_1E               : 12;
        U16 minPdRaidLevel_1E               : 4;

        U16 maxPdRaidLevel_6                : 12;
        U16 minPdRaidLevel_6                : 4;

        U16 maxPdRaidLevel_10               : 12;
        U16 minPdRaidLevel_10               : 4;

        U16 maxPdRaidLevel_50               : 12;
        U16 minPdRaidLevel_50               : 4;

        U16 maxPdRaidLevel_60               : 12;
        U16 minPdRaidLevel_60               : 4;

        U16 maxPdRaidLevel_1E_RLQ0          : 12;
        U16 minPdRaidLevel_1E_RLQ0          : 4;

        U16 maxPdRaidLevel_1E0_RLQ0         : 12;
        U16 minPdRaidLevel_1E0_RLQ0         : 4;
#endif  // MFI_BIG_ENDIAN

        U16 reserved[6];                    // Reserved for future (like 1ee, 5ee, 00, etc)
    } pdsForRaidLevels;

    U16 maxPds;                             //0x780 // Max Physical Drives per Controller
    U16 maxDedHSPs;                         //0x782 // Max Dedicated Hotspares per Controller
    U16 maxGlobalHSPs;                      //0x784 // Max Global Hotspares per Controller
    U16 ddfSize;                            //0x786 // DDF Structure Size in terms of Mbytes
    U8  maxLdsPerArray;                     //0x788 // Max LD's per Array
    U8  partitionsInDDF;                    //0x789 // Max LD's per Array in configuration
    U8  lockKeyBinding;                     //0x78a // Type of lock key binding (MR_CTRL_LOCK_KEY_BINDING)
    U8  maxPITsPerLd;                       //0x78b // Max PITs per LD
    U8  maxViewsPerLd;                      //0x78c // Max Views per LD
    U8  maxTargetId;                        //0x78d // Max TargetId value that can be assigned to an LD/VIEW.
    U16 maxBvlVdSize;                       //0x78e // maximum VD size in Tbytes for BVL features like Snapshot

    U16 maxConfigurableSSCSize;             //0x790 // Size of SSC that can yet be configured, in GB.
    U16 currentSSCsize;                     //0x792 // Size of currently configured SSC, in GB.

    char    expanderFwVersion[12];          //0x794 // if MR_CTRL_INFO.hwPresent.expander is present.  (NULL=not available).

    U16 PFKTrialTimeRemaining;              //0x7A0 // Time remaining for any features activated by a trial PFK

    U16 cacheMemorySize;                    //0x7A2 // DRAM memory (MB) configured for cache usage

    // supported Adapter operations continued, extension of adpaterOperations
    struct {                                //0x7A4
#ifndef MFI_BIG_ENDIAN
        U32     supportPIcontroller         :1;         // 1=LD SCSI PI Type 0, but controller uses PI internally
        U32     supportLdPIType1            :1;         // 1=LD SCSI PI Type 1 may be created
        U32     supportLdPIType2            :1;         // 1=LD SCSI PI Type 2 may be created
        U32     supportLdPIType3            :1;         // 1=LD SCSI PI Type 3 may be created
        U32     supportLdBBMInfo            :1;         // 1=LD BBM (bad block mgmt) table API is available
        U32     supportShieldState          :1;         // 1=Support PD shield state (diagnostics on array PD)
        U32     blockSSDWriteCacheChange    :1;         // 1 = Block SSD write cache settings from its MFG settings
        U32     supportSuspendResumeBGops   :1;         // 1 = support suspend and resume of background operations
        U32     supportEmergencySpares      :1;         // 1 = support Emergency spare feature
        U32     supportSetLinkSpeed         :1;         // 1 = supports setting phy link speed
        U32     supportBootTimePFKChange    :1;         // 1 = FW supports boot time PFK, supportPFK has to be 1
        U32     supportJBOD                 :1;         // 1 = JBOD mode supported
        U32     disableOnlinePFKChange      :1;         // 1 = PFK is NOT supported online, supportPFK has to be 1
        U32     supportPerfTuning           :1;         // 1 = FW supports perf tuning modes (MR_PERF_TUNE_MODES)
        U32     supportSSDPatrolRead        :1;         // support for SSDPatrolRead
        U32     realTimeScheduler           :1;         // 1 = FW scheduled operations will always trigger at the specified time on a weekly, or
                                                        //     on a daily frequency
        U32     supportResetNow             :1;         // 1 = FW supports user initiated online reset
        U32     supportEmulatedDrives       :1;         // 1 = FW supports 512e drives
        U32     headlessMode                :1;         // 1 = FW supports headless modes
        U32     dedicatedHotSparesLimited   :1;         // 1 = Hot spare can be dedicated to one or more arrays, as long as all the LDs mapped to
                                                        //     these arrays are same.
                                                        //     For spanned LDs, the hot spare must be dedicated to all constituent arrays.
        U32     supportUnevenSpans         : 1;         // Span will be auto selected by FW, exposed thru MR_DCMD_LD_GET_SPAN_LIST
        U32     supportPointInTimeProgress : 1;         // 1 = FW provides MR_PROGRESS.elapsedSecsForLastPercent for some operations
        U32     supportDataLDonSSCArray     :1;         // 1 = FW supports a portion of SSC array to be exposed as data LD(s)
        U32     mpio                        :1;         // 1=Supports multipath VD (eg. TGPS ALUA)
        U32     supportConfigAutoBalance    :1;         // 0=no configuration auto balance support, 1=configuration auto balance support is present
                                                        // i.e. user can set enable/disable config auto balance
        U32     activePassive               :2;         // 00=Undefined, 01=Active/Passive in HA
                                                        // 10=Active/Active in HA, 11=reserved
//      U32     supportVirtualCache         :1;         // Deprecated and replaced with supportVCorDLC
        U32     supportVCorDLC              :1;         // 1 = FW supports Virtual Cache (VC) on SSC array
                                                        // or support Dynamic Logical Capacity (DLC) on Flash VD
        U32     supportExtEnclDrvCount      :1;         // >64 drive support in an enclosure
        U32     supportMaintenanceMode      :1;         // 1=FW supports maintenance mode
        U32     supportDiagResults          :1;         // FW provides diag status for each test
        U32     supportPowerThrottle        :1;         // 1=FW support power throttle based on MR_CTRL_POWER_USAGE
#else
        U32     supportPowerThrottle        :1;         // 1=FW support power throttle based on MR_CTRL_POWER_USAGE
        U32     supportDiagResults          :1;         // FW provides diag status for each test
        U32     supportMaintenanceMode      :1;         // 1=FW supports maintenance mode
        U32     supportExtEnclDrvCount      :1;         // >64 drive support in an enclosure
//      U32     supportVirtualCache         :1;         // Deprecated and replaced with supportVCorDLC
        U32     supportVCorDLC              :1;         // 1 = FW supports Virtual Cache (VC) on SSC array
                                                        // or support Dynamic Logical Capacity (DLC) on Flash VD
        U32     activePassive               :2;         // 00=Undefined, 01=Active/Passive in HA
                                                        // 10=Active/Active in HA, 11=reserved
        U32     supportConfigAutoBalance    :1;         // 0=no configuration auto balance support, 1=configuration auto balance support is present
                                                        // i.e. user can set enable/disable config auto balance
        U32     mpio                        :1;         // 1=Supports multipath VD (eg. TGPS ALUA)
        U32     supportDataLDonSSCArray     :1;         // 1 = FW supports a portion of SSC array to be exposed as data LD(s)
        U32     supportPointInTimeProgress  :1;         // 1 = FW provides MR_PROGRESS.elapsedSecsForLastPercent for some operations
        U32     supportUnevenSpans          :1;         // Span will be auto selected by FW, exposed thru MR_DCMD_LD_GET_SPAN_LIST
        U32     dedicatedHotSparesLimited   :1;         // 1 = Hot spare can be dedicated to one or more arrays, as long as all the LDs mapped to
                                                        //     these arrays are same.
                                                        //     For spanned LDs, the hot spare must be dedicated to all constituent arrays.
        U32     headlessMode                :1;         // 1 = FW supports headless modes
        U32     supportEmulatedDrives       :1;         // 1 = FW supports 512e drives
        U32     supportResetNow             :1;         // 1 = FW supports user initiated online reset
        U32     realTimeScheduler           :1;         // 1 = FW scheduled operations will always trigger at the specified time on day of weekly, or
                                                        //     on a daily frequency
        U32     supportSSDPatrolRead        :1;         // support for SSDPatrolRead
        U32     supportPerfTuning           :1;         // 1 = FW supports perf tuning modes (MR_PERF_TUNE_MODES)
        U32     disableOnlinePFKChange      :1;         // 1 = PFK is NOT supported online, supportPFK has to be 1
        U32     supportJBOD                 :1;         // 1 = JBOD mode supported
        U32     supportBootTimePFKChange    :1;         // 1 = FW supports boot time PFK, supportPFK has to be 1
        U32     supportSetLinkSpeed         :1;         // 1 = supports setting phy link speed
        U32     supportEmergencySpares      :1;         // 1 = support Emergency spare feature
        U32     supportSuspendResumeBGops   :1;         // 1 = support suspend and resume of background operations
        U32     blockSSDWriteCacheChange    :1;         // 1 = Block SSD write cache settings from its MFG settings
        U32     supportShieldState          :1;         // 1=Support PD shield state (diagnostics on array PD)
        U32     supportLdBBMInfo            :1;         // 1=LD BBM (bad block mgmt) table API is available
        U32     supportLdPIType3            :1;         // 1=LD SCSI PI Type 3 may be created
        U32     supportLdPIType2            :1;         // 1=LD SCSI PI Type 2 may be created
        U32     supportLdPIType1            :1;         // 1=LD SCSI PI Type 1 may be created
        U32     supportPIcontroller         :1;         // 1=SCSI PI Type 0 presented to host, but controller uses PI internally
#endif  // MFI_BIG_ENDIAN
    } adapterOperations2;

    U8  driverVersion[32];                  //0x7A8 // driver version (null terminated)

    U8  maxDAPdCountSpinup60;               //0x7C8 // Max direct-attach drives that can be spun up in 60 secods

    U8  temperatureROC;                     //0x7C9 // temperature of ROC in Celsius, if hwPresent.tempSensorROC (0xFF - invalid temp)
    U8  temperatureCtrl;                    //0x7CA // temperature of controller in Celsius, if hwPresent.tempSensorCtrl (0xFF - invalid temp)
    U8  reserved4;                          //0x7CB
    U16 maxConfigurablePds;                 //0x7CC // 0 means 16 to keep compatibility with older controllers, non-zero reflects actual value
                                                    // This field has meaning only when enableJBOD is enabled.

    U8  maxTransportableVDs;                //0x7CD // Indicates the max number of VDs that can be marked as transport ready
    U8  maxTransportableVDsMsb;             //0x7CE // takes the MSB value of maxLDs param to support max maxTransportableVDs more than 256

    /*
     * HA cluster information
     */
    struct {
#ifndef MFI_BIG_ENDIAN                      //0x7D0
        U32     peerIsPresent               :1;         // 1=Peer controller is present
        U32     peerIsIncompatible          :1;         // 1=Peer controller is incompatible,  HA cluster is not available
                                                        //  see other bits in this structure for reason
        U32     hwIncompatible              :1;         // 1=controller hardware incompatible
        U32     fwVersionMismatch           :1;         // 1=FW versions don't match with peer controller
        U32     ctrlPropIncompatible        :1;         // 1=Controller properties don't match with peer controller
        U32     premiumFeatureMismatch      :1;         // 1=Premium Features don't match with peer controller
        U32     passive                     :1;         // 1=FW is in passive mode, 0 = FW is in active mode, applicable for HA
        U32     exclusiveLDNotSupported     :1;         // 0=FW supports exclusive LDs, 1=FW supports only shared or non-cluster LDs
        U32     securityKeyMismatch         :1;         // 0=No security key mismatch between controllers, 1=Security key mismatch between controllers
        U32     currentTopology             :2;         // 0=Cluster/Scaleout,1=SSHA,2=Simple Pooling,3=Advanced Pooling
        U32     reserved                    :21;
#else
        U32     reserved                    :21;
        U32     currentTopology             :2;         // 0=Cluster/Scaleout,1=SSHA,2=Simple Pooling,3=Advanced Pooling
        U32     securityKeyMismatch         :1;         // 0=No security key mismatch between controllers, 1=Security key mismatch between controllers
        U32     exclusiveLDNotSupported     :1;         // 0=FW supports exclusive LDs, 1=FW supports only shared or non-cluster LDs
        U32     passive                     :1;         // 1=FW is in passive mode, 0 = FW is in active mode, applicable for HA
        U32     premiumFeatureMismatch      :1;         // 1=Premium Features don't match with peer controller
        U32     ctrlPropIncompatible        :1;         // 1=Controller properties don't match with peer controller
        U32     fwVersionMismatch           :1;         // 1=FW versions don't match with peer controller
        U32     hwIncompatible              :1;         // 1=controller hardware incompatible
        U32     peerIsIncompatible          :1;         // 1=Peer controller is incompatible
        U32     peerIsPresent               :1;         // 1=Peer controller is present
#endif
    } cluster;

    char clusterId[16];                     //0x7D4 // Unique ID for group of controllers in the same domain (not null terminated)
                                                   //For e.g : LSI 2 node solution, it is cluster identifier, LSI 2 MRSS solution, it is
                                                   // multipathing domain identifier (it can be same cluster identifier. It requires to
                                                   //be unique in both the contorller). For Hydra, it is same as cluster identifier
                                                   //For Hydra, it is same as cluster identifier
    struct {
        U8  maxVFsSupported;                //0x7E4 reflects HW supported max VFs
        U8  numVFsEnabled;                  //0x7E5 reflects FW enabled number of VFs
        U8  requestorId;                    //0x7E6 0:PF, 1:VF1, 2:VF2,...
        U8  reserved;                       //0x7E7 reserved
    } iov;

    struct {
#ifndef MFI_BIG_ENDIAN
        U32     supportPersonalityChange     :2;         // 0x0 = FW doesnt support personality change (get and set)
                                                         // 0x1 = FW supports personality change between the supported modes
                                                         // 0x2 = FW supports personality get, set not possible
                                                         // 0x3 = reserved
        U32     supportThermalPollInterval   :1;         // FW supports setting the poll interval for temperature monitoring
        U32     supportDisableImmediateIO    :1;         // FW supports enable/disable Legacy ImmeditateIO
        U32     supportT10RebuildAssist      :1;         // FW supports T10 Rebuild Assist
        U32     supportMaxExtLDs             :1;         // FW supports MAX_API_LOGICAL_DRIVES_EXT LDs
        U32     supportCrashDump             :1;         // FW supports crash dump
        U32     supportSwZone                :1;         // FW supports software zone
        U32     supportDebugQueue            :1;         // FW supports debug queue
        U32     supportNVCacheErase          :1;         // FW supports NVCache Erase
        U32     supportForceTo512e           :1;         // FW supports forcing LD to 512e irrespective of PD
        U32     supportHOQRebuild            :1;         // FW supports head of queue rebuild
        U32     supportAllowedOpsforDrvRemoval:1;        // PD allowed ops bit prepareForRemoval can be used to know whether
                                                         // the drive is ready for removal
        U32     supportDrvActivityLEDSetting :1;         // Drive activity LED setting available in controller properties
        U32     supportNVDRAM                :1;         // FW supports NVDRAM
        U32     supportForceFlash            :1;         // FW supports force flash option to update controller firmware
        U32     supportDisableSESMonitoring  :1;         // FW supports disbaling SESMonitoring
        U32     supportCacheBypassModes      :1;         // FW supports cache bypass modes
        U32     supportSecurityonJBOD        :1;         // FW supports secure enabled system PDs/JBODs
        U32     discardCacheDuringLDDelete   :1;         // FW supports cache to be discarded during LD deletion
        U32     supportTTYLogCompression     :1;         // FW support TTY log compression
        U32     supportCPLDUpdate            :1;         // FW supports CPLD update
        U32     supportDiskCacheSettingForSysPDs:1;      // FW supports disk cache setting for all the system PDs
        U32     supportExtendedSSCSize       :1;         // FW supports >512G SSC size. When set, applications can read the
                                                         // configured SSC size in blocks
        U32     supportCfgSeqNumForSysPDs    :1;         // FW supports config sequence number for system PDs
        U32     supportFWFlashDeviceStatus   :1;         // FW supports status of FW used FLASH device
        U32     supportMoreThan16Phys        :1;         // 0=API supports at most 16 phys/ports etc., 1=API supports MAX_PHYS_PER_CONTROLLER
        U32     supportCacheFlushWriteVerify :1;         // FW supports write verify during cache flush
        U32     supportLargeQD               :1;         // FW supports >256 QD while in iMR mode
        U32     supportSRIOVOptions          :1;         // FW support MR_CTRL_PROP.sriovOptions
        U32     supportLargeIO               :1;         // FW supports >256K IO
        U32     supportHostDebug             :1;         // FW supports MR_DCMD_CTRL_SET_DEBUG_ATTRIBUTES

#else
        U32     supportHostDebug             :1;         // FW supports MR_DCMD_CTRL_SET_DEBUG_ATTRIBUTES
        U32     supportLargeIO               :1;         // FW supports >256K IO
        U32     supportSRIOVOptions          :1;         // FW support MR_CTRL_PROP.sriovOptions
        U32     supportLargeQD               :1;         // FW supports >256 QD while in iMR mode
        U32     supportCacheFlushWriteVerify :1;         // FW supports write verify during cache flush
        U32     supportMoreThan16Phys        :1;         // 0=API supports at most 16 phys/ports etc., 1=API supports MAX_PHYS_PER_CONTROLLER
        U32     supportFWFlashDeviceStatus   :1;         // FW supports status of FW used FLASH device
        U32     supportCfgSeqNumForSysPDs    :1;         // FW supports config sequence number for system PDs
        U32     supportExtendedSSCSize       :1;         // FW supports >512G SSC size. When set, applications can read the
                                                         // configured SSC size in blocks
        U32     supportDiskCacheSettingForSysPDs:1;      // FW supports disk cache setting for all the system PDs
        U32     supportCPLDUpdate            :1;         // FW supports CPLD update
        U32     supportTTYLogCompression     :1;         // FW support TTY log compression
        U32     discardCacheDuringLDDelete   :1;         // FW supports cache to be discarded during LD deletion
        U32     supportSecurityonJBOD        :1;         // FW supports secure enabled system PDs/JBODs
        U32     supportCacheBypassModes      :1;         // FW supports cache bypass modes
        U32     supportDisableSESMonitoring  :1;         // FW supports disbaling SESMonitoring
        U32     supportForceFlash            :1;         // FW supports force flash option to enable controller firmware
        U32     supportNVDRAM                :1;         // FW supports NVDRAM
        U32     supportDrvActivityLEDSetting :1;         // Drive activity LED setting available in controller properties
        U32     supportAllowedOpsforDrvRemoval:1;        // PD allowed ops bit prepareForRemoval can be used to know whether
                                                         // the drive is ready for removal
        U32     supportHOQRebuild            :1;         // FW supports head of queue rebuild
        U32     supportForceTo512e           :1;         // FW supports forcing LD to 512e irrespective of PD
        U32     supportNVCacheErase          :1;         // FW supports NVCache Erase
        U32     supportDebugQueue            :1;         // FW supports debug queue
        U32     supportSwZone                :1;         // FW supports software zone
        U32     supportCrashDump             :1;         // FW supports crash dump
        U32     supportMaxExtLDs             :1;         // FW supports MAX_API_LOGICAL_DRIVES_EXT LDs
        U32     supportT10RebuildAssist      :1;         // FW supports T10 Rebuild Assist
        U32     supportDisableImmediateIO    :1;         // FW supports enable/disable Legacy ImmeditateIO
        U32     supportThermalPollInterval   :1;         // FW supports setting the poll interval for temperature monitoring
        U32     supportPersonalityChange     :2;         // 0x0 = FW doesnt support personality change (get and set)
                                                         // 0x1 = FW supports personality change between the supported modes
                                                         // 0x2 = FW supports personality get, set not possible
                                                         // 0x3 = reserved

#endif
    } adapterOperations3;

    struct {
#ifndef MFI_BIG_ENDIAN
        U8 cpldInFlash:1;                                // Indicates whether the CPLD image is part of the package and stored in flash
        U8 reserved:7;
#else
        U8 reserved:7;
        U8 cpldInFlash:1;                                // Indicates whether the CPLD image is part of the package and stored in flash
#endif
        U8 reserved1 [3];
        U8 userCodeDefinition [12];                      // Null terminated string. Has the version information if cpldInFlash = FALSE
    } cpld;                                              // Valid only if upgradableCPLD is TRUE

    struct {
#ifndef MFI_BIG_ENDIAN
        U16 ctrlInfoExtSupported:1;
        U16 supportIbuttonLess  :1;
        U16 supportedEncAlgo    :1;
        U16 supportEncryptedMfc :1;
        U16 imageUploadSupported:1;
        U16 supportSESCtrlInMultipathCfg:1;              // FW supports LUN based association and target port based
                                                         // association for the SES device connected in multipath mode
        U16 supportPdMapTargetId:1;                      // FW defines Jbod target Id within MR_PD_CFG_SEQ
        U16 FWSwapsBBUVPDInfo   :1;                      // FW swaps relevant fields in MR_BBU_VPD_INFO_FIXED to provide the data in little endian order
        U16 supportSSCrev3      :1;                      // FW supports CacheCade 3.0, only one SSCD creation allowed
        U16 supportDualFWUpdate :1;                      // FW supports dual firmware update feature
        U16 supportHostInfo     :1;                      // FW supports MR_DCMD_CTRL_HOST_INFO_SET/GET
        U16 supportFlashCompInfo:1;                      // FW supports MR_DCMD_CTRL_FLASH_COMP_INFO_GET
        U16 supportPLDebugInfo  :1;                      // FW supports retrieval of PL debug information through apps
        U16 supportNVMePassthru :1;                      // FW supports NVMe passthru commands
        U16 supportOOBCtrlFwDownload:1;                  // FW supports controller FW download using OOB
        U16 DivertIOWithChainFrame:1;                    // FW supports diverting FP IO with chain frame
#else
        U16 DivertIOWithChainFrame:1;                    // FW supports diverting FP IO with chain frame
        U16 supportOOBCtrlFwDownload:1;
        U16 supportNVMePassthru :1;
        U16 supportPLDebugInfo  :1;
        U16 supportFlashCompInfo:1;
        U16 supportHostInfo     :1;
        U16 supportDualFWUpdate :1;
        U16 supportSSCrev3      :1;
        U16 FWSwapsBBUVPDInfo   :1;
        U16 supportPdMapTargetId:1;
        U16 supportSESCtrlInMultipathCfg:1;
        U16 imageUploadSupported:1;
        U16 supportEncryptedMfc :1;
        U16 supportedEncAlgo    :1;
        U16 supportIbuttonLess  :1;
        U16 ctrlInfoExtSupported:1;
#endif

    } adapterOperations4;

    U8  pad[0x800-0x7FE];                       //pad to 2K

    U32             size;                       // size of total data structure (including size field)

    U32             pad1;

    struct {
        U16         maxLDs;                     // max number of LDs FW will support this value may change
        U16         maxPds;                     // Max Physical Drives per Controller
        U16         maxTargetId;                // Max TargetId value that can be assigned to an LD/VIEW.
        U16         maxArrays;                  // max arrays
        U16         maxArms;                    // max arms per LD
        U16         maxSpans;                   // max spans per LD
        U16         maxDedHSPs;                 // Max Dedicated Hotspares per Controller
        U16         maxGlobalHSPs;              // Max Global Hotspares per Controller
        U16         maxLdsPerArray;             // Max LD's per Array
        U8          maxDevicePhy;               // Max maximum number of device PHYs per controller
        U8          reserved[13];
    } supportedProfileConfig;

    struct {
        U16         maxLDs;                     // max number of LDs FW will support this value may change
        U16         maxPds;                     // Max Physical Drives per Controller
        U16         maxTargetId;                // Max TargetId value that can be assigned to an LD/VIEW.
        U16         maxArrays;                  // max arrays
        U16         maxArms;                    // max arms per LD
        U16         maxSpans;                   // max spans per LD
        U16         maxDedHSPs;                 // Max Dedicated Hotspares per Controller
        U16         maxGlobalHSPs;              // Max Global Hotspares per Controller
        U16         maxLdsPerArray;             // Max LD's per Array
        U8          maxDevicePhy;               // Max maximum number of device PHYs per controller
        U8          reserved[13];
    } supportedFWConfig;

    struct {
        struct {
#ifndef MFI_BIG_ENDIAN
        U32 mrConfigExt2Supported:1;            // FW Supports following formats
                                                // MR_ARRAY_EXT
                                                // MFI_REG_STATE_EXT2
                                                // MAX_API_PHYSICAL_DEVICES_EXT2
                                                // MAX_API_LOGICAL_DRIVES_EXT2
                                                // MAX_API_ARRAYS_EXT2
                                                // MAX_API_TARGET_ID_EXT2
                                                // MR_PR_PROPERTIES_EXT2
                                                // MR_CC_SCHEDULE_PROPERTIES_EXT2
        U32 supportProfileChange:2;             // 0x0 = FW doesnt support Profile Mgmt
                                                // 0x1 = FW supports profile change between the supported profiles
                                                // 0x2 = FW supports profile list get, set not possible
                                                // 0x3 = reserved;
        U32 supportCVHealthInfo:1;              // FW supports cachevault health info
        U32 supportPCIe:1;                      // 0x0 = FW doesnt support NVMe/PCIe devices
        U32 supportExtMfgVPD:1;                 // Support MPB VPD pages
        U32 supportOCEOnly:1;                   // No raid level change is supported during RLM (OCE only)
        U32 supportNVMeTM:1;                    // Support task management commands for NVMe devices
        U32 supportSnapDump:1;                  // context based snapdump support
        U32 supportFDETypeMix:1;                // This applies FDE type mixing for security enabled arrays
                                                // 0 = disable different FDE protocol mix, 1 = enable different FDE protocol mix
        U32 supportForcePersonalityChange:1;    // 0x1 = FW supports forced personality change operation
                                                // 0x0 = FW does not support forced personality change operation
        U32 supportPSOCUpdate:1;                // FW supports PSOC update
        U32 supportSecureBoot:1;                // HW-based secure boot is supported
        U32 supportOnDemandSnapDump:1;          // On demand snapdump support
        U32 supportClearSnapDump:1;             // Clear snapdump support
        U32 disableLeastLatencyMode:1;          // 0x1 = FW does not support least latency mode
        U32 supportSMPOverOOB:1;                // 0x0 = FW doesn't support SMP passthrough command on OOB interface
                                                // 0x1 = FW support SMP passthrough command on OOB interface
        U32 reserved:15;
#else
        U32 reserved:15;
        U32 supportSMPOverOOB:1;                // 0x0 = FW doesn't support SMP passthrough command on OOB interface
                                                // 0x1 = FW support SMP passthrough command on OOB interface
        U32 disableLeastLatencyMode:1;          // 0x1 = FW does not support least latency mode
        U32 supportClearSnapDump:1;             // Clear snapdump support
        U32 supportOnDemandSnapDump:1;          // On demand snapdump support
        U32 supportSecureBoot:1;                // HW-based secure boot is supported
        U32 supportPSOCUpdate:1;                // FW supports PSOC update
        U32 supportForcePersonalityChange:1;    // 0x1 = FW supports forced personality change operation
                                                // 0x0 = FW does not support forced personality change operation
        U32 supportFDETypeMix:1;                // This applies FDE type mixing for security enabled arrays
                                                // 0 = disable different FDE protocol mix, 1 = enable different FDE protocol mix
        U32 supportSnapDump:1;                  // context based snapdump support
        U32 supportNVMeTM:1;                    // Support task management commands for NVMe devices
        U32 supportOCEOnly:1;                   // No raid level change is supported during RLM (OCE only)
        U32 supportExtMfgVPD:1;                 // Support MPB VPD pages
        U32 supportPCIe:1;                      // 0x0 = FW doesnt support NVMe/PCIe devices
        U32 supportCVHealthInfo:1;              // FW supports cachevault health info
        U32 supportProfileChange:2;             // 0x0 = FW doesnt support Profile Mgmt
                                                // 0x1 = FW supports profile change between the supported profiles
                                                // 0x2 = FW supports profile list get, set not possible
                                                // 0x3 = reserved;
        U32 mrConfigExt2Supported:1;            // FW Supports following formats
                                                // MR_ARRAY_EXT
                                                // MFI_REG_STATE_EXT2
                                                // MAX_API_PHYSICAL_DEVICES_EXT2
                                                // MAX_API_LOGICAL_DRIVES_EXT2
                                                // MAX_API_ARRAYS_EXT2
                                                // MAX_API_TARGET_ID_EXT2
                                                // MR_PR_PROPERTIES_EXT2
                                                // MR_CC_SCHEDULE_PROPERTIES_EXT2
#endif
        } adapterOperations5;

        U32 rsvdForAdptOp[63];                  // Reserve future Adapter operations.
    } adapterOperationExt;

    // CacheCade 3.0 new elements
    U16         minConfigurableSSCSize;         // minimum supported size of SSCD (in GB)
    U8          maxSSCDs;                       // maximum number of SSCDs allowed (value 0 means legacy value of 16)

    U8          NVMeTaskAbortTO;                // Timeout value in seconds used by Nvme Abort Task TM Request.
    U8          NVMeMaxControllerResetTO;       // Nvme Max Support Controller Reset timeout in seconds.
    U8          reserved5[1];                   // reserved
    // Adapter's dynamically updated status/operation.
    // These bits indicate adapter behavior that is not static, nor sticky and may change in realtime.
    struct {
#ifndef MFI_BIG_ENDIAN
        U16     systemShutdownRequired : 1;  // 1= System shutdown required
        U16     reserved               : 15;
#else
        U16     reserved               : 15;
        U16     systemShutdownRequired : 1;  // 1= System shutdown required
#endif  // MFI_BIG_ENDIAN
    } adapterStatus2;

    // Secure boot elements
    U8          secureBootHashSlotsRemaining;   // Remaining secure boot key slots in the OTP memory
    struct {
#ifndef MFI_BIG_ENDIAN
        U8      softSecure:1;                   // Controller is in soft secure mode based on efuse
        U8      hardSecure:1;                   // Controller is in hard secure mode based on efuse
        U8      secureBootEnabled:1;            // Controller booted in secure mode
        U8      keyUpdatePending:1;             // An update of the key in the efuse is pending
        U8      reserved:4;                     // reserved
#else
        U8      reserved:4;                     // reserved
        U8      keyUpdatePending:1;             // An update of the key in the efuse is pending
        U8      secureBootEnabled:1;            // Controller booted in secure mode
        U8      hardSecure:1;                   // Controller is in hard secure mode based on efuse
        U8      softSecure:1;                   // Controller is in soft secure mode based on efuse
#endif
    } secureBootDetails;
    U8          reserved7[2];                   // pad to U32 boundary

    // new fields will be added here

} MR_CTRL_INFO;

/*
 * defines the Boot environment for proper boot message handling
 */
typedef enum _MR_BOOT_ENV {
    MR_BOOT_ENV_UNKNOWN                = 0,    // Boot environment unknown
    MR_BOOT_ENV_LEGACY                 = 1,    // Legacy boot with x86 bios
    MR_BOOT_ENV_UEFI                   = 2,    // uEFI boot
 } MR_BOOT_ENV;

/*
 * defines the controller boot message data structure
 */
 typedef struct _MR_BOOT_MESSAGE_DATA{
    S16     delay;              // 0=none, -1=forever, n=seconds
    U8      waitForKey;         // TRUE if host should wait for a keystroke to acknowledge message
    struct {
#ifndef MFI_BIG_ENDIAN
        U8  waitForString   : 1;   // 1=collect a string
        U8  hideString      : 1;   // 1=echo '*' for each character entered
        U8  reserved        : 6;
#else
        U8  reserved        : 6;
        U8  hideString      : 1;   // 1=echo '*' for each character entered
        U8  waitForString   : 1;   // 1=collect a string
#endif  // MFI_BIG_ENDIAN
    } options;
    U16     errorCode;            // boot message specific error code defined by OEM
    U8      msgSeverity;          // boot message severity
                                  // 0x0F=Critical, 0x09=Caution, 0x06=Repaired, 0x02=Status
    U8      bootMsgPendingCount;  // 0 either means field unsupported or none pending
    char    msg[1024];          // ASCII text message to display for user
} MR_BOOT_MESSAGE_DATA;            // 1032 bytes

#define MR_BOOT_MSG_INDEX_INVALID   0xFFFF      // caller should pass this message index value in dcmd.mbox.s[0] to request count of boot messages (if FW supports)

typedef enum _MR_BOOT_ACTION {
    MR_BOOT_ACTION_CONTINUE             = 0,    // continue the boot process
    MR_BOOT_ACTION_READ_ANOTHER_KEY     = 1,    // wait for another keystroke from the user
    MR_BOOT_ACTION_FORCE_CONFIG_UTILITY = 2,    // force user into the configuraiton utility
    MR_BOOT_ACTION_FORCE_REBOOT         = 3     // reboot the system to cause a controller restart
} MR_BOOT_ACTION;

typedef enum _MR_BOOT_PARAMS {
    MR_BOOT_MSG_UNUSED                  = 0,    // reserved
    MR_BOOT_MSG_TIMEOUT                 = 1     // If "delay field of MR_BOOT_MESSAGE_DATA" time is over
                                                // without user key entry
} MR_BOOT_PARAMS;

/*
 * defines the bit-map constants for PD microcode update options
 *
 */
typedef enum _MR_PD_FW_DOWNLOAD_OPTIONS {               // PD microcode update Options
    MR_PD_FW_DOWNLOAD_OPTIONS_USE_ONE_CDB     = 0x01,   // FW should concatenate data from multiple CDBs, and issue only
                                                        // one CDB to the device, similar to a SCSI Write Buffer Mode 5.
                                                        // Use this option, when OS restrictions do not permit the complete
                                                        // microcode image to be sent to FW in one CDB.
                                                        // Apps should use same CDB for each microcode chunk sent to FW.
} MR_PD_FW_DOWNLOAD_OPTIONS;
#define MR_PD_FW_DOWNLOAD_OPTIONS_MAX           0x01    // maximum permissible value

/*
 * defines the parameters required for physical device microcode update
 */
typedef struct _MR_PD_FW_DOWNLOAD_PARAMS {
    U8    cdbCount;                                 // count of CDBs to be sent by host for the microcode update
    U8    updateTime;                               // time (in secs, max = 120 sec) the device requires to update microcode
                                                    // 0=FW decide appropriate time.
    U8    options;                                  // options that define the behavior of microcode update (MR_PD_FW_DOWNLOAD_OPTIONS)
    U8    pad;
    U32   imageSize;                                // size of image in bytes
    U8    pdFwDownloadMap[MAX_API_PHYSICAL_DEVICES_EXT2/8];  // bitmap of devices which shall be simultaneously updated
} MR_PD_FW_DOWNLOAD_PARAMS;

/*
 * defines the microcode update status of each physical device
*/
typedef struct _MR_PD_FW_DOWNLOAD_STATUS {
    U8    pdFwDownloadMap[MAX_API_PHYSICAL_DEVICES/8];  // Bitmap of devices which shall be simultaneously updated
    U8    pdFwDownloadStatus[MAX_API_PHYSICAL_DEVICES]; // MFI status propagated for each PD
    U8    senseBuffer[MAX_API_PHYSICAL_DEVICES][64];    // Sense data information propagated to host for microcode update failure
    U8    reserved[MAX_API_PHYSICAL_DEVICES];           // Reserved
} MR_PD_FW_DOWNLOAD_STATUS;                             // Total size 16KB

/*
 * defines the controller escrow lock key structures
 */
typedef struct _MR_CTRL_LOCK_KEY_PASSPHRASE {
    U8      passPhrase[33];         // NULL terminated string
    U8      pad[3];                 // Keep alignment
} MR_CTRL_LOCK_KEY_PASSPHRASE;

typedef struct _MR_CTRL_ESCROW_LOCK_KEY {
    U8      escrowLockKeyLength;    // escrow key length 0=NULL key, 1=256-bit
    U8      pad[3];
    U8      escrowLockKey[32];      // Byte array
    U8      reserved[32];
} MR_CTRL_ESCROW_LOCK_KEY;

typedef struct _MR_CTRL_LOCK_KEY_FROM_ESCROW {
    MR_CTRL_LOCK_KEY_PASSPHRASE lockKeyPassPhrase;
    MR_CTRL_ESCROW_LOCK_KEY     escrow;
} MR_CTRL_LOCK_KEY_FROM_ESCROW;

/*
 * defines the parameters to create lock key
 */
typedef struct _MR_CTRL_LOCK_KEY_PARAMS {
    MR_CTRL_LOCK_KEY_PASSPHRASE lockKeyPassPhrase;
    MR_CTRL_LOCK_KEY_PASSPHRASE secretKey;
    MR_CTRL_LOCK_KEY_PASSPHRASE backupPassPhrase;
    U8                          keyId[256];         // NULL terminated string
} MR_CTRL_LOCK_KEY_PARAMS;

typedef enum _MR_CTRL_LOCK_KEY_BINDING {
    MR_CTRL_LOCK_KEY_BINDING_NONE       = 0,        // Controller lock key is not bound
    MR_CTRL_LOCK_KEY_BINDING_CTRL       = 1,        // Controller lock key is bound to controller
    MR_CTRL_LOCK_KEY_BINDING_USER       = 2,        // Controller lock key bound to user
    MR_CTRL_LOCK_KEY_BINDING_TPM        = 3,        // Controller lock key bound to a TPM
    MR_CTRL_LOCK_KEY_BINDING_EKMS       = 4         // Lock key(s) are bound (retrieved) from an EKMS
} MR_CTRL_LOCK_KEY_BINDING;

/*
 * defines the type of key operations
 */
typedef enum _MR_SECURITY_KEY_TYPE {
    MR_SECURITY_KEY_TYPE_UNLOCK         = 0,        // Lock key needed to ulock a device
    MR_SECURITY_KEY_TYPE_NEW_KEY        = 1,        // New lock key needed to secure a device
    MR_SECURITY_KEY_TYPE_REKEY          = 2,        // Lock key and new key needed to re-key a device
} MR_SECURITY_KEY_TYPE;

/*
 * define the lock key structure used by EKM agents to apply key(s) to a device
 */
typedef struct _MR_CTRL_LOCK_KEY {
    U8          lockKeyLength;      // lock key length 0=NULL key, 1=256-bit, 32=256-bit key
    U8          keyIdLength;
    U8          newlockKeyLength;   // set to 0, if no new key infomation
    U8          newkeyIdLength;     // set to 0, if no new key information
    U8          lockKey[1];         // variable length array, default 32 bytes
    U8          keyId[1];           // variable length array
    U8          newlockKey[1];      // variable length array, default 32 bytes
    U8          newkeyId[1];        // variable length array
} MR_CTRL_LOCK_KEY;

/*
 * defines the Erase type for secure erase on non-SED drives/VDs
 */
typedef enum _MR_ERASE_TYPE {
    MR_ERASE_SIMPLE           = 0,     // single pass, single pattern write
    MR_ERASE_NORMAL           = 1,     // three pass,  three pattern write
    MR_ERASE_THOROUGH         = 2,     // nine pass, Repeats MR_ERASE_NORMAL 3 times
    MR_ERASE_CRYPTOGRAPHIC    = 3,     // (Deprecated) Cryptographic erase for ISE drives
} MR_ERASE_TYPE;

/*
 * defines the Erase type for secure erase on solid-state devices/VDs
 */
typedef enum _MR_SS_ERASE_TYPE {
    MR_SS_ERASE_STANDARD      = 0,     // ATA secure erase.
                                       // If used for LD, LD will be deleted after erase.
                                       // May only be issued on an LD if it is the only LD in the disk group
    MR_SS_ERASE_3PASS_VERIFY  = 1,     // three pass verify,  pass1 random pattern write, pass2,3 write zero, verify
    MR_SS_ERASE_CRYPTOGRAPHIC = 2,     // (Deprecated) Crytographic erase of ISE drives
} MR_SS_ERASE_TYPE;

/*
* defines the Sanitize type bitmap (service action) for Sanitize operation on physical devices
*/
typedef enum _MR_SANITIZE_TYPE {
    MR_SANITIZE_NOT_SUPPORTED       = 0x00,   // Sanitize not supported
    MR_SANITIZE_CRYPTOGRAPHIC_ERASE = 0x01,   // Sanitize Crypto Erase
    MR_SANITIZE_OVERWRITE           = 0x02,   // Sanitize Overwrite for physical drives
    MR_SANITIZE_BLOCK_ERASE         = 0x04,   // Sanitize Block Erase for physical drives
    MR_SANITIZE_FREEZE_LOCK         = 0x08,   // Sanitize FREEZE LOCK for SATA physical drives
    MR_SANITIZE_ANTIFREEZE_LOCK     = 0x10,   // Sanitize ANTIFREEZE LOCK for SATA physical drives
} MR_SANITIZE_TYPE;

/*
 * defines a private structure for IOV implementations. Not intended for general purpose
 */
typedef struct _MR_CTRL_IOV_PRIVATE_INFO {
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     supportResetNowEx            :1;         // IOV FW supports special reset handling, continue to honor
                                                        // other shutdown mbox.b[x] implementations
        U32     reserved                     :31;
#else
        U32     reserved                     :31;
        U32     supportResetNowEx            :1;         // IOV FW supports special reset handling, continue to honor
                                                        // other shutdown mbox.b[x] implementations
#endif
    } ctrlCapabilities;

    U8  reserved[28];
} MR_CTRL_IOV_PRIVATE_INFO;


/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures commonly referred in other structures
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * defines the progress structure
 */
typedef union _MR_PROGRESS {
    struct  {
        U16     progress;       // 0-0xFFFF - %complete: progress*100/0xFFFF
        union {
            U16     elapsedSecs;               // how long the operation is in progress in secs.
                                               //applicable for all the operations when supportPointInTimeProgress is not set
                                               //applicable for some of the operation even if supportPointInTimeProgress is set.
                                               //Refer specific DCMD for the details
            U16     elapsedSecsForLastPercent; // time to complete the last 1% of the operation in secs
                                               //applicable only when supportPointInTimeProgress is set
        };
    } mrProgress;               // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U32     w;
} MR_PROGRESS;              // 4 bytes
#define MR_PROGRESS_DONE    0xffff      // value for 100%

/*
 * defines the time-stamp structure
 */
typedef struct _MR_TIME_STAMP {
    U16     year;           // year starting from 2000
    U8      month;          // month
    U8      day;            // day
    U8      hour;           // hour
    U8      min;            // minutes
    U8      sec;            // seconds
    U8      reserved;       // pad
} MR_TIME_STAMP;            // 8 bytes


/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to the physical drives
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * defines the physical drive reference structure
 */
typedef union  _MR_PD_REF {
    struct {
        U16     deviceId;   // PD Device Id
        U16     seqNum;     // Sequence Number
    } mrPdRef;              // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U32     ref;            // shorthand reference to full 32-bits
} MR_PD_REF;                // 4 bytes
#define MR_PD_INVALID   0xffff  // constant for 'invalid' or 'missing' PDs


/*
 * defines the physical drive address structure
 */
typedef struct _MR_PD_ADDRESS {
    U16     deviceId;           // sticky device Id
    U16     enclDeviceId;       // enclosure's device Id; if device is not within enclosure value is 0xFFFF
                                // If same as deviceId, it is the enclosure itself
    union {
        struct {                // For legacy usage, if MR_CTRL_INF0.adapterOperations.supportEnclEnumeration is not set.
                                // Or for Physical Drive, if MR_CTRL_INF0.adapterOperations.supportEnclEnumeration is set
            U8  enclIndex;      // for enclosures, 0 means not within an enclosure. DEPRECATED
            U8  slotNumber;     // Slot number of the drive.
        } mrPdAddress;          // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
        struct {                // For Enclosures, if MR_CTRL_INF0.adapterOperations.supportEnclEnumeration is set
            U8  enclPosition;   // Position of an enclosure in a cascaded chain.
            U8  enclConnectorIndex; // MR_SAS_CONNECTORS.connectors[enclConnectorIndex] to which this enclosure (chain) is attached.
        } mrEnclAddress;        // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    };
    U8      scsiDevType;        // 0-disk,
    union {
        U8      connectedPortBitmap;    // bitmap of ports on which this device is connected (bit 0=port 0, bit 1=port 1, etc).
        U8      connectedPortNumbers;   // If MR_CTRL_INFO.adapterOperations.supportMoreThan8Phys=1, use connectedPortNumbers
                                        // Lower nibble is Port Number for path 0, Upper Nibble is Port Number for path 1.
                                        // if MR_CTRL_INFO.adapterOperations3.supportMoreThan16Phys is set this field has no meaning
    };
    U64     sasAddr[2];         // SAS addresses for port 0/1 (0 means port not used)
} MR_PD_ADDRESS;                // 24 bytes

/*
 * defines the physical drive list structure
 */
typedef struct _MR_PD_LIST {
    U32             size;       // size of total data structure (including size/count fields)
    U32             count;      // count of number of drives in the system (i.e. entries in this list)
    MR_PD_ADDRESS   addr[1];    // Instances of count number of PD addresses
} MR_PD_LIST;                   // size = count * sizeof(MR_PD_ADDRESS) bytes, today for 256 devices it is 6152 bytes

/*
 * defines the physical drive deviceId list structure
 */
typedef struct _MR_PD_DEVICEID_LIST {
    U32             size;           // size of total data structure (including size/count fields)
    U32             count;          // count of number of deviceId entries in this list
    U8              pad[2];
    U16             deviceId[1];    // Instances of count number of PD deviceId
} MR_PD_DEVICEID_LIST;              // size =  10 + count * sizeof(U16) bytes, today for 256 devices it is 522 bytes

/*
 * defines the enum for physical drive states (inShield is not in SNIA DDF spec, please get it added)
 */
typedef enum _MR_PD_STATE {                 // Corresponding DDF bits in PD_INFO       inShield inTrans  inVD Online Rbld isSpare Failed
    MR_PD_STATE_UNCONFIGURED_GOOD   = 0x00, // Unconfigured good drive                   0         0       0    X      X    0       0
    MR_PD_STATE_UNCONFIGURED_BAD    = 0x01, // Unconfigured bad drive                    0         0       0    X      X    0       1
    MR_PD_STATE_HOT_SPARE           = 0x02, // Hot spare drive                           0         0       0    0      0    1       0
    MR_PD_STATE_SANITIZE            = 0x04, // Drive is under sanitize erase             X         X       0    0      1    0       0
    MR_PD_STATE_OFFLINE             = 0x10, // Configured - good drive (data invalid)    0         0       1    0      0    0       0
    MR_PD_STATE_FAILED              = 0x11, // Configured - bad drive (data invalid)     0         0       1    X      X    0       1
    MR_PD_STATE_REBUILD             = 0x14, // Configured - drive is rebuilding          0         0       1    0      1    0       0
    MR_PD_STATE_ONLINE              = 0x18, // Configured - drive is online              0         0       1    1      0    0       0
    MR_PD_STATE_COPYBACK            = 0x20, // drive is getting copied                   0         1       0    0      0    0       0
    MR_PD_STATE_SYSTEM              = 0x40, // drive is exposed and controlled by host   0         0       0    X      0    0       0
    MR_PD_STATE_SHIELD_UNCONFIGURED = 0x80, // UnConfigured - shielded                   1         0       0    X      X    0       0
    MR_PD_STATE_SHIELD_HOT_SPARE    = 0x82, // Hot Spare - shielded                      1         0       0    0      0    1       0
    MR_PD_STATE_SHIELD_CONFIGURED   = 0x90, // Configured - shielded                     1         0       1    0      0    0       0
} MR_PD_STATE;

/*
 * define constants for device speed
 */
typedef enum _MR_PD_SPEED {
    MR_PD_SPEED_UNKNOWN             = 0,    // not defined
    MR_PD_SPEED_1p5G                = 1,    // 1.5Gb/s - SATA 150
    MR_PD_SPEED_3G                  = 2,    // 3.0Gb/s
    MR_PD_SPEED_6G                  = 3,    // 6.0Gb/s
    MR_PD_SPEED_12G                 = 4,    // 12.0Gb/s

    MR_PD_SPEED_PCIE_2_5GT          = 0x80, // 2.5GT/s
    MR_PD_SPEED_PCIE_5GT            = 0x81, // 5GT/s
    MR_PD_SPEED_PCIE_8GT            = 0x82, // 8GT/s
    MR_PD_SPEED_PCIE_16GT           = 0x83  // 16GT/s
} MR_PD_SPEED;

/*
 * define constants for device media
 */
typedef enum _MR_PD_MEDIA_TYPE {
    MR_PD_MEDIA_TYPE_ROTATIONAL     = 0,    // rotating media, HDD
    MR_PD_MEDIA_TYPE_SSD            = 1,    // SSD
    MR_PD_MEDIA_TYPE_SSM_FLASH      = 2,    // Solid state flash module
} MR_PD_MEDIA_TYPE;

/*
 * defines the enum for type of flash
 */
typedef enum _MR_FLASH_TYPE {
    MR_FLASH_TYPE_UNKNOWN       =0xFF,
    MR_FLASH_TYPE_SLC           = 0,    // Flash memory type SLC
    MR_FLASH_TYPE_CMLC          = 1,    // Flash memory type cMLC
    MR_FLASH_TYPE_EMLC          = 2,    // Flash memory type eMLC
} MR_FLASH_TYPE;

/*
 * define constants for device power states
 */
typedef enum _MR_PD_POWER_STATE {
    MR_PD_POWER_STATE_ACTIVE        = 0,    // device is active (spun up)
    MR_PD_POWER_STATE_STOP          = 1,    // device is stopped (spun down)
    MR_PD_POWER_STATE_TRANSITIONING = 0xff, // device is transitioning between power states
} MR_PD_POWER_STATE;
#define MR_PD_POWER_STATE_MAX         1     // maximum permissible value

/*
 * define constants for device list query options
 */
typedef enum _MR_PD_QUERY_TYPE {
    MR_PD_QUERY_TYPE_ALL                = 0,    // return all PDs, equivalent to MR_DCMD_PD_GET_LIST
    MR_PD_QUERY_TYPE_STATE              = 1,    // query for PD State
    MR_PD_QUERY_TYPE_POWER_STATE        = 2,    // query for PD Power State
    MR_PD_QUERY_TYPE_MEDIA_TYPE         = 3,    // query for PD media type
    MR_PD_QUERY_TYPE_SPEED              = 4,    // query for device speed
    MR_PD_QUERY_TYPE_EXPOSED_TO_HOST    = 5,    // query for devices such as System drives, SEP, CD-ROMs, TAPEs etc. (value is ignored)
    MR_PD_QUERY_TYPE_SECURITY_KEY_TYPE  = 6,    // query for PD requiring a key, of type MR_SECURITY_KEY_TYPE
} MR_PD_QUERY_TYPE;
#define MR_PD_QUERY_TYPE_MAX              6     // maximum permissible value

/*
 * define constants for FDE type of PD
 */
typedef enum _MR_PD_FDE_TYPE {
    MR_PD_FDE_TYPE_NONE        = 0,    // PD does not support TCG (non-FDE PD)
    MR_PD_FDE_TYPE_ENTERPRISE  = 1,    // PD supports TCG Enterprise SSC
    MR_PD_FDE_TYPE_OPAL        = 2,    // PD supports TCG OPAL SSC
} MR_PD_FDE_TYPE;

/*
 * defines the physical drive progress structure
 */
typedef struct _MR_PD_PROGRESS {
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     rbld    : 1;    // rebuild
        U32     patrol  : 1;    // patrol read
        U32     clear   : 1;    // clear all data
        U32     copyBack: 1;    // copy all data to the destination PD
        U32     erase   : 1;    // secure erase
        U32     locate  : 1;    // locate drive
        U32     sanitize: 1;    // sanitize
        U32     reserved:25;
#else
        U32     reserved:25;
        U32     sanitize: 1;    // sanitize
        U32     locate  : 1;    // locate drive
        U32     erase   : 1;    // secure erase
        U32     copyBack: 1;    // copy all data to the destination PD
        U32     clear   : 1;    // clear all data
        U32     patrol  : 1;    // patrol read
        U32     rbld    : 1;    // rebuild
#endif  // MFI_BIG_ENDIAN
    } active;
    MR_PROGRESS     rbld;       // rbld/copyBack progress
    MR_PROGRESS     patrol;     // patrol read progress
    union {
        MR_PROGRESS     clear;      // clear progress
        MR_PROGRESS     erase;      // erase progress
        MR_PROGRESS     sanitize;   // sanitize progress
    };

    struct {
#ifndef MFI_BIG_ENDIAN
        U32     rbld    : 1;    // rebuild
        U32     patrol  : 1;    // patrol read
        U32     clear   : 1;    // clear all data
        U32     copyBack: 1;    // copy all data to the destination PD
        U32     erase   : 1;    // secure erase
        U32     reserved:27;
#else
        U32     reserved:27;
        U32     erase   : 1;    // secure erase
        U32     copyBack: 1;    // copy all data to the destination PD
        U32     clear   : 1;    // clear all data
        U32     patrol  : 1;    // patrol read
        U32     rbld    : 1;    // rebuild
#endif  // MFI_BIG_ENDIAN
    } pause;

    MR_PROGRESS     reserved[3];
} MR_PD_PROGRESS;               // size = 4 * (4 * number of bits defined) bytes, today it is 20 bytes

/*
 * define the DDF Type bit structure
 */
typedef union _MR_PD_DDF_TYPE {
    struct {
        union {
            struct {
#ifndef MFI_BIG_ENDIAN
                U16     forcedPDGUID    : 1;    // TRUE if PD GUID was forced
                U16     inVD            : 1;    // TRUE if this PD is used in a VD
                U16     isGlobalSpare   : 1;    // TRUE if this is a global hot spare
                U16     isSpare         : 1;    // TRUE if this is a dedicated hot spare
                U16     isForeign       : 1;    // TRUE if this drive contains a foreign configuration
                U16     reserved        : 7;    // reserved for future use
                U16     intf            : 4;    // 0-Unknown, 1-parallel SCSI, 2-SAS, 3-SATA, 4-FC, 5-NVME
#else
                U16     intf            : 4;    // 0-Unknown, 1-parallel SCSI, 2-SAS, 3-SATA, 4-FC, 5-NVME
                U16     reserved        : 7;    // reserved for future use
                U16     isForeign       : 1;    // TRUE if this drive contains a foreign configuration
                U16     isSpare         : 1;    // TRUE if this is a dedicated hot spare
                U16     isGlobalSpare   : 1;    // TRUE if this is a global hot spare
                U16     inVD            : 1;    // TRUE if this PD is used in a VD
                U16     forcedPDGUID    : 1;    // TRUE if PD GUID was forced
#endif  // MFI_BIG_ENDIAN
            } pdType;
            U16     type;                       // alternate name for full 16-bits
        };
        U16     reserved;                       // reserved
    } ddf;                                      // All the fields are from DDF
    struct {
        U32    reserved;                        // reserved for non-disk devices
    } nonDisk;
    U32     type;                               // shorthand reference to full 32 bits
} MR_PD_DDF_TYPE;

/*
 * defines the physical drive allowed operations structure
 */
typedef struct _MR_PD_ALLOWED_OPS {
#ifndef MFI_BIG_ENDIAN
    U32 makeOnline          : 1;
    U32 makeOffline         : 1;
    U32 makeFailed          : 1;
    U32 makeBad             : 1;
    U32 makeUnconfigured    : 1;
    U32 makeSpare           : 1;
    U32 removeSpare         : 1;
    U32 replaceMissing      : 1;
    U32 markMissing         : 1;
    U32 startRebuild        : 1;
    U32 stopRebuild         : 1;
    U32 locate              : 1;
    U32 pdClear             : 1;
    U32 foreignImportNotAllowed : 1;    // This field is valid if MR_PD_INFO.state.ddf.pdType.isForeign is set
    U32 startCopyBack       : 1;
    U32 stopCopyBack        : 1;
    U32 fwDownloadNotAllowed    : 1;
    U32 reprovision         : 1;
    U32 makeSystem          : 1;
    U32 T10PowerModeSupported : 1;
    U32 suspendRebuild        : 1;  // 1=suspend Rebuild operation
    U32 resumeRebuild         : 1;  // 1=resume Rebuild operation
    U32 suspendCopyback       : 1;  // 1=suspend Copyback operation
    U32 resumeCopyback        : 1;  // 1=resume Copyback operation
    U32 startSecureEraseNonSED: 1;  // 1=start: secure erase for non SED drives
    U32 stopSecureEraseNonSED : 1;  // 1=stop: secure erase for non SED drives
    U32 SMARTSupported      : 1;    // 1=SMART info supported
    U32 prepareForRemoval   : 1;    // 1=prepare PD for removal
    U32 enableSecurityonJBOD: 1;    // 1=security can be enabled on system PD/JBOD
    U32 sanitize            : 1;    // 1=Sanitize can be run on PD now
    U32 reserved            : 2;
#else
    U32 reserved            : 2;
    U32 sanitize            : 1;    // 1=Sanitize can be run on PD now
    U32 enableSecurityonJBOD: 1;    // 1=security can be enabled on system PD/JBOD
    U32 prepareForRemoval   : 1;    // 1=prepare PD for removal
    U32 SMARTSupported      : 1;    // 1=SMART info supported
    U32 stopSecureEraseNonSED : 1;  // 1=stop: secure erase for non SED drives
    U32 startSecureEraseNonSED: 1;  // 1=start: secure erase for non SED drives
    U32 resumeCopyback        : 1;  // 1=resume Copyback operation
    U32 suspendCopyback       : 1;  // 1=suspend Copyback operation
    U32 resumeRebuild         : 1;  // 1=resume Rebuild operation
    U32 suspendRebuild        : 1;  // 1=suspend Rebuild operation
    U32 T10PowerModeSupported : 1;
    U32 makeSystem          : 1;
    U32 reprovision         : 1;
    U32 fwDownloadNotAllowed    : 1;
    U32 stopCopyBack        : 1;
    U32 startCopyBack       : 1;
    U32 foreignImportNotAllowed : 1;    // This field is valid if MR_PD_INFO.state.ddf.pdType.isForeign is set
    U32 pdClear             : 1;
    U32 locate              : 1;
    U32 stopRebuild         : 1;
    U32 startRebuild        : 1;
    U32 markMissing         : 1;
    U32 replaceMissing      : 1;
    U32 removeSpare         : 1;
    U32 makeSpare           : 1;
    U32 makeUnconfigured    : 1;
    U32 makeBad             : 1;
    U32 makeFailed          : 1;
    U32 makeOffline         : 1;
    U32 makeOnline          : 1;
#endif  // MFI_BIG_ENDIAN
} MR_PD_ALLOWED_OPS;

/*
 * defines the physical drive allowed operations list structure
 */
typedef struct _MR_PD_ALLOWED_OPS_LIST {
    U32                 count;                                  // count of valid entries in this list
    U32                 pad;
    MR_PD_ALLOWED_OPS   allowedOpsList[MAX_API_PHYSICAL_DEVICES_EXT2];   // Use deviceId to index into this array
} MR_PD_ALLOWED_OPS_LIST;                                       // This structure is 32776 bytes for all 8192 devices


/*
 * defines the physical drive information structure
 */
typedef struct  _MR_PD_INFO {
    MR_PD_REF   ref;
    union{
        U8      inquiryData[96];
        U8      name[20];               // For specific OEMs, this will be populated with the Backplane Name
    };
    U8      vpdPage83[64];              // VPD page 83. Also, see below for vpdPage83Ext.

    U8      notSupported;               // Set, if this drive is not supported. Like SATA disk if SAS only supported...etc
                                        // Non-certified drive may or may not fall under this category.
    U8      scsiDevType;                // Disk, Tape, CDROM

    union {
        U8      connectedPortBitmap;    // bitmap of ports on which this device is connected (bit 0=port 0, bit 1=port 1, etc).
        U8      connectedPortNumbers;   // If MR_CTRL_INFO.adapterOperations.supportMoreThan8Phys=1, use connectedPortNumbers
                                        // Lower nibble is Port Number for path 0, Upper Nibble is Port Number for path 1.
                                        // if MR_CTRL_INFO.adapterOperations3.supportMoreThan16Phys is set this field has no meaning
    };

    U8      deviceSpeed;                // device maximum supported speed (MR_PD_SPEED)

    U32     mediaErrCount;
    U32     otherErrCount;              // transient errors detected, while communicating with this device
    U32     predFailCount;
    U32     lastPredFailEventSeqNum;    // sequence number of the last pred fail event on this pd

    U16     fwState;                    // MR_PD_STATE. Firmware's interpretation of drive state
    U8      disabledForRemoval;         // If set, drive is ready for removal
    U8      linkSpeed;                  // negotiated link speed (MR_PD_SPEED)
                                        // For dual-ported drives,
                                        // if only one port is connected or active, then it is the active ports link speed
                                        // if both ports are present and only one port is used, then it is that ports link speed
                                        // if both ports are present and both ports are simultaneously used,
                                        // then it is that ports whose bit is unset (lsb) in the pathInfo isPathBroken.
                                        // see linkSpeedOther below
                                        // Note: This can be higher than the deviceSpeed if there is an external assistance.
                                        // Ex: 6G drives behind 12G Cobra expander can link up at 12G/s if EDFB feature is enabled

    MR_PD_DDF_TYPE  state;              // DDF state bits

    struct {
        U8      count;                  // 0-none is valid, 1,2 can be a valid value
#ifndef MFI_BIG_ENDIAN
        U8      isPathBroken    : 4;    // Each bit will tell if the path (address) is broken or ok
        U8      reserved3       : 3;
        U8      widePortCapable : 1;    // Device is wide port capable, see sasAddr as well
#else
        U8      widePortCapable : 1;    // Device is wide port capable, see sasAddr as well
        U8      reserved3       : 3;
        U8      isPathBroken    : 4;    // Each bit will tell if the path (address) is broken or ok
#endif  // MFI_BIG_ENDIAN

        U8      connectorIndex[2];      // Index into MR_SAS_CONNECTORS.connectors, to which this path is attached.
                                        // Only used if MR_CTRL_INFO.adapterOperations.supportMultipath is set.

        U8      connectedPortNumbers[2];// If MR_CTRL_INFO.adapterOperations3.supportMoreThan16Phys=1 otherwise use use connectedPortNumbers union
                                        // The SAS port number is only valid if the corresponding SAS address is valid
        U8      reserved[2];            // pad for 64-bit alignment
        U64     sasAddr[2];             // Device SAS addresses, upto 2 (both may be same if its a wide port)
        union {
            U8      enclLogicalID[8];   // Enclosure Logical ID a.k.a ELI (used only for an enclosure device)
            U8      reserved5[8];
        };
        U8      reserved2[8];
    } pathInfo;

    U64     rawSize;                    // size before COD and coercion
    U64     nonCoercedSize;             // size after COD is reserved but before coercion
    U64     coercedSize;                // size after COD is reserved and after coercion

    U16     enclDeviceId;               // enclosure's device Id; if device is not within enclosure value is 0xFFFF
    U8      enclIndex;                  // DEPRECATED   Firmware internal enclosure index

    union {
        U8      slotNumber;             // slot number of this drive, if this PD is an enclosure then this has no meaning

        U8      totalslots;             // For specific OEMS, this would give the total number of slots on the backplane

        U8      enclConnectorIndex;     // DEPRECATED
                                        // MR_SAS_CONNECTORS.connectors[enclConnectorIndex] to which this enclosure (chain) is attached.
                                        // Only used if MR_CTRL_INFO.adapterOperations.supportEnclEnumeration is set,
                                        // and this MR_PD_INFO pertains to an enclosurePD.Also see MR_PD_INFO.enclPostion
    };



    MR_PD_PROGRESS progInfo;            // drive progress information, for non-disk devices, no meaning

    U8      badBlockTableFull;          // flag indicating this drive's bad block table is full (primarily used for MPT IM/IR)
    U8      unusableInCurrentConfig;    // TRUE when this drive can't be used as currently configured

    U8      vpdPage83Ext[64];           // This field works in conjunction with vpdPage83 above.

    U8      powerState;                 // power condition of the device (MR_PD_POWER_STATE)
    U8      enclPosition;               // Position of the enclosure in a cascaded chain.
                                        // Only used if MR_CTRL_INFO.adapterOperations.supportEnclEnumeration is set,
                                        // and this MR_PD_INFO pertains to an enclosurePD. Also see MR_PD_INFO.enclConnectorIndex.


    MR_PD_ALLOWED_OPS   allowedOps;     // Allowed operations on the PD.
    U16     copyBackPartnerId;          // copyBack Partner device Id during copyBack, else MR_PD_INVALID

    U16     enclPartnerDeviceId;        // If drive, and it is in an enclosure with dual paths, this is the deviceId of the redundant enclosure module.
                                        // If enclosure, and there are 2 EMMs this field is deviceId of redundant EMM.
                                        // Set to MR_PD_INVALID in all other cases.
                                        // This field is valid only if MR_CTRL_INFO.adapterOperations.supportMultipath is set.

    struct {
#ifndef MFI_BIG_ENDIAN
        U16 fdeCapable      : 1; // 1=FDE drive, 0=non-FDE
        U16 fdeEnabled      : 1; // FDE drive with 1=security enabled, 0=security disabled
        U16 secured         : 1; // 1=drive is secured, 0=not secured
        U16 locked          : 1; // 1=drive is locked, 0=unlocked
        U16 foreign         : 1; // 1=drive is secured by a foreign lock key
        U16 needsEKM        : 1; // 1=drive requires EKM
        U16 fdeType         : 3; // FDE type in enum MR_PD_FDE_TYPE
        U16 fdeNotSupported : 1; // FDE Capability not supported by MR FW. Only relevant if fdeCapable is set.
        U16 reserved        : 6;
#else
        U16 reserved        : 6;
        U16 fdeNotSupported : 1; // FDE Capability not supported by MR FW. Only relevant if fdeCapable is set.
        U16 fdeType         : 3; // FDE type in enum MR_PD_FDE_TYPE
        U16 needsEKM        : 1; // 1=drive requires EKM
        U16 foreign         : 1; // 1=drive is secured by a foreign lock key
        U16 locked          : 1; // 1=drive is locked, 0=unlocked
        U16 secured         : 1; // 1=drive is secured, 0=not secured
        U16 fdeEnabled      : 1; // FDE drive with 1=security enabled, 0=security disabled
        U16 fdeCapable      : 1; // 1=FDE drive, 0=non-FDE
#endif  // MFI_BIG_ENDIAN
    } security;

    U8      mediaType;                  // type of device media (MR_PD_MEDIA_TYPE)

    U8      notCertified;               // Set if the drive is not certified

    U8      bridgeVendor[8];                 // per VPD89 bytes 8-15
    U8      bridgeProductIdentification[16]; // per VPD89 bytes 16-31
    U8      bridgeProductRevisionLevel[4];   // per VPD89 bytes 32-35
    U8      satBridgeExists;                 // Indicates that SATA device is behind a SAS-SATA bridge

    U8      interfaceType;              // 0-Unknown, 1-parallel SCSI, 2-SAS, 3-SATA, 4-FC
    U8      temperature;                // in Celsius, if MR_CTLR_INFO.pdOperations.supportTemperature (0xFF - invalid temp)
    U8      emulatedBlockSize;          // Ratio of physical block size to logical block size, expressed as a power of 2
    U16     userDataBlockSize;          // Number of bytes of user data in LBA
    U16     reserved2;

    struct {
#ifndef MFI_BIG_ENDIAN
        U32 piType              : 3; // SCSI Protection Information Type (MR_PROT_INFO_TYPE)
        U32 piFormatted         : 1; // 1=drive is formatted for PI information, 0=no PI data
        U32 piEligible          : 1; // 1=drive can be used for PD Enabled LD
        U32 NCQ                 : 1; // drive's NCQ setting (valid if MR_CTLR_INFO.pdOperations.supportNCQ = 1)
        U32 WCE                 : 1; // drive's write-cache setting (valid if MR_CTLR_INFO.pdOperations.supportWCE = 1)
        U32 commissionedSpare   : 1; // this PD is a commissioned spare. Ex: previously this was a revertible spare and now
                                     // is commisioned in the array.
        U32 emergencySpare      : 1; // this PD is commsisioned as emergency spare
        U32 ineligibleForSSCD   : 1; // this PD cannot be used for an SSCD
        U32 ineligibleForLd     : 1; // this PD cannot be used for regular LD
        U32 useSSEraseType      : 1; // 1=use MR_SS_ERASE_TYPE for MR_DCMD_PD_SECURE_ERASE
        U32 wceUnchanged        : 1; // the drive's WCE setting shall default to this value ("unchanged WCE setting")
                                     //     (valid if MR_CTLR_INFO.pdOperations.supportWCE = 1)
        U32 supportScsiUnmap    : 1; // 1=PD supports TRIM/UNMAP
        U32 supportT10RebuildAssist   : 1; // 1=PD supports T10 Rebuild Assist
        U32 isEPD               : 1; // 1 = PD is an enhanced physical drive
        U32 ISECapable          : 1; // (Deprecated, use sanitizeType instead) 1 = PD is an ISE drive and supports cryptographic erase
        U32 sanitizeType        : 5; // PD supports sanitize types:
                                     // Bit 0 - CRYPTO ERASE ; 1 - OVERWRITE; 2 - BLOCK_ERASE; 3 - FREEZE LOCK; 4 - ANTI-FREEZE LOCK
                                     // All 5 bits = 0 - Sanitize not supported by PD
        U32 unmapCapableForLDs  : 1; // 1 = PDs can be used for Raid LDs for unmap feature
        U32 pdSupportsSSDWearGauge : 1; //PD supports Wear Gauge metrics
        U32 reserved            : 8;
#else
        U32 reserved            : 8;
        U32 pdSupportsSSDWearGauge : 1; //PD supports Wear Gauge metrics
        U32 unmapCapableForLDs  : 1; // 1 = PDs can be used for Raid LDs for unmap feature
        U32 sanitizeType        : 5; // PD supports sanitize types:
                                     // Bit 0 - CRYPTO ERASE ; 1 - OVERWRITE; 2 - BLOCK_ERASE; 3 - FREEZE LOCK; 4 - ANTI-FREEZE LOCK
                                     // All 5 bits = 0 - Sanitize not supported by PD
        U32 ISECapable          : 1; // (Deprecated, use sanitizeType instead) 1 = PD is an ISE drive and supports cryptographic erase
        U32 isEPD               : 1; // 1 = PD is an enhanced physical drive
        U32 supportT10RebuildAssist   : 1; // 1=PD supports T10 Rebuild Assist
        U32 supportScsiUnmap    : 1; // 1=PD supports TRIM/UNMAP
        U32 wceUnchanged        : 1; // the drive's WCE setting shall default to this value ("unchanged WCE setting")
                                     //     (valid if MR_CTLR_INFO.pdOperations.supportWCE = 1)
        U32 useSSEraseType      : 1; // 1=use MR_SS_ERASE_TYPE for MR_DCMD_PD_SECURE_ERASE
        U32 ineligibleForLd     : 1; // this PD cannot be used for regular LD
        U32 ineligibleForSSCD   : 1; // this PD cannot be used for an SSCD
        U32 emergencySpare      : 1; // this PD is commsisioned as emergency spare
        U32 commissionedSpare   : 1; // this PD is a commissioned spare. Ex: previously this was a revertible spare and now
                                     // is commisioned in the array.
        U32 WCE                 : 1; // drive's write-cache setting (valid if MR_CTLR_INFO.pdOperations.supportWCE = 1)
        U32 NCQ                 : 1; // drive's NCQ setting (valid if MR_CTLR_INFO.pdOperations.supportNCQ = 1)
        U32 piEligible          : 1; // 1=drive can be used for PD Enabled LD
        U32 piFormatted         : 1; // 1=drive is formatted for PI information, 0=no PI data
        U32 piType              : 3; // SCSI Protection Information Type (MR_PROT_INFO_TYPE)
#endif
    } properties;

    MR_TIME_STAMP   shieldDiagCompletionTime;   // last successful Diagnostics completion time. 0 is N/A
    U8              shieldCounter;              // how many times the shield diagnostics was run before it was marked failed

    U8      linkSpeedOther;     // negotiated link speed (MR_PD_SPEED): Applies to only dual ported drives
                                // This would be the passive ports link speed if only one is being used as active
                                // If both ports are simultaneously used, then it is that ports whose bit
                                // is unset after the first unset lsb in the pathInfo isPathBroken
    U8      reserved4[2];

    struct {
#ifndef MFI_BIG_ENDIAN
        U32     bbmErrCountSupported : 1;
        U32     bbmErrCount          : 31;
#else
        U32     bbmErrCount          : 31;
        U32     bbmErrCountSupported : 1;
#endif
    } bbmErr;

    U8      boxNumber;                  // Populated with the box number for Direct Attached SAS Smart Carrier Backplane
    U8      ssdPercentUsed;             // ssd Percent Used from drive values 0-100
    U16     ssdLifeRemainingInDays;     // ssd estimated life remaining calculated based on wear gauge industry standards
    U8      reserved1[512-432];
} MR_PD_INFO;                           // Total size 512 bytes

/*
 * define the SMART information structure
 * interpreted values are only for known or LSI proprietary devices
 * All reserved/unused fields should be filled with 0xFF
 */
typedef struct _MR_PD_SMART_INFO {
    U32    size;                            // size of total data structure (including size fields)
    U8     mediaType;                       // info for PD media type (MR_PD_MEDIA_TYPE)
    U8     reserved[3];                     // reserved for future

    union {
        struct {                            // For media type MR_PD_MEDIA_TYPE_SSM_FLASH
            U8     flashType;                    // Flash Type (MR_FLASH_TYPE)
            U8     temperatureWarningThreshold;
            U8     temperatureErrorThreshold;
            U8     lifeCurveStatus;              // SSD Throttling status.
                                                 // 100% - no throttling
                                                 //  90% - IO throttled by 10%, and so on.
            U8     lifeLeft;                     // Percentage of Life left
            U8     lifeLeftWarningThreshold;
            U8     lifeLeftErrorThreshold;
            U8     lifeLeftPrecision;
            struct {
#ifndef MFI_BIG_ENDIAN
                U16     thermalThrottle : 1;     // 1=IOs are being throttled due to elevated temperature
                U16     lockedUp        : 1;     // 1=device failure, locked up
                U16     serviceable     : 1;     // 1=device is removable for service
                U16     onboard         : 1;     // 1=device is on board
                U16     reserved        : 12;
#else
                U16     reserved        : 12;
                U16     onboard         : 1;     // 1=device is on board
                U16     serviceable     : 1;     // 1=device is removable for service
                U16     lockedUp        : 1;     // 1=device failure, locked up
                U16     thermalThrottle : 1;     // 1=IOs are being throttled due to elevated temperature
#endif  // MFI_BIG_ENDIAN
            } status;
            U8     reserved1[14];                // For future expansion;
        } ssmFlashSmart;
        U8  pad[24];
    };
} MR_PD_SMART_INFO;

/*
 * define the SMART LD information structure
 * This structure will have Flash related information at LD level
 */

typedef struct _MR_LD_SMART_INFO {
    U32    size;                            // size of total data structure (including size fields)
    U8     isNonRotationalMediaType;        // 1=SSD or Flash Module based LD
    U8     reserved[3];                     // reserved for future
    union {
        struct {                            // For media type Non-Rotating Media Type exposed as SCSI Disk to Host
            U8     reserved1[8];            // For future expansion;
            U64    hostWrittenInGB;         // size of data written by host on this LD and it is persisted across reboot
        } scsiFlashSmart;
        U8  pad[120];
    };

}MR_LD_SMART_INFO;   //128 bytes structure



/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to the logical drives
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * defines the logical drive reference structure
 */
typedef union  _MR_LD_REF {        // LD reference structure
    struct {
        U8      targetId;           // LD target id (0 to MAX_TARGET_ID)
        U8      targetIdMsb;        // LD target id (0 to MAX_API_TARGET_ID_EXT2)
        U16     seqNum;             // Sequence Number
    } mrLdRef;                      // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U32     ref;                    // shorthand reference to full 32-bits
} MR_LD_REF;                        // 4 bytes

/*
 * defines the logical drive list structure
 */
typedef struct _MR_LD_LIST {
    U32     ldCount;                // number of LDs
    U32     reserved;               // pad to 8-byte boundary
    struct {
        MR_LD_REF   ref;            // LD reference
        U8          state;          // current LD state (MR_LD_STATE)
        U8          reserved[3];    // pad to 8-byte boundary
        U64         size;           // LD size
    } ldList[MAX_API_LOGICAL_DRIVES_EXT];
} MR_LD_LIST;                       // 4 + 4 + (MAX_API_LOGICAL_DRIVES_EXT * 16), for 40LD it is = 648 bytes

/*
 * defines the logical drive targetId list structure
 */
typedef struct _MR_LD_TARGETID_LIST {
    U32             size;           // size of total data structure (including size/count fields)
    U32             count;          // count of number of targetId entries in this list
    U8              pad[3];
    union {                         // deprecated
        U8          targetId[1];    // Instances of count number of LD targetId (0 to MAX_TARGET_ID)
        U8          reserved;       // unused in case mbox specifies MAX_API_LOGICAL_DRIVES_EXT2
    };
    U16             targetIdExt[2]; // Instances of count number of LD targetId (0 to MAX_API_TARGET_ID_EXT2)
                                    // (sized at 2 to maintain overall structure U32 alignment)
} MR_LD_TARGETID_LIST;

/*
 * defines the logical drive GUID list structure
 */
typedef struct _MR_LD_GUID {
    MR_LD_REF       ref;            // ld reference
    U8              guid[24];       // LD GUID
    U8              reserved[4];    // pad + reserved for future
} MR_LD_GUID;

/*
 * defines the LD GUID List structure
 */
typedef struct _MR_LD_GUID_LIST {
    U32             size;           // size of total data structure (including size/count fields)
    U32             count;          // count of number of entries in this list
    MR_LD_GUID      guidList[1];    // Instances of count number of LD GUIDs
} MR_LD_GUID_LIST;

/*
 * define constants for logical drive target id list query options
 */
typedef enum _MR_LD_QUERY_TYPE {
    MR_LD_QUERY_TYPE_ALL                = 0,    // return all LDs (no value, result maybe greater than MAX_LOGICAL_DRIVES)
    MR_LD_QUERY_TYPE_EXPOSED_TO_HOST    = 1,    // query for all LDs that are exposed to OS (no value)
    MR_LD_QUERY_TYPE_USED_TGT_IDS       = 2,    // query for all LD target Ids that are in use (no value),
                                                // such as those pinned by cache or orphan PiTs, and all LDs exposed to OS, etc.
    MR_LD_QUERY_TYPE_CLUSTER_ACCESS     = 3,    // LDs by HA domain access.  Value: 0=exclusive access, 1=shared HA access, (query supported only by HA ctrl)
    MR_LD_QUERY_TYPE_CLUSTER_LOCALE     = 4,    // LDs by HA domain locality. Value: 0=remote, 1=local. (query supported only by HA ctrl)
} MR_LD_QUERY_TYPE;
#define MR_LD_QUERY_TYPE_MAX              5     // maximum permissible value

/*
 * define disk write cache policy settings
 */
typedef enum _MR_PD_CACHE {
    MR_PD_CACHE_UNCHANGED   = 0,    // don't change disk cache policy - use disk's default
    MR_PD_CACHE_ENABLE      = 1,    // enable disk write cache
    MR_PD_CACHE_DISABLE     = 2,    // disable disk write cache
} MR_PD_CACHE;
#define MR_PD_CACHE_MAX     2       // maximum permissible value

/*
 * define the LD cache policy settings
 */
typedef enum _MR_LD_CACHE {         // LD Cache Policy bit definitions
    MR_LD_CACHE_WRITE_BACK          = 0x01,     // write back
    MR_LD_CACHE_WRITE_ADAPTIVE      = 0x02,     // Adaptive Write (not currently implemented)
    MR_LD_CACHE_READ_AHEAD          = 0x04,     // Read Ahead
    MR_LD_CACHE_READ_ADAPTIVE       = 0x08,     // Adaptive Read ahead (deprecated)
    MR_LD_CACHE_WRITE_CACHE_BAD_BBU = 0x10,     // write caching ok if bad BBU
    MR_LD_CACHE_ALLOW_WRITE_CACHE   = 0x20,     // write caching allowed
    MR_LD_CACHE_ALLOW_READ_CACHE    = 0x40,     // read caching allowed
} MR_LD_CACHE;
#define MR_LD_CACHE_MASK            0x7f        // mask of all valid bits

/*
 * define helper macros for traditional MegaRAID cache policies.  The read, write, and IO
 * policies may be ored together to define the complete cache policy.
 */
#define MR_LD_CACHE_POLICY_READ_AHEAD_NONE      0
#define MR_LD_CACHE_POLICY_READ_AHEAD_ALWAYS    MR_LD_CACHE_READ_AHEAD
#define MR_LD_CACHE_POLICY_READ_AHEAD_ADAPTIVE  (MR_LD_CACHE_READ_AHEAD | MR_LD_CACHE_READ_ADAPTIVE)

#define MR_LD_CACHE_POLICY_WRITE_THROUGH        0
#define MR_LD_CACHE_POLICY_WRITE_BACK           MR_LD_CACHE_WRITE_BACK
#define MR_LD_CACHE_POLICY_WRITE_CACHE_BAD_BBU  (MR_LD_CACHE_WRITE_BACK | MR_LD_CACHE_WRITE_CACHE_BAD_BBU)

#define MR_LD_CACHE_POLICY_IO_CACHED            (MR_LD_CACHE_ALLOW_WRITE_CACHE | MR_LD_CACHE_ALLOW_READ_CACHE)
#define MR_LD_CACHE_POLICY_IO_DIRECT            0

/*
 * define a bitmap structure for alternate access to cache policy settings
 */
typedef union _MR_LD_CACHE_STRUCT {
    struct {
#ifndef MFI_BIG_ENDIAN
        U8      writeBack           : 1;
        U8      writeBackAdaptive   : 1;
        U8      readAhead           : 1;
        U8      readAheadAdaptive   : 1;  // deprecated
        U8      writeCacheBadBbu    : 1;
        U8      allowWriteCache     : 1;
        U8      allowReadCache      : 1;
        U8      reserved            : 1;
#else
        U8      reserved            : 1;
        U8      allowReadCache      : 1;
        U8      allowWriteCache     : 1;
        U8      writeCacheBadBbu    : 1;
        U8      readAheadAdaptive   : 1;  // deprecated
        U8      readAhead           : 1;
        U8      writeBackAdaptive   : 1;
        U8      writeBack           : 1;
#endif  // MFI_BIG_ENDIAN
    } mrLdCachePolicy;                     // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U8      b;
} MR_LD_CACHE_STRUCT;

/*
 * define the LD access policy settings
 */
typedef enum _MR_LD_ACCESS {        // LD Access Policy definitions
    MR_LD_ACCESS_RW        = 0,
    MR_LD_ACCESS_READ_ONLY = 2,
    MR_LD_ACCESS_BLOCKED   = 3,
    MR_LD_ACCESS_TRANSPORT_READY = 14,
    MR_LD_ACCESS_HIDDEN    = 15     // LD is not blocked but hidden from the OS.
} MR_LD_ACCESS;
#define MR_LD_ACCESS_MASK   0x0F

/*
 * define the reason/status for LD access policy settings
 */
typedef enum _MR_LD_ACCESS_STATUS { // LD Access Policy Status definitions
    MR_LD_ACCESS_STATUS_USER                = 0x00,     // policy set by user
    MR_LD_ACCESS_STATUS_SSC_OFFLINE         = 0x01,     // secondary cache is offline, some data is unavailable
    MR_LD_ACCESS_STATUS_PEER_UNAVAILABLE    = 0x02,     // peer controller in HA domain is offline,  some data is unavailable
} MR_LD_ACCESS_STATUS;

/*
 * define the enum for logical drive states
 */
typedef enum _MR_LD_STATE {
    MR_LD_STATE_OFFLINE             = 0,
    MR_LD_STATE_PARTIALLY_DEGRADED  = 1,
    MR_LD_STATE_DEGRADED            = 2,
    MR_LD_STATE_OPTIMAL             = 3
} MR_LD_STATE;

/*
 * define the enum for logical drive encryption type
 */
typedef enum _MR_LD_ENCRYPTION_TYPE
{
    MR_LD_ENCRYPTION_TYPE_NONE = 0,     // No encryption is used
    MR_LD_ENCRYPTION_TYPE_FDE  = 1,     // FDE drive encryption
    MR_LD_ENCRYPTION_TYPE_CTRL = 2      // Controller-based encryption
} MR_LD_ENCRYPTION_TYPE;

/*
 * define the enum for SCSI protection type
 */
typedef enum _MR_PROT_INFO_TYPE
{
    MR_PROT_INFO_TYPE_NONE = 0,         // No SCSI protection type used
    MR_PROT_INFO_TYPE_1 = 1,            // SCSI PI Type 1
    MR_PROT_INFO_TYPE_2 = 2,            // SCSI PI Type 2
    MR_PROT_INFO_TYPE_3 = 3,            // SCSI PI Type 3
    MR_PROT_INFO_TYPE_RES_4 = 4,        // Reserved SCSI PI Type 4
    MR_PROT_INFO_TYPE_RES_5 = 5,        // Reserved SCSI PI Type 5
    MR_PROT_INFO_TYPE_RES_6 = 6,        // Reserved SCSI PI Type 6
    MR_PROT_INFO_TYPE_RES_7 = 7,        // Reserved SCSI PI Type 7
    // Subsequent values are only valid for LDs
    MR_PROT_INFO_TYPE_CONTROLLER = 8,   // Proprietary method - LD is type 0, but controller appends and strips PI
} MR_PROT_INFO_TYPE;

/*
 * define the enum for logical drive power saving params
 */
typedef enum _MR_LD_PS_PARAMS
{
    MR_LD_PS_PARAMS_SPINUP60        = 0x01,     // LD can spin up in 60 seconds, recommended for max power savings
    MR_LD_PS_PARAMS_T10SUPPORT      = 0x02,     // LD has drives that support T10 power conditions
    MR_LD_PS_PARAMS_MAX_WITHCACHE   = 0x04,     // LD's IO profile support MAX power savings with cached writes
} MR_LD_PS_PARAMS;

/*
 * define the enum for logical drive power saving policy
 */
typedef enum _MR_LD_PS_POLICY
{
    MR_LD_PS_POLICY_CTRL_DEFAULT    = 0x00,     // LD shall use controller default power saving policy
    MR_LD_PS_POLICY_AUTO            = 0x01,     // LD power savings are managed by FW (will use T10 power conditions and max as appropriate)
    MR_LD_PS_POLICY_MAX             = 0x02,     // LD should use MAX power savings (not recommended if MR_LD_PS_PARAMS_SPINUP60=0)
    MR_LD_PS_POLICY_DONTCACHE       = 0x04,     // LD shall not cache writes to maximise power savings
    MR_LD_PS_POLICY_T10             = 0x08,     // LD should use T10 power conditions for power savings
    MR_LD_PS_POLICY_NONE            = 0xfe,     // LD power savings are disabled
    MR_LD_PS_POLICY_RESERVED        = 0xff,     // This value is reserved
} MR_LD_PS_POLICY;

/*
 * define the enum for SSCD type
 */
typedef enum _MR_LD_SSCD_TYPE
{
    MR_LD_SSCD_TYPE_SIMPLE          = 0x01,     // LD is a regular SSCD
    MR_LD_SSCD_TYPE_ELASTIC_CACHE   = 0x02,     // LD is a Elastic Cache SSCD
} MR_LD_SSCD_TYPE;

/*
 * define the enum for 512e emulation type
 */
typedef enum _MR_LD_EMULATION_TYPE
{
    MR_LD_EMULATION_DEFAULT         = 0x0,     // Defaults to MFCD.enable512eSupport
    MR_LD_EMULATION_DISABLE         = 0x1,     // Override MFCD and disable 512e support
} MR_LD_EMULATION_TYPE;

/*
 * defines the logical drive properties structure
 */
typedef struct _MR_LD_PROPERTIES {
    MR_LD_REF   ldRef;              // LD reference
    char    name[16];               // LD name (NULL-terminated, so max length is 15+NULL)
    U8      defaultCachePolicy;     // default cache policy settings (MR_LD_CACHE)
    U8      accessPolicy;           // access policy settings (MR_LD_ACCESS)
    U8      diskCachePolicy;        // physical disk cache policy (MR_PD_CACHE)
    U8      currentCachePolicy;     // current cache policy settings (MR_LD_CACHE) - can differ from default due to bad battery
    U8      noBGI;                  // set TRUE to disable BGI on this LD
#ifndef MFI_BIG_ENDIAN
    U8      autoSnapshot    : 1;    // set TRUE to enable auto Snapshot
    U8      disableAutoDeleteOldestPIT  : 1;     // Auto delete oldest PIT, 0 = enabled, 1 - disabled. FW will also auto delete View if
                                                 // one exists on this PiT
    U8      exclusiveAccess : 1;    // set TRUE to make access, exclusive. MR_LD_INFO.peerHasExclusiveAccess indicates which ctrl has access
    U8      emulationType   : 1;    // MR_LD_EMULATION_TYPE
    U8      scsiUnmapEnabled: 1;    // user request: 1 = enable unmap on this LD, or 0 = disable unmap
    U8      forceEmulation  : 1;    // Only set for forcing emulation status. If set, emulationType has no meaning
//    U8      disableHOQRebuild: 1;   // Deprecated. set TRUE to disable Head of Queue improvement for Rebuild operation
    U8      SSCWriteBuferMode: 1;   // SSCD write buffer mode (0 = disabled, 1 = enabled) (applicable for CacheCade 3.0 only)
    U8      reserved        : 1;
#else
    U8      reserved        : 1;
    U8      SSCWriteBuferMode: 1;   // SSCD write buffer mode (0 = disabled, 1 = enabled) (applicable for CacheCade 3.0 only)
//    U8      disableHOQRebuild: 1;   // Deprecated. set TRUE to disable Head of Queue improvement for Rebuild operation
    U8      forceEmulation  : 1;    // Only set for forcing emulation status. If set, emulationType has no meaning
    U8      scsiUnmapEnabled: 1;    // user request: 1 = enable unmap on this LD, or 0 = disable unmap
    U8      emulationType   : 1;    // MR_LD_EMULATION_TYPE
    U8      exclusiveAccess : 1;    // set TRUE to make access, exclusive. MR_LD_INFO.peerHasExclusiveAccess indicates which ctrl has access
    U8      disableAutoDeleteOldestPIT  : 1;
    U8      autoSnapshot    : 1;
#endif  // MFI_BIG_ENDIAN

    U8      defaultPSPolicy;        // power savings policy set for this LD (bitmap of MR_LD_PS_POLICY)
    U8      currentPSPolicy;        // current power savings mode for this LD (bitmap of MR_LD_PS_POLICY)
#ifndef MFI_BIG_ENDIAN
    U8      enabledAtomicityType : 2; // 0 = LD doesn't support Atomicity (enum MR_ATOMICITY_TYPE)
                                      // 1 = FW supports Atomicity for every Write command
                                      // 2 = FW supports T10 compliance Atomic Write command
    U8      cacheBypassIOSize  : 3;   // IO size to be cache bypassed (enum MR_CACHEBYPASS_IOSIZE)
    U8      cacheBypassMode  : 3;     // Cache bypass mode (enum MR_CACHEBYPASS_MODE)
#else
    U8      cacheBypassMode  : 3;     // Cache bypass mode (enum MR_CACHEBYPASS_MODE)
    U8      cacheBypassIOSize  : 3;   // IO size to be cache bypassed (enum MR_CACHEBYPASS_IOSIZE)
    U8      enabledAtomicityType : 2; // 0 = FW doesn't support Atomicity (enum MR_ATOMICITY_TYPE)
                                      // 1 = FW supports Atomicity for every Write command
                                      // 2 = FW supports T10 compliance Atomic Write command
#endif  // MFI_BIG_ENDIAN
    U8      reserved1[3];           // pad
} MR_LD_PROPERTIES;                 // 32 bytes

/*
 * defines the logical drive parameters structure
 */
typedef struct _MR_LD_PARAMETERS {
    U8      PRL;                    // DDF - primay raid level
    U8      RLQ;                    // DDF - raid level qaulifier
    U8      SRL;                    // DDF - secondary raid level
    U8      stripeSize;             // Strip_Size per DDF (0=512, 1=1K, 2=2K, 3=4K, 7=64K, 11=1MB, etc.)  -  (should be named stripSize)
    U8      numDrives;              // number of drivers per span
    U8      spanDepth;              // number of spans. If only one array then it is considered as 1 span
    U8      state;                  // logical drive state (MR_LD_STATE)
    U8      initState;              // 0=no init, 1=quick init, 2=full init
    U8      isConsistent;           // 0=LD not consistent (or n/a), 1=LD is consistent
    U8      isMorphing;             // 0=LD is not morphing, 1=LD is morphing (CE/RLM is active)
    U8      encryptionType;         // logical drive encryption type (MR_LD_ENCRYPTION_TYPE)
    U8      isCachePinned;          // 0=LD doesn't have pinned cache, 1=LD has pinned controller cache
    U8      badBlocksExist;         // 0=LD bad block table is empty, 1=LD bad block table is not empty
    U8      snapshotEnabled;        // 0=snapshot is not enabled, 1=snapshot is enabled
    U8      notReady;               // 1=LD is not ready to process OS requests
    union {
        U8      isSSCD;             // 1=LD is used for Secondary Cache
        U8      SSCDType;           // Type of Secondary Cache (MR_LD_SSCD_TYPE)
    };
    U8      powerSavingOptions;     // Parameters controlling power savings for this LD (bitmap of MR_LD_PS_PARAMS)
    U8      piType;                 // logical drive protection type (MR_PROT_INFO_TYPE)
    U8      accessPolicyStatus;     // Status bits indicating reason for MR_LD_ACCESS (MR_LD_ACCESS_STATUS)
    U8      currentAccessPolicy;    // current access policy (MR_LD_ACCESS), valid when accessPolicyStatus != MR_LD_ACCESS_STATUS_USER
                                    // see accessPolicyStatus if different from MR_LD_PROPERTIES.accessPolicy
    struct {
#ifndef MFI_BIG_ENDIAN
        U8  fastPathWBEnabled : 1;   // 0 = FastPathWB disabled, 1 = FastPathWB enabled
        U8  ldSmartEnabled    : 1;   // 0 = ldSmart not supported/disabled, 1 = ldSmart enabled
        U8  isEPD             : 1;   // 1 = VD is an enhanced physical drive
        U8  scsiUnmapEnabled  : 1;   // DEPRECATED 1 = Unmap enabled, 0 = Unmap disabled.
        U8  reserved          : 4;
#else
        U8  reserved          : 4;
        U8  scsiUnmapEnabled  : 1;   // DEPRECATED 1 = Unmap enabled, 0 = Unmap disabled.
        U8  isEPD             : 1;   // 1 = VD is an enhanced physical drive
        U8  ldSmartEnabled    : 1;   // 0 = ldSmart not supported/disabled, 1 = ldSmart enabled
        U8  fastPathWBEnabled : 1;   // 0 = FastPathWB disabled, 1 = FastPathWB enabled

#endif  // MFI_BIG_ENDIAN
    } characteristics;
    U8      rsvd2[11];              // pad
} MR_LD_PARAMETERS;                 // 32 bytes


/*
 * defines the logical drive progress structure
 */
typedef struct _MR_LD_PROGRESS {
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     cc      : 1;    // check consistency is in progress
        U32     bgi     : 1;    // Background Init is in progress
        U32     fgi     : 1;    // Foreground Init is in progress
        U32     recon   : 1;    // CE/RLM is in progress
        U32     erase   : 1;    // secure erase
        U32     disassociate :1;// SSC disassociate is in progress
        U32     locate  : 1;    // Locate
        U32     reserved:25;
#else
        U32     reserved:25;
        U32     locate  : 1;    // Locate
        U32     disassociate :1;// SSC disassociate is in progress
        U32     erase   : 1;    // secure erase
        U32     recon   : 1;    // CE/RLM is in progress
        U32     fgi     : 1;    // Foreground Init is in progress
        U32     bgi     : 1;    // Background Init is in progress
        U32     cc      : 1;    // check consistency is in progress
#endif  // MFI_BIG_ENDIAN
    } active;
    union {
        MR_PROGRESS     cc;         // progress for CC
        MR_PROGRESS     erase;      // progress for erase
    };
    MR_PROGRESS     bgi;        // progress for BGI
    MR_PROGRESS     fgi;        // progress for FGI
    MR_PROGRESS     recon;      // progress for CE/RLM
    MR_PROGRESS     disassociate; // progress for LD disassociate from SSC
                                  // If LD is an SSCD, then progress of disassociating all LDs from this SSCD
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     cc      : 1;    // check consistency is in pause
        U32     bgi     : 1;    // Background Init is in pause
        U32     fgi     : 1;    // Foreground Init is in pause
        U32     recon   : 1;    // CE/RLM is in pause
        U32     erase   : 1;    // secure erase in pause
        U32     disassociate :1;// SSC disassociate is in pause
        U32     reserved:26;
#else
        U32     reserved:26;
        U32     disassociate :1;// SSC disassociate is in pause
        U32     erase   : 1;    // secure erase in pause
        U32     recon   : 1;    // CE/RLM is in pause
        U32     fgi     : 1;    // Foreground Init is in pause
        U32     bgi     : 1;    // Background Init is in pause
        U32     cc      : 1;    // check consistency is in pause
#endif  // MFI_BIG_ENDIAN
    } pause;

    MR_PROGRESS     reserved[2];
} MR_LD_PROGRESS;               // 4 + (4 * number of bits defined) bytes. Today it is 36 bytes


/*
 * defines the span structure
 */
typedef struct _MR_SPAN {
    U64     startBlock;             // span physical disk starting block
    U64     numBlocks;              // span physical disk block count
    U16     arrayRef;               // array reference (index)
#ifndef MFI_BIG_ENDIAN
    U8      stateValid : 1;
    U8      state      : 7;         // span state (MR_LD_STATE)
#else
    U8      state      : 7;         // span state (MR_LD_STATE)
    U8      stateValid : 1;
#endif  // MFI_BIG_ENDIAN
    U8      reserved[5];
} MR_SPAN;                          // 24 bytes


/*
 * defines the logical drive config structure
 */
typedef struct _MR_LD_CONFIG {
    MR_LD_PROPERTIES    properties;
    MR_LD_PARAMETERS    params;
    MR_SPAN             span[MAX_SPAN_DEPTH];
} MR_LD_CONFIG;         // 32 +32 + (MAX_SPAN_DEPTH * 24) bytes. For 8 span, it is 256 bytes


/*
 * defines the logical drive allowed operations structure
 */
typedef struct _MR_LD_ALLOWED_OPS {
#ifndef MFI_BIG_ENDIAN
    U32 startFGI            : 1;  // Please see fullFGDisallow also
    U32 stopFGI             : 1;
    U32 startCC             : 1;
    U32 stopCC              : 1;
    U32 startRecon          : 1;
    U32 deleteLD            : 1;
    U32 secure              : 1;  // 1=unsecured LD, made up of FDE drives, can be secured
    U32 enableSnapshot      : 1;  // 0=if SnapVD, OR snapVD doesn't exist OR exists but no space
    U32 disableSnapshot     : 1;  // 0=when rollback is in progress OR snapShot is not enabled.
    U32 rollbackPIT         : 1;  // 0=when rollback is in progress, OR PIT doesn't exist.
    U32 createPITView       : 1;  // 1=a PIT and optionally VIEW can be created
    U32 deleteView          : 1;  // 1=there is a View for a PIT on the VD
    U32 enableAutoSnapshot  : 1;  // 1=auto snapshot cannot be enabled
    U32 disableAutoSnapshot : 1;  // 0=auto snapshot cannot be disabled
    U32 SSCCacheable        : 1;  // 1=LD can be associated with an SSC
    U32 breakMirror         : 1;  // 1=break mirror is allowed
    U32 joinMirror          : 1;  // 1=join mirror is allowed
    U32 suspendCCBGI        : 1;  // 1=suspend BGI and CC operation
    U32 resumeCCBGI         : 1;  // 1=resume BGI and CC operation
    U32 suspendRecon        : 1;  // 1=suspend Reconstruction operation
    U32 resumeRecon         : 1;  // 1=resume Reconstruction operation
    U32 startSecureEraseNonSED: 1;  // 1=start: secure erase for LDs (non SED drives)
    U32 stopSecureEraseNonSED : 1;  // 1=stop: secure erase for LDs (non SED drives)
    U32 forceDeleteLD       : 1;  // 1=force LD delete, as normal LD delete is not allowed, or will take long time.
    U32 fullFGDisallow      : 1;  //
                                  // startFGI fullFGDisallow             Meaning
                                  //  ---------------------------------------------------
                                  //     0       -           No foreground Init operation allowed
                                  //     1       1           Only Fast Init allowed
                                  //     1       0           Both fast and full foreground init allowed
    U32 disassociatedSSC    : 1;  // 1=Disassociation from cache pool allowed, 0=not allowed
    U32 disablePI           : 1;  // 1=disable PI allowed, there is no separate enable PI
    U32 enableHAShare       : 1;  // 1=LD can be shared in HA domain.  (boot LDs cannot be shared)
    U32 scsiUnmap           : 1;  // 1=LD unmap/trim is allowed
    U32 hideLD              : 1;  // 1=Hidden Property can be set on LD
    U32 unhideLD            : 1;  // 1=Hidden Property can be unset on LD
    U32 atomicityChangeable : 1;  // 1=atomicity can be changed from Enable to Disable or from Disable to Enable
#else
    U32 atomicityChangeable : 1;  // 1=atomicity can be changed from Enable to Disable or from Disable to Enable
    U32 unhideLD            : 1;  // 1=Hidden Property can be unset on LD
    U32 hideLD              : 1;  // 1=Hidden Property can be set on LD
    U32 scsiUnmap           : 1;  // 1=LD unmap/trim is allowed
    U32 enableHAShare       : 1;  // 1=LD can be shared in HA domain
    U32 disablePI           : 1;  // 1=disable PI allowed, there is no separate enable PI
    U32 disassociatedSSC    : 1;  // 1=Disassociation from cache pool allowed, 0=not allowed
    U32 fullFGDisallow      : 1;  // Described above
    U32 forceDeleteLD       : 1;  // 1=force LD delete, as normal LD delete is not allowed, or will take long time.
    U32 stopSecureEraseNonSED : 1;  // 1=stop: secure erase for LDs (non SED drives)
    U32 startSecureEraseNonSED: 1;  // 1=start: secure erase for LDs (non SED drives)
    U32 resumeRecon         : 1;  // 1=resume Reconstruction operation
    U32 suspendRecon        : 1;  // 1=suspend Reconstruction operation
    U32 resumeCCBGI         : 1;  // 1=resume BGI and CC operation
    U32 suspendCCBGI        : 1;  // 1=suspend BGI and CC operation
    U32 joinMirror          : 1;  // 1=join mirror is allowed
    U32 breakMirror         : 1;  // 1=break mirror is allowed
    U32 SSCCacheable        : 1;  // 1=LD can be associated with an SSC
    U32 disableAutoSnapshot : 1;  // 0=auto snapshot cannot be disabled
    U32 enableAutoSnapshot  : 1;  // 1=auto snapshot cannot be enabled
    U32 deleteView          : 1;  // 1=there is a View for a PIT on the VD
    U32 createPITView       : 1;  // 1=a PIT and optionally VIEW can be created
    U32 rollbackPIT         : 1;  // 0=when rollback is in progress, OR PIT doesn't exist.
    U32 disableSnapshot     : 1;  // 0=when rollback is in progress OR snapShot is not enabled.
    U32 enableSnapshot      : 1;  // 0=if SnapVD, OR snapVD doesn't exist OR exists but no space
    U32 secure              : 1;  // 1=unsecured LD, made up of FDE drives, can be secured
    U32 deleteLD            : 1;
    U32 startRecon          : 1;
    U32 stopCC              : 1;
    U32 startCC             : 1;
    U32 stopFGI             : 1;
    U32 startFGI            : 1;  // Please see fullFGDisallow also
#endif  // MFI_BIG_ENDIAN
} MR_LD_ALLOWED_OPS;

/*
 * TRANSPORT READY DHSP Options
 */
typedef enum _MR_TRANSPORT_READY_DHSP_OPTIONS {
    MR_TRANSPORT_READY_DHSP_OPTIONS_INCLUDE_ALL          = 0,    // Include All (that is include both exclusive and shared DHSP if available) or include what is
                                                                 // available (e.g., only shared DHSP is available or only exclusive DHSP is available for this DG)
    MR_TRANSPORT_READY_DHSP_OPTIONS_EXCLUDE_ALL          = 1,    // Exclude All
    MR_TRANSPORT_READY_DHSP_OPTIONS_INCLUDE_EDHSP        = 2     // Include exclusive DHSP only (exclude shared DHSP)
} MR_TRANSPORT_READY_DHSP_OPTIONS;

/*
 * defines the ext logical drive allowed operations structure
 */
typedef struct _MR_LD_ALLOWED_OPS_EXT {
#ifndef MFI_BIG_ENDIAN
    U32 startFGI            : 1;  // Please see fullFGDisallow also
    U32 stopFGI             : 1;
    U32 startCC             : 1;
    U32 stopCC              : 1;
    U32 startRecon          : 1;
    U32 deleteLD            : 1;
    U32 secure              : 1;  // 1=unsecured LD, made up of FDE drives, can be secured
    U32 reserved1           : 7;  // reserved for future use
    U32 SSCCacheable        : 1;  // 1=LD can be associated with an SSC
    U32 breakMirror         : 1;  // 1=break mirror is allowed
    U32 joinMirror          : 1;  // 1=join mirror is allowed
    U32 suspendCCBGI        : 1;  // 1=suspend BGI and CC operation
    U32 resumeCCBGI         : 1;  // 1=resume BGI and CC operation
    U32 suspendRecon        : 1;  // 1=suspend Reconstruction operation
    U32 resumeRecon         : 1;  // 1=resume Reconstruction operation
    U32 startSecureEraseNonSED: 1;  // 1=start: secure erase for LDs (non SED drives)
    U32 stopSecureEraseNonSED : 1;  // 1=stop: secure erase for LDs (non SED drives)
    U32 forceDeleteLD       : 1;  // 1=force LD delete, as normal LD delete is not allowed, or will take long time.
    U32 fullFGDisallow      : 1;  //
                                  // startFGI fullFGDisallow             Meaning
                                  //  ---------------------------------------------------
                                  //     0       -           No foreground Init operation allowed
                                  //     1       1           Only Fast Init allowed
                                  //     1       0           Both fast and full foreground init allowed
    U32 disassociatedSSC    : 1;  // 1=Disassociation from cache pool allowed, 0=not allowed
    U32 disablePI           : 1;  // 1=disable PI allowed, there is no separate enable PI
    U32 enableHAShare       : 1;  // 1=LD can be shared in HA domain.  (boot LDs cannot be shared)
    U32 scsiUnmap           : 1;  // 1=LD unmap/trim is allowed
    U32 hideLD              : 1;  // 1=Hidden Property can be set on LD
    U32 unhideLD            : 1;  // 1=Hidden Property can be unset on LD
    U32 atomicityChangeable : 1;  // 1=atomicity can be changed from Enable to Disable or from Disable to Enable
    U32 setTransportReady   : 1;  // 1= LD can be set as transport ready
    U32 reserved2           : 1;  // reserved for future use
    U32 clearTransportReady : 1;  // 1= Clear transport ready status for the LD
    U32 disableLDSetProperty: 1;  // 1= LD set property not allowed
    U32 reserved            : 28;
#else
    U32 reserved            : 28;
    U32 disableLDSetProperty: 1;  // 1= LD set property not allowed
    U32 clearTransportReady : 1;  // 1= Clear transport ready status for the LD
    U32 reserved2           : 1;  // reserved for future use
    U32 setTransportReady   : 1;  // 1= LD can be set as transport ready
    U32 atomicityChangeable : 1;  // 1=atomicity can be changed from Enable to Disable or from Disable to Enable
    U32 unhideLD            : 1;  // 1=Hidden Property can be unset on LD
    U32 hideLD              : 1;  // 1=Hidden Property can be set on LD
    U32 scsiUnmap           : 1;  // 1=LD unmap/trim is allowed
    U32 enableHAShare       : 1;  // 1=LD can be shared in HA domain
    U32 disablePI           : 1;  // 1=disable PI allowed, there is no separate enable PI
    U32 disassociatedSSC    : 1;  // 1=Disassociation from cache pool allowed, 0=not allowed
    U32 fullFGDisallow      : 1;  // Described above
    U32 forceDeleteLD       : 1;  // 1=force LD delete, as normal LD delete is not allowed, or will take long time.
    U32 stopSecureEraseNonSED : 1;  // 1=stop: secure erase for LDs (non SED drives)
    U32 startSecureEraseNonSED: 1;  // 1=start: secure erase for LDs (non SED drives)
    U32 resumeRecon         : 1;  // 1=resume Reconstruction operation
    U32 suspendRecon        : 1;  // 1=suspend Reconstruction operation
    U32 resumeCCBGI         : 1;  // 1=resume BGI and CC operation
    U32 suspendCCBGI        : 1;  // 1=suspend BGI and CC operation
    U32 joinMirror          : 1;  // 1=join mirror is allowed
    U32 breakMirror         : 1;  // 1=break mirror is allowed
    U32 SSCCacheable        : 1;  // 1=LD can be associated with an SSC
    U32 reserved1           : 7;  // reserved for future use
    U32 secure              : 1;  // 1=unsecured LD, made up of FDE drives, can be secured
    U32 deleteLD            : 1;
    U32 startRecon          : 1;
    U32 stopCC              : 1;
    U32 startCC             : 1;
    U32 stopFGI             : 1;
    U32 startFGI            : 1;  // Please see fullFGDisallow also
#endif  // MFI_BIG_ENDIAN
} MR_LD_ALLOWED_OPS_EXT;

/*
 * defines the logical drive allowed operations list structure
 */
typedef struct _MR_LD_ALLOWED_OPS_LIST {
    U32                 count;                              // count of valid entries in this list
    U32                 pad;
    MR_LD_ALLOWED_OPS   allowedOpsList[MAX_API_LOGICAL_DRIVES_EXT]; // Use targetId to index into this array
} MR_LD_ALLOWED_OPS_LIST;                                   // This structure is 264 bytes

/*
 * defines the ext logical drive allowed operations list structure
 */
typedef struct _MR_LD_ALLOWED_OPS_LIST_EXT {
    U32                     size;                           // size of total data structure (including size/count fields)
    U32                     count;                          // count of valid entries in this list
    U32                     pad[2];
    MR_LD_ALLOWED_OPS_EXT   allowedOpsList[1];              // Use targetId to index into this array
} MR_LD_ALLOWED_OPS_LIST_EXT;                               // This structure is 264 bytes

/*
 * defines the logical drive information structure
 */
typedef struct _MR_LD_INFO {
    MR_LD_CONFIG        ldConfig;
    U64                 size;                   // Total size in 512-byte blocks of the logical drive
    MR_LD_PROGRESS      progInfo;
    U16                 clusterOwnerDeviceId;   // DEPRECATED LD owner's device Id
    U8                  reconstructActive;      // TRUE when reconstruction is active o nthis LD
#ifndef MFI_BIG_ENDIAN
    U8                  snapshotRepository  : 1;    // This LD is a snapshot repository
    U8                  autoSnapshotEnabled : 1;    // auto Snapshot is enabled
    U8                  peerHasExclusiveAccess : 1;    // Peer ctrl has exclusive access to this LD
    U8                  peerHasNoAccess : 1;        // Peer can't access this shared ld, not applicable for exclusive LD
    U8                  reserved1           : 4;
#else
    U8                  reserved1           : 4;
    U8                  peerHasNoAccess : 1;        // Peer can't access this shared ld, not applicable for exclusive LD
    U8                  peerHasExclusiveAccess : 1;    // Peer ctrl has exclusive access to this LD
    U8                  autoSnapshotEnabled : 1;    // this LD has auto Snapshot enabled
    U8                  snapshotRepository  : 1;    // This LD is a snapshot repository
#endif  // MFI_BIG_ENDIAN
    U8                  logicalVPDPage83[64];   // Firmware generated VPD page for the LD
    union{
        MR_LD_ALLOWED_OPS       allowedOps;         // Operations allowed on this LD
        MR_LD_ALLOWED_OPS_EXT   allowedOpsExt;      // extended operations allowed on this LD
    };
    U8                  reserved2[8];
} MR_LD_INFO;                                   // Today it is 384 bytes

/*
 * defines the logical drive join-mirror operations
 */
typedef enum _MR_LD_MIRROR_OP {             // LD join-mirror operations
    MR_LD_MIRROR_OP_NORMAL          = 0,    // Rebuild broken mirror after join-mirror
    MR_LD_MIRROR_OP_RETAIN_MIRROR   = 1,    // Use broken mirror as rebuild source after join-mirror
    MR_LD_MIRROR_OP_CREATE_VD       = 2,    // Import broken mirror as a new VD
} MR_LD_MIRROR_OP;


/*
 * defines the logical drive reconstruction (CE/RLM) structure
 */
typedef enum _MR_RECON_OP {                 // reconstruct operations
    MR_RECON_OP_NONE    = 0,                // no drives added/removed - 'a RAID level change OR OCE within the array for the LD'
    MR_RECON_OP_INSERT  = 1,                // add new drives
    MR_RECON_OP_DELETE  = 2,                // remove drives
} MR_RECON_OP;

typedef struct _MR_RECON {
    MR_LD_REF   ldRef;                      // LD reference
    U8          operation;                  // reconstruct operation(type MR_RECON_OP)
    U8          newRaidLevel;               // New primary RAID level after RLM - Same as MR_LD_PARAMETERS.PRL
    U8          newPdCount;                 // Number of drives to be added
    union {
        U8      percentExpansion;           // This today applies to MR_RECON_OP_NONE with a OCE requested within that array for the LD.
                                            // The percent is the empty space left in the array for the LD to expand. FW will adjust the
                                            // expansion size to match to the nearest multiple of RAID level row data size.
                                            // A value of of 0 and 100 will be assumed 100%
        U8      reserved;                   // pad
    };
    MR_PD_REF   newPd[MAX_API_ROW_SIZE];    // PD device Id's of newPdCount
} MR_RECON;                                 // 136 bytes

/*
 * defines the logical drive node (e.g IOV) structure
 */
typedef struct _MR_LD_VF_MAP {
    U32        size;                          // size of total data structure (including size/count fields)
    MR_LD_REF  ref;                           // Virtual disk for which the access policy is specified.
    U8         ldVfCount;                     // Total number of virtual functions or nodes of this LD
    U8         reserved[6];
    U8         policy[1];                     // Variable list of MR_LD_ACCESS policy for each node(s).
} MR_LD_VF_MAP;

typedef struct _MR_LD_VF_AFFILIATION {
    U32             size;                     // size of total data structure (including size/count fields)
    U8              ldCount;                  // Number of virtual disks for which the information is returned.
    U8              vfCount;                  // Total number of VFs/Nodes in the system
    U8              thisVf;                   // Set this value to the current VF index or node index.
    U8              ldCountMsb;               // MSB value of the ldCount param
    U8              reserved[8];
    MR_LD_VF_MAP    map[1];                   // Variable list of MR_LD_VF_MAP
} MR_LD_VF_AFFILIATION;

/*
 * defines the LD boost priority structures
 */

typedef enum _MR_LD_BOOST_TYPE {
    MR_LD_BOOST_TYPE_NONE       = 0,
    MR_LD_BOOST_TYPE_FILESYSTEM = 1,
} MR_LD_BOOST_TYPE;

typedef struct _MR_LD_BOOST_PRIORITY_TYPE {
    MR_LD_REF  ldRef;                       // LD reference
    U8         boostType;                   // current LD boost type (MR_LD_BOOST_TYPE)
    U8         reserved[3];
} MR_LD_BOOST_PRIORITY_TYPE;

typedef struct _MR_LD_BOOST_PRIORITY_TYPE_LIST {
    U32  size;                              // size of total data structure (including size/count fields)
    U32  count;                             // count of number of deviceId entries in this list
    U8   maxBoostType;                      // max supported boost type (MR_LD_BOOST_TYPE)
    U8   defaultType;                       // default boost type (MR_LD_BOOST_TYPE)
    U8   pad[2];
    MR_LD_BOOST_PRIORITY_TYPE  type[1];     // boost type attribute per LD
} MR_LD_BOOST_PRIORITY_TYPE_LIST;


/*
 * defines the logical drive performance metrics structure
 * These metrics are valid for the current collection period
 */

typedef struct _MR_IO_METRICS_SIZE {
    U32         lessThan512B;                       // Number of IOs: size <= 512B
    U32         between512B_4K;                     // Number of IOs: 512B < size <=4K
    U32         between4K_16K;                      // Number of IOs:   4K < size <=16K
    U32         between16K_64K;                     // Number of IOs:  16K < size <=64K
    U32         between64K_256K;                    // Number of IOs:  64K < size <=256K
    U32         moreThan256K;                       // Number of IOs: 256K < size
} MR_IO_METRICS_SIZE;

/*
 * define the structure to capture the randomness of the IOs
 * each counter is for IOs, whose LBA is set distance apart from the previous IO.
 */
typedef struct _MR_IO_METRICS_RANDOMNESS {
    U32         sequential;                         // Number of IOs: sequential ( inter-LBA distance is 0)
    U32         lessThan64K;                        // Number of IOs: within 64KB of previous IO
    U32         between64K_512K;                    // Number of IOs:  64K < LBA <=512K
    U32         between512K_16M;                    // Number of IOs: 512K < LBA <=16M
    U32         between16M_256M;                    // Number of IOs:  16M < LBA <=256M
    U32         between256M_1G;                     // Number of IOs: 256M < LBA <=1G
    U32         moreThan1G;                         // Number of IOs:   1G < LBA
} MR_IO_METRICS_RANDOMNESS;

/*
 * define the structure for LD cache usage
 */
typedef struct _MR_IO_METRICS_LD_CACHE {
    U8                          targetId;           // LD target id (0 to MAX_TARGET_ID)
    U8                          targetIdMsb;        // LD target id (0 to MAX_API_TARGET_ID_EXT2)
    U8                          reserved[6];        // For future use
    MR_IO_METRICS_SIZE          readSizeCache;      // Reads to Primary Cache
    MR_IO_METRICS_SIZE          writeSizeCache;     // Writes to Primary Cache
    MR_IO_METRICS_SIZE          readSizeSSC;        // Reads to Secondary Cache
    MR_IO_METRICS_SIZE          writeSizeSSC;       // Writes to Secondary Cache
} MR_IO_METRICS_LD_CACHE;

/*
 * define the structure for detailed SSC cache usage
 */
typedef struct _MR_IO_METRICS_SSC_WINDOW_USAGE {
    U8          numCacheLines;                      // Number of cache lines in use
    U8          reserved[3];
    U32         numWindows;                         // Number of windows with numCacheLines in use
    U32         numReads;                           // Number of read
    U32         numWrite;
} MR_IO_METRICS_SSC_WINDOW_USAGE;

typedef enum _MR_SSC_WINDOW_ACCESS_RATE {
    MR_SSC_WINDOW_ACCESS_RATE_0         =  0,       // No accesses (beyond allocation)
    MR_SSC_WINDOW_ACCESS_RATE_100       =  1,       // Number of accesses: 0    < access <=100
    MR_SSC_WINDOW_ACCESS_RATE_500       =  2,       // Number of accesses: 100  < access <=500
    MR_SSC_WINDOW_ACCESS_RATE_1K        =  3,       // Number of accesses: 500  < access <=1K
    MR_SSC_WINDOW_ACCESS_RATE_5K        =  4,       // Number of accesses: 1K   < access <=5K
    MR_SSC_WINDOW_ACCESS_RATE_10K       =  5,       // Number of accesses: 5K   < access <=10K
    MR_SSC_WINDOW_ACCESS_RATE_50K       =  6,       // Number of accesses: 10K  < access <=50K
    MR_SSC_WINDOW_ACCESS_RATE_100K      =  7,       // Number of accesses: 50K  < access <=100K
    MR_SSC_WINDOW_ACCESS_RATE_500K      =  8,       // Number of accesses: 100K < access <=500K
    MR_SSC_WINDOW_ACCESS_RATE_1M        =  9,       // Number of accesses: 500K < access <=1M
    MR_SSC_WINDOW_ACCESS_RATE_1Mplus    = 10,       // Number of accesses: more than 1 million

    MR_SSC_WINDOW_ACCESS_RATE_MAX,
} MR_SSC_WINDOW_ACCESS_RATE;

typedef struct _MR_IO_METRICS_SSC_WINDOW_ACCESS {
    U32         readAccess[MR_SSC_WINDOW_ACCESS_RATE_MAX];
    U32         writeAccess[MR_SSC_WINDOW_ACCESS_RATE_MAX];
} MR_IO_METRICS_SSC_WINDOW_ACCESS;

typedef struct _MR_IO_METRICS_SSC_DETAIL {
    U8                                  targetId;           // LD target id (0 to MAX_TARGET_ID)
    U8                                  targetIdMsb;        // LD target id (0 to MAX_API_TARGET_ID_EXT2)
    U8                                  reserved[2];        // For future use
    U32                                 numWindows;         // Number of SSC windows currently allocated for LD
    U32                                 numNewWindows;      // Number of New SSC windows allocated for LD in this collection period
    MR_IO_METRICS_SSC_WINDOW_USAGE      windowUsage;        // Provides fill-data for windows, how many are lightly or fully in use
    MR_IO_METRICS_SSC_WINDOW_ACCESS     windowAccess;       // Provide hit-rates for windows, how many times window are accessed per second
} MR_IO_METRICS_SSC_DETAIL;

/*
 * define the structure for controller cache usage
 */
typedef struct _MR_IO_METRICS_CACHE {
    U32                         size;               // size of this data structure (including size field)
    U32                         collectionPeriod;   // Time (sec), taken to collect this data

    U32                         avgDirtyCache;      // Running average of dirty cache (% of cache size)
    U32                         avgCacheUsed;       // Running average of total cache in use
    U32                         readAheadCache;     // Cache(MB) used for Read Ahead data
    U32                         readAheadSSC;       // Secondary Cache(MB) used for Read Ahead data
    U32                         unusedReadAheadCache;   // Cache(MB) for Read Ahead data, that wasn't accessed
    U32                         unusedReadAheadSSC; // Secondary Cache(MB) for Read Ahead data, that wasn't accessed
    U32                         flushBlockTime;     // Time(ms) IOs were blocked while cache is flushed etc.
    U16                         countSSCDetail;     // count of number of targetId SSCDetail entries
    U16                         count;              // count of number of targetId ldIoCache entries
    MR_IO_METRICS_LD_CACHE      ldIoCache[1];       // Variable list of LD IO metrics
    MR_IO_METRICS_SSC_DETAIL    SSCDetail[1];       // Variable list of LD SSC detailed metrics
} MR_IO_METRICS_CACHE;

/*
 * define the structure for overall LD IO metrics (from host perspective)
 */
typedef struct _MR_IO_METRICS_LD_OVERALL {
    U8                          targetId;           // LD target id (0 to MAX_TARGET_ID)
    U8                          targetIdMsb;        // LD target id (0 to MAX_API_TARGET_ID_EXT2)
    U16                         idleTime;           // Total seconds, LD has been idle
    U32                         reserved;
    U32                         readMB;             // Total read data transferred in MB
    U32                         writeMB;            // Total write data transferred in MB
    MR_IO_METRICS_SIZE          readSize;           // Aggregagate the number of read IOs for total IO count
    MR_IO_METRICS_SIZE          writeSize;          // Aggregate the number write IOs for write total IO count
    MR_IO_METRICS_RANDOMNESS    readRandomness;
    MR_IO_METRICS_RANDOMNESS    writeRandomness;
} MR_IO_METRICS_LD_OVERALL;

typedef struct _MR_IO_METRICS_LD_OVERALL_LIST {
    U32                         size;               // size of this data structure (including size field)
    U32                         collectionPeriod;   // Time (sec), taken to collect this data

    MR_IO_METRICS_LD_OVERALL    ldIOOverall[1];     // Variable list of overall LD IO metrics
} MR_IO_METRICS_LD_OVERALL_LIST;

/*
 * define the structure for controller's IO metrics
 */
typedef struct _MR_IO_METRICS {
    MR_IO_METRICS_CACHE             ctrlIoCache;    // controller cache usage
    MR_IO_METRICS_LD_OVERALL_LIST   ldIoMetrics;    // overall host IO metrics
} MR_IO_METRICS;

/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to SNAPSHOT
 * Any structure ending in _PARAMS here is user provided and _INFO is FW provided
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */


/*
 * define the LD snapshot enable parameters
 */
typedef struct _MR_LD_SNAPSHOT_ENABLE_PARAMS {
    U64 size;                   // Total size in 512-byte blocks to be used from snap VD (targetId for Repository) for max PITs
                                // includes meta-data + user data
    U8  targetIDforSnapshot;    // targetId (of SnapVD) for the PITs, not exposed to host disk mgmt
    U8  enableAutoSnapShot;     // enable auto snapshot
    U8  rsvd[6];                // reserved for future
} MR_LD_SNAPSHOT_ENABLE_PARAMS;

/*
 * Defines the snapshot schedule state enum
 */
typedef enum _MR_SNAPSHOT_SCHEDULE_STATE {
    MR_SNAPSHOT_SCHEDULE_STATE_DISABLED       = 0x00,   // scheduler is disabled
    MR_SNAPSHOT_SCHEDULE_STATE_ENABLED        = 0x01,   // scheduler is enabled
} MR_SNAPSHOT_SCHEDULE_STATE;

/*
 * defines the Snapshot Schedule properties structure
 */
typedef struct _MR_SNAPSHOT_SCHEDULE_PROPERTIES {
    U32             nextExec;                           // time of next execution (seconds past 1/1/2000)
    U16             execFreq;                           // execution frequency in hours (i.e. how often snapshot runs). 0=never scheduled
    U8              state;                              // Snapshot scheduler state (MR_SNAPSHOT_SCHEDULE_STATE)
    U8              pad;                                // Padded for alignment
} MR_SNAPSHOT_SCHEDULE_PROPERTIES;                      // size = 8 bytes

/*
 * define the enum for PIT states
 */
typedef enum _MR_PIT_STATE {
    MR_PIT_STATE_NORMAL             = 0,
    MR_PIT_STATE_ROLLBACK           = 1
} MR_PIT_STATE;

/*
 * define the LD PIT/VIEW INFO
 *
 * Applications shouldn't populate the fields marked (IN), FW shall ignore these
 * values.
 */

typedef struct _MR_LD_SNAPSHOT_PIT_VIEW_PARAMS {    // PIT & VIEW PARAMS
    MR_LD_REF       ldRef;                          // MR_LD_REF of source LD
    MR_TIME_STAMP   PITCreationTime;                // PIT creation time (IN for PIT creation)
    char            PITName[16];                    // PIT name (NULL-terminated, so max length is 15+NULL)
    U8              pad[4];
    U64             viewSize;                       // VIEW capacity in 512-byte blocks.  Zero to use default value
    char            viewName[16];                   // VIEW name (NULL-terminated, so max length is 15+NULL)
    MR_TIME_STAMP   viewCreationTime;               // VIEW creation time, associated with this PIT, if it exists (IN)
    U8              accessPolicy;                   // VIEW access policy (MR_LD_ACCESS)
    U8              viewTargetId;                   // VIEW target ID (IN)
    U8              rsvd[6];
} MR_LD_SNAPSHOT_PIT_VIEW_PARAMS;

/*
 * define the LD PIT's data
 */
typedef struct _MR_LD_PIT_INFO {
    MR_LD_SNAPSHOT_PIT_VIEW_PARAMS    params;  // PIT creation information
    union {
        MR_PROGRESS     progress;              // Rollback progress if state says that
        U32             rsvd;                  // could be used for something else later
    };
    U8                  status;                // PIT status: PITs targetID status (MR_LD_STATE)
    U8                  state;                 // PIT state: Currently only Rollback or Normal (MR_PIT_STATE)
    U16                 capacityUsed;          // 0-0xFFFF - %used: capacityUsed*100/0xFFFF%: from the snapshot enable
                                               // capacity field currently this PIT is using
    U8                  rsvd1[8];              // reserved for future
} MR_LD_PIT_INFO;

/*
 * define the LD VIEW Info
 */
typedef struct _MR_LD_VIEW_INFO {
    MR_LD_SNAPSHOT_PIT_VIEW_PARAMS    params;       // PIT creation information
    U8                      logicalVPDPage83[64];   // Firmware generated VPD page for the LD
    U8                      sourceTargetId;         // Target Id of VIEW's source LD
    U8                      status;                 // VIEW status: VIEW targetId status (MR_LD_STATE)
    U16                     capacityUsed;           // 0-0xFFFF - %used: capacityUsed*100/0xFFFF%: from the VIEW capacity
                                                    // that is currently used
    U8                      rsvd[4];                // reserved for future
} MR_LD_VIEW_INFO;

/*
 * define the VIEW LIST for all LDs
 */
typedef struct _MR_LD_VIEW_LIST {
    U32                 size;                   // size of valid data
    U32                 count;                  // number of Views
    MR_LD_VIEW_INFO     views[1];               // array of MR_LD_VIEW_INFO
} MR_LD_VIEW_LIST;

/*
 * define the LD snapshot info
 */
typedef struct _MR_LD_SNAPSHOT_INFO {           // valid if MR_LD_CONFIG says snapshot enabled
    U32                         size;           // size of valid data
    U8                          pad[4];
    MR_LD_SNAPSHOT_ENABLE_PARAMS params;
    U8                          numPITs;        // number of PITs on this LD
    U8                          numViews;       // number of VIEWs on this LD
    U8                          state;          // LD snapshot state (today operational=0 and non-operational=1)
    U8                          sourceTargetId; // Target Id of this LD
    U16                         capacityLeft;   // 0-0xFFFF - %Left: capacityLeft*100/0xFFFF%:Remaining PIT capacity %
                                                // from the capacity field of enable snapshot dcmd
    U8                          rsvd1[9];       // for pad and future use
    U8                          lastSnapSchedMissed; // 1=scheduled snapshot not taken. Cleared when createPiT is received on this LD
    MR_SNAPSHOT_SCHEDULE_PROPERTIES schedProp;  // snapshot schedule properties
    MR_LD_PIT_INFO              PITInfo[1];     // PIT Info for numPITs
    MR_LD_VIEW_INFO             viewInfo[1];    // VIEW-PIT Info for numVIEWs
} MR_LD_SNAPSHOT_INFO;                          // TBD today it stands at 320 bytes for 7PITs+1VIEW

/*
 * define all LDs snapshot info
 */
typedef struct _MR_LD_SNAPSHOT_ALL_INFO {         // For all LDs which has snapshot enabled
    U32                         size;             // size of valid data
    U8                          numVDs;           // number of LDs snapshot is enabled
    U8                          resvd[3];
    MR_LD_SNAPSHOT_INFO         snapshotInfo[1];  // snapshot info for numVDs
} MR_LD_SNAPSHOT_ALL_INFO;                        // TBD Can be a max of 63 * size of MR_LD_SNAPSHOT_DATA = 0x4ec0 bytes

/*
 * define the recoverable snapshot repository space
 */
typedef struct _MR_LD_SNAPSHOT_REPOSITORY_HOLE_INFO {
    U8                      holeRecoverable;    // 1=The sourceTargetId's hole is avaialble for garbage collection
    U8                      sourceTargetId;     // LD which owned this hole, 0xFF if no source LD is associated.
    U8                      pad[6];             // pad
    U64                     reserved;           // future use
    U64                     holeSize;           // Size of hole in Mbytes
} MR_LD_SNAPSHOT_REPOSITORY_HOLE_INFO;

/*
 * define the amount of space left in snapshot repository
 */
typedef struct _MR_LD_SNAPSHOT_REPOSITORY_SPACE {       // Contains all space available in a given snapshot repository
    U32                                     size;       // size of valid data
    MR_LD_REF                               ref;        // Snapshot repository  reference
    U8                                      numHoles;   // number of fragmented spaces available
    U8                                      pad[7];     // pad
    MR_LD_SNAPSHOT_REPOSITORY_HOLE_INFO     recoverableholeInfo[1];
} MR_LD_SNAPSHOT_REPOSITORY_SPACE;                      // Can be any number depending on the numHoles.



/*
 * define the LD BBM table
 */
typedef struct _MR_LD_BBM_LOG { // Contains LD BBM log
    U64 numCorrected;           // Number of corrected entries
    U64 numUnCorrected;         // Number of uncorrected entries
    U8  rsvd[16];               // rsvd for future
} MR_LD_BBM_LOG;                // 32 bytes per LD


/*
 * defines the LD BBM table list structure
 */
typedef struct _MR_LD_BBM_LOG_LIST {
    U32                 count;                              // count of valid entries in this list
    U32                 pad;
    MR_LD_BBM_LOG       bbmLog[MAX_API_LOGICAL_DRIVES_EXT2];// Use targetId to index into this array
} MR_LD_BBM_LOG_LIST;


/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to the RAID configuration. Also foreign configuration data structures.
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * define the array structure
 */
typedef struct _MR_ARRAY {
    U64         size;                   // size of smallest PD in array (one drive's block count)
    U8          numDrives;              // number of drives for this array
    U8          reserved;               // pad
    U16         arrayRef;               // tag for the array (may change when logical drives are deleted)
    U8          pad[20];                // for 64-bit alignment
    struct {
        MR_PD_REF   ref;                // The device IDs of the drives (MR_PD_INVALID(FFFF) = missing drive)
        U16         fwState;            // (type MR_PD_STATE) current PD state for read operations, desired PD state for CONFIG_ADD operations
        struct {                        // enclosure info - meaningful for configuration reads only - ignored for config writes
            U8      pd;                 // DeviceID of containing enclosure ((U8)MR_PD_INVALID(FF) = not within an enclosure).
            U8      slot;               // Slot number within enclosure. If not within an enclosure, slot = (U8)ref.deviceId
                                        //
                                        // HACK: 2-Byte deviceId, is shoe horned into a byte in both these fields.
                                        // This is ok as long as MAX_API_PHYSICAL_DEVICES is 256. Notice the casts above.
        } encl;                         // DEPRICATED for Configurations greater than 240 Pds or 240 Lds.
    } pd[MAX_API_ROW_SIZE];
} MR_ARRAY;                             // Today for 32 MAX_ROW_SIZE, it is 256+32=288 bytes

/*
 * define the array structure
 */
typedef struct _MR_ARRAY_EXT {
    U64         size;                   // size of smallest PD in array (one drive's block count)
    U16         numDrives;              // number of drives for this array
    U16         reserved;               // pad
    U16         arrayRef;               // tag for the array (may change when logical drives are deleted)
    U8          pad[18];                // for 64-bit alignment
    struct {
        MR_PD_REF   ref;                // The device IDs of the drives (MR_PD_INVALID(FFFF) = missing drive)
        U16         fwState;            // (type MR_PD_STATE) current PD state for read operations, desired PD state for CONFIG_ADD operations
        struct {                        // enclosure info - meaningful for configuration reads only - ignored for config writes
            U16     pd;                 // DeviceID of containing enclosure ((U8)MR_PD_INVALID(FF) = not within an enclosure).
            U16     slot;               // Slot number within enclosure. If not within an enclosure, slot = (U8)ref.deviceId
            U8      reserved[6];
        } encl;
    } pd[MAX_API_ROW_SIZE];
} MR_ARRAY_EXT;                         // Today for 32 MAX_ROW_SIZE, it is 512+32=544 bytes

/*
 * define the spare structure
 */
typedef union _MR_SPARE_TYPE {          // define spare type bitmap
    struct {
#ifndef MFI_BIG_ENDIAN
        U8      isDedicated    : 1;     // dedicated bit
        U8      isRevertible   : 1;     // revertibe bit
        U8      isEnclAffinity : 1;     // enclosure affinity bit. Should only be set, if Spare is part of an enclosure.
        U8      reserved       : 5;
#else
        U8      reserved       : 5;
        U8      isEnclAffinity : 1;     // enclosure affinity bit. Should only be set, if Spare is part of an enclosure.
        U8      isRevertible   : 1;     // revertibe bit
        U8      isDedicated    : 1;     // dedicated bit
#endif  // MFI_BIG_ENDIAN
    } mrSpareType;                      // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U8      type;
} MR_SPARE_TYPE;

typedef struct  _MR_SPARE {
    MR_PD_REF       ref;                // PD reference
    MR_SPARE_TYPE   spareType;          // hot-spare type qualifiers
    U8              reserved[2];
    U8              arrayCount;         // the number of arrays to which the PD can be dedicated
    U16             arrayRef[MAX_API_ARRAYS_DEDICATED]; // arrayReference numbers of arrayCount
} MR_SPARE;                             // Today for MAX_API_ARRAYS_DEDICATED=16 it is 40 bytes


/*
 * defines the firmware RAID configuration structure
 */
typedef struct  _MR_CONFIG_DATA {
    U32             size;           // total size of the structure
    U16             arrayCount;
    U16             arraySize;      // Size of each array struct
    U16             logDrvCount;
    U16             logDrvSize;     // Size of each logical drive struct
    U16             sparesCount;
    U16             sparesSize;     // Size of each spare struct
    U64             currentSSCSize; // Size of currently configured SSC, in 512 byte blocks
    U8              reserved[8];    // reserved for future use - pad to 8 bytes

    union {
        MR_ARRAY        array[1];   // variable count of MR_ARRAY entries
#if defined(MR_IMPLEMENTED_MAX_API_EXT2)
        MR_ARRAY_EXT    arrayExt[1];// variable count of MR_ARRAY_EXT entries
#endif
    };

    MR_LD_CONFIG    ld[1];          // variable count of MR_LD_CONFIG entries
    MR_SPARE        spare[1];       // variable count of MR_SPARE entries
} MR_CONFIG_DATA;

/*
 * defines the allowed configuration operations structure
 */
typedef struct _MR_CONFIG_ALLOWED_OPS {
#ifndef MFI_BIG_ENDIAN
    U32 addConfig                : 1;
    U32 clearConfig              : 1;
    U32 disallowForceClearConfig : 1;
    //To disallow clearconfig completely, "clearConfig" should be "0" and "disallowForceClearConfig" should be "1"
    U32 makeEPD                  : 1;   // 1 = EPDs may be created, via MR_DCMD_CFG_MAKE_EPD
    U32 reserved                 :28;
#else
    U32 reserved                 :28;
    U32 makeEPD                  : 1;
    //To disallow clearconfig completely, "clearConfig" should be "0" and "disallowForceClearConfig" should be "1"
    U32 disallowForceClearConfig : 1;
    U32 clearConfig              : 1;
    U32 addConfig                : 1;
#endif  // MFI_BIG_ENDIAN
} MR_CONFIG_ALLOWED_OPS;

/*
 * defines the DDF header GUID structure
 */
typedef struct _MR_DDF_HEADER_GUID {
        U8      headerGuid[24];         // DDF header GUID pg 57
} MR_DDF_HEADER_GUID;                   // 24 bytes

/*
 * defines the Foreign configurations GUID list structure
 */
typedef struct _MR_FOREIGN_CFG_GUIDS {
    U32                 count;          // count of valid header GUID's
    MR_DDF_HEADER_GUID  ddf[MAX_API_FOREIGN_CONFIGS];
} MR_FOREIGN_CFG_GUIDS;                 // Maximum of 196 bytes


/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to the enclosure device
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * defines the types of enclosures
 */
typedef enum _MR_ENCL_TYPE {                // enclosure types
    MR_ENCL_TYPE_UNKNOWN = 0,               // unknown enclosure
    MR_ENCL_TYPE_SAFTE   = 1,
    MR_ENCL_TYPE_SES     = 2,
    MR_ENCL_TYPE_SGPIO   = 3,
    MR_ENCL_TYPE_SC      = 4,               // OEM specific enclosure type
} MR_ENCL_TYPE;

/*
 * defines the sub-types of enclosures
 */
typedef enum _MR_ENCL_SUBTYPE {             // enclosure sub-types
    MR_ENCL_SUBTYPE_UNKNOWN = 0,            // unknown enclosure sub-type
    MR_ENCL_SUBTYPE_SC      = 1,            // OEM specific enclosure sub-type
    MR_ENCL_SUBTYPE_IOM_INT = 2,            // OEM specific enclosure sub-type
    MR_ENCL_SUBTYPE_IOM_EXT = 3,            // OEM specific enclosure sub-type
    MR_ENCL_SUBTYPE_SPADE   = 4,            // OEM specific enclosure sub-type
    MR_ENCL_SUBTYPE_VSES    = 5,            // vSES enclosure sub-type
    MR_ENCL_SUBTYPE_I2C     = 6,            // I2C based enclosure sub-type

} MR_ENCL_SUBTYPE;

/*
 * define the enclosure information structure
 */
typedef struct _MR_ENCL_INFO {
    U16     deviceId;           // enclosure device Id
    U8      index;              // DEPRECATED firmware internal enclosure index
    U8      slotCount;          // number of slots
    U8      psCount;            // number of power supplies
    U8      fanCount;           // number of fans
    U8      tempSensorCount;    // number of temperature sensors
    U8      alarmCount;         // number of alarms
    U8      SIMCount;           // number of SIM modules
    U8      isFault;            // enclosure has entered a FAULT state
    U8      type;               // type of enclosure (type MR_ENCL_TYPE)
    U8      pdCountSpinup60;    // number of drives that can be spun up at a time within 60 seconds

                                // Following two fields are valid if, MR_CTRL_INFO.adapterOperations.supportEnclEnumeration is set
    U8      enclPosition;       // position of enclosure in a cascaded chain.
    U8      enclConnectorIndex; // MR_SAS_CONNECTORS.connectors[enclConnectorIndex] to which this enclosure (chain) is attached.
                                // Set to MR_SAS_CONNECTOR_NONE, if EMM is not connected via a SAS connector.

    U8      slotOffset;         // Slot offset of the PD in the enclosure
    U8      pdCount;            // number of physical drives present
    U16     pdId[MAX_API_PD_PER_ENCLOSURE]; // physical drive deviceId's
} MR_ENCL_INFO;

typedef struct _MR_ENCL_INFO_EXT {
    U32     sizeOfThisEnclInfo;  // Size of this enclosure info structure
    U16     offsetOfPdList;      // Offset to the pdIdList in this structure
    U16     deviceId;            // enclosure device Id
    U16     slotCount;           // number of slots
    U8      index;               // DEPRECATED firmware internal enclosure index
    U8      psCount;             // number of power supplies
    U8      fanCount;            // number of fans
    U8      tempSensorCount;     // number of temperature sensors
    U8      alarmCount;          // number of alarms
    U8      SIMCount;            // number of SIM modules
    U8      isFault;             // enclosure has entered a FAULT state
    U8      type;                // type of enclosure (type MR_ENCL_TYPE)
    U8      pdCountSpinup60;     // number of drives that can be spun up at a time within 60 seconds
                                 // Following two fields are valid if, MR_CTRL_INFO.adapterOperations.supportEnclEnumeration is set
    U8      enclPosition;        // position of enclosure in a cascaded chain.
    U8      enclConnectorIndex;  // MR_SAS_CONNECTORS.connectors[enclConnectorIndex] to which this enclosure (chain) is attached.
                                 // Set to MR_SAS_CONNECTOR_NONE, if EMM is not connected via a SAS connector.
    U8      subtype;             // sub-type of enclosure (type MR_ENCL_SUBTYPE)
    U8      reserved[4];         // reserved for future use
    U16     slotOffset;          // Slot offset of the PD in the enclosure
    U16     pdCount;             // number of physical drives present
    U16     pdIdList[1];         // physical drive deviceId's. Array of variable size depending on the 'slotCount' within this enclosure
} MR_ENCL_INFO_EXT;

typedef struct _MR_ENCL_LIST {
    U32     count;              // number of entries in the list
    U32     pad;                // pad to 8-byte boundary
    MR_ENCL_INFO    encl[MAX_API_ENCLOSURES];   // list of enclosures
} MR_ENCL_LIST;

typedef struct _MR_ENCL_LIST_EXT {
   U32     totalSize;          // size of total data structure (including totalSize/count fields)
   U32     count;              // number of entries in the list
   MR_ENCL_INFO_EXT encl[1];   // list of enclosures
} MR_ENCL_LIST_EXT;

/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to the FW-resident applications
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * defines the exit codes for FW-resident application
 */
typedef enum _MR_DCMD_APP_EXIT {
    MR_DCMD_APP_EXIT_DONE           = 1,    // application has completed execution
    MR_DCMD_APP_EXIT_NEED_INPUT     = 2,    // application input buffer is empty - more needed
    MR_DCMD_APP_EXIT_OUTPUT_FULL    = 3,    // applicaiton output buffer is full - more output pending
    MR_DCMD_APP_EXIT_OUTPUT_FLUSH   = 4,    // application requested buffer flush - host should pull output buffer
} MR_DCMD_APP_EXIT_CODE;

/*
 * defines the execution results structure
 */
typedef union _MR_DCMD_APP_EXEC_RESULTS {
    struct {
        U16     exitCode;               // cause for the exit (MR_DCMD_APP_EXIT)
        U16     exitArg;                // argument for exit cause (app exit code for EXIT, buffer ID for full/empty)
    } mrAppExecResults;                 // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U32     results;
} MR_DCMD_APP_EXEC_RESULTS;             // 4 bytes

/*
 * defines the  FW application initialization structure
 */
typedef struct _MR_DCMD_APP_INFO {
    U32     appHandle;                  // FW-supplied handle for the application to use on subsequent calls
    U32     bufferSize;                 // size of FW input/output buffers
    U32     bufferCount;                // number of FW buffers (handles) (0=stdin, 1=stdout, 2=stderr, ...)
    U32     timeout;                    // number of seconds after which inactivity will allow the FW-app to be closed
    U32     reserved[4];                // reserved for future use
} MR_DCMD_APP_INFO;                     // 32 bytes


/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to the FW-BIOS
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * defines the EKM Status enum
 */
typedef enum _MR_SECURITY_EKM_STATUS {  // current EKM agents' status
    MR_SECURITY_EKM_STATUS_UNKNOWN           = 0x00, // not defined
    MR_SECURITY_EKM_STATUS_BA_OK             = 0x01, // boot agent is available
    MR_SECURITY_EKM_STATUS_BA_NOT_SUPPORTED  = 0x02, // System BIOS doesn't support EKM
    MR_SECURITY_EKM_STATUS_BA_FAILED         = 0x04, // EKMS interaction failed
    MR_SECURITY_EKM_STATUS_BA_UNCONFIGURED   = 0x08, // EKMS rejected key request
    MR_SECURITY_EKM_STATUS_EKMC_PRESENT      = 0x10, // EKMC is present
} MR_SECURITY_EKM_STATUS;

/*
 * defines the controller boot mode enum
 */
typedef enum _MR_BOOT_MODE { // boot mode
    MR_BOOT_MODE_USER          = 0,   // Post all messages, user interaction required (not applicable for uEFI environments)
    MR_BOOT_MODE_COE           = 1,   // continue boot on error (legacy). User interaction only required for critical messages
    MR_BOOT_MODE_HCOE          = 2,   // Headless mode Continue On Error (HCOE). No user interaction. FW may halt for HW faults.
    MR_BOOT_MODE_HSM           = 3,   // Headless Safe Mode (HSM). For critical messages, controller shall boot to safe mode.
                                      // Note - for certain features, user interaction may be configured in headless mode, for example
                                      //        to obtain boot passphrase.
} MR_BOOT_MODE;
#define MR_BOOT_MODE_MAX         3    // maximum permissible value

/*
 * defines the BIOS private data strcuture
 */
typedef struct _MR_BIOS_DATA {
    U16     bootTargetId;       // target ID of boot LD (0xFFFF is none)
                                // this may be a physical device ID, see bootDeviceIsPD
    U8      doNotExpose;        // TRUE if LDs should NOT be exposed via BIOS INT 13
    U8      continueOnError;    // controller boot mode (MR_BOOT_MODE)
    U8      verbose;            // Define BIOS verbosity - 0=normal, 1=minimal, 2=verbose
    U8      geometry;           // BIOS geometry - 0=default(8G), 1=1G, 2=2G, otheres reserved
    union {
      U8    exposeAllDrives;    // Deprecated
      U8    deviceExposure;     // 0 and 1 - Expose all (to retain backward compatibility) value
                                // 2 to 255: the actual number of devices to be exposed
                                // (example: value 4 represents expose only four devices;
                                // value 16 represents expose 16 devices so on and so forth)
                                // Will override the value in MR_MFC_DEFAULTS
    };
    U8      disableCTO;         // TRUE to disable PCI CTO
    U8      bootDeviceIsPD;     // TRUE if boot device is NOT an LD, but a PD (such as SYSTEM PD)
                                // When set, bootTargetID is really a deviceID
    U8      EKMStatus;          // status of various EKM agents (MR_SECURITY_EKM_STATUS)
    U8      autoSelectBootLd;   // TRUE to allow BIOS to pick the best LD for boot
    U8      reserved[52];       // reserved for future use
    U8      checkSum;           // 8-bit checksum of BIOS data (a value that causes entire structure to sum to zero)
} MR_BIOS_DATA;                 // 64 bytes

/*
 * defines the MR_HOST_INFO data structure
 */
typedef struct _MR_HOST_INFO {
    U8      pciSlotNumber;          // PCIe slot number of controller in server.  Special values: 0 if embedded, 0xFF if unknown
    char    hostSerialNumber[16];   // Serial number of server that controller is installed in.  0's if unknown, pad to end with 0's
    U8      reserved[64-17];        // 64 bytes
} MR_HOST_INFO;

/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to the Patrol Read
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * defines the Patrol Read State enum
 */
typedef enum _MR_PR_STATE {         // current PR state
    MR_PR_STATE_STOPPED   = 0,      // not started
    MR_PR_STATE_READY     = 1,      // PR is ready to start
    MR_PR_STATE_ACTIVE    = 2,      // PR is active
    MR_PR_PAUSED          = 3,      // PR is paused
    MR_PR_USER_PAUSED     = 4,      // PR paused by user
    MR_PR_STATE_ABORTED   = 0xff    // PR has been aborted
} MR_PR_STATE;

/*
 * defines the Patrol Read Status structure
 */
typedef struct _MR_PR_STATUS {
    U32         numIteration;       // number of PR iterations completed
    U8          state;              // PR current state (MR_PR_STATE)
    U8          numPdDone;          // number PDs that have completed PR in the current cycle
    U8          numPdDoneMsb;       // MSB of number PDs that have completed PR in the current cycle if mbox.b[0] is 1
    U8          reserved[9];        // reserved for future use
} MR_PR_STATUS;                     // 16 bytes

/*
 * defines the Patrol Read Mode enum
 */
typedef enum _MR_PR_OPMODE {                // PR Operational Mode
    MR_PR_OPMODE_AUTO       = 0,
    MR_PR_OPMODE_MANUAL     = 1,
    MR_PR_OPMODE_DISABLED   = 2,
} MR_PR_OPMODE;

/*
 * defines the bit-map constants for Patrol Read options
 *
 */
typedef enum _MR_PR_OPTIONS {               // PR Options
    MR_PR_OPTIONS_INCLUDE_SSD     = 0x01,   // PR should include VDs constituting only SSD drives
    MR_PR_OPTIONS_INCLUDE_EPD     = 0x02,   // PR should include EPDs (applies only to HDDs, unless MR_PR_OPTIONS_INCLUDE_SSD is also set)
} MR_PR_OPTIONS;
#define MR_PR_OPTIONS_MAX           0x02    // maximum permissible value

/*
 * defines the Patrol Read Properties structure
 */
typedef struct _MR_PR_PROPERTIES {
    U8      opMode;                             // current operational mode (MR_PR_OPMODE)
    U8      maxPD;                              // max PDs on which PR may be run concurrently
    U8      options;                            // options that define the behavior of scheduled PR (MR_PR_OPTIONS)
    U8      excludeLdCount;                     // number of entries in the "excludedLD" list
    U16     excludedLD[MAX_API_LOGICAL_DRIVES_LEGACY];     // list of LD targetIds excluded from PR
    U8      curPdMap[MAX_API_PHYSICAL_DEVICES/8];   // bitmap of devices that completed a PR cycle since the last clear
    U8      lastPdMap[MAX_API_PHYSICAL_DEVICES/8];  // bitmap of devices that completed a PR cycle before the last clear
    U32     nextExec;                           // time of next execution (seconds past 1/1/2000)
    U32     execFreq;                           // execution frequency in seconds (i.e. how often PR runs)
                                                // 0=reserved, 24*60*60=24Hrs, -1U=continuous
    U32     clearFreq;                          // PR "current bit map" clear frequency (seconds)
} MR_PR_PROPERTIES;                             // For MAX_API_PHYSICAL_DEVICES=256,MAX_API_LOGICAL_DRIVES_LEGACY=40 it is 160 bytes

/*
 * defines the Extended Patrol Read Properties structure
 */
typedef struct _MR_PR_PROPERTIES_EXT {
    U8               opMode;                    // current operational mode (MR_PR_OPMODE)
    U8               maxPD;                     // max PDs on which PR may be run concurrently
    U8               options;                   // options that define the behavior of scheduled PR (MR_PR_OPTIONS)
    U8               excludeLdCount;            // number of excluded Lds
    U8               pad[4];                // padded for alignment
    MR_LD_BITMAP     excludedLdMap;             // list of LD targetIds excluded from PR
    MR_PD_BITMAP     curPdMap;                  // bitmap of devices that completed a PR cycle since the last clear
    MR_PD_BITMAP     lastPdMap;                 // bitmap of devices that completed a PR cycle before the last clear
    U32              nextExec;                  // time of next execution (seconds past 1/1/2000)
    U32              execFreq;                  // execution frequency in seconds (i.e. how often PR runs)
                                                // 0=reserved, 24*60*60=24Hrs, -1U=continuous
    U32              clearFreq;                 // PR "current bit map" clear frequency (seconds)
    U8               reserved2[4];
} MR_PR_PROPERTIES_EXT;

/*
 * defines the Extended Patrol Read Properties structure
 */
typedef struct _MR_PR_PROPERTIES_EXT2 {
    U8               opMode;                    // current operational mode (MR_PR_OPMODE)
    U8               options;                   // options that define the behavior of scheduled PR (MR_PR_OPTIONS)
    U16              maxPD;                     // max PDs on which PR may be run concurrently
    U16              excludeLdCount;            // number of excluded Lds
    U8               pad[2];                    // padded for alignment
    MR_LD_BITMAP_EXT excludedLdMap;             // list of LD targetIds excluded from PR
    MR_PD_BITMAP_EXT curPdMap;                  // bitmap of devices that completed a PR cycle since the last clear
    MR_PD_BITMAP_EXT lastPdMap;                 // bitmap of devices that completed a PR cycle before the last clear
    U32              nextExec;                  // time of next execution (seconds past 1/1/2000)
    U32              execFreq;                  // execution frequency in seconds (i.e. how often PR runs)
                                                // 0=reserved, 24*60*60=24Hrs, -1U=continuous
    U32              clearFreq;                 // PR "current bit map" clear frequency (seconds)
    U8               reserved2[4];
} MR_PR_PROPERTIES_EXT2;


/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to CC Scheduling
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * defines the CC Schedule State enum
 */
typedef enum _MR_CC_SCHEDULE_STATE {
    MR_CC_SCHEDULE_STOPPED      = 0,        // not started
    MR_CC_SCHEDULE_READY        = 1,        // ready to start (could be paused)
    MR_CC_SCHEDULE_ACTIVE       = 2,        // in progress
    MR_CC_SCHEDULE_ABORTED      = 0xff      // aborted
} MR_CC_SCHEDULE_STATE;

/*
 * defines the CC Schedule Mode enum
 */
typedef enum _MR_CC_SCHEDULE_MODE {
    MR_CC_SCHEDULE_MODE_DISABLE         = 0,    // disable cc scheduling
    MR_CC_SCHEDULE_MODE_SEQUENTIAL      = 1,    // CC is scheduled to run on one LD at a time
    MR_CC_SCHEDULE_MODE_CONCURRENT      = 2,    // CC is scheduled to run on MR_CC_SCHEDULE_PROPERTIES.maxLds concurrently
} MR_CC_SCHEDULE_MODE;

/*
 * defines the CC Schedule Properties structure
 */
typedef struct _MR_CC_SCHEDULE_PROPERTIES {
    U8      opMode;                         // current operational mode (MR_CC_SCHEDULE_MODE)
    U8      maxLds;                         // max LDs on which CC may be run concurrently
    U8      reserved;
    U8      excludeLdCount;                 // number of entries in the "excludedLD" list
    U16     excludedLD[MAX_API_LOGICAL_DRIVES_LEGACY]; // list of LD targetIds excluded from CC
    U8      pad1[4];                        // Padded for alignment
    U64     curLdMap;                       // bitmap of LD targetIds that completed a CC cycle since the last clear
    U64     lastLdMap;                      // bitmap of LD targetIds that completed a CC cycle before the last clear
    U32     nextExec;                       // time of next execution (seconds past 1/1/2000)
    U32     execFreq;                       // execution frequency in seconds (i.e. how often CC runs)
                                            // 0=reserved, 24*60*60=24Hrs, -1U=continuous
    U32     clearFreq;                      // CC "current bit map" clear frequency (seconds)
    U8      pad2[4];                        // Padded for alignment
} MR_CC_SCHEDULE_PROPERTIES;                // For MAX_API_LOGICAL_DRIVES_LEGACY=64 it is 168 bytes

/*
 * defines the Extended CC Schedule Properties structure
 */
typedef struct _MR_CC_SCHEDULE_PROPERTIES_EXT {
    U8               opMode;                // current operational mode (MR_CC_SCHEDULE_MODE)
    U8               maxLds;                // max LDs on which CC may be run concurrently
    U8               reserved1;
    U8               excludeLdCount;        // number of excludedLDs
    U8               pad[4];                // padded for alignment
    MR_LD_BITMAP     excludedLdMap;         // bitmap of LD targetIds that excluded from CC
    MR_LD_BITMAP     curLdMap;              // bitmap of LD targetIds that completed a CC cycle since the last clear
    MR_LD_BITMAP     lastLdMap;             // bitmap of LD targetIds that completed a CC cycle before the last clear
    U32              nextExec;              // time of next execution (seconds past 1/1/2000)
    U32              execFreq;              // execution frequency in seconds (i.e. how often CC runs)
                                            // 0=reserved, 24*60*60=24Hrs, -1U=continuous
    U32              clearFreq;             // CC "current bit map" clear frequency (seconds)
    U8               reserved2[4];
} MR_CC_SCHEDULE_PROPERTIES_EXT;

/*
 * defines the Extended2 CC Schedule Properties structure
 */
typedef struct _MR_CC_SCHEDULE_PROPERTIES_EXT2 {
    U8               opMode;                // current operational mode (MR_CC_SCHEDULE_MODE)
    U8               reserved1;
    U16              maxLds;                // max LDs on which CC may be run concurrently
    U16              excludeLdCount;        // number of excludedLDs
    U8               pad[2];                // padded for alignment
    U32              nextExec;              // time of next execution (seconds past 1/1/2000)
    U32              execFreq;              // execution frequency in seconds (i.e. how often CC runs)
                                            // 0=reserved, 24*60*60=24Hrs, -1U=continuous
    U32              clearFreq;             // CC "current bit map" clear frequency (seconds)
    U8               reserved2[4];

    MR_LD_BITMAP_EXT excludedLdMap;         // bitmap of LD targetIds that excluded from CC
    MR_LD_BITMAP_EXT curLdMap;              // bitmap of LD targetIds that completed a CC cycle since the last clear
    MR_LD_BITMAP_EXT lastLdMap;             // bitmap of LD targetIds that completed a CC cycle before the last clear

} MR_CC_SCHEDULE_PROPERTIES_EXT2;

/*
 * defines the CC Schedule Status structure
 */
typedef struct _MR_CC_SCHEDULE_STATUS {
    U32     numIteration;                   // number of CC iterations completed
    U8      state;                          // CC current state (MR_CC_SCHEDULE_STATE)
    U8      numLdDone;                      // number LDs that have completed CC in the current cycle
    U8      numLdDoneMsb;                   // MSB of number LDs that have completed CC in the current cycle if mbox.b[0] is 1
    U8      reserved[9];                    // reserved for future use
} MR_CC_SCHEDULE_STATUS;                    // 16 bytes


/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to MFC data
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * define MFC PCI structure
 */
typedef struct _MR_MFC_PCI {
    U16     subVendorId;                // sub-vendor ID for this board
    U16     subDeviceId;                // sub-device ID for this board
                                        // Note: The following memebers can only be accessed if
                                        //       MR_CTRL_INFO.adpaterOperations.supportConfigPageModel is set
    U16     vendorId;                   // vendor ID for this board
    U16     deviceId;                   // device ID for this board
} MR_MFC_PCI;

/*
 * define the MFC Defaults structure
 */
typedef struct _MR_MFC_DEFAULTS {
    U64     sasAddr;                    // controller SAS address
    U8      phyPolarity;                // polarity for each possible PHY (bitmap...1=inverted) - used with phyPolaritySplit - see below
    U8      backgroundRate;             // default rebuild rate for this card (0-100)
    U8      stripeSize;                 // default LD strip size (per DDF: 4=8K, 5=16K, 7=64K, 11=1MB, etc.)
    U8      flushTime;                  // default cache flush time in seconds
    U8      writeBack;                  // default LD is WriteBack (0=WT, 1=WB, 2=Adaptive)
    U8      readAhead;                  // default LD read-ahead policy (0=none, 1=RA, 2=Adaptive). Adaptive read ahead is deprecated
    U8      cacheWhenBBUBad;            // default LD setting for enabling WB when BBU bad - 1=enable WB even when BBU bad
    U8      cachedIo;                   // default LD is cached
    U8      smartMode;                  // default smart mode (0=mode 6, 1=disable, 2=mode 2)
    U8      alarmDisable;               // default alarm to disabled if set
    U8      coercion;                   // 0=none, 1=128M, 2=1G, 3=reserved
    U8      zcrConfig;                  // ZCR configuration data (0=unknown, 1=RADIOS, 16-31=IDSEL for ZCR)
    U8      dirtyLedShowsDriveActivity; // 1=change dirty LED to show drive activity
    U8      biosContinueOnError;        // 0=Post all messages, user interaction required (not applicable for uEFI environments)
                                        // 1=continue boot on error (legacy). User interaction only required for critical messages
                                        // 2=Headless mode Continue On Error (HCOE). No user interaction. FW may halt for HW faults.
                                        // 3=Headless Safe Mode (HSM). For critical messages, controller shall boot to safe mode.
                                        // Note - for certain features, user interaction may be configured in headless mode, for example
                                        //        to obtain boot passphrase.
    U8      spindownMode;               // drive spindown mode on shutdown (0=none, 1=internal only, 2=external only, 3=all)
    U8      allowedDeviceTypes;         // 0=SAS/SATA mix, 1=no SATA(SAS only), 2=no SAS(SATA only)
    U8      allowMixInEnclosure;        // allow SAS/SATA mix within an enclosure
    U8      allowMixInLD;               // allow SAS/SATA mix within an LD
    U8      allowSataInCluster;         // allow SATA drives to be used in a cluster configuration
    U8      maxChainedEnclosures;       // maximum number of enclosures that may be chained
    U8      disableCtrlR;               // 1=disable Ctrl-R configuration utility, 0=enable Ctrl-R utility
    U8      enableWebBios;              // 1=enable WebBIOS configuration utility, 0=disable WebBIOS utility
    U8      phyPolaritySplit;           // polarity for TX/RX pair is different (bitmap...0=RX/TX same, 1=RX/TX different; phyPolarity.x=RX)
    U8      directPdMapping;            // 1=enable direct PD mapping, 0=persistent PD mapping
    U8      biosEnumerateLds;           // 1=BIOS to display LDs at POST
    U8      restoreHotSpareOnInsertion; // HSP kicks in, insert the failed drive, it becomes HSP
    U8      exposeEnclosureDevices;     // 1=device driver should expose enclosure devices, 0=hide enclosure devices
    U8      maintainPdFailHistory;      // 1=enable tracking of bad PDs across reboot; will also show failed LED status for missing bad drives
    U8      disablePuncturing;          // 1=Don't puncture LBA's in PD's. 0=okay to puncture. This is for SAS drives
    U8      zeroBasedEnclEnumeration;   // 1=Enclosure enumeration start with Zero. 0=Enclosure enumeration start with One.
    U8      disableBootCLI;             // 0=enable pre boot CLI tool, 1=disable pre boot CLI.
    U8      quadPortConnectorMap;       // Bitmap defining connector map for 8 quad-ports, 0=Internal connector/unused, 1=External connector/SAS MUX control
    U8      driveActivityLed;           // 0=No LED for drive activity, 1=Show all drive activity
    U8      disableAutoDetectBackplane; // 0 - use auto detect logic of backplanes like SGPIO, i2c SEP using h/w mechansim like GPIO pins
                                        // 1 - disable auto detect SGPIO,
                                        // 2 - disable i2c SEP auto detect
                                        // 3 - disable both auto detect
    U8      useFdeOnly;                 // 0 = FDE and controller encryption (if HW supports) is allowed
                                        // 1 = Only support FDE encryption, disallow controller encryption
    U8      enableLedHeaders;           // 0=No LED for drive faults, 1=enable LED for drive faults, 2=enable LED for drive activity
    U8      allowSSDMixInLD;            // allow SAS/SATA SSD mix within an LD (can only be 0/1)
    U8      allowMixSSDHDDInLD;         // allow SSD/HDD mix within an LD (can only be 0/1)
    U8      delayPOST;                  // intentional delay to be introduced during FW POST. Possible coded values: 0 to 7 with a scale of 15
                                        // which gives a value of 0,15,30,45,60,75,90,105 seconds
    U8      enableCrashDump;            // 1=enable collection of system core in event of a controller crash
    U8      enableLDBBM;                // 1=enable LD Bad block management with 1024 entries,2- LDBBM with 2048 x 64K entries
    U8      allowUnCertifiedHDDs;       // 1=allow, 0=block, 2=Warn & allow
    U8      treatR1EAsR10;              // 1=treat single span R1E as R10
    U8      maxLdsPerArray;             // 0=16 LDs per array, 1=64 LDs per array
    U8      failPdOnSMARTer;            // 1=fail a PD that's reached SMART failure threshold, if it doesn't take an LD offline
    U8      nonRevertibleSpares;        // 1=hot spares are not revertible
    U8      disableOnlineCtrlReset;     // 1=disallow online controller resets
    U8      disablePowerSavings;        // 0x00=enable all power saving options
                                        // 0xff=disable all power saving options
                                        // 0x01=Don't spin down unconfigured drives
                                        // 0x02=Don't spin down Hot spares
                                        // 0x04=Don't auto spin down configured drives
                                        // 0x08=Apply settings to all drives - don't allow individual PD/LD configuration
                                        // 0x10=Disallow MAX power saving option for LDs - only T10 power conditions are to be used
                                        // 0x20=Don't use cached writes for spun down VDs
                                        // 0x40=Provide option to schedule disable power savings at controller level
    U16     spinDownTime;               // Time device should be idle before power saving mode is initiated (0=use FW defaults)
    U8      snapVDSpace;                // % of source LD to be reserved for a VDs snapshot in snapshot repository, for metadata and user data
                                        // 1=5%, 2=10%, 3=15% and so on
    U8      autoSnapVDSpace;            // % of source LD to be reserved for auto snapshot in snapshot repository, for metadata and user data
                                        // 1=5%, 2=10%, 3=15% and so on
    U8      viewSpace;                  // snapshot writeable VIEWs capacity as a % of source LD capacity. 0=READ only

    U8      disableCacheBypass;         // 1=disable cache-bypass-performance-improvement feature
    U8      length;                     // x=sizeof MR_MFC_DEFAULTS.  0=64 bytes
                                        // This structure is a variable length structure
    U8      enableJBOD;                 // 1=enable JBOD mode, 0=disable JBOD mode by default

#ifndef MFI_BIG_ENDIAN
    U32     ttyLogInFlash      : 1;     // 1=tty logs are maintained in flash
    U32     autoEnhancedImport : 1;     // 1=import foreign configuration automatically, at boot
    U32     breakMirrorRAIDSupport :2;  // 0=supported for all redundant RAID level - R1, R10 (PRL1 & PRL11), and triple mirror R6
                                        // 1=single span R1 (PRL1)
                                        // 2=All R1 and R10 combinations (PRL1 & PRL11)
    U32     disableJoinMirror  : 1;     // 1=join LD mirror operation is not supported
    U32     enablePI           : 1;     // 0=Disable SCSI PI for controller.  Remove any active protection information
    U32     preventPIImport    : 1;     // 1=Prevent import of SCSI PI protected logical disks
    U32     enableShieldState  : 1;     // 1=Shield state array PD feature is enabled
    U32     enableDriveWCEforRebuild :1;// 1=Enable drive write cache during rebuild
    U32     enableEmergencySpare     :1;// 1=Enable Emergency Spare feature
    U32     useGlobalSparesForEmergency:1;//1=use global spares for emergency feature
    U32     useUnconfGoodForEmergency:1;// 1=use unconfigured good drives for emergency feature
    U32     blockSSDWriteCacheChange :1;// 1=don't allow SSD write cache setting to be changed
    U32     disableHII         : 1;     // 1=disable HII, 0=enable HII
    U32     SMARTerEnabled     : 1;     // 1=SMARTer enabled for HDDs (copyback on SMART error)
    U32     SSDSMARTerEnabled  : 1;     // 1=SMARTer enabled for SSDs (copyback on SMART error)
    U32     perfTunerMode      : 2;     // currently 2 modes supported: 0 - BestIOPS, 1 - Least Latency
    U32     enable512eSupport  : 1;     // 1=512 emulated drive support enabled
    U32     disableImmediateIO : 1;     //  1 = Disable Legacy Immediate IO, 0 = Enable
    U32     cacheVaultOption   : 2;     // 0=Force dual write [DDR image will be copied to 2 different locations in flash]
                                        // 1=Force single write [DDR image will be copied to single location in flash]
                                        // 2=FW controls single/dual write [Dual write for DDR:Flash of 1:4 or 1:8
                                        //                                  Single write for DDR:Flash of 1:2]
    U32     ignore64ldRestriction : 1;  // 0 - limit LD to 64 even if more LD support exists, 1 - support more than 64 ld with new DDF format
    U32     autoCfgOption      : 4;     // MR_AUTO_CFG_OPTIONS
    U32     maxLDs             : 4;     // maximum number of supported LDs (MR_LD_LIMIT)
    U32     predictiveFailureLED: 1;    // 1=Blink LED for drives exhibiting predictive failure condition
#else
    U32     predictiveFailureLED: 1;
    U32     maxLDs             : 4;     // maximum number of supported LDs (MR_LD_LIMIT)
    U32     autoCfgOption      : 4;     // MR_AUTO_CFG_OPTIONS
    U32     ignore64ldRestriction : 1;  // 0 - limit LD to 64 even if more LD support exists, 1 - support more than 64 ld with new DDF format
    U32     cacheVaultOption   : 2;     // 0=Force dual write [DDR image will be copied to 2 different locations in flash]
                                        // 1=Force single write [DDR image will be copied to single location in flash]
                                        // 2=FW controls single/dual write [Dual write for DDR:Flash of 1:4 or 1:8
                                        //                                  Single write for DDR:Flash of 1:2]
    U32     disableImmediateIO : 1;     //  1 = Disable Legacy Immediate IO, 0 = Enable
    U32     enable512eSupport  : 1;     // 1=512 emulated drive support enabled
    U32     perfTunerMode      : 2;     // currently 2 modes supported: 0 - BestIOPS, 1 - Least Latency
    U32     SSDSMARTerEnabled  : 1;     // 1=SMARTer enabled for SSDs (copyback on SMART error)
    U32     SMARTerEnabled     : 1;     // 1=SMARTer enabled for HDDs (copyback on SMART error)
    U32     disableHII         : 1;     // 1=disable HII, 0=enable HII
    U32     blockSSDWriteCacheChange :1;// 1=don't allow SSD write cache setting to be changed
    U32     useUnconfGoodForEmergency:1;// 1=use unconfigured good drives for emergency feature
    U32     useGlobalSparesForEmergency:1;//1= use global spares for emergency feature
    U32     enableEmergencySpare     :1;// 1=Enable Emergency Spare feature
    U32     enableDriveWCEforRebuild :1;// 1=Enable drive write cache during rebuild
    U32     enableShieldState  : 1;     // 1=Shield state array PD feature is enabled
    U32     preventPIImport    : 1;     // 1=Prevent import of SCSI PI protected logical disks
    U32     enablePI           : 1;     // 0=Disable SCSI PI for controller.  Remove any active protection information
    U32     disableJoinMirror  : 1;     // 1=join LD mirror operation is not supported
    U32     breakMirrorRAIDSupport :2;
    U32     autoEnhancedImport : 1;     // 1=import foreign configuration automatically, at boot
    U32     ttyLogInFlash      : 1;     // 1=tty logs are maintained in flash
#endif  // MFI_BIG_ENDIAN

    U8      detectCMETimer;             // Time(sec) taken to detect CME. 0, 15-60 are valid values. 0=60 seconds
    U8      adapterPersonality;         // enum MR_CTRL_PERSONALITY, a value of 0 is undefined and so behavior also undefined
    U8      deviceExposure;             // Number of devices to be exposed to BIOS.
                                        // Value 0,1 - expose all devices and will be consistent with MR_BIOS_DATA
                                        // Any other value - actual number of devices to be exposed

    U8      pdDegradedMediaOptions;     // see MR_CTRL_PROP.pdDegradedMediaOptions for details
    U8      enableLargeIOSupport;       // 0 = LargeIOSupport (>256K) Disabled, 1 = LargeIOSupport Enabled
    U8      enableLargeQD;              // 0 = Large QD (>256 for iMR) Disabled, 1 = Large QD Enabled
    U8      enableTransportReady;       // 0 = Disable Transport Ready, 1 = Enable Transport Ready
#ifndef MFI_BIG_ENDIAN
    U8      SESVPDAssociationTypeInMultiPathCfg : 1; // 1 = Association type based on target port
                                                     // 0 = Association type based on LUN
    U8      SSCWriteBufferMode : 1;     // SSCD write buffer mode (0 = disabled, 1 = enabled) (applicable for CacheCade 3.0 only)
    U8      enableUnmapSupport:1;       // Enable Unmap support (0 = disabled, 1 = enabled)
    U8      enableFDETypeMix:1;         // This applies FDE type mixing for security enabled arrays
                                        // 0 = disable different FDE protocol mix, 1 = enable different FDE protocol mix
    U8      enableParallelPDFwUpdate:1; // Enable Parallel PD FW update feature(0 = disabled, 1 = enabled)
    U8      reserved : 3;               // Reserved for future use
#else
    U8      reserved : 3;               // Reserved for future use
    U8      enableParallelPDFwUpdate:1; // Enable Parallel PD FW update feature(0 = disabled, 1 = enabled)
    U8      enableFDETypeMix:1;         // This applies FDE type mixing for security enabled arrays
                                        // 0 = disable different FDE protocol mix, 1 = enable different FDE protocol mix
    U8      enableUnmapSupport:1;       // Enable Unmap support (0 = disabled, 1 = enabled)
    U8      SSCWriteBufferMode : 1;     // SSCD write buffer mode (0 = disabled, 1 = enabled) (applicable for CacheCade 3.0 only)
    U8      SESVPDAssociationTypeInMultiPathCfg : 1; // 1 = Association type based on target port
                                                     // 0 = Association type based on LUN
#endif
} MR_MFC_DEFAULTS;

typedef enum _MR_LD_LIMIT {     // maximum supported drives
    MR_LD_LIMIT_MAX = 0,        // FW max
    MR_LD_LIMIT_32  = 1,        // up to 32 LDs
    MR_LD_LIMIT_64  = 2,        // up to 64 LDs
    MR_LD_LIMIT_128 = 3,        // up to 128 LDs
    MR_LD_LIMIT_256 = 4,        // up to 256 LDs
    MR_LD_LIMIT_512 = 5         // up to 512 LDs
} MR_LD_LIMIT;

/*
 * In the above MR_MFC_DEFAULTS structure, the definition of the PHY polarity fields is explained in more detail below.  Basically
 * each field is a bitmap for up to 8 PHYs, indicating the desired polarity.  The following table shows the possible bit values and
 * the resulting polarity:
 *
 * phyPolarity.N    phyPolaritySplit.N          Phy N RX Polarity     PHY N TX Polarity
 * -------------    ------------------          -----------------     -----------------
 *       0                  0            ===>   normal                normal
 *       1                  0            ===>   inverted              inverted
 *       0                  1            ===>   normal                inverted
 *       1                  1            ===>   inverted              normal
 */

/*
 * define the MFC DATA structure
 */
typedef struct _MR_MFC_DATE {
#ifndef MFI_BIG_ENDIAN
    U16     day     : 5;                // day of the month (1-31)
    U16     month   : 4;                // month of the year (1-12)
    U16     year    : 7;                // last two digits of the year
#else
    U16     year    : 7;                // last two digits of the year
    U16     month   : 4;                // month of the year (1-12)
    U16     day     : 5;                // day of the month (1-31)
#endif  // MFI_BIG_ENDIAN
} MR_MFC_DATE;

/*
 * define the MFC Manufacturing structure
 */
typedef struct _MR_MFC_MFG {
    MR_MFC_DATE mfgDate;                // manufacturing date
    char        sn[16];                 // ASCII Serial Number (up to 16 bytes)
    MR_MFC_DATE reworkDate;             // date of last rework
    char        rev[8];                 // board revision identifier, may be used as controller FRU
    U8          reserved[4];            // pad to 32 bytes
} MR_MFC_MFG;

/*
 * define the MFC Manufacturing 2 structure
 */
typedef struct _MR_MFC_MFG_2 {
    char        batteryFRU[8];          // holds the battery FRU (up to 8 alpha numeric characters)
    union {
        char        tmmFRU[8];          // FRU for memory module/FPGA for to cache offload
        char        resv1[8];           // reserved for a possible 2nd battery, or battery related info.
    };
    union {
        char        customSN[16];       // holds OEM specific serial number
        char        resv2[16];          // reserved for future
    };
} MR_MFC_MFG_2;

/*
 * define enums for Premium Features
 */
typedef enum _MR_PF_LOCALE {
    MR_PF_LOCALE_ACTIVE        = 0,       // premium features enabled on the controller
    MR_PF_LOCALE_PRIMARY_KEY   = 1,       // default premium features
    MR_PF_LOCALE_UPGRADE_KEY   = 2,       // premium features upgraded via upgrade Key (HW)
    MR_PF_LOCALE_PFK_NVRAM     = 3,       // premium features enabled via PFK, stored in NVRAM (not protected)
    MR_PF_LOCALE_PKF_KEYVAULT  = 4,       // premium features enabled via PFK, stored in key-vault (protected)
    MR_PF_LOCALE_PFK_BOOT      = 5,       // premium features enabled via PFK provide at boot time
    MR_PF_LOCALE_PKF_TRIAL     = 6,       // premium features enabled via trial PFK
} MR_PF_LOCALE;
#define MR_PF_LOCALE_MAX         6        // maximum permissible value

typedef enum _MR_PF_ACTION {
    MR_PF_ACTION_EXACT       = 0,
    MR_PF_ACTION_ACTIVATE    = 1,
    MR_PF_ACTION_DEACTIVATE  = 2,
    MR_PF_ACTION_TRIAL       = 3,
} MR_PF_ACTION;
#define MR_PF_ACTION_MAX       3           // maximum permissible value

/*
 * define the Premium Features structure
 */
typedef struct  _MR_PREMIUM_FEATURES {
#ifndef MFI_BIG_ENDIAN
    U8   cluster     : 1;           /* DEPRECATED */
    U8   SAS         : 1;           /* SAS drives supported */
    U8   maxDisks    : 2;           /* maximum supported disks (MR_MFC_PD_LIMIT) */
    U8   Raid6       : 1;           /* Raid6 enabled */
    U8   WideCache   : 1;           /* Wide Cache (72-bit) supported */
    U8   Raid5       : 1;           /* Raid5 enabled */
    U8   Security    : 1;           /* security enabled. */

    U8   CTIO        : 1;           /* CTIO enabled */
    U8   Snapshot    : 1;           /* Snapshot enabled */
    U8   SSC         : 1;           /* SSC enabled */
    U8   ReducedFeatureSet : 1;     /* Reduced Feature set */
    U8   CacheOffload : 1;          /* Cache offload enabled */
    U8   SSCWB       : 1;           /* Cache Cade enabled in WB mode */
    U8   resv1       : 2;           /* for future use */
#else
    U8   resv1       : 2;           /* for future use */
    U8   SSCWB       : 1;           /* Cache Cade enabled in WB mode */
    U8   CacheOffload : 1;          /* Cache offload enabled */
    U8   ReducedFeatureSet : 1;     /* Reduced Feature set */
    U8   SSC         : 1;           /* SSC enabled */
    U8   Snapshot    : 1;           /* Snapshot enabled */
    U8   CTIO        : 1;           /* CTIO enabled */

    U8   Security    : 1;           /* security enabled. */
    U8   Raid5       : 1;           /* Raid5 enabled */
    U8   WideCache   : 1;           /* Wide Cache (72-bit) supported */
    U8   Raid6       : 1;           /* Raid6 enabled */
    U8   maxDisks    : 2;           /* maximum supported disks (MR_MFC_PD_LIMIT) */
    U8   SAS         : 1;           /* SAS drives supported */
    U8   cluster     : 1;           /* DEPRECATED */
#endif  // MFI_BIG_ENDIAN

#ifndef MFI_BIG_ENDIAN
            U8     maxHANodes       : 3;           /* 0=HA feature is disabled, non-zero value=enable HA feature
                                                     * 0x7=enable Max nodes HA cluster
                                                     * 0x1=enable 2 Node HA cluster
                                                     */
            U8     numVFSkew        : 2;           /* 0=SRIOV features is disabled,0x3=16 VFs supported, 0x2=8 VFs supported, 0x1=4 VFs supported */
            U8     topology         : 2;           /* 0=Cluster/Scaleout,1=SSHA,2=Simple Pooling,3=Advanced Pooling */
            U8     reserved1        : 1;           /* for future use */
#else
            U8     reserved1        : 1;           /* for future use */
            U8     topology         : 2;           /* 0=Cluster/Scaleout,1=SSHA,2=Simple Pooling,3=Advanced Pooling */
            U8     numVFSkew        : 2;
            U8     maxHANodes       : 3;           /* 0=HA feature is disabled, non-zero value=enable HA feature */
#endif

    U8   resv;                      /* reserved for future features */
} MR_PREMIUM_FEATURES;

/*
 * define the Premium Features Info structure
 */
typedef struct  _MR_PF_INFO {
    U8  locale;                     // location of premium features (MR_PREMIUM_FEATURES_LOCALE)

#ifndef MFI_BIG_ENDIAN
    U8                      featureApplication  : 3;  // MR_PF_ACTION
    U8                      maxDisksValid       : 1;  // 0=ignore maxDisks, 1=maxDisks is valid
    U8                      reserved            : 4;  // reserved for future use
#else
    U8                      reserved            : 4;  // reserved for future use
    U8                      maxDisksValid       : 1;  // 0=ignore maxDisks, 1=maxDisks is valid
    U8                      featureApplication  : 3;  // MR_PF_ACTION
#endif  // MFI_BIG_ENDIAN

    MR_PREMIUM_FEATURES     features;
    U8   resv[2];                   // reserved for future features
} MR_PF_INFO;

typedef struct  _MR_PF_LIST {
    U32         count;
    MR_PF_INFO  featureInfo[1];     // Variable length array of locales with features
} MR_PF_LIST;

/*
 * define struct to pass parameters to apply a feature key
 *
 * Applications shouldn't populate the fields marked (IN), FW shall ignore these
 * values.
 */
typedef struct  _MR_PF_FEATURE_SET_PARAMS {
    MR_PF_INFO  featureInfo;                        // List of features in the key and its local [IN]
    U8          featureKey[64];                     // NULL terminated string [OUT]
} MR_PF_FEATURE_SET_PARAMS;

/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to SAS Configuration commands
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */
typedef enum _MR_SAS_DEVICE_TYPE {
    MR_SAS_DEVICE_END_DEVICE        = 1,    // device is an End Device
    MR_SAS_DEVICE_EDGE_EXPANDER     = 2,    // device is an edge expander
    MR_SAS_DEVICE_FANOUT_EXPANDER   = 3,    // device is a fanout expander
} MR_SAS_DEVICE_TYPE;

typedef enum _MR_SAS_INTERFACE_TYPE {
    MR_SAS_INTERFACE_MPI_GEN_1      = 0,    // SAS interface is MPI generation 1
    MR_SAS_INTERFACE_MPI_GEN_2      = 1,    // SAS interface is MPI generation 2
} MR_SAS_INTERFACE_TYPE;

/*
 * SAS connector information identifies the location and name/label of a connector.
 * Each SAS PHY is mapped to an external and/or internal connector Index.
 * This is an index into MR_SAS_CONNECTORS.connectors.
 * If a SAS PHY is mapped to both an external as well as an internal connector, then there
 * must exist a SAS MUX enabling the user to choose which connector to use.
 * MR_SAS_CONNECTOR_INFO.sasMuxIndex is used to provide this information.
 * SAS connector information is static and will only change if the controller is changed.
 */
typedef enum _MR_SAS_CONN_LOCATION_TYPE {
    MR_SAS_CONN_LOCATION_UNKNOWN    = 0,    // Location unknown
                                            // = 1, reserved
    MR_SAS_CONN_LOCATION_INTERNAL   = 2,    // For devices internal to the system
    MR_SAS_CONN_LOCATION_EXTERNAL   = 3,    // For devices external to the system
} MR_SAS_CONN_LOCATION_TYPE;

typedef struct _MR_SAS_CONNECTOR_INFO {
    U8      location;       // Location of the connector (MR_SAS_LOCATION_TYPE)
    U8      quadPhyMap[2];  // A bitmap to indicate which sets of quad-phys are attached to this connector
    U8      sasMuxIndex;    // Index into MR_SAS_MUX_LIST, if this connector is attached to a MUX.
    char    name[16];       // Connector name
} MR_SAS_CONNECTOR_INFO;

typedef struct _MR_SAS_CONNECTORS {
    U32                     count;                  // count of connectors
    U32                     reserved;
    MR_SAS_CONNECTOR_INFO   connectors[MAX_SAS_CONNECTORS_PER_CONTROLLER];
} MR_SAS_CONNECTORS;

#define MR_SAS_CONNECTOR_NONE       0xff
#define MR_SAS_MUX_NONE             0xff
#define MR_SAS_ADDRESS_INVALID      0

#define MR_SAS_PHY_CONNECTION_PORTNUMBER_RANGE  0x7F
typedef enum _MR_SAS_PHY_LINK_SPEED {
    MR_SAS_PHY_LINK_SPEED_NO_LIMIT  = 0,    // No limit
    MR_SAS_PHY_LINK_SPEED_1_5G      = 1,    // 1.5Gb/s
    MR_SAS_PHY_LINK_SPEED_3G        = 2,    // 3.0 Gb/s
    MR_SAS_PHY_LINK_SPEED_6G        = 4,    // 6.0 Gb/s
    MR_SAS_PHY_LINK_SPEED_12G       = 8,    // 12.0 Gb/s
} MR_SAS_PHY_LINK_SPEED;

typedef struct _MR_SAS_PHY_CONNECTION {
    U64     sasAddr;                // SAS address of this device (0 == no device installed)
    U8      phyIdentifier;          // PHY number of this device connection, phy id is the subscript from MR_SAS_PHY_CONNECTIONS
    U8      reserved2[2];
    U8      connectorIndex;         // MR_SAS_CONNECTORS.connectors[connectorIndex] to which this PHY is attached.
#ifndef MFI_BIG_ENDIAN
    U32     deviceType      : 3;    // SAS device type (type MR_SAS_DEVICE_TYPE)
    U32     deviceIsSata    : 1;    // TRUE if device is a SATA device
    U32     smpInitiator    : 1;    // TRUE if device is an SMP initiator
    U32     stpInitiator    : 1;    // TRUE if device is an STP initiator
    U32     sspInitiator    : 1;    // TRUE if device is an SSP initiator
    U32     smpTarget       : 1;    // TRUE if device is an SMP target
    U32     stpTarget       : 1;    // TRUE if device is an STP target
    U32     sspTarget       : 1;    // TRUE if device is an SSP target
    U32     disabled        : 1;    // TRUE if PHY is disabled
    U32     sasPortNumber   : 6;    // SAS port number
    U32     sasPortNumberValid : 1; // SAS port number valid (0 == not valid)
    U32     linkSpeedControl: 6;    // Link speed control
    U32     reserved1       : 8;
#else
    U32     reserved1       : 8;
    U32     linkSpeedControl: 6;    // Link speed control
    U32     sasPortNumberValid : 1; // SAS port number valid (0 == not valid)
    U32     sasPortNumber   : 6;    // SAS port number
    U32     disabled        : 1;    // TRUE if PHY is disabled
    U32     sspTarget       : 1;    // TRUE if device is an SSP target
    U32     stpTarget       : 1;    // TRUE if device is an STP target
    U32     smpTarget       : 1;    // TRUE if device is an SMP target
    U32     sspInitiator    : 1;    // TRUE if device is an SSP initiator
    U32     stpInitiator    : 1;    // TRUE if device is an STP initiator
    U32     smpInitiator    : 1;    // TRUE if device is an SMP initiator
    U32     deviceIsSata    : 1;    // TRUE if device is a SATA device
    U32     deviceType      : 3;    // SAS device type (type MR_SAS_DEVICE_TYPE)
#endif  // MFI_BIG_ENDIAN
} MR_SAS_PHY_CONNECTION;

typedef struct _MR_SAS_PHY_CONNECTIONS {
    U32     count;                  // count of returned PHY information which supports SAS/SATA
    U32     reserved;               // reserved for padding
    MR_SAS_PHY_CONNECTION   phy[MAX_PHYS_PER_CONTROLLER];
} MR_SAS_PHY_CONNECTIONS;

typedef enum _MR_SAS_MUX_MODE {
    MR_SAS_MUX_NO_MUX       = 0,
    MR_SAS_MUX_INTERNAL     = 1,
    MR_SAS_MUX_EXTERNAL     = 2,
    MR_SAS_MUX_AUTO         = 3,
} MR_SAS_MUX_MODE;

typedef struct _MR_SAS_MUX_LIST {
    U8      sasMuxCount;                               // Count of available SAS MUXes.
    U8      sasMuxMode[MAX_SAS_QUADS_PER_CONTROLLER];  // MR_SAS_MUX_MODE
} MR_SAS_MUX_LIST;

/*
 * defines the data required to issue a passthrough request to retreive or update SAS Config Pages
 * See section 5 of MPI Specification - Configuration and configuration requests.
 * Application is responsible for populating these fields and interpreting the returned data,
 * per the MPI sepcification.
 *
 * Note:  While the API is generic, the actual allowed operation is dependant on the underlying FW.
 */
typedef struct _MR_SAS_CONFIG_PAGE {
    U8      action;                 // See MPI specification.
    U8      mpiGenVersion;          // Type of interface (MR_SAS_INTERFACE_TYPE)
    U8      chainOffset;            // reserved for future use.
    U8      function;               // reserved for future use.
    U16     extPageLength;          // See MPI specification. Defines length of pageBuffer in bytes
    U8      extPageType;            // See MPI specification.
    U8      messageFlags;           // reserved for future use.
    U32     messageContext;         // reserved for future use.
    union {
        U32     reserved2[2];       // reserved for future use.
        struct {                    // Return status
                                    // Populated by FW if mpiGenVersion != MR_SAS_INTERFACE_MPI_GEN_1
            U16 reserved3;          // reserved for future use.
            U16 iocStatus;          // See MPI specification for return status.
            U32 reserved4;          // reserved for future use.
        } mrSasConfigStatus;
    };
    U8      pageVersion;            // reserved for future use.
    U8      pageLength;             // See MPI specification. Defines length of pageBuffer in bytes
    U8      pageNumber;             // See MPI specification.
    U8      pageType;               // See MPI specification.
    U32     pageAddress;            // See MPI specification.

    U8      chipIndex;              // 0-based chipIndex in the event the controller has multiple SAS cores.
    U8      pad[2];
    U8      pageBuffer[1];          // Byte array of returned page data.  Interpret per MPI specification.
} MR_SAS_CONFIG_PAGE;

typedef enum _MR_PCIE_LANE_SPEED {
    MR_PCIE_LANE_SPEED_UNKNOWN   = 0,    // UNKNOWN
    MR_PCIE_LANE_SPEED_2_5GT     = 1,    // 2.5GT/s
    MR_PCIE_LANE_SPEED_5GT       = 2,    // 5GT/s
    MR_PCIE_LANE_SPEED_8GT       = 4,    // 8GT/s
    MR_PCIE_LANE_SPEED_16GT      = 8     // 16GT/s
} MR_PCIE_LANE_SPEED;

#define MR_WWID_INVALID      0

typedef struct _MR_PCIE_LANE_CONNECTION {
    U64     wwid;                   // WW identifier of this device (0 == no device installed)
    U16     laneIdentifier;         // LANE number of this downstream connection, will be 0xffff if not valid
    U8      reserved2;
    U8      connectorIndex;         // MR_SAS_CONNECTORS.connectors[connectorIndex] to which this LANE is attached.
#ifndef MFI_BIG_ENDIAN
    U32     supportedLaneSpeed: 8;  // bitmask of MR_PCIE_LANE_SPEED. If all bits are 0 then link speed is not configurable
    U32     disabled          : 1;  // TRUE if LANE is disabled
    U32     pcieLinkNumber    : 6;  // PCIe link number
    U32     linkNumberValid   : 1;  // PCIe link number valid (0 == not valid)
    U32     laneSpeedControl  : 8;  // max speed configured for this lane- MR_PCIE_LANE_SPEED
    U32     reserved1         : 8;
#else
    U32     reserved1         : 8;
    U32     laneSpeedControl  : 8;  // max speed configured for this lane- MR_PCIE_LANE_SPEED
    U32     linkNumberValid   : 1;  // PCIe link number valid (0 == not valid)
    U32     pcieLinkNumber    : 6;  // PCIe link number
    U32     disabled          : 1;  // TRUE if LANE is disabled
    U32     supportedLaneSpeed: 8;  // bitmask of MR_PCIE_LANE_SPEED. If all bits are 0 then link speed is not configurable
#endif  // MFI_BIG_ENDIAN
} MR_PCIE_LANE_CONNECTION;

typedef struct _MR_PCIE_LANE_CONNECTIONS {
    U32     count;                  // count of returned LANE information  which supports PCIe
    U32     reserved;               // reserved for padding
    MR_PCIE_LANE_CONNECTION   lane[MAX_LANES_PER_CONTROLLER];
} MR_PCIE_LANE_CONNECTIONS;

typedef struct _MR_PCIE_LINK_CONFIG_INFO {
    U8      linkConfigIndex;         // This index will be used in currentCSWConfig, pendingCSWConfig and set DCMD

    U8      startLaneIdentifier;     // The start laneIdentifier for this link

    U8      reserved[13];

    U8      linkCount;               // number of valid entry in linkWidths
    U8      linkWidths[MAX_PCIE_LANE_PER_CSW];
} MR_PCIE_LINK_CONFIG_INFO;

typedef enum _MR_PCIE_LINK_CONFIG_OPTIONS {
    MR_PCIE_LINK_CONFIG_NOT_SUPPORTED   = 0,    // User can't change the config
    MR_PCIE_LINK_CONFIG_HOMOGENEOUS     = 1,    // Same config across CSW
    MR_PCIE_LINK_CONFIG_HETEROGENEOUS   = 2     // May have different config across CSW
                                                // 03-07 - reserved
} MR_PCIE_LINK_CONFIG_OPTIONS;

typedef struct _MR_PCIE_LINK_CONFIG {
    U32     size;                   // size of total data structure (including size/count fields)
#ifndef MFI_BIG_ENDIAN
    U32     supportLinkConfig : 3;  // MR_PCIE_LINK_CONFIG_OPTIONS
    U32     reserved1         : 29;
#else
    U32     reserved1         : 29;
    U32     supportLinkConfig : 3;  // MR_PCIE_LINK_CONFIG_OPTIONS
#endif  // MFI_BIG_ENDIAN

    U8      reserved2[3];

    U8      cswCount;               // Available CSW count
    U8      pdbCount;               // Available PDB count
    U8      pdbPerCSW;              // Number of PDB in each CSW
    U8      lanePerPDB;             // PCIe lane in each PDB

    U8      currentCSWConfig[MAX_CSW_PER_CONTROLLER]; // Each CSW index will have linkConfigIndex
    U8      reserved3[8];
    U8      pendingCSWConfig[MAX_CSW_PER_CONTROLLER]; // Each CSW index will have pending linkConfigIndex. 0xFF if nothing is pending
    U8      reserved4[8];

    U8      linkConfigCount;       // Allowed config count per CSW
    MR_PCIE_LINK_CONFIG_INFO linkConfigInfo[1];
} MR_PCIE_LINK_CONFIG;

/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to the BBU
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * defines the battery TTY state enum
 */
typedef enum _MR_TTY_BBU_STATE {
    MR_TTY_HIST_BBU_OFF = 0,        // battery is off for tty history
    MR_TTY_HIST_BBU_ON  = 1,        // battery is on for tty history
    MR_TTY_HIST_BBU_TEMP_OFF = 2    // battery is temporarily off for tty history for this boot
} MR_TTY_BBU_STATE;

/*
 * define the TTY buffer
 */
typedef struct _MR_TTY_DATA {
    U32     actualOffset;           // actual offset of returned data
    U32     actualByteCount;        // actual returned byte count
    U8      data[4];                // variable-size returned data buffer
} MR_TTY_DATA;

/*
 *-------------------------------------------------------------------------------------------------
 */

/*
 * define BBU learn modes
 */
typedef enum _MR_BBU_AUTO_LEARN_MODE {
    MR_BBU_AUTO_LEARN_AUTO      = 0,    // perform automatic learn mode based on time settings
    MR_BBU_AUTO_LEARN_DISABLED  = 1,    // disable FW-initiated auto-learn
    MR_BBU_AUTO_LEARN_WARN      = 2     // warn (via event) that a learn cycle is needed, based on time settings
} MR_BBU_AUTO_LEARN;

/*
 * define battery properties (changeable values)
 */
typedef struct _MR_BBU_PROPERTIES {
    U32     autoLearnPeriod;        // time (in seconds) between learn cycles (READ ONLY - should write with READ data)
    U32     nextLearnTime;          // time of next scheduled lean cycle (seconds past 1/1/2000)
                                    // (READ ONLY - should write with READ data, when MR_CTRL_INFO.adapterOperations2.realTimeScheduler=0)
                                    // (To be provided, when MR_CTRL_INFO.adapterOperations2.realTimeScheduler=1)
    U8      learnDelayInterval;     // number of hours to delay a learn cycle; not greater than 7 days
    U8      autoLearnMode;          // Auto-learn mode (type MR_BBU_AUTO_LEARN_MODE)
    U8      bbuMode;                // Not applicable for all BBU types. 0=undefined if not supported by bbuType.
                                    // If set to 0=applicable default shall be used. See MR_BBU_DESIGN_INFO.bbuMode
    U8      reserved1;
    U32     lastSuccessfulLearnTime;// Time-stamp of Last successful learn cycle (seconds past 1/1/2000)
                                    // Special values: 0 = Not supported, 1 = Learn Status unknown, 0xFFFFFFFF = Learn successful but time not available.
    U8      reserved[16];           // reserved
} MR_BBU_PROPERTIES;                // 32 bytes

/*
 * Defines the current capacity information for the battery.  This information is read from the
 * battery when requested by an application.
 */
typedef struct _MR_BBU_CAPACITY_INFO {
    U16     relativeStateOfCharge;  // predicted remaining capacity expressed as % of FullChargeCapacity() (0-100)
    U16     absoluteStateOfCharge;  // predicted remaining capacity expressed as % of DesignCapacity() (0-100)
    U16     remainingCapacity;      // predicted charge or energy remaining in the battery in mAh
    U16     fullChargeCapacity;     // predicted pack capacity when it is fully charged in mAh
    U16     runTimeToEmpty;         // (minutes)Relative gain or loss in remaining life in response to change in power policy
    U16     averageTimeToEmpty;     // one-minute rolling average of the predicted remaining battery life
    U16     averageTimeToFull;      // one-minute rolling average of predicted remaining time to full charge
    U16     cycleCount;             // number of charge cycles the battery has experienced
    U16     maxError;               // expected margin of error (%) in charge calculation (2-100)
    U16     remainingCapacityAlarm; // read-only; unsupported by FW.  Refer to BQ2060A manual for details (units is mAh)
    U16     reminingTimeAlarm;      // read-only; unsupported by FW.  Refer to BQ2060A manual for details (units is minutes)
    U8      reserved[26];
} MR_BBU_CAPACITY_INFO;             // 48 bytes

/*
 * Define the BBU Design Information Structure.  It contains read only data that is
 * initialization at boot time.
 */
typedef struct _MR_BBU_DESIGN_INFO {
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     day      : 5;       // 0 to 31 corresponds to date
        U32     month    : 4;       // 1 to 12 corresponds to month number
        U32     year     : 16;      // year, valid years from 1980+
        U32     reserved : 7;
#else
        U32     reserved : 7;
        U32     year     : 16;      // year, valid years from 1980+
        U32     month    : 4;       // 1 to 12 corresponds to month number
        U32     day      : 5;       // 0 to 31 corresponds to date
#endif  // MFI_BIG_ENDIAN
    } mfgDate;                      // date of manufacture in format as mentioned above

    U16     designCapacity;         // theoretical/nominal capacity of a new pack in mAh,
                                    // in Joules for MR_TMMC_STATE
    U16     designVoltage;          // theoretical(expected) terminal voltage of a new pack in mV
    U16     specificationInfo;      // refer to page 27 of the bq2060A manual
                                    // invalid for MR_TMMC_STATE
    U16     serialNumber;           // 16 bit serial number; will repeat after 65535
    U16     packStatConfiguration;  // pack configuration and status. Refer to page 28 of bq2060A manual
                                    // invalid for MR_TMMC_STATE

    U8      mfgName[12];            // battery manufacturer's name (NULL-terminated)
    U8      deviceName[8];          // battery name (NULL-terminated)
    U8      deviceChemistry[5];     // battery chemistry (NULL-terminated)
    U8      mfgData[8];             // binary encoded manufacturing information ([0]=length, [1..7]=manufacturing data)
    U8      bbuMode;                // bbuMode=0 is undefined, see BBU documentation for definition
    U8      transparentLearns;      // if set, auto learns shall be transparent, and cannot be set to WARN only mode.
                                    //         also when autoLearn is set to AUTO, autoLearnPeriod or delay cannot be set
    U8      moduleVersion[8];       // null terminated string
    U8      appData;                // Populated based on BBU HW
    U8      reserved[6];
} MR_BBU_DESIGN_INFO;               // 64 bytes

/*
 * define the BBU Mode structure
 */
typedef struct _MR_BBU_MODE {
    U8      bbuModeId;              // number identifying the bbuMode
    U8      retentionTime;          // module's design data retention time in hours, (0xFF - infinite for flash modules)
    U8      serviceLife;            // module's design service life in years
    U8      temperature;            // module's temperature rating in degree centigrade.
#ifndef MFI_BIG_ENDIAN
    U16     transparentLearn : 1;   // 1= module supports transparent learns, for this bbuMode
    U16     reserved         :15;
#else
    U16     reserved         :15;
    U16     transparentLearn : 1;   // 1= module supports transparent learns, for this bbuMode
#endif  // MFI_BIG_ENDIAN
    U16     pad;                    // For future use
} MR_BBU_MODE;

/*
 * define the BBU Mode structure
 */
typedef struct _MR_BBU_MODE_LIST {
    U32             count;           // count of BBU Modes
    MR_BBU_MODE     bbuModeList[1];  // Variable length array of bbu modes
} MR_BBU_MODE_LIST;

/*
 * define the Gas Guage Battery Status structure
 */
typedef union _MR_BBU_GAS_GUAGE_STATUS {
    struct {
#ifndef MFI_BIG_ENDIAN
        U16     error                   : 4;    // error codes - 0==>OK (see BQ2060A, page 25 for error code definitions)
        U16     fullyDischarged         : 1;
        U16     fullyCharged            : 1;
        U16     discharging             : 1;
        U16     initialized             : 1;
        U16     remainingTimeAlarm      : 1;
        U16     remainingCapacityAlarm  : 1;
        U16     reserved1               : 1;
        U16     terminateDischargeAlarm : 1;
        U16     overTemperatureAlarm    : 1;
        U16     reserved2               : 1;
        U16     terminateChargeAlarm    : 1;
        U16     overchargedAlarm        : 1;
#else
        U16     overchargedAlarm        : 1;
        U16     terminateChargeAlarm    : 1;
        U16     reserved2               : 1;
        U16     overTemperatureAlarm    : 1;
        U16     terminateDischargeAlarm : 1;
        U16     reserved1               : 1;
        U16     remainingCapacityAlarm  : 1;
        U16     remainingTimeAlarm      : 1;
        U16     initialized             : 1;
        U16     discharging             : 1;
        U16     fullyCharged            : 1;
        U16     fullyDischarged         : 1;
        U16     error                   : 4;    // error codes - 0==>OK (see BQ2060A, page 25 for error code definitions)
#endif  // MFI_BIG_ENDIAN
    } mrBbuGasGaugeStatus;                      // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U16     s;
} MR_BBU_GAS_GUAGE_STATUS;

/*
 * define the iBBU STATE structures
 */
typedef struct  _MR_IBBU_STATE {
    U16     gasGuageStatus;         // gas guage status based on MR_BBU_TYPE and subType
                                    // (type MR_BBU_GAS_GUAGE_STATUS for MR_BBU_TYPE_IBBU, MR_BBU_TYPE_iTBBU3 and MR_BBU_TYPE_ZCR_LEGACY)
    U16     relativeStateOfCharge;  // same as defined in _MR_BBU_CAPACITY_INFO
    union {
        struct {
            U16     chargerSystemState;     // from Smart charger (See Linear Tech 1760 documentation, page 14)
            U16     chargerSystemCtrl;      // from Smart charger (See Linear Tech 1760 documentation, page 14)
        } mrIbbuCharger;                    // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
        struct {
            U16     iTBBU3chargerStatus;    // from Smart charger (See Linear Tech 4101 documentation, page 13)
            U16     pad;
        } mrItbbu3Charger;                  // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
        struct {                            // dumb charger status
#ifndef MFI_BIG_ENDIAN
            U16     charging : 1;
            U16     fault    : 1;
            U16     reserved :14;
#else
            U16     reserved :14;
            U16     fault    : 1;
            U16     charging : 1;
#endif  // MFI_BIG_ENDIAN
            U16     pad;
        } mrIbbu08ChargerStatus;
    };
    U16     chargingCurrent;        // charging current (mAh) requested by the gas guage from Smart Charger
    U16     absoluteStateOfCharge;  // same as defined in _MR_BBU_CAPACITY_INFO
    U16     maxError;               // same as defined in _MR_BBU_CAPACITY_INFO
    U8      retentionTime;          // current retention time in hours. 0 is undefined if battery is operational
    U8      pad1;

    U8      reserved[16];
} MR_IBBU_STATE;                    // 32 bytes

/*
 * define the BBU STATE structures
 */
typedef enum _MR_BBU_CHARGER_STATUS {           // define the charger status values
    MR_BBU_CHARGER_STATUS_OFF           = 0,
    MR_BBU_CHARGER_STATUS_COMPLETE      = 1,
    MR_BBU_CHARGER_STATUS_IN_PROGRESS   = 2,
} MR_BBU_CHARGER_STATUS;

typedef struct  _MR_BBU_STATE {
    U16     gasGuageStatus;         // gas guage status based on MR_BBU_TYPE and subType
                                    // (type MR_BBU_GAS_GUAGE_STATUS for subType=0)
    U16     relativeStateOfCharge;  // same as defined in _MR_BBU_CAPACITY_INFO
    U16     chargerStatus;          // charger status signals (type MR_BBU_CHARGER_STATUS)
    U16     remainingCapacity;      // predicted charge remaining in battery in mAh
    U16     fullChargeCapacity;     // predicted pack capacity when fully charged
    U8      isSOHGood;              // boolean flag which says SOH good or bad
    U8      autoLearnModeSupport;   // 1=MR_BBU_PROPERTIES.autoLearnMode can be set
    U32     badBlockCount;          // number of bad blocks
    U8      reserved[16];
} MR_BBU_STATE;                     // 32 bytes

/*
 * define the TMMC STATE structures
 */
typedef struct  _MR_TMMC_STATE {
    U8  packEnergy;                 // Energy in pack in Joules(LSB)
    U8  capacitance;                // Capacitance % of design value (0xFF invalid, need relearn to update)
    U8  remainingReserveSpace;      // Remaining reserve pool % of design value
    U8  packEnergyMSB;              // MSB of packEnergy field
    U8  reserved[28];               // For future use
} MR_TMMC_STATE;

/*
 * define the status union for BBU and iBBU state structures
 */
typedef union _MR_BBU_STATUS_DETAIL {
     MR_IBBU_STATE  ibbu;           // represents iTBBU/iBBU information: bq2060A, LTC1760/LTC4101, and dumb charger
     MR_BBU_STATE   bbu;            // represents BBU implementation based on bq2060A and bq24105
     MR_TMMC_STATE  tmmc;           // represents TMM-C (supercap) related information
} MR_BBU_STATUS_DETAIL;             // 32 bytes

typedef enum _MR_BBU_TYPE {
    MR_BBU_TYPE_NO_BATT     = 0,
    MR_BBU_TYPE_IBBU        = 1,
    MR_BBU_TYPE_BBU         = 2,
    MR_BBU_TYPE_ZCR_LEGACY  = 3,
    MR_BBU_TYPE_iTBBU3      = 4,
    MR_BBU_TYPE_iBBU08      = 5,
    MR_BBU_TYPE_TMMC        = 6,
    MR_BBU_TYPE_SMARTBATTERY= 7,
} MR_BBU_TYPE;

/*
 * fwBatteryStatus variable can represent following conditions of the battery
 */
typedef union _MR_BBU_FW_STATUS {
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     packMissing         : 1;    // battery unplugged
        U32     voltageLow          : 1;
        U32     temperatureHigh     : 1;
        U32     chargeActive        : 1;
        U32     dischargeActive     : 1;
        U32     learnCycleRequested : 1;
        U32     learnCycleActive    : 1;
        U32     learnCycleFailed    : 1;
        U32     learnCycleTimeout   : 1;
        U32     i2cErrorsDetected   : 1;
        U32     replacePack         : 1;    // battery needs to be replaced
        U32     remainingCapacityLow: 1;
        U32     periodicLearnRequired : 1;  // 1=learn cycle requested for battery maintenance
                                            // manual learn maybe required
        U32     transparentLearn    : 1;    // 1=Learn cycle is transparent, cache is not disabled
        U32     noSpace             : 1;    // No space for cache-offload
        U32     predictiveFailure   : 1;    // pack is about to fail - should be replaced

        U32     premiumFeatureReqd  : 1;    // Premium Feature Required, caching maybe disabled
        U32     microcodeUpdateReqd : 1;    // Module microcode update required
        U32     cacheVaultDisabled  : 1;    // Cache vault disabled
        U32     batteryCableFailure : 1;    // unable to communicate with the battery
        U32     reserved            : 12;
#else
        U32     reserved            : 12;
        U32     batteryCableFailure : 1;    // unable to communicate with the battery
        U32     cacheVaultDisabled  : 1;    // Cache vault disabled
        U32     microcodeUpdateReqd : 1;    // Module microcode update required
        U32     premiumFeatureReqd  : 1;    // Premium Feature Required, caching maybe disabled
        U32     predictiveFailure   : 1;    // pack is about to fail - should be replaced
        U32     noSpace             : 1;    // No space for cache-offload
        U32     transparentLearn    : 1;    // 1=Learn cycle is transparent, cache is not disabled
        U32     periodicLearnRequired : 1;  // 1=learn cycle requested for battery maintenance
                                            // manual learn maybe required
        U32     remainingCapacityLow: 1;
        U32     replacePack         : 1;    // battery needs to be replaced
        U32     i2cErrorsDetected   : 1;
        U32     learnCycleTimeout   : 1;
        U32     learnCycleFailed    : 1;
        U32     learnCycleActive    : 1;
        U32     learnCycleRequested : 1;
        U32     dischargeActive     : 1;
        U32     chargeActive        : 1;
        U32     temperatureHigh     : 1;
        U32     voltageLow          : 1;
        U32     packMissing         : 1;    // battery unplugged
#endif  // MFI_BIG_ENDIAN
    } mrBbuFwStatus;                        // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U32     w;
} MR_BBU_FW_STATUS;

/*
 * define helper macros for MR_BBU_FW_STATUS
 *
 * To verify whether BBU fw status is bad, i.e. cannot support Write Back cache, "and" the
 * respective MR_BBU_STATUS_BAD_FOR_xxx macro with its MR_BBU_FW_STATUS.w and check for a
 * non-zero result.
 */
#define MR_BBU_STATUS_BAD_FOR_IBBU         0x00000C73      // For MR_BBU_TYPE_IBBU and MR_BBU_TYPE_iTBBU3
#define MR_BBU_STATUS_BAD_FOR_IBBU08       0x00000C03      // For MR_BBU_TYPE_IBBU08
#define MR_BBU_STATUS_BAD_FOR_BBU          0x00000C01      // For MR_BBU_TYPE_BBU
#define MR_BBU_STATUS_BAD_FOR_TMMC         0x00034C03      // For MR_BBU_TYPE_TMMC
#define MR_BBU_STATUS_BAD_FOR_SMARTBATTERY 0x00080609      // For MR_BBU_TYPE_SMART_BATTERY


/*
 * define the BBU status structure.  This data is updated every poll cycle (5 seconds) by FW.
 */
typedef struct _MR_BBU_STATUS {
    U8          batteryType;        // type of BBU HW present (type MR_BBU_TYPE)
    U8          subType;            // sub type of batteryType for BBU HW
    U16         voltage;            // accurate battery terminal voltage in mV, resolution of 1mV
    S16         current;            // current being supplied (or accepted) through the battery's terminals
    U16         temperature;        // battery temperature in 1 degree centigrade.
    U32         fwStatus;           // battery staus (type MR_BBU_FW_STATUS)
#ifndef MFI_BIG_ENDIAN
    U8          vpdSupported : 1;   // 1 = VPD supported
    U8          reserved     : 7;
#else
    U8          reserved     : 7;
    U8          vpdSupported : 1;   // 1 = VPD supported
#endif
    U8          pad[19];
    MR_BBU_STATUS_DETAIL    detail; // based on battery type, intepret the battery hw status information accordingly
} MR_BBU_STATUS;                    // 64 bytes

typedef enum _MR_BBU_VPD_TYPE {
    MR_BBU_VPD_TYPE_iBBU08      = 0,    // iBBU08 VPD
    MR_BBU_VPD_TYPE_TFM         = 1,    // TFM VPD for TMMC
    MR_BBU_VPD_TYPE_SUPERCAP    = 2,    // SuperCap VPD for TMMC
} MR_BBU_VPD_TYPE;

typedef enum _MR_BBU_VPD_SUBTYPE {
    MR_VPD_SUBTYPE_FIXED        = 0,
} MR_BBU_VPD_SUBTYPE;

typedef struct _MR_BBU_VPD_INFO_FIXED {
    U16     batteryType;
    U16     PCBversionNo;               // Should be used as ASCII
    U16     BATversionNo;               // Should be used as ASCII
    U16     serialNo;

    struct {
#ifndef MFI_BIG_ENDIAN
        U16     day     : 5;            // day of the month (1-31)
        U16     month   : 4;            // month of the year (1-12)
        U16     year    : 7;            // last two digits of the year. Valid years from 1980+
#else
        U16     year    : 7;            // last two digits of the year. Valid years from 1980+
        U16     month   : 4;            // month of the year (1-12)
        U16     day     : 5;            // day of the month (1-31)
#endif  // MFI_BIG_ENDIAN
    } dateOfMfg;                        // date of manufacture in format as mentioned above

    U8      pcbAssmNo[11];
    U8      batPackAssmNo[10];
    U8      reserved[33];
} MR_BBU_VPD_INFO_FIXED;                    // 0x64 bytes

typedef union _MR_BBU_VPD_INFO {
    MR_BBU_VPD_INFO_FIXED fixed;
    U8                    reserved[64];
} MR_BBU_VPD_INFO;                          // 0x64 bytes

/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to DPM and AEC
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * define constants for DPM DATA retrieval
 */
typedef enum _MR_DPM_DATA_TYPE {
    MR_DPM_DATA_TYPE_RESERVED0       = 0,    // reserved
    MR_DPM_DATA_TYPE_RESERVED1       = 1,    // reserved
    MR_DPM_DATA_TYPE_LONGCMDS        = 2,    // Get longest commands
    MR_DPM_DATA_TYPE_HISTOGRAM       = 3,    // Get histogram of response times
    MR_DPM_DATA_TYPE_SUMMARY         = 4,    // Get summary i.e. running average and cumulative stats
} MR_PDM_DATA_TYPE;

typedef enum _MR_DPM_DATA_CTRL {
    MR_DPM_DATA_CTRL_STOP            = 0,    // Stop DPM
    MR_DPM_DATA_CTRL_START           = 1,    // Start DPM
    MR_DPM_DATA_CTRL_CLEAR           = 2,    // Clear DPM statistics
    MR_DPM_DATA_CTRL_RAFACTOR        = 3,    // Change running average factor
} MR_DPM_DATA_CTRL;

typedef union _MR_DPM_DATA_CLEAR {
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     reserved1     :1;   // reserved
        U32     clearLongCmds :1;   // clear longest commands
        U32     clearHist     :1;   // clear histogram
        U32     clearRunAvg   :1;   // clear running average
        U32     clearTotals   :1;   // clear total cumulative stats
        U32     reserved2     :27;  // reserved
#else
        U32     reserved2     :27;  // reserved
        U32     clearTotals   :1;   // clear total cumulative stats
        U32     clearRunAvg   :1;   // clear running average
        U32     clearHist     :1;   // clear histogram
        U32     clearLongCmds :1;   // clear longest commands
        U32     reserved1     :1;   // reserved
#endif

    } mrDpmDataClear;               // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U32 w;
} MR_DPM_DATA_CLEAR;

typedef struct _MR_DPM_CFG {
    U8      started;                // Is performance monitoring started?
    U8      reserved1;              // reserved
    U16     cmdCount;               // number of commands that can be saved
    U16     longCmds;               // number of long time commands saved
    U16     raFactor;               // divisor to use when calculating running average 1-999
    U32     tUnits;                 // units of time (used in tStart/tEnd/tResp)
    U32     bootTime;               // local time stamp at offset=0 (secs since 1/1/1970, RTC_Timestamp)
} MR_DPM_CFG;

typedef struct _MR_DPM_DATA {
    U8      status;                 // command status
    U8      reserved1;              // reserved
    U16     reserved2;              // reserved
    U16     opCode;                 // command opcode to match
    U16     posted;                 // posted command count
    U32     dataLen;                // data byte count

    U32     tStart;                 // time command started
    U32     tEnd;                   // time command completed

    U32     localTime;              // timestamp of this cmd (secs since 1/1/1970, RTC_Timestamp)
} MR_DPM_DATA;

typedef struct _MR_DPM_DATA_EX {
    MR_DPM_DATA    baseInfo;        // base info for this saved cmd
    U8             cmdBytes[16];    // SCSI CDB or ATA task file
} MR_DPM_DATA_EX;

typedef struct _MR_DPM_RUNAVG {
    U32     iops;                   // IO/sec
    U32     kBps;                   // kBytes/sec
    U32     tResp;                  // response time (in tUnits)
    U32     posted;                 // posted cmds at completion
} MR_DPM_RUNAVG;

typedef struct _MR_DPM_TOTALS {
    U64     sectorsRead;            // total number of sectors read
    U64     sectorsWritten;         // total number of sectors written
    U64     sectorsWrittenFua;      // total number of sectors written with FUA
    U32     numRead;                // total number of read commands
    U32     numWrite;               // total number of write commands
    U32     numWriteFua;            // total number of write commands with FUA
    U32     numFlush;               // total number of flush commands
} MR_DPM_TOTALS;

// The following data structure is updated by drivers and firmware for tracking
// the running averages and response histogram.
typedef struct _MR_DPM_SUMMARY {
    MR_DPM_RUNAVG  runAvg;          // running average statistics
    MR_DPM_TOTALS  totals;          // cumulative statistics
} MR_DPM_SUMMARY;

/*
 * the size of each segment buffer when AEC_GET
 */
#define MR_AEC_SEGMENT_BUFFER_SIZE  (32*1024)

/*
 * defines the AEC Capture mode enum
 */
typedef enum _MR_AEC_CAPTURE_MODE {
    MR_AEC_CAPTURE_MODE_STOP    = 0,        // capture until stop (default)
    MR_AEC_CAPTURE_MODE_FULL    = 1         // capture until buffer is full
} MR_AEC_CAPTURE_MODE;

/*
 * defines the AEC Capture control enum
 */
typedef enum _MR_AEC_CAPTURE_CTRL {
    MR_AEC_CAPTURE_CTRL_STOPPED = 0,        // capture has stopped
    MR_AEC_CAPTURE_CTRL_STARTED = 1         // capture has started
} MR_AEC_CAPTURE_CTRL;

/*
 * defines the group number enum
 */
typedef enum _MR_AEC_GROUP_NUM {
    MR_AEC_GROUP_NUM_HOST_IO    = 0,        // host IO group
    MR_AEC_GROUP_NUM_FW_IO      = 1         // FW IO group
} MR_AEC_GROUP_NUM;

/*
 * defines the Host IO Filter Mask
 */
typedef enum _MR_AEC_HOST_IO_FM {
    MR_AEC_HOST_IO_FM_DISABLE_CAPTURE_HOST_READ_READ_VERIFY     = 0x00000001,   // disable capture of host read and read-verify cmds
    MR_AEC_HOST_IO_FM_DISABLE_CAPTURE_HOST_WRITE_WRITE_WITH_FUA = 0x00000002,   // disable capture of host write and write with FUA cmds
    MR_AEC_HOST_IO_FM_DISABLE_CAPTURE_DCMD                      = 0x00000004,   // disable capture of host DCMD cmds
    MR_AEC_HOST_IO_FM_DISABLE_CAPTURE_COMPLETION_RW             = 0x00000008,   // disable capture of completions from read/write and slow cmds
    MR_AEC_HOST_IO_FM_DISABLE_CAPTURE_HOST_READ_HIT             = 0x00000020,   // disable capture of host read hits
    MR_AEC_HOST_IO_FM_DISABLE_CAPTURE_HOST_LD_SCSI              = 0x00000040,   // disable capture of host LD scsi cmds
} MR_AEC_HOST_IO_FM;


/*
 * defines the FW IO Filter Mask
 */
typedef enum _MR_AEC_FW_IO_FM {
    MR_AEC_FW_IO_FM_DISABLE_CAPTURE_INTERNAL_WRITE      = 0x00000001,           // disable capture of internal writes (cache flush)
    MR_AEC_FW_IO_FM_DISABLE_CAPTURE_INTERNAL_READ       = 0x00000002,           // disable capture of internal reads (read ahead)
    MR_AEC_FW_IO_FM_DISABLE_TEXT_STRING                 = 0x00000008,           // disable capture of text strings
    MR_AEC_FW_IO_FM_DISABLE_USER_EVENT_R0               = 0x00000010,           // disable capture of user events range (00-7F)
    MR_AEC_FW_IO_FM_DISABLE_USER_EVENT_R1               = 0x00000020,           // disable capture of user events range (80-BF)
    MR_AEC_FW_IO_FM_DISABLE_USER_EVENT_R2               = 0x00000040,           // disable capture of user events range (C0-FF)
} MR_AEC_FW_IO_FM;

/*
 * defines the AEC Settings structure
 */
typedef struct _MR_AEC_SETTINGS {
    U8      state;                          // enabled (has buffer allocated for AEC) or disabled
    U8      control;                        // capture control(MR_AEC_CAPTURE_CTRL)
    U8      mode;                           // capture mode (MR_AEC_CAPTURE_MODE)
    U8      reserved0;                      //
    U32     numEvents;                      // number of events to capture.
    U32     numCapturedEvents;              // number of events captured in buffer.
    U32     numCapturedSegments;            // number of segments in buffer.
    U32     filterMaskHost;                 // filter mask (MR_AEC_HOST_IO_FM)
    U32     filterMaskFw;                   // filter mask (MR_AEC_FW_IO_FM)
    U8      reserved[16];                   // reserved for future.

} MR_AEC_SETTINGS;

typedef enum _MR_PERF_TUNE_MODES {
    MR_PERF_TUNE_BEST_IOPS          = 0x0000,   // Tuned to provide best IOPS, currently applicable to non FastPath
    MR_PERF_TUNE_LEAST_LATENCY      = 0x0001,   // Tuned to provide least latency, currently applicable to non FastPath
} MR_PERF_TUNE_MODES;
#define MR_PERF_TUNE_MODES_COUNT        2       // Count of MR_PERF_TUNE_MODES

/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures related to polling for object info over OOB interface
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * define MR OOB object type
 */
typedef enum _MR_OOB_OBJ_TYPE {
    MR_OOB_OBJ_EVT_LOG_INFO         = 0x0001,   // Latest event sequence number
    MR_OOB_OBJ_CTRL_ADAPTER_STATUS  = 0x0002,   // MR_CTRL_INFO.adapterStatus
    MR_OOB_OBJ_CTRL_TEMP_INFO       = 0x0004,   // Controller temperature info
    MR_OOB_OBJ_BBU_STATUS           = 0x0008,   // Battery related information
    MR_OOB_OBJ_PD_TEMP_INFO         = 0x0010,   // PD information
    MR_OOB_OBJ_PD_INFO              = 0x0020,   // PD information
    MR_OOB_OBJ_PD_REMOVED           = 0x0040,   // PD removal notification
    MR_OOB_OBJ_LD_INFO              = 0x0080,   // LD information

    MR_OOB_OBJ_ALL                  = 0xffff,   // All objects
} MR_OOB_OBJ_TYPE;
#define MR_OOB_OBJ_TYPE_COUNT       8           // Count of MR_OOB_OBJ_TYPEs

/*
 * define MR OOB Bitmap
 */
typedef struct _MR_OOB_TYPE_BITMAP {
    U8      count;          // Number of bytes in bitmap
    U8      bitmap[1];      // LSB of bitmap is bit 0 of evtBitmap[0].
                            // MSB of bitmap is bit 8 of evtBitmap[count-1].
} MR_OOB_TYPE_BITMAP;

/*
 * define MR OOB Objects
 */
typedef struct _MR_OOB_OBJ_RECORD {
   union {
       struct {                         // MR_OOB_OBJ_EVT_INFO
            U32  newestEventSeqNum;
            U32  newestEventSeqNumOOB;
       } eventInfo;

       struct {                         // MR_OOB_OBJ_CTRL_ADAPTER_STATUS
           U16  adapterStatus;              // MR_CTRL_INFO.adapterStatus
           U16  reserved;
       } adapterInfo;

       struct {                         // MR_OOB_OBJ_CTRL_TEMP_INFO
           U8   ctrlTemp;                   // Temp in degrees C
           U8   ctrlROCTemp;                // Temp in degrees C
       } ctrlTemp;

       struct {                         // MR_OOB_OBJ_BBU_STATUS
           U16  voltage;                    // accurate battery terminal voltage in mV
           S16  current;                    // current being supplied (or accepted) through the battery's terminals
           U32  fwStatus;                   // battery staus (type MR_BBU_FW_STATUS)
           U8   temperature;                // Temp in degrees C
           U8   pad[3];                     // to compile
       } bbuStatus;

       struct {                         // MR_OOB_OBJ_PD_TEMP_INFO
           MR_OOB_TYPE_BITMAP  pd;         // bitmap of affected PD deviceIDs
           U8                  temp[1];    // variable array of temperature for each affected PD
       } pdTempInfo;

       struct {                         // MR_OOB_OBJ_PD_INFO
           MR_OOB_TYPE_BITMAP  pd;         // bitmap of affected PD deviceIDs
           struct {
               U8   state;                  // fw state of the device (MR_PD_STATE)
#ifndef MFI_BIG_ENDIAN
               U8   powerState  : 4;        // power condition of the device (MR_PD_POWER_STATE)
               U8   predFailure : 1;        // device is reporting a predictive failure
               U8   otherErr    : 1;        // transient error(s) were detected in communicating with this device
               U8   locate      : 1;        // locate drive
               U8   reserved    : 1;        // For future use
#else
               U8   reserved    : 1;        // For future use
               U8   locate      : 1;        // locate drive
               U8   otherErr    : 1;        // transient error(s) were detected in communicating with this device
               U8   predFailure : 1;        // device is reporting a predictive failure
               U8   powerState  : 4;        // power condition of the device (MR_PD_POWER_STATE)
#endif
           } pdState [1];                   // variable array of PD states for each affected PD
       } pdinfo;

       MR_OOB_TYPE_BITMAP pdRemoved;    // MR_OOB_OBJ_PD_REMOVED - bitmap of PDs removed since last poll

       struct {                         // MR_OOB_OBJ_LD_INFO
           MR_OOB_TYPE_BITMAP  ld;         // bitmap of affected LD targetIds
           struct {
               U8   state;                  // state of the LD (MR_LD_STATE)
               U8   currentCachePolicy;     // current cache based on battery state (MR_LD_CACHE)
               U8   powerState;             // power condition of the LD (MR_LD_PS_POLICY)
           } ldState [1];                   // variable array of info for each affected LD
       } ldinfo;

   } record;
} MR_OOB_OBJ_RECORD;

typedef struct _MR_OOB_OBJ_INFO {
    U8       length;                    // 000 - byte length of object info record, including length field.
    U8       record[3];                 // variable length record buffer (MR_OOB_OBJ_RECORD)
} MR_OOB_OBJ_INFO;

/*
 * define MR OOB OBJ LIST
 */
typedef struct _MR_OOB_OBJ_LIST {
    U16                 objectTypeMap;  // Bitmap of MR_OOB_OBJ_TYPEs.
                                            // objects are packed in order of their enumeration
                                            // Example:
                                            // objectType = 0x0011, so number of object records is 2
                                            // objInfo[0] is MR_OOB_OBJ_EVT_INFO
                                            // objInfo[1] is MR_OOB_OBJ_PD_TEMP_INFO
    U8                  countEvents;    // Count of events packed at the end

    MR_OOB_OBJ_INFO     objInfo[1];     // A variable-length set of records for each object, which changed since last poll
    U8                  events[1];      // variable length buffer of a list of events (MR_EVT_PACKED)
} MR_OOB_OBJ_LIST;

/*
 * define parameters to control the OOB interface, especially POLL response
 */
typedef struct _MR_OOB_PARAMS {
    U32     eventClassLocale;       // Query to fliter events to be sent over OOB interface (MR_EVT_CLASS_LOCALE)
    U16     objMap;                 // Query to filter MR_OOB_OBJ_TYPE to be sent over OOB interface
                                    // 0x0000 - don't send MR_OOB_OBJ_INFO in  POLL response
#ifndef MFI_BIG_ENDIAN
    U16     sendProgressEvents : 1; // Send events of class MR_EVT_CLASS_PROGRESS
    U16     dontWaitForCmdCompletion : 1; // Send pending status if command not completed and waiting.
    U16     resvd              :14;       // For future use
#else
    U16     resvd              :14;       // For future use
    U16     dontWaitForCmdCompletion : 1; // Send pending status if command not completed and waiting.
    U16     sendProgressEvents : 1; // Send events of class MR_EVT_CLASS_PROGRESS
#endif
    U8      maxPollRespPackets;     // Don't exceed max packets for a poll response
                                    // 0=send all packets necessary to complete poll response
    U8      reserved;               // For future use
    U16     maxBytesPerTransaction; // This is used by FW to determine the max bytes the application can send in a
                                    // single transaction, 256 bytes being the default value. The value can be changed
                                    // by the application while sending DCMD MR_DCMD_OOB_PARAM_SET.
} MR_OOB_PARAMS;                    // 12 bytes

/*
 * defines the block size
 */
typedef enum _MR_BLOCK_SIZE {
    MR_BLOCK_SIZE_UNKNOWN             = 0,    // unknown block size, use 1 MB in MR_NVCACHE_ERASE_INFO for backward compatibility
    MR_BLOCK_SIZE_1_MB                = 1,    // block size 1 MB
    MR_BLOCK_SIZE_2_MB                = 2,    // block size 2 MB
    MR_BLOCK_SIZE_4_MB                = 3,    // block size 4 MB
    MR_BLOCK_SIZE_8_MB                = 4     // block size 8 MB
} MR_BLOCK_SIZE;

typedef struct _MR_NVCACHE_ERASE_INFO {
    U64    totalBlockCount;     // Total number of blocks in NVCACHE
    U64    badBlockCount;       // Number of bad blocks in NVCACHE
    U8     blockSize;           // MR_BLOCK_SIZE
    U8     reserved[7];         // For future use
} MR_NVCACHE_ERASE_INFO;

typedef struct _MR_CONFIG_SPAN {
    U32                 size;           // size of total data structure (including size/count fields)
    U32                 spanCount;      // count of number of span entries in this list
    U8                  pdPerSpan[MAX_SPAN_DEPTH];
    MR_PD_DEVICEID_LIST pdDeviceList;
} MR_CONFIG_SPAN;

/*
 * defines the power setting options
 */
typedef enum _MR_POWER_USAGE_OPTIONS {
    MR_POWER_USAGE_GET      = 0,
    MR_POWER_USAGE_SET      = 1,
    MR_POWER_USAGE_RESET    = 2,
} MR_POWER_USAGE_OPTIONS;


/*
 * Power Throttle API Definitions:
 */
typedef enum _MR_POWER_SLAB_FLAGS {
    MR_POWER_SLAB_FLAGS_POWER_USED    = 0x01,    //this indicate powerUsed is valid
    MR_POWER_SLAB_FLAGS_MIN_MAX       = 0x02,    //this indicate min and max is valid

}MR_POWER_SLAB_FLAGS;

/*
 * Power SLAB information
 */
typedef struct _MR_POWER_SLAB {
    U16 flags;                      //indicate which field is valid in union(MR_POWER_SLAB_FLAGS)
    U16 reserved;
    union {
        struct {
            U16 minimum;            //minimum watts allowed in this slab
            U16 maximum;            //maximum watts allowed in this slab
            U16 actualPowerUsed;    //current watt used
            U16 actualVoltage;      //actual voltage used
        }powerUsed;
        struct {
            U16 minimum;            //minimum watts allowed in this slab
            U16 maximum;            //maximum watts allowed in this slab
        }slab;
        U16 pad[6];
    };
} MR_POWER_SLAB;            //(Total 16 bytes)

/*
 * Available Power Throttle Schemes
 *
 */

typedef enum _MR_POWER_THROTTLE_OPTIONS {
    //Basic Power Options (Boot time options)
    MR_POWER_MAX_PERFORMANCE_OPTION = 0x01,    //Max Performance (Max watts used) - default
    MR_POWER_OPTIMIZED_OPTION       = 0x02,    //Intelligently power throttle without sacrificing normal (reasonable or ideal) performance
    MR_POWER_REDUCED_OPTION         = 0x04,    //Low power and Low performance

    //Extended Power Options (Runtime Changeable Parameters)
    MR_EXTENDED_POWER_OPTION1       = 0x10,    //ROC/Core/Onboard device(s) throttle, custom power slab between Max and Reduce power option
    MR_EXTENDED_POWER_OPTION2       = 0x20,    //ROC/Core/Onboard device(s) throttle, similar to MR_EXTENDED_POWER_OPTION1 but different power range/slab

}MR_POWER_THROTTLE_OPTIONS;



/*
 * Available Power Throttle Schemes
 */

typedef struct _MR_POWER_OPTION_SCHEME {
    U8            runtime; //1: means it change on fly, 0: required reboot
    U8            pad;
    U16           options; // MR_POWER_THROTTLE_OPTIONS
    MR_POWER_SLAB slab;    // SLAB
}MR_POWER_OPTION_SCHEME;

typedef struct _MR_CTRL_POWER_USAGE {
    U16 size;                // total size
    U16 seqNum;              // seq # to control concurrent updates
    U8  countOfOptions;               // number of options supported from MR_POWER_THROTTLE_OPTIONS
    U8  deafultOption;                // default option (MR_POWER_THROTTLE_OPTIONS)
    U8  currentOption;                // current selected option (MR_POWER_THROTTLE_OPTIONS)
    U8  pad;
    MR_POWER_SLAB powerUsed; // Power consumption in watts get or set

    MR_POWER_OPTION_SCHEME scheme[1]; //These are list of schemes available for this domain
} MR_CTRL_POWER_USAGE;

/*
 * defines Unmap Statistics
 */
typedef struct _MR_UNMAP_STATISTICS {
    U32 size;                  //size of struct
    U32 ldCount;               //ldCount
    U64 unmappedSectorCount[MAX_API_LOGICAL_DRIVES_EXT2];
} MR_UNMAP_STATISTICS;


/*
 * defines SSC PARAMETERs for statistics collection
 */


typedef  struct _MR_SSC_METRICS_PARAMETER
{
    U16  collectionTime;    /* Collection period in sec; 0 to stop collection */
    struct{
#ifndef MFI_BIG_ENDIAN
        U8  user        : 1;
        U8  exhaustive  : 1;
        U8  ldSpecific  : 1;
        U8  reserved    : 4;
        U8  debug       : 1;  //exclusive can't combine with other options
#else
        U8  debug       : 1;  //exclusive can't combine with other options
        U8  reserved    : 4;
        U8  ldSpecific  : 1;
        U8  exhaustive  : 1;
        U8  user        : 1;
#endif
    } paramType;
    U8 reserved2;
} MR_SSC_METRICS_PARAMETER;


/*
 * defines the SSC STATISTICS STRUCTURES
 */
typedef struct _MR_SSC_USER_METRICS {
    U32 size;           // size of total data structure (including size/count fields)
    U32 sscId :8;
    U32 reserved :24;
    U32 freeWindows;
    U32 dirtyWindows;
    U32 validWindows;
    U32 totalWindows;
    U32 hostReads;
    U32 hostReadHits;
    U32 hostReadMisses;
    U32 hostWrites;
    U32 reserved2[7];
} MR_SSC_USER_METRICS;

typedef struct _MR_SSC_EXHAUSTIVE_METRICS {
    U32     size;                // size of total data structure (including size/count fields)
    U32     sscId :8;
    U32     reserved :24;
    U32     freeWindows;         // Number of free windows
    U32     dirtyWindows;        // Number of dirty windows
    U32     validWindows;        // number of valid windows holding valid data
    U32     totalWindows;        // Total number of windows
    U32     totalWBWindows;      // Total writeback windows
    U32     hostReads;           // Total read (hits+misses)
    U32     hostReadHits;        // Read cache hit count
    U32     hostReadMisses;      // Read miss count
    U32     hostWrites;          // Total write (hits+misses)
    U32     hostWriteHits;       // Writes happened in CCVD
    U32     hostWriteMisses;     // Writes happened to HDD
    U32     partialReadHits;     // Data served from HDD as well as CCVD
    U32     partialWriteHits;    // Write happened in both HDD and CCVD
    U32     cacheWriteFills;     // number of times data read from source volume to pack the host writes
    U32     cacheReadFills;      // number of times data read from source volume to pack the host reads (partial read hits)
    U32     cacheFlushes;        // Total number of cache lines flush (from the power-on or after the counter reset manually or variable overflow)
    U32     dirtyCacheLines;     // Total dirty cache lines in all the dirty windows
    U32     residentCacheLines;  // Total resident cache lines in all valid windows
    U32     totalDiskRead;       // HDD reads from cachecade =read misses + write fills + read fills + host read
    U32     totalDiskWrite;      // HDD write from cachecade =  fast path IO count + cache bypass
    U32     cacheBypassIOs;      // How many IOs we skipped writing into Cache
    U32     sequentialWrites;    // Wr I/Os detected as Sequential
    U32     sequentialReads;     // Rd I/Os detected as Sequential
    U32     cacheIOPending;      // Command queue depth in cachecade ( no read of read and write IO pending)
    U32     timingResets;        // Number of times timer expired and reset the counter
    U32     reserved2[5];
} MR_SSC_EXHAUSTIVE_METRICS;

typedef struct _MR_LD_METRICS {
    U8  targetId;                // LD target id (0 to MAX_TARGET_ID)
    U8  targetIdMsb;             // LD target id (0 to MAX_API_TARGET_ID_EXT2)
    U8  reserved[2];
    U32 mbLatency;               //moving average latency per IO
    U32 mbRead;                  //total MBs READ in the time interval. Can roll over
    U32 mbWrite;                 //total MBs WRITE in the time interval. Can roll over.
    U32 numReadIO;               //number of read IOs in the collection interval. If collection interval is high, this can roll over, no guarantee
    U32 numWriteIO;              // number of write IOs in the collection interval. If collection interval is high, this can roll over, no guarantee
    U32 reserved2[2];
} MR_LD_METRICS;

/*
 * defines the MR_DEBUG_ENTRY structure
 */
typedef struct _MR_DEBUG_ENTRY {
        U32 entryId;
        U32 entryVal;
} MR_DEBUG_ENTRY;


typedef struct _MR_DEBUG_METRICS_LIST {
    U32             size;       // size of total data structure (including size/count fields)
    U32             entryCount; // counts number of metrics variables for the cachecade drive
    MR_DEBUG_ENTRY  entry[1];   // id , value combinations. Possible ID values are defined in enum below
} MR_DEBUG_METRICS_LIST;


typedef union _MR_SSC_METRICS {
    MR_SSC_USER_METRICS       usr;        //16 words
    MR_SSC_EXHAUSTIVE_METRICS exa;        //32 words
    MR_DEBUG_METRICS_LIST     dbg;        // only support in debug f/w
    U32                       detail[64]; //64 max
} MR_SSC_METRICS;

typedef struct _MR_SSC_METRICS_LIST {
    U32 size;
    U32 metricsCount;
    U32 metricsSize;
    U32 ldCount;
    U32 ldMetricsSize;
    MR_SSC_METRICS_PARAMETER param;     //Return  MR_SSC_METRICS_PARAMETERs including elapse time
    MR_SSC_METRICS metrics[1];          //metricsCount of MR_SSC_METRICS
    MR_LD_METRICS  ldMetrics[1];        //ldCount of MR_LD_METRICS
} MR_SSC_METRICS_LIST;


/*
 * defines the auto cfg options (this is not a bit-field)
 */
typedef enum _MR_AUTO_CFG_OPTIONS {
    MR_CFG_USER_CONFIGURABLE                = 0,  // Auto-config option if off
    MR_CFG_SINGLE_PD_R0_ASSOCIATED_SSC      = 1,  // Only auto-config SSC R0 for CacheCade
    MR_CFG_SINGLE_PD_R0                     = 2,  // auto-config only non-SSC R0 (single PD R0)
    MR_CFG_SINGLE_PD_R0_SSC_R0              = 3,  // Auto-config both SSC and non-SSC as R0
    MR_CFG_SINGLE_VD_R0_WB_WITH_ALL_PDS     = 4,  // Auto-config single R0 with all PD in WB
                                                  //    Options 5 and 6 are only available with MR_CTRL_PERSONALITY_SDS
                                                  //    Execute once only is not available for these CFG option
    MR_CFG_EPD                              = 5,  // Auto-config all unconfigured drives as EPD upon discovery
                                                  //    This option is only available with MR_CTRL_PERSONALITY_SDS
                                                  //    Execute once only is not available for this CFG option
    MR_CFG_EPD_PASSTHROUGH                  = 6,  // Auto-config all unconfigured drives as pass-through EPDs

} MR_AUTO_CFG_OPTIONS;
#define MR_MAX_AUTO_CFG_OPTIONS               6

/*
 * defines the timer options to auto discard the pinned cache(this is not a bit-field)
 */
typedef enum _MR_PINNED_CACHE_DISCARD_TIMER_OPTIONS {
    MR_PINNED_CACHE_DISCARD_IMMEDIATE                       = 0,  // Discard pinned cache immediately
    MR_PINNED_CACHE_DISCARD_TIMER_60SEC                     = 1   // Discard pinned cache after 60 seconds from the time cache was pinned

} MR_PINNED_CACHE_DISCARD_TIMER_OPTIONS;

/*
 * defines the latency options
 */
typedef enum _MR_LATENCY_OPTIONS {
    MR_LATENCY_OPTIONS_NONE                 = 0,  // No latency support, writes issued directly to devices
    MR_LATENCY_OPTIONS_LOW                  = 1,  // Low latency, all writes are cached
} MR_LATENCY_OPTIONS;
#define MR_MAX_LATENCY_OPTIONS                1

typedef struct _MR_CFG_EPD_PARAMs {
#ifndef MFI_BIG_ENDIAN
    U16     lowLatencyOptions     : 2;      // latency options for drives, see MR_LATENCY_OPTIONS
    U16     lowLatencyBadBBU      : 1;      // 1 = Honor lowLatencyOptions, regardless of DRAM backup availability
                                            // 0 = disable lowLatencyOptions, if DRAM cannot be protected due to power failure
   U16      disableErrorRecovery  : 1;      // 1 = FW shall not perform any error recovery for EPD
                                            //     This setting is ignored if lowLatencyOptions is set
    U16     dontFailPDOnError     : 1;      // 1 = FW shall not fail PD, if error recovery fails,
                                            // also see failPDOnReadME for options related to medium errors found during reads
                                            //     This setting may be overriden if lowLatencyOptions is set
    U16     MRIEMode              : 2;      // Manages presenting SMART trips to host.  Supported values are
                                            //     0 = Managed via NVDATA setting MR_MFC_DEFAULTS.smartMode, typically T10 MRIE mode 6
                                            //     1 = Disable FW SMART poll
                                            //     2 = Present UNIT ATTENTION sense key once per 24 hours, per initiator, T10 MREI mode 2
                                            //     3 = Present RECOBVERED ERROR sense key, once per 24 hours, T10 MRIE mode 4
                                            //  Note for T10 MRIE mode 5, set MRIEmode=1, and host must program IEC page directly.
    U16     disablePinnedCache    : 1;      // 1 = FW will not pin cache for EPDs
    U16     exposeMultipath       : 1;      // 1 = FW shall expose multiple paths to host for EPDs with multiple paths
    U16     supportXCopy          : 1;      // 1 = FW supports third party commands, EXTENDED COPY(LID4) for block -> block copy * for future use *
    U16     multiInitSupport      : 1;      // 1 = FW will enable shared environment support
    U16     failPDOnReadME        : 1;      // 1 = FW will fail the drive on observing a read medium error.
    U16     pinnedCacheDiscardTimerOptions : 2; // 1 = FW will automatically discard the pinned cache based on the timer in secs.
                                                // Please refer MR_PINNED_CACHE_DISCARD_TIMER_OPTIONS.
                                                // This field will be valid only if disablePinnedCache is set.
                                                // Default value will be '0', means 0 secs, 1 = 60 secs. others reserved now
    U16     reserved              : 2;
#else
    U16     reserved              : 2;
    U16     pinnedCacheDiscardTimerOptions : 2; // 1 = FW will automatically discard the pinned cache based on the timer in secs.
                                                // Please refer MR_PINNED_CACHE_DISCARD_TIMER_OPTIONS.
                                                // This field will be valid only if disablePinnedCache is set.
                                                // Default value will be '0', means 0 secs, 1 = 60 secs. others reserved now
    U16     failPDOnReadME        : 1;      // 1 = FW will fail the drive on observing a read medium error.
    U16     multiInitSupport      : 1;
    U16     supportXCopy          : 1;      // 1 = FW supports third party commands, EXTENDED COPY(LID4) for block -> block copy * for future use *
    U16     exposeMultipath       : 1;      // 1 = FW shall expose multiple paths to host for EPDs with multiple paths
    U16     disablePinnedCache    : 1;
    U16     MRIEMode              : 2;
    U16     dontFailPDOnError     : 1;
    U16     disableErrorRecovery  : 1;
    U16     lowLatencyBadBBU      : 1;
    U16     lowLatencyOptions     : 2;
#endif

    U16     reserved2;
} MR_CFG_EPD_PARAMS;

typedef struct _MR_AUTO_CFG_PARAMETER {
    U16  size;                                    // total size
    U16  seqNum;                                  // seq # to control concurrent updates
    U16  maxCfgOptions;                           // maximum options available allowed in MR_AUTO_CFG_OPTIONS
    U16  autoCfgOption;                           // pointing to enum MR_AUTO_CFG_OPTIONS

    union {
        U32  reserved;
        MR_CFG_EPD_PARAMS EPDParams;
    };

    /*
     * Properties or status which apply to all auto-configure options, for the current personality
     */
#ifndef MFI_BIG_ENDIAN
    U16  cannotExecuteOnce        : 1;      // 1 = MR_DCMD_CFG_AUTOCFG cannot be issued to execute once
    U16  autoSecureSED            : 1;      // 1 = If Drive security feature is enabled, auto secure, the auto-configured devices
    U16  disableSESManagement     : 1;      // 1 = FW shall not manage SES devices
    U16  supportMultiBehaviorMode : 1;      // 1 = support multiple behavior mode and selective parameter retrieval using
                                            //     mainbox dcmd.mbox.b[4] = MR_AUTO_CFG_OPTIONS and dcmd.mbox.w[0] = 0 or 3
    U16  supportEPDWithAutoConfigOff : 1;   // 1 = FW allows EPD/EPD-PT creation in MR_CFG_USER_CONFIGURABLE mode
    U16  rsvd                     :11;      // reserved for future use
#else
    U16  rsvd                     :11;
    U16  supportEPDWithAutoConfigOff : 1;  // 1 = FW allows EPD/EPD-PT creation in MR_CFG_USER_CONFIGURABLE mode
    U16  supportMultiBehaviorMode : 1;     // 1 = support multiple behavior mode and selective parameter retrieval using
                                           //     mainbox dcmd.mbox.b[4] = MR_AUTO_CFG_OPTIONS and dcmd.mbox.w[0] = 0 or 3
    U16  disableSESManagement     : 1;     // 1 = FW shall not manage SES devices
    U16  autoSecureSED            : 1;     // 1 = If Drive security feature is enabled, auto secure, the auto-configured devices
    U16  cannotExecuteOnce        : 1;     // 1 = MR_DCMD_CFG_AUTOCFG cannot be issued to execute once
#endif

    U16  allowedCfgOptions;                 // Bitmap of allowed cfg options
                                            // Bitmap is valid if bit 0 is set (i.e. allowedCfgOptions & 0x1 == 1)
                                            // bit 1 corresponds to MR_AUTO_CFG_OPTIONS option 1, and so on.
} MR_AUTO_CFG_PARAMETER;

/*
 * defines the controller HA mode enum
 * EHA: HA FW code, WT only, 50% Clines, no RLM/OCE, dual controller, failover, A/P
 * ESA: HA FW code, WB is available, 100% CL, RLM/OCE, single controller, no failover
 */
typedef enum _MR_CTRL_HA_CONTROL_MODE {
    MR_CTRL_HA_MODE_SC                      = 0,  // Like ESA or similar
    MR_CTRL_HA_MODE_DC_REDUCED_FEATURE      = 1,  // Like EHA or similar
    MR_CTRL_HA_MODE_DC_FULL_FEATURE         = 2,  // Mode that will have closer or full feature parity with non-HA
} MR_CTRL_HA_CONTROL_MODE;


/*
 * Different modes of the controller
 */
typedef enum _MR_CTRL_MAINTENANCE_MODE {
    MR_CTRL_MAINTENANCE_MODE_NORMAL             = 0,    // FW in Normal Mode (Not in maintenance mode)
    MR_CTRL_MAINTENANCE_MODE_NO_LDS_OR_DEVICES  = 1,    // FW will not present LDs to host or communicate with
                                                        // devices on the storage bus.  No I/O possible, but will
                                                        // communicate with host and can process DCMDs. The controller will enter
                                                        // this mode upon the next system reboot or controller restart
                                                        // and persists until maintenance mode is disabled.
                                                        // Example: OS that don't support MPIO during Install/Upgrade of OS
} MR_CTRL_MAINTENANCE_MODE;


#define     MR_DEBUG_QUEUE_MAX_WRITERS         0x05   // Max DQ writers
#define     MR_DEBUG_QUEUE_MAX_LABELS          0x10   // Max DQ labels

/*
* Structure that contains the Dqueue configuration details
*/
typedef struct _MR_DEBUG_QUEUE_CONFIG
{
    U32     reserved;
    U32     writeOffset;                      // offset from where the new element is inserted in the dqueue
    U32     endOffset;                        // offset of the element towards the end of the dqueue
    U32     readOffset;                       // offset of the oldest element in the dqueue
    U32     queueSize;                        // size of the dqueue
    U32     actualSize;
    U32     metaStrRowSize;
    U8      metaStrColumn;
    U8      wrapCount;                        // flag which tell whether the queue is wrapped or not
    U8      metaStrActiveColumn;              // number of writers used by FW
    U8      pad;
    U8      logMaskBitmap[MR_DEBUG_QUEUE_MAX_WRITERS][MR_DEBUG_QUEUE_MAX_LABELS];
    U8      monitorBitmap[MR_DEBUG_QUEUE_MAX_WRITERS][MR_DEBUG_QUEUE_MAX_LABELS];
} MR_DEBUG_QUEUE_CONFIG;

/*
 * Structures that are returned as extended diagnostic results
 */
typedef enum _MR_DIAG_TEST {
    MR_DIAG_TEST_DMA    = 0,
    MR_DIAG_TEST_XOR    = 1,
    MR_DIAG_TEST_DISK   = 2,
    MR_DIAG_TEST_MEMORY = 3,
    MR_DIAG_TEST_NVRAM  = 4,
    MR_DIAG_TEST_LINK   = 5,
} MR_DIAG_TEST;
#define MR_MAX_DIAG_TEST    5

typedef struct _MR_DIAG_DETAILS {
    char testName[64];
    U8   diagTest;   // type of DIAG test (MR_DIAG_TEST)
    U8   status;     // the status of the diags
    U8   reserved1[6];
} MR_DIAG_DETAILS;

typedef struct _MR_DIAG_INFO {
    U32 size;       // size of total data structure (including size/count fields)
    U16 diagCount;  // number of entries in the list
    U8  reserved[2];
    MR_DIAG_DETAILS diagInfo[1];
} MR_DIAG_INFO;

/*
 * Different type of controller flush
 */
typedef enum _MR_CTRL_CACHE_FLUSH_TYPE {    // bit values (1, 2, 4, 8....)
    MR_CTRL_CACHE_FLUSH_TYPE_DDR    = 1,    // flush controller dram cache
    MR_CTRL_CACHE_FLUSH_TYPE_DISK   = 2,    // flush disk cache
    MR_CTRL_CACHE_FLUSH_TYPE_SSC    = 4     // flush controller SSC cache
} MR_CTRL_CACHE_FLUSH_TYPE;


/*
 * Controller SSC flush status
 */
typedef struct _MR_CTRL_SSC_FLUSH_STATUS {
    U32     windowSize;                 // ssc cache window size in bytes
    U32     forceFlushCount;            // dirty ssc cache windows remaining to flush in current Force Flush Cycle
    U32     totalDirtyWindows;          // total number of dirty ssc cache windows at present
    U32     reserved[3];
} MR_CTRL_SSC_FLUSH_STATUS;

/*
 * Personality mode related definitions
 */

typedef enum _MR_CTRL_PERSONALITY {             // bitmap field of personalities
    MR_CTRL_PERSONALITY_UNDEFINED   = 0x00,     // Not defined
    MR_CTRL_PERSONALITY_RAID        = 0x01,     // Regular mode where VDs can be created and exposed to host
    MR_CTRL_PERSONALITY_HBA         = 0x02,     // HBA mode where VDs cannot be created. Not the same as JBOD mode.
                                                // Physical drives will be exposed to host.
                                                // The controller will enter either of these modes upon the next system reboot or
                                                // controller restart and persists until the personality is changed.
    MR_CTRL_PERSONALITY_SDS         = 0x04,     // SDS Personality, compatible with MR_CTRL_PERSONALITY_RAID, MR_CTRL_PERSONALITY_JBOD
    MR_CTRL_PERSONALITY_JBOD        = 0x08,     // JBOD Personality, compatible with MR_CTRL_PERSONALITY_RAID, MR_CTRL_PERSONALITY_SDS
} MR_CTRL_PERSONALITY;

#define MR_MAX_PERSONALITY_MODES  8

typedef struct _MR_CTRL_PERSONALITY_INFO {

    U8      personalityBitmap;                  // Bitmap of the current personality mode/s
                                                // Currently, 0,1 are defined- so only 2 bits will be set
                                                // Refer to enum MR_CTRL_PERSONALITY.
    U8      requestedPersonality;               // Bitmap of the requested personality mode/s
                                                // Will be set to MR_CTRL_PERSONALITY_UNDEFINED -
                                                // - If there is no pending request to be applied
                                                // - When the personality mode gets updated after system reboot or controller restart
    U8      reserved[6];

    /*
     * For A supported personality of the enum, what other personalities can coexist. If persoanlity in the below
     * structure is MR_CTRL_PERSONALITY_UNDEFINED then ignore that element
     *
     * Ex: If RAID mode, HBA mode and together mode are supported, the values in this array would be
     * supportedModes[0].personality=1,supportedModes[0].comaptibliltyPersonalities=0x  0000 0010
     * supportedModes[1].personality=2,supportedModes[1].comaptibliltyPersonalities=0x  0000 0001
     * supportedModes[2 to 7].personality=MR_CTRL_PERSONALITY_UNDEFINED ignore these
     */
    struct {
        U8   personality;                       // for this personality, if MR_CTRL_PERSONALITY_UNDEFINED skip it
        U8   compatiblePersonalities;           // other supproted personalities for above mode
    } supportedPersonalities[MR_MAX_PERSONALITY_MODES];
} MR_CTRL_PERSONALITY_INFO;

#define MR_MAX_PROFILE_CONFIGS               256
#define MR_MAX_PROFILE_CONFIGS_PER_CARD        9 // each card will support only 9 different profiles
#define MR_PROFILE_ID_INVALID             0xffff // constant for invalid profile id

/*
 * Defines a profile as specified by OEM
 */
typedef struct  _MR_CTRL_PROFILE_ENTRY {
    U16  profileId;                         // 0x00 - Unique profileId
    U8   personalityMode;                   // 0x02 - Valid personality mode for this profile
    U8   reserved1[5];                      // 0x03 - pad
    U16  maxPdCount;                        // 0x08 - Maximum number of drives (SAS/SATA/PCIe) this profile supports
    U16  maxLdCount;                        // 0x0A - Maximum number of RAID + EPD logical devices this profile supports
    U16  maxAHCICount;                      // 0x0C - Maximum number of AHCI devices
    U16  maxNVMeCount;                      // 0x0E - Maximum number of NVMeCount

#ifndef MFI_BIG_ENDIAN
    U32  isDefaultProfile: 1;               // 0x10 - Bit to indicate default profiles
    U32  isCompatibleWithCurTopology: 1;    // 0x10 - Bit indicating if profile is compatible with current topology
    U32  isOptimizedProfile: 1;             // 0x10 - Bit to indicate profile optimized for current topology
    U32  isNVMeOnlyProfile: 1;              // 0x10 - Bit to indicate if Profile supports only NVMe device
    U32  isWBSupported:1;                   // 0x10 - Bit to indicate if Profile supports WB VDs
    U32  reserved3: 27;
#else
    U32  reserved3: 27;
    U32  isWBSupported:1;                   // 0x10 - Bit to indicate if Profile supports WB VDs
    U32  isNVMeOnlyProfile: 1;              // 0x10 - Bit to indicate if Profile supports only NVMe device
    U32  isOptimizedProfile: 1;             // 0x10 - Bit to indicate profile optimized for current topology
    U32  isCompatibleWithCurTopology: 1;
    U32  isDefaultProfile: 1;
#endif

    U8     reserved2[44];                     // 0x14 - pad
} MR_CTRL_PROFILE_ENTRY;                    // size - 0x40 bytes

/*
 * Defines the multiple profiles supported on MR/iMR Cards
 */

typedef struct _MR_CTRL_PROFILE_LIST {
    U16 curProfileId;                       // 0x00 - Current Profile Id configured on the card
    U16 requestedProfileId;                 // 0x02 - Requested Profile Id
                                            // Will be set to MR_PROFILE_ID_INVALID
                                            // - If there is no pending request to be applied
                                            // - When the profile gets updated after system reboot or controller restart
    U16 profileEntrySize;                   // 0x04 - Size of each profile entry in the profile List
    U16 profileEntryCount;                  // 0x06 - Number of Profiles included in this list
    U8  profileListOffset;                  // 0x08 - Offset of the first profile list entry
    U8  reserved[3];                        // 0x09 - pad

    // supported fields which will be available before profileList
    struct {                                // 0x0c
#ifndef MFI_BIG_ENDIAN
        U32     reserved            : 32;
#else
        U32     reserved            : 32;
#endif  // MFI_BIG_ENDIAN
    } support;
    MR_CTRL_PROFILE_ENTRY profileList [1];  // 0x10 - variable size array containing the profile list
} MR_CTRL_PROFILE_LIST;                     // Size - 0x50 minimum

/*
 * Controller FRU related definitions
 */
typedef enum _MR_IOV_CTRL_FRU_STATE_DIRECTION {
    MR_IOV_FRU_HOST_TO_CONTROLLER = 0,          // Syncing FRU state information from host to controller
    MR_IOV_FRU_CONTROLLER_TO_HOST = 1,          // Syncing FRU state information from controller to host
} MR_IOV_CTRL_FRU_STATE_DIRECTION;

typedef struct _MR_IOV_CTRL_FRU_STATE {
    U64     ctrlFruID;          // controller FRU id
    U32     ctrlFruSize;        // size of controller FRU data
    U8      reserved[12];       // reserved for future use
} MR_IOV_CTRL_FRU_STATE;

#define MR_FW_CRASH_DUMP_MAX_SIZE  512    // This value is represented in MBs

/*
 * define constants for FW crash dump status
 */
typedef enum _MR_CRASH_DUMP_STATUS {
    MR_CRASH_DUMP_TURN_OFF = 0,
    MR_CRASH_DUMP_TURN_ON  = 1
} MR_CRASH_DUMP_STATUS;

/*
 * define for Atomicity type
 */
typedef enum _MR_ATOMICITY_TYPE {
    MR_ATOMIC_OP_TYPE_OFF         = 0,     //Atomicity disabled or unsupported
    MR_ATOMIC_WRITE_ALL           = 1,     //Atomicity enabled for every write command
    MR_ATOMIC_WRITE_T10           = 2,     //Atomicity enabled for Atomic Write CDB as per T10 Atomic Write proposal
} MR_ATOMICITY_TYPE;


typedef struct _MR_PCI_OEM_INFO {
    MR_MFC_PCI PCIInfo;
    U8         OEM;
    U8         subOEM;
    U8         reserved[8];
} MR_PCI_OEM_INFO;

/*
 * define for NVDRAM
 */
typedef struct _MR_NVDRAM_PROPERTIES {
    U8      structSizeBytes;               // Size of structure
    U8      flags;                         // Flags usage model
    U8      reserved[2];
    U32     maxConfigureableSizeInMB;      // Maximum allowed size in MB
    U32     minConfigureableSizeInMB;      // Minimum allowed size in MB
    U32     currentConfiguredTotalSizeinMB;// Total configured Size in MB
    U32     currentProgrammableIOSizeInMB; // Size of PIO in MB
    U32     reserved2[3];
} MR_NVDRAM_PROPERTIES;

typedef enum _MR_CACHEBYPASS_IOSIZE {
    MR_CACHEBYPASS_IOSIZE_64K     = 0,
    MR_CACHEBYPASS_IOSIZE_128K    = 1,
    MR_CACHEBYPASS_IOSIZE_256K    = 2,
} MR_CACHEBYPASS_IOSIZE;

#define MR_CACHEBYPASS_MAX_IOSIZE_OPTIONS 7

typedef enum _MR_CACHEBYPASS_MODE {
    MR_CACHEBYPASS_ENABLE_INTELLIGENT      = 0,  // Involves streaming detection, can be used with any size in enum MR_CACHEBYPASS_IOSIZE
    MR_CACHEBYPASS_ENABLE_STANDARD         = 1,  // Will bypass the IO if its size > the value chosen through enum MR_CACHEBYPASS_IOSIZE
    MR_CACHEBYPASS_ENABLE_CUSTOM_MODE_1    = 2,  // Custom bypass mode. Not implemented currently
    MR_CACHEBYPASS_ENABLE_CUSTOM_MODE_2    = 3,  // Custom bypass mode. Not implemented currently
    MR_CACHEBYPASS_ENABLE_CUSTOM_MODE_3    = 4,  // Custom bypass mode. Not implemented currently
    MR_CACHEBYPASS_ENABLE_RESERVED_MODE_1  = 5,  // Reserved for future use
    MR_CACHEBYPASS_ENABLE_RESERVED_MODE_2  = 6,  // Reserved for future use
    MR_CACHEBYPASS_DISABLE                 = 7,  // Disable cache bypass
} MR_CACHEBYPASS_MODE;

#define MR_CACHEBYPASS_MAX_MODES 7

typedef enum _MR_IOV_QD_OPTIONS {
    MR_IOV_QD_EQUAL_DISTRIBUTION = 0,            // In this mode, the total QD will be divided equally among all the licensed VFs
    MR_IOV_QD_MAX_AVAILABLE      = 1,            // In this mode, each VF would be able to utilize the max QD supported by the adapter
    MR_IOV_QD_CUSTOM_SETTING     = 2,            // In this mode, user gets to set the QD for each VF
} MR_IOV_QD_OPTIONS;

#define MR_MAX_IOV_QD_OPTIONS 2

typedef struct _MR_IOV_QD_INFO {
    U16   minQDperVF;                           // Minimum QD that needs to be allocated to any VF (cannot be changed by apps)
    U16   maxQDperVF;                           // Maximum QD that needs to be allocated to any VF (cannot be changed by apps)
    U16   requestorID;                          // VF ID or the requestor ID. FF indicates for all VFs
    U16   QDValue;                              // The value needs to be between minQDperVF and maxQDperVF
} MR_IOV_QD_INFO;

typedef struct _MR_PD_ERROR_STATS {
    U32 size;   // size of entire structure
    union {
        U16 driveErrorCounter [MAX_API_PHYSICAL_DEVICES];   // error counter for drives
        U16 slotErrorCounter  [MAX_API_PHYSICAL_DEVICES];   // error counter for device slots
    };
} MR_PD_ERROR_STATS;

typedef struct _MR_DEBUG_INPUTS {
    U32   size;                // size of structure including the size field
    U16   debugVar;            // debug variable index or debug operation index
    U8    debugOpcode;         // debug operation set/clear/cear-all/dump
    U8    argCount;            // debug arg count
    U64   debugArgs[8];        // debug argument 1, it could be debug level or argument to a DM debug command
    U8    reserved[8];         // for future use
} MR_DEBUG_INPUTS;

/*
 * define the enum for uploaad image type
 */
typedef enum _MR_IMAGE_TYPE
{
    MR_IMAGE_SBR_1   = 0,       // Primary SBR Image
    MR_IMAGE_SBR_2   = 1,       // Secondary SBR Image
    MR_IMAGE_BB      = 2,       // BB image
    MR_IMAGE_BIOS    = 3,       // BIOS Image
    MR_IMAGE_MR_FW   = 4,       // MR FW Image
    MR_IMAGE_iMR_PRI = 5,       // iMR Primary FW Image
    MR_IMAGE_iMR_SEC = 6,       // iMR Secondary FW Image
    MR_IMAGE_NVDATA  = 7,       // NVDATA
    MR_IMAGE_CPLD    = 8,       // CPLD image
    MR_IMAGE_PSOC    = 9,       // PSOC image
    MR_IMAGE_MAX                // Max image
} MR_IMAGE_TYPE;

typedef enum _MR_SBAT_POWER_STATUS {
    MR_BACKUP_POWER_NOT_PRESENT  = 0,  // Backup power is not present in the system
    MR_BACKUP_POWER_ENABLED      = 1,  // Backup power is fully charged and providing backup power
    MR_BACKUP_POWER_CHARGING     = 2,  // Backup power is not fully charged, but after charging the solution will be enabled
    MR_BACKUP_POWER_FAILED       = 3,  // Backup power has encountered a fatal condition and will not be enabled
    MR_BACKUP_POWER_UNKNOWN      = 4,  // Cannot communicate with backup power. Backup power may or may not be providing charge
} MR_SBAT_POWER_STATUS;

typedef struct _MR_SBAT_INFO {
    U8    backupPowerInfoVersion;     // Integer value (not ASCII)
    U8    platformBackupPowerCount;   // Number of backup cycles allowed on this platform
    U8    backupPowerStatus;          // Aggregate status of all individual cells (refer enum MR_SBAT_POWER_STATUS)
    U8    reserved;
    U32   loadInMilliWatts;           // Power needed for cache offload (in milliWatts)
    U32   backupTimeInMilliSeconds;   // Cache offload time (in milliSeconds)
    U32   backupPowerChargeTime;      // Max charging time of any individual cell (in seconds)
} MR_SBAT_INFO;

typedef struct _MR_SC_BKPLN_DATA {
    U16  boxNumber;                  // box number associated with the NVRAM
    U8   reserved1[6];
    U64  sasExpanderWwid;
    U8   i2cAddress;                 // I2C address associated with the NVRAM
    U8   reserved2[7];
    U64  reserved;
    U8   nvramData[128];             // NVRAM data
} MR_SC_BKPLN_DATA;

typedef struct _MR_SC_BKPLN_NVRAM_DATA
{
    U32              size;           // size of total data structure (including size/count fields)
    U32              count;          // count of number of entries in this list
    MR_SC_BKPLN_DATA bkplnData[1];   // Instances of count number of MR_SC_BKPLN_DATAs
} MR_SC_BKPLN_NVRAM_DATA;

typedef struct _MR_SC_NVRAM_DATA {
    U8      recordVersion;
    U8      smartCarrierFwVersion;
    U8      typeCtrl;                      // Controller Info Record
    struct {
        U8      failure :1;
        U8      predictiveFailure :1;          // Device Failure
        U8      driveInstallInfo  :1;          // Device has Predictive Failure
        U8      reserved          :5;          // Drive Presence Detected
    } triggerInfo;
    U16     pciRevisionId;
    U16     pciSubsystemId;
    U16     pciSubsystemVendorId;
    struct {
        U8    dev;
        U8    unit;
        U8    minor;
        U8    major;
    } controllerFwVersion;
    struct {
        U8    dev;
        U8    unit;
        U8    minor;
        U8    major;
    } nvDataVersion;
    char    boardName[16];
    char    boardAssembly[16];
    char    boardTraceNumber[16];
    U8      sasAddress[8];
    U8      driveFailureReasonCode;
    U8      deviceDiscoveryStatus;
    U8      reserved[45];
    U8      typeEncl;                    // Enclosure Info Record
    U16     backPlaneId;
    U8      reserved1[29];
    U8      typeTarget;                  /* 0 */    //Target Info Record
    char    modelNumber[12];             /* 1-12 */
    char    firmwareVersion[4];          /* 13-16 */
    char    serialNumber[20];            /* 17-36 */
    U8      reserved2[16];               /* 37-52 */
    char    controllerPortNumber[2];     /* 53-54 */
    U8      boxNumber;                   /* 55 */
    U8      bayNumber;                   /* 56 */
    U8      reserved3[11];               /* 57-67 */
    U8      devicePowerOnMinutes[4];     /* 68-71 */
    U8      timeOfFailure[5];            /* 72-76 */
    U8      deviceTemperature;           /* 77 */
    U8      smartTripAscq;               /* 78 */
    U8      checkConditionSenseKey;      /* 79 */
    U8      checkConditionAsc;           /* 80 */
    U8      checkConditionAscq;          /* 81 */
    U8      reserved4[14];               /* 82-95 */
    U8      checkSum;
    U8      pad2[6];
} MR_SC_NVRAM_DATA;

/* Structure for snapdump properties */
typedef struct  _MR_SNAPDUMP_PROPERTIES {
    U8       offloadNum;                 // number of times snapdump will be persisted in case user doesn’t collect it.
    U8       maxNumSupported;            // DEPRECATED : max number of snapshot supported
    U8       curNumSupported;            // DEPRECATED : number of snapshot currently supported
    U8       triggerMinNumSecBeforeOcr;  // Driver needs to trigger snapdump triggerMinNumSecBeforeOcr sec before it initiates ocr.
    U8       reserved[12];               // For future use
} MR_SNAPDUMP_PROPERTIES;

typedef  struct  _MR_SNAPDUMP_HDR {
    U8  snapDumpNum;            // snapDump number
    U8  reserved1[7];
    MR_TIME_STAMP snapDumpTime;    // Time Stamp Of The Snap-Dump
    U8  reserved2[12];
    U32 snapDumpSizeBytes;      // size of the snapDump in Bytes
} MR_SNAPDUMP_HDR;

/* user will retrieve snap info before it initiates actual data transfer */
typedef struct _MR_SNAPDUMP_INFO {
    U8              countOfSnapInfo; // count of snapInfo entries
    U8              reserved[7];
    MR_SNAPDUMP_HDR snapInfo[1];
} MR_SNAPDUMP_INFO;

/* this structure will be used to retrieve the snapdump data */
typedef struct _MR_SNAPDUMP_DATA {
    U32     snapDumpNum;            // Snapdump number
    U32     actualOffset;           // actual offset of returned data
    U32     actualByteCount;        // actual returned byte count
    U8      data[4];                // variable-size returned data buffer
} MR_SNAPDUMP_DATA;

#endif /* INCLUDE_MR */
