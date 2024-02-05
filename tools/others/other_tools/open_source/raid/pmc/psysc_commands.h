/** @file psysc_commands.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore command(s) header file

*/

#ifndef PSYSC_COMMANDS_H
#define PSYSC_COMMANDS_H

/** @addtogroup syscore
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/** @defgroup syscore_commands Commands
 * @brief
 * @details
 * @{
 */

/** @name Transfer type for all requests.
 * @{ */
typedef SA_UINT32 EnTransferType;    /**< Command transfer type. */
#define   eDATA_TRANSFER_IN           0 /**< Transfer type IN. */
#define   eDATA_TRANSFER_IN_MESSAGE   1 /**< Transfer type MESSAGE IN. */
#define   eDATA_TRANSFER_OUT          2 /**< Transfer type OUT. */
#define   eDATA_TRANSFER_OUT_MESSAGE  3 /**< Transfer type MESSAGE OUT. */
#define   eDATA_TRANSFER_INOUT        4 /**< Transfer type IN & OUT. */
#define   eDATA_TRANSFER_NONE         5 /**< Transfer type NONE. */
/** @} */
/** @name Command status types for all command types.
 * @{ */
typedef SA_UINT32 EnCommandStatus; /**< Command status type. */
#define   eCOMMAND_STATUS_OK       0 /**< Command status OK/good. */
#define   eCOMMAND_STATUS_FAILED   1 /**< Command status failed. */
/** @} */

/** @defgroup syscore_commands_bmic BMIC Commands
 * @{ */
/** BMIC request */
struct _BMICRequest
{
   EnTransferType transfer_type;    /**< (in) Transfer type of the request. */
   SA_UINT32 timeout_sec;           /**< (in) Max timeout for the request. */
   SA_UINT8  bmic_opcode;           /**< (in) Command opcode. */
   SA_UINT32 bmic_detail;           /**< (in) BMIC detail. */
   /** (in) Target LUN. */
   union {
      SA_UINT16 lun_number;         /**< (in) Target LUN. */
      /** (in) Low/High bytes of target LUN. */
      struct {
         SA_UINT8  lun_number_low;  /**< (in) Low byte of target LUN. */
         SA_UINT8  lun_number_high; /**< (in) High byte of target LUN. */
      } BYTES;
   } LUN;
   SA_UINT8 *io_buffer;             /**< (in/out) Address to I/O data buffer. */
   SA_UINT32 io_buffer_size;        /**< (in/out) Size (in bytes) of I/O data buffer. */
   SA_INT32  low_level_status;      /**< (out) Low level status of the request. */
   SA_UINT16 command_status;        /**< (out) Command status of the request. */
   SA_UINT8  scsi_status;           /**< (out) SCSI status of the request. */
   SA_UINT8 *sense_data;            /**< (out) Address of the sense buffer for the request. */
   SA_UINT32 sense_data_size;       /**< (in/out) Size of the sense buffer (in bytes). */
   SA_UINT32 block_count_override;  /**< (in) Used to override `io_buffer_size` in command (in blocks). */
   SA_UINT32 valid_sense_data_size; /**< (out) Size of available sense data reported by the device for this command.
                                     *  This can be:
                                     *  - Smaller than sense_data_size if the device did response with sense data to fill sense_data
                                     *  - Larger thatn sense_data_size if the device had more sense then was allowed by sense_data_size.
                                     *  The user should use the smaller value between this field and sense_data_size when
                                     *  reading from snse_data.
                                     */
};
typedef struct _BMICRequest BMICRequest;
typedef struct _BMICRequest *PBMICRequest;
/** @} */ /* syscore_commands_bmic */

/** @defgroup syscore_commands_scsi SCSI Commands
 * @{ */
/** SCSI request */
struct _SCSIRequest
{
   EnTransferType transfer_type;    /**< (in) Transfer type of request. */
   SA_UINT32 timeout_sec;           /**< (in) Maximum timeout of request. */
   SA_UINT8 *cdb;                   /**< (in) Address of CDB. */
   SA_UINT8  cdb_size;              /**< (in) Size of CDB in bytes. */
   SA_UINT8 *io_buffer;             /**< (in/out) Address to I/O data (optional). */
   SA_UINT32 io_buffer_size;        /**< (in/out) Size (in bytes) of I/O data (optional). */
   SA_INT32  low_level_status;      /**< (out) Low level status of request. */
   SA_UINT16 command_status;        /**< (out) Command status of request. */
   SA_UINT8  scsi_status;           /**< (out) SCSI status of request. */
   SA_UINT8 *sense_data;            /**< (out) Address to sense buffer of request. */
   SA_UINT32 sense_data_size;       /**< (in/out) Size of sense buffer (in bytes). */
   SA_UINT32 valid_sense_data_size; /**< (out) Size of available sense data reported by the device for this command.
                                     *  This can be:
                                     *  - Smaller than sense_data_size if the device did response with sense data to fill sense_data
                                     *  - Larger than sense_data_size if the device had more sense then was allowed by sense_data_size.
                                     *  The user should use the smaller value between this field and sense_data_size when
                                     *  reading from snse_data.
                                     */
};
typedef struct _SCSIRequest SCSIRequest;
typedef struct _SCSIRequest *PSCSIRequest;

/** @name SCSI Status codes.
 * @{ */
typedef SA_UINT32 EnSCSIErrorCode;  /**< Command error type. */
#define   eERROR_CODE_NO_ERROR         0
#define   eERROR_CODE_BUFFER_UNDERRUN  1
#define   eERROR_CODE_BUFFER_OVERRRUN  2
/** @} */
/** @} */ /* syscore_commands_scsi */

/** @defgroup syscore_commands_io IO Requests
 * @{ */
/** IO Request structure */
struct _IORequest
{
   EnTransferType transfer_type;  /**< (in) Transfer type of request (IN = READ, OUT = WRITE). */
   SA_UINT64 lba_offset;          /**< (in) LBA for request. */
   SA_UINT8 *io_buffer;           /**< (in/out) Address to I/O data. */
   SA_UINT32 io_buffer_size;      /**< (in) Size (in bytes) of I/O data. */
   SA_UINT16 block_size_in_bytes; /**< (in) Device logical block size. */
   SA_INT32 low_level_status;     /**< (out) Low level status of request. */
};
typedef struct _IORequest IORequest;
typedef struct _IORequest *PIORequest;
/** @} */ /* syscore_commands_io */

/** @defgroup syscore_commands_ata ATA Commands
 * @{ */
/** ATA request */
struct _ATARequest
{
   EnTransferType transfer_type;   /**< (in) Transfer type of the request. */
   SA_UINT32 timeout_sec;          /**< (in) Max timeout of the request. */
   union {
      struct {
         SA_UINT8 previous_task_file[8]; /**< (in) */
         SA_UINT8 current_task_file[8];  /**< (in) */
      }TaskFiles;
      struct {
         SA_UINT8 previous_feature;     /**< (in) */
         SA_UINT8 previous_count;       /**< (in) */
         SA_UINT8 previous_lba_low;     /**< (in) */
         SA_UINT8 previous_lba_mid;     /**< (in) */
         SA_UINT8 previous_lba_high;    /**< (in) */
         SA_UINT8 previous_device;
         SA_UINT8 previous_command;
         SA_UINT8 previous_reserved;
         SA_UINT8 current_feature;     /**< (in) */
         SA_UINT8 current_count;       /**< (in) */
         SA_UINT8 current_lba_low;     /**< (in) */
         SA_UINT8 current_lba_mid;     /**< (in) */
         SA_UINT8 current_lba_high;    /**< (in) */
         SA_UINT8 current_device;      /**< (in) */
         SA_UINT8 current_command;     /**< (in) */
         SA_UINT8 curremt_reserved;
      }Registers;
      SA_UINT8 cdb[16];                /**< (in) Union of cdb/TaskFiles/Registers */
   } u;
   struct {
      SA_UINT8 error;               /**< (out) */
      SA_UINT8 sector_count_msb;    /**< (out) */
      SA_UINT8 sector_count_lsb;    /**< (out) */
      SA_UINT8 lba_low_msb;         /**< (out) */
      SA_UINT8 lba_low_lsb;         /**< (out) */
      SA_UINT8 lba_mid_msb;         /**< (out) */
      SA_UINT8 lba_mid_lsb;         /**< (out) */
      SA_UINT8 lba_high_msb;        /**< (out) */
      SA_UINT8 lba_high_lsb;        /**< (out) */
      SA_UINT8 device;              /**< (out) */
      SA_UINT8 status;              /**< (out) */
      SA_UINT8 sense_format;        /**< (out) */
   } ReturnRegisters;
   SA_UINT8 *io_buffer;                 /**< (in/out) */
   SA_UINT32 io_buffer_size;            /**< (in/out) */
   SA_INT32  low_level_status;          /**< (out) */
   SA_UINT16 command_status;            /**< (out) */
   SA_UINT8  scsi_status;               /**< (out) */
   SA_UINT8 *sense_data;                /**< (in/out) */
   SA_UINT32 sense_data_size;           /**< (in/out) */
};
typedef struct _ATARequest ATARequest;
typedef struct _ATARequest *PATARequest;
/** @} */ /* syscore_commands_ata */

/** @defgroup syscore_commands_nvme NVME Commands
 * @{ */
/** NVME Request. */
struct _NVMERequest {
   EnTransferType transfer_type;      /**< (in) */
   SA_UINT32 timeout_sec;             /**< (in) */
   SA_UINT32 command[16];             /**< (in) */
   SA_UINT32 queue_id;                /**< (in) */
   SA_UINT8 *io_buffer;               /**< (in/out) */
   SA_UINT32 io_buffer_size;          /**< (in/out) */
   SA_INT32  low_level_status;        /**< (out) */
};
typedef struct _NVMERequest NVMERequest;
typedef struct _NVMERequest *PNVMERequest;
/** @} */ /* syscore_commands_nvme */

/** @defgroup syscore_commands_smp SMP Commands
 * @{ */
/** SMP Request. */
struct _SMPRequest
{
   /** (in) Phy identifier that specifies which phy shall be used to issue the request.
    */
   SA_UINT8 phy_identifier;
   /** (in) Port identifier that specified which port shall be used to issue the request.
    */
   SA_UINT8 port_identifier;
   /** (in) The SAS address of expander.
    * Populated automatically if using
    * [SA_SendPhysicalDeviceSMPCommand](@ref storc_commands_pdevice_smp).
    */
   SA_UINT8 sas_address[8];
   /** (in/out) The frame type of SMP request.
    * Initialize this with 0x40 for SMP Requests.
    */
   SA_UINT8 header_frame_type;
   /** (in/out) Contains the SMP function to request.
    */
   SA_UINT8 header_function;
   /** (in)
    */
   SA_UINT8 header_allocation_response_size;
   /** (in)
    */
   SA_UINT8 header_request_length;
   /** (in) Pointer to additional request data.
    */
   SA_UINT8 *additional_request_data;
   /** (in) Size (in bytes) of additional request data.
    */
   SA_UINT32 additional_request_data_size;
   /** (out) Function field of SMP request result.
    */
   SA_UINT8 header_function_result;
   /** (out) Size (in bytes) of response header.
    */
   SA_UINT8 header_response_size;
   /** (out) Pointer to additional response data.
    */
   SA_UINT8 *additional_response_data;
   /** (in) Size (in bytes) of additional response data.
    */
   SA_UINT32 additional_response_data_size;
   /** (out) Low level status of request.
    */
   SA_INT32 low_level_status;
};
typedef struct _SMPRequest SMPRequest;
typedef struct _SMPRequest *PSMPRequest;
/** @} */ /* syscore_commands_smp */

/** @addtogroup syscore_commands_bmic
 * @{ */
/** Can a BMIC command be executed?
 * @post If valid, user may call @ref SC_ExecuteBMICCommand.
 * @param[in] device_descriptor  BMIC device handle.
 * @return See [Can Execute BMIC Command Returns](@ref syscore_commands_bmic)
 */
SA_UINT8 SC_CanExecuteBMICCommand(DeviceDescriptor device_descriptor);
   /** @name Can Execute BMIC Command Returns
    * @outputof SC_CanExecuteBMICCommand
    * @{ */

   /** BMIC Command can be executed.
    * @outputof SC_CanExecuteBMICCommand.
    * @allows SC_ExecuteBMICCommand.
    */
   #define kCanExecuteBMICCommandOK                       0x01
   /** BMIC Command cannot be executed because the device does not support it.
    * @outputof SC_CanExecuteBMICCommand.
    * @disallows SC_ExecuteBMICCommand.
    */
   #define kCanExecuteBMICCommandNotSupported             0x02
   /** BMIC Command cannot be executed because the device descriptor is invalid.
    * @outputof SC_CanExecuteBMICCommand.
    * @disallows SC_ExecuteBMICCommand.
    */
   #define kCanExecuteBMICCommandInvalidDevice            0x03
   /** @} */

/** Execute BMIC command on target device.
 * @pre The target device supports BMIC requests.
 * @param[in]     device_descriptor  BMIC device handler.
 * @param[in,out] bmic_request       Initialized/Populated BMIC Request.
 * @return Command status.
 */
EnCommandStatus SC_ExecuteBMICCommand(DeviceDescriptor device_descriptor, PBMICRequest bmic_request);
/** @} */ /* syscore_commands_bmic */

/** @addtogroup syscore_commands_scsi
 * @{ */
/** Can a SCSI command be executed?
 * @post If valid, user may call @ref SC_ExecuteSCSICommand.
 * @param[in] device_descriptor  SCSI device handle.
 * @return See [Can Execute SCSI Command Returns](@ref syscore_commands_scsi)
 */
SA_UINT8 SC_CanExecuteSCSICommand(DeviceDescriptor device_descriptor);
   /** @name Can Execute SCSI Command Returns
    * @outputof SC_CanExecuteSCSICommand
    * @{ */

   /** SCSI Commands can be executed.
    * @outputof SC_CanExecuteSCSICommand.
    * @allows SC_ExecuteSCSICommand.
    */
   #define kCanExecuteSCSICommandOK                       0x01
   /** SCSI Command cannot be executed because the device does not support it.
    * @outputof SC_CanExecuteSCSICommand.
    * @disallows SC_ExecuteSCSICommand.
    */
   #define kCanExecuteSCSICommandNotSupported             0x02
   /** SCSI Command cannot be executed because the device descriptor is invalid.
    * @outputof SC_CanExecuteSCSICommand.
    * @disallows SC_ExecuteSCSICommand.
    */
   #define kCanExecuteSCSICommandInvalidDevice            0x03
   /** @} */

/** Execute SCSI command on target device.
 *
 * @pre The target device supports SCSI requests.
 * @param[in]     device_descriptor  SCSI device handler.
 * @param[in,out] scsi_request       Initialized/Populated SCSI Request.
 * @return Command status.
 */
EnCommandStatus SC_ExecuteSCSICommand(DeviceDescriptor device_descriptor, PSCSIRequest scsi_request);

/** Function type for SCSI command callback. */
typedef void(*SC_SCSICommandCallback)(const PSCSIRequest scsi_request);
/** Set/Unset the registered SCSI command callback.
 * The callback is invoke after executing a SCSI command.
 * The callback is given the SCSI request with populated
 * scsi status & sense data.
 * @attention The callback shall not edit any data of the SCSI request.
 * @param[in] callback  Address to callback function or NULL to disable callback.
 */
void SC_SetSCSICommandCallback(SC_SCSICommandCallback callback);
/** @} */ /* syscore_commands_scsi */

/** @addtogroup syscore_commands_io
 * @{ */

/** Can an IO Request be executed?
 * @post If valid, user may call @ref SC_ExecuteIORequest.
 * @param[in] device_descriptor  Device handle.
 * @return See [Can Execute IO Request Command Returns]
 */
SA_UINT8 SC_CanExecuteIORequest(DeviceDescriptor device_descriptor);
/** @name Can Execute IO Request Command Returns
   * @outputof SC_CanExecuteIORequest
   * @{ */

   /** Command can be executed */
   #define kCanExecuteIORequestOK                       0x01
   /** The command is not supported on the given handle */
   #define kCanExecuteIORequestNotSupported             0x02
   /** Invalid device descriptor. */
   #define kCanExecuteIORequestInvalidDevice            0x03
   /** @} */

/** Execute IO Request (Read or Write) on the device
 * @pre The target device supports read/write requests.
 * @param[in]     device_descriptor  Device handle.
 * @param[in,out] io_request         Initialized/Populated IO Request structure.
 * @return Command status.
 */
EnCommandStatus SC_ExecuteIORequest(DeviceDescriptor device_descriptor, PIORequest io_request);
/** @} */ /* syscore_commands_io */

/** @addtogroup syscore_commands_smp
 * @{ */
/** Can an SMP command be executed?
 * @post If valid, user may call @ref SC_ExecuteSMPCommand.
 * @param[in] device_descriptor  Device handle.
 * @return See [Can Execute SMP Request Command Returns]
 */
SA_UINT8 SC_CanExecuteSMPCommand(DeviceDescriptor device_descriptor);
/** @name Can Execute SMP Command Returns
   * @outputof SC_CanExecuteSMPCommand
   * @{ */
   /** SMP commands can be executed */
   #define kCanExecuteSMPCommandOK                       0x01
   /** SMP commands are not supported on this device */
   #define kCanExecuteSMPCommandNotSupported             0x02
   /** Invalid device descriptor. */
   #define kCanExecuteSMPCommandInvalidDevice            0x03
   /** @} */

/** Execute SMP Command on the device
 * @param[in]     device_descriptor  Device handle.
 * @param[in,out] smp_request        Initialized/Populated SMP Request structure.
 * @return Command status.
 */
EnCommandStatus SC_ExecuteSMPCommand(DeviceDescriptor device_descriptor, PSMPRequest smp_request);
/** @} */

/** @addtogroup syscore_commands_ata
 * @{ */

/** Can an ATA command be executed?
 * @post If valid, user may call @ref SC_ExecuteATACommand.
 * @param[in] device_descriptor  Device handle.
 * @return See [Can Execute ATA Request Command Returns]
 */
SA_UINT8 SC_CanExecuteATACommand(DeviceDescriptor device_descriptor);
/** @name Can Execute ATA Command Returns
   * @outputof SC_CanExecuteATACommand
   * @{ */
   /** ATA commands can be executed */
   #define kCanExecuteATACommandOK                       0x01
   /** ATA commands are not supported */
   #define kCanExecuteATACommandNotSupported             0x02
   /** Invalid device descriptor. */
   #define kCanExecuteATACommandInvalidDevice            0x03
   /** @} */

/** Execute ATA command on target device.
 * @pre The target device supports ATA requests.
 * @param[in]     device_descriptor  ATA device handler.
 * @param[in,out] ata_request        Initialized/Populated ATA Request.
 * @return Command status.
 */
EnCommandStatus SC_ExecuteATACommand(DeviceDescriptor device_descriptor, PATARequest ata_request);
/** @} */

/** @addtogroup syscore_commands_nvme
 * @{ */

/** Can an NVME command be executed?
 * @post If valid, user may call @ref SC_ExecuteNVMECommand.
 * @param[in] device_descriptor  Device handle.
 * @return See [Can Execute NVME Request Command Returns]
 */
SA_UINT8 SC_CanExecuteNVMECommand(DeviceDescriptor device_descriptor);
/** @name Can Execute NVME Command Returns
   * @outputof SC_CanExecuteNVMECommand
   * @{ */
   /** NVME commands can be executed */
   #define kCanExecuteNVMECommandOK                       0x01
   /** NVME commands are not supported */
   #define kCanExecuteNVMECommandNotSupported             0x02
   /** Invalid device descriptor. */
   #define kCanExecuteNVMECommandInvalidDevice            0x03
   /** @} */

/** Execute NVME command on target device.
 * @pre The target device supports NVME requests.
 * @param[in]     device_descriptor  NVME device handler.
 * @param[in,out] nvme_request       Initialized/Populated NVME Request.
 * @return Command status.
 */
EnCommandStatus SC_ExecuteNVMECommand(DeviceDescriptor device_descriptor, PNVMERequest nvme_request);
/** @} */ /* syscore_commands_nvme */

/** @defgroup syscore_command_reset Reset Command
 * @{ */
/** Reset controller.
* @warning Understand the risk of reseting an active controller.
* @param[in] device_descriptor  Controller raw descriptor handler.
* @param[in] reset_type         Reset type (see [Controller Reset Types](@ref syscore_command_reset)).
* @return eCOMMAND_STATUS_OK if command successful
* @return eCOMMAND_STATUS_FAILED if command unsuccessful
*/
EnCommandStatus SC_SendResetControllerCommand(DeviceDescriptor device_descriptor, SA_UINT8 reset_type);
/** @name Controller Reset Types
 * Input to @ref SC_SendResetControllerCommand.
 * @{ */
#define kResetControllerTypeIOP         0x01 /**< Driver initiated IOP reset; Input to @ref SC_SendResetControllerCommand. */
#define kResetControllerTypeDriverSoft  0x02 /**< Driver initiated soft reset; Input to @ref SC_SendResetControllerCommand. */
#define kResetControllerTypeDriverFull  0x03 /**< Driver initiated full reset; Input to @ref SC_SendResetControllerCommand. */
/** @} */
/** @} */ /* syscore_command_reset */

/** Inform the driver that a change has been made to the exposed disk(s).
 * @param[in] device_descriptor  Device handler.
 * @return Command status.
 */
EnCommandStatus SC_ExecuteRegNewDiskMessage(DeviceDescriptor device_descriptor);

/** @} */ /* syscore_commands */

#if defined(__cplusplus)
}    /* extern "C" { */
#endif

/** @} */ /* syscore */

#endif /* PSYSC_COMMANDS_H */

