
/** @file pdiagc_types.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.
*/

#ifndef PDIAGC_TYPES_H
#define PDIAGC_TYPES_H

/************************//**
 * @addtogroup diagcore
 * @{
 ***************************/

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__FREEBSD12)
#include "psysc_types.h"
#endif

/** @name Diagnostic Callbacks/Handler Types.
 * @{ */

/** Meta data for a diagnostic data entry.
 *
 * Used in [Diagnostic Callbacks (Event Handlers)](@ref diagcore).
 */
struct _DIAG_META_DATA
{
   /** Name/title of the data as a C-string (NULL-terminated). */
   const char * data_name;
#define MAX_DATA_NAME_SIZE 65 /**< Maximum number of bytes in @ref DIAG_META_DATA.data_name. */

   /** Description of the data as a C-string (NULL-terminated) or can be NULL. */
   const char * data_description;
#define MAX_DATA_DESCRIPTION_SIZE 65 /**< Maximum number of bytes in @ref DIAG_META_DATA.data_description. */

   /** Enum of data type. */
   SA_BYTE data_type;
   /** Size of data in bytes.
    * For @ref DIAG_META_DATA.data_type @ref kDiagMetaDataTypeInteger or @ref kDiagMetaDataTypeRaw this
    * is the size of the diagnostic data object (i.e. byte->1, int16->2, etc...)
    * For @ref DIAG_META_DATA.data_type @ref kDiagMetaDataTypeString this field is the length of the
    * string (not counting any terminating NULL).
    */
   size_t data_size_bytes;
   /** Data type of @ref DIAG_META_DATA.data_type for integers. */
#define kDiagMetaDataTypeInteger 0
   /** Value of a integer data object (regardless of size).
    * @pre Only valid of `data_type` is @ref kDiagMetaDataTypeInteger.
    * @pre Use @ref DIAG_META_DATA.data_size_bytes to get integer size in bytes.
    */
   SA_QWORD data_integer_value;

   /** Data type of @ref DIAG_META_DATA.data_type for strings. */
#define kDiagMetaDataTypeString  1
   /** Value of data which should be interpreted as a string of ASCII characters.
    * @warning This field is *NOT* null-terminated.
    * @pre Only valid if @ref DIAG_META_DATA.data_type is @ref kDiagMetaDataTypeString.
    * @pre Use @ref DIAG_META_DATA.data_size_bytes to get string length.
    */
   const char * data_string_value;

   /** Data type of @ref DIAG_META_DATA.data_type for raw [hex] buffers. */
#define kDiagMetaDataTypeRaw     2
   /** Value of data which should be interpreted as set of bytes or hex characters.
    * @pre Only valid if @ref DIAG_META_DATA.data_type is @ref kDiagMetaDataTypeRaw.
    * @pre Use @ref DIAG_META_DATA.data_size_bytes to get string length.
    */
   const void * data_buffer_value;
   /** Size (in bytes) of each element in @ref DIAG_META_DATA.data_buffer_value.
    * @pre Only valid if @ref DIAG_META_DATA.data_type is @ref kDiagMetaDataTypeRaw.
    * @pre Use @ref DIAG_META_DATA.data_size_bytes to get total buffer size in bytes.
    */
   int data_buffer_element_size;
   /** Number of @ref data_buffer_element_size size elements in @ref _DIAG_META_DATA::data_buffer_value.
    * @pre Only valid if @ref DIAG_META_DATA.data_type is @ref kDiagMetaDataTypeRaw.
    * @pre Use @ref DIAG_META_DATA.data_size_bytes to get total buffer size in bytes.
    */
   size_t data_buffer_count;
};
typedef struct _DIAG_META_DATA DIAG_META_DATA;
typedef struct _DIAG_META_DATA* PDIAG_META_DATA;

/** Meta data for a physical/logical device used in [Diagnostic Callbacks (Event Handlers)](@ref diagcore).
 */
struct _DIAG_META_DEVICE
{
   /** Enum for device type. */
   SA_BYTE device_type;
#define kDiagMetaDeviceTypeController      1 /**< Enum for @ref DIAG_META_DEVICE.device_type for a Controller. */
#define kDiagMetaDeviceTypeArray           2 /**< Enum for @ref DIAG_META_DEVICE.device_type for a drive Array. */
#define kDiagMetaDeviceTypeLogicalDrive    3 /**< Enum for @ref DIAG_META_DEVICE.device_type for a logical drive. */
#define kDiagMetaDeviceTypeEnclosure       4 /**< Enum for @ref DIAG_META_DEVICE.device_type for a storage enclosure. */
#define kDiagMetaDeviceTypePhysicalDrive   5 /**< Enum for @ref DIAG_META_DEVICE.device_type for a physical drive. */
#define kDiagMetaDeviceTypeSEP             6 /**< Enum for @ref DIAG_META_DEVICE.device_type for a SEP. */
#define kDiagMetaDeviceTypeExpander        7 /**< Enum for @ref DIAG_META_DEVICE.device_type for an expander. */
#define kDiagMetaDeviceTypeRemoteVolume    8 /**< Enum for @ref DIAG_META_DEVICE.device_type for a remote volume. */
   /** Device's number used by storageCore API.
    * @pre Only valid if @ref device_type is one of:
    *      @ref kDiagMetaDeviceTypeArray
    *      @ref kDiagMetaDeviceTypeLogicalDrive
    *      @ref kDiagMetaDeviceTypeEnclosure
    *      @ref kDiagMetaDeviceTypePhysicalDrive.
    *      @ref kDiagMetaDeviceTypeSEP.
    *      @ref kDiagMetaDeviceTypeExpander.
    *      @ref kDiagMetaDeviceTypeRemoteVolume.
    */
   SA_WORD device_number;
};
typedef struct _DIAG_META_DEVICE DIAG_META_DEVICE;
typedef struct _DIAG_META_DEVICE *PDIAG_META_DEVICE;

/** Meta data for a message used in [Diagnostic Callbacks (Event Handlers)](@ref diagcore).
 */
struct _DIAG_META_MESSAGE
{
   /** Enum for message type or severity. */
   SA_BYTE message_type;
#define kDiagMetaMessageTypeInformational  1 /**< Enum for @ref DIAG_META_MESSAGE.message_type for an informational message. */
#define kDiagMetaMessageTypeWarning        2 /**< Enum for @ref DIAG_META_MESSAGE.message_type for a warning message. */
#define kDiagMetaMessageTypeError          3 /**< Enum for @ref DIAG_META_MESSAGE.message_type for an error message. */
#define kDiagMetaMessageTypeCritical       4 /**< Enum for @ref DIAG_META_MESSAGE.message_type for a critical error message. */
   const char * message; /**< NULL-terminated ASCII string message. */
};
typedef struct _DIAG_META_MESSAGE DIAG_META_MESSAGE;
typedef struct _DIAG_META_MESSAGE *PDIAG_META_MESSAGE;

/** Type for a call back for start/end of a meta-list used by [Diagnostic Callbacks (Event Handlers)](@ref diagcore).
 * @see DC_SetMetaDeviceStructuresHandler.
 * @see DC_SetMetaMessageStructuresHandler.
 * @see DC_SetMetaStructuresHandler.
 * @see DC_SetMetaPropertyStructurePropertiesHandler.
 * @param[out] cda Controller handler.
 */
typedef void(*MetaStructuresHandler)(PCDA cda);

/** Type for a call back for start/end of a message used by [Diagnostic Callbacks (Event Handlers)](@ref diagcore).
 * @see DC_SetMetaMessageStructureHandler.
 * @param[out] cda     Controller handler.
 * @param[out] device  Device meta data.
 * @param[out] message Message meta data.
 */
typedef void(*MetaMessageStructureHandler)(PCDA cda, const PDIAG_META_DEVICE device, const PDIAG_META_MESSAGE message);

/** Type for a call back for start/end of a device used by [Diagnostic Callbacks (Event Handlers)](@ref diagcore).
 * @see DC_SetMetaDeviceStructureHandler.
 * @param[out] cda Controller handler.
 * @param[out] device  Device meta data.
 */
typedef void(*MetaDeviceStructureHandler)(PCDA cda, const PDIAG_META_DEVICE device);

/** Type for a call back for a named/sized meta-structure used by [Diagnostic Callbacks (Event Handlers)](@ref diagcore).
 * @see DC_SetMetaStructureHandler.
 * @param[out] cda   Controller handler.
 * @param[out] id    Structure title/id/name.
 * @param[out] size  Structure size in bytes.
 */
typedef void(*MetaStructureHandler)(PCDA cda, const char *id, size_t size);

/** Type for a call back for a meta-data object used by [Diagnostic Callbacks (Event Handlers)](@ref diagcore).
 * @see DC_SetMetaPropertyHandler.
 * @see DC_SetMetaPropertyStructureHandler.
 * @see DC_SetMetaPropertyStructurePropertyHandler.
 * @param[out] cda        Controller handler.
 * @param[out] meta_data  Field type/value/description informaton.
 */
typedef void(*MetaPropertyHandler)(PCDA cda, const PDIAG_META_DATA meta_data);

/** @} */ /* Diagnostic Callbacks/Handler Types */

#if defined(__cplusplus)
}
#endif

/** @} */ /* diagcore */

#endif /* PDIAGC_TYPES_H */
