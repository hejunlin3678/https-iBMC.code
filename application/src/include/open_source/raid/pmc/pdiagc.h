
/** @file pdiagc.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.
*/

#ifndef PDIAGC_H
#define PDIAGC_H

#include "pstorc.h"
#include "pdiagc_types.h"

/** @defgroup diagcore diagnosticCore
 * @details
 * ### Examples
 * - @ref storc_diagcore_examples
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/** @page diagpage diagnosticCore
 */

/** Initialize global objects of @ref diagcore.
 * @attention Clients should invoke this before using any of the @ref diagcore module API.
 * @deprecated Parameter `checksum` is not longer used.
 */
void InitializeDiagCore(const char *checksum);

/** Uninitialize global objects of @ref diagcore.
 * @attention Clients can no longer invoke any of the @ref diagcore module API after this call.
 */
void UnInitializeDiagCore(void);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef void* PDIAG_REPORT;
#endif

/** @name Diagnostic Report Create/Delete
 *
 * @code{.c}
 *
 * // Define callbacks...
 *
 * PDIAG_REPORT pdr = DC_CreateDiagnosticReport(cda);
 *
 * // Setup callbacks
 * DC_SetMetaDeviceStructuresHandler(pdr, _DC_StartDevices, _DC_EndDevices);
 * DC_SetMetaDeviceStructureHandler(pdr, _DC_StartDevice, _DC_EndDevice);
 * DC_SetMetaMessageStructuresHandler(pdr, _DC_StartMessages, _DC_EndMessages);
 * DC_SetMetaMessageStructureHandler(pdr, _DC_StartMessage, _DC_EndMessage);
 * DC_SetMetaStructuresHandler(pdr, _DC_StartBuffers, _DC_EndBuffers);
 * DC_SetMetaStructureHandler(pdr, _DC_StartBuffer, _DC_EndBuffer);
 * DC_SetMetaPropertyHandler(pdr, _DC_StartData, _DC_EndData);
 * DC_SetMetaPropertyStructureHandler(pdr, _DC_StartTable, _DC_EndTable);
 * DC_SetMetaPropertyStructurePropertiesHandler(pdr, _DC_StartTableRow, _DC_EndTableRow);
 * DC_SetMetaPropertyStructurePropertyHandler(pdr, _DC_StartTableData, _DC_EndTableData);
 *
 * // Generate report by invoking callbacks
 * DC_GenerateDiagnosticReport(pdr);
 *
 * // Cleanup
 * DC_DestroyDiagnosticReport(pdr);
 * @endcode
 *
 * @{ */
/** Create/initialize a diagnostic reporter.
 * @param[in] cda Controller data area.
 * @return Address to a new diagnostic report.
 * @post To populate report, set callbacks ([Diagnostic Callbacks (Event Handlers)](@ref diagcore)) and call @ref DC_GenerateDiagnosticReport.
 * @post User should use @ref DC_DestroyDiagnosticReport to deallocate diagnostic report when finished.
 */
PDIAG_REPORT DC_CreateDiagnosticReport(PCDA cda);
/** Clean/deallocate report previously created with @ref DC_CreateDiagnosticReport.
 * @param[in] pdr Address to initialized diagnostic report.
 */
void DC_DestroyDiagnosticReport(PDIAG_REPORT pdr);
/** Populate diagnostic report.
 * @pre Diagnostic report has been initialized from @ref DC_CreateDiagnosticReport.
 * @pre User has set [Diagnostic Callbacks (Event Handlers)](@ref diagcore) before calling.
 * @param[in] pdr Address to initialized diagnostic report.
 */
void DC_GenerateDiagnosticReport(PDIAG_REPORT pdr);
/** @} */ /* Diagnostic Report Create/Delete */

/** @name Diagnostic Callbacks (Event Handlers)
 *
 * Overall Callback Sequence:
 * @code{.unparsed}
 * @ref DC_SetMetaDeviceStructuresHandler
 *   This device
 *   @ref DC_SetMetaDeviceStructureHandler
 *     Messages
 *     @ref DC_SetMetaMessageStructuresHandler
 *       Message
 *       @ref DC_SetMetaMessageStructureHandler
 *       More messages...
 *     Buffers
 *     @ref DC_SetMetaStructuresHandler
 *       Buffer
 *       @ref DC_SetMetaStructureHandler
 *         Field
 *         @ref DC_SetMetaPropertyHandler
 *         ...
 *         Field with sub-structure/table
 *         @ref DC_SetMetaPropertyStructureHandler
 *           Table row
 *           @ref DC_SetMetaPropertyStructurePropertiesHandler
 *             Table row+column/cell
 *             @ref DC_SetMetaPropertyStructurePropertyHandler
 *             More columns...
 *           More rows...
 *         More fields...
 *       More buffers...
 *
 *     Repeated for all child devices
 *     @ref DC_SetMetaDeviceStructuresHandler
 *       Repeat...
 * @endcode
 * @{
 */

/** Set start/end callbacks for diagnostic report for a list of devices.
 * @see MetaStructuresHandler.
 * @param[in] pdr Address to initialized diagnostic report.
 * @param[in] start_handler Handler to call at the start of a list of devices (can be set to NULL).
 * @param[in] end_handler Handler to call at the end of a list of devices (can be set to NULL).
 *
 * __Example__
 * @code{.c}
 * void _DC_StartDevices(PCDA cda)
 * {
 *    printf("<Devices>");
 * }
 * void _DC_EndDevices(PCDA cda)
 * {
 *    printf("</Devices>");
 * }
 * DC_SetMetaDeviceStructuresHandler(pdr, _DC_StartDevices, _DC_EndDevices);
 * DC_GenerateDiagnosticReport(pdr);
 * // Outputs:
 * // <Devices></Devices>
 * @endcode
 */
void DC_SetMetaDeviceStructuresHandler(PDIAG_REPORT pdr, MetaStructuresHandler start_handler, MetaStructuresHandler end_handler);

/** Set start/end callbacks for diagnostic report for a device.
 * @see MetaDeviceStructureHandler.
 * @see _DIAG_META_DEVICE.
 * @param[in] pdr Address to initialized diagnostic report.
 * @param[in] start_handler Handler to call at the start of a device (can be set to NULL).
 * @param[in] end_handler Handler to call at the end of a device (can be set to NULL).
 *
 * __Example__
 * @code{.c}
 * void _DC_StartDevice(PCDA cda, PDIAG_META_DEVICE device)
 * {
 *    if (device->device_type == kDiagMetaDeviceTypeController)
 *    {
 *       char device_id[32];
 *       char slot_buffer[9];
 *       char family_id[33];
 *       char product_id[17];
 *       char vendor_id[9];
 *       SA_GetControllerSlotNumber(cda, slot_buffer, sizeof(slot_buffer));
 *       SA_GetControllerFamilyID(cda, family_id, sizeof(family_id));
 *       SA_GetControllerProductID(cda, product_id, sizeof(product_id));
 *       SA_GetControllerVendorID(cda, vendor_id, sizeof(vendor_id));
 *       SA_sprintf_s(device_id, sizeof(device_id), "Controller/%s", (const char*)slot_buffer);
 *       printf("%*s<Device deviceType=\"ArrayController\" id=\"%s\" marketingName=\"%s %s %s in Slot %s\">",
 *          device_id, vendor_id, family_id, slot_buffer);
 *    }
 *    //...
 * }
 * void _DC_EndDevice(PCDA cda, PDIAG_META_DEVICE device)
 * {
 *    printf("</Device>");
 * }
 * DC_SetMetaDeviceStructureHandler(pdr, _DC_StartDevice, _DC_EndDevice);
 * DC_GenerateDiagnosticReport(pdr);
 * // Outputs:
 * // <Device deviceType="ArrayController" id="Controller/#" marketingName="MSCC SmartRAID 2100-8i8e in Slot #"></Device>
 * @endcode
 */
void DC_SetMetaDeviceStructureHandler(PDIAG_REPORT pdr, MetaDeviceStructureHandler start_handler, MetaDeviceStructureHandler end_handler);

/** Set start/end callbacks for diagnostic report for a list of messages.
 * @see MetaStructuresHandler.
 * @param[in] pdr Address to initialized diagnostic report.
 * @param[in] start_handler Handler to call at the start of a list of messages (can be set to NULL).
 * @param[in] end_handler Handler to call at the end of a list of messages (can be set to NULL).
 *
 * __Example__
 * @code{.c}
 * void _DC_StartMessages(PCDA cda)
 * {
 *    printf("<Errors>\n");
 * }
 * void _DC_EndMessages(PCDA cda)
 * {
 *    printf("</Errors>\n");
 * }
 * DC_SetMetaMessageStructuresHandler(pdr, _DC_StartMessages, _DC_EndMessages);
 * DC_GenerateDiagnosticReport(pdr);
 * // Outputs:
 * // <Errors></Errors>
 * @endcode
 */
void DC_SetMetaMessageStructuresHandler(PDIAG_REPORT pdr, MetaStructuresHandler start_handler, MetaStructuresHandler end_handler);

/** Set start/end callbacks for diagnostic report for a message.
 * @see MetaMessageStructureHandler.
 * @see _DIAG_META_MESSAGE.
 * @param[in] pdr Address to initialized diagnostic report.
 * @param[in] start_handler Handler to call at the start of a message (can be set to NULL).
 * @param[in] end_handler Handler to call at the end of a message (can be set to NULL).
 *
 * __Example__
 * @code{.c}
 * void _DC_StartMessage(PCDA cda, PDIAG_META_DEVICE device, PDIAG_META_MESSAGE message)
 * {
 *    if (message->message_type == kDiagMetaMessageTypeInformational)
 *    {
 *       printf("<Message severity=\"Informational\" message=\"%s\">",
 *          message->message);
 *    }
 *    //...
 * }
 * void _DC_StartMessage(PCDA cda, PDIAG_META_DEVICE device, PDIAG_META_MESSAGE message)
 * {
 *    printf("</Message>");
 * }
 * DC_SetMetaMessageStructureHandler(pdr, _DC_StartMessage, _DC_EndMessage);
 * DC_GenerateDiagnosticReport(pdr);
 * // Outputs:
 * // <Message severty="Informational" message="Message..."></Message>
 * @endcode
 */
void DC_SetMetaMessageStructureHandler(PDIAG_REPORT pdr, MetaMessageStructureHandler start_handler, MetaMessageStructureHandler end_handler);

/** Set start/end callbacks for diagnostic report for a list of buffers.
 * @see MetaStructuresHandler.
 * @param[in] pdr Address to initialized diagnostic report.
 * @param[in] start_handler Handler to call at the start of a list of buffers (can be set to NULL).
 * @param[in] end_handler Handler to call at the end of a list of buffers (can be set to NULL).
 *
 * __Example__
 * @code{.c}
 * void _DC_StartBuffers(PCDA cda)
 * {
 *    printf("<MetaStructures>\n");
 * }
 * void _DC_EndBuffers(PCDA cda)
 * {
 *    printf("</MetaStructures>\n");
 * }
 * DC_SetMetaStructuresHandler(pdr, _DC_StartBuffers, _DC_EndBuffers);
 * DC_GenerateDiagnosticReport(pdr);
 * // Outputs:
 * // <MetaStructures></MetaStructures>
 * @endcode
 */
void DC_SetMetaStructuresHandler(PDIAG_REPORT pdr, MetaStructuresHandler start_handler, MetaStructuresHandler end_handler);

/** Set start/end callbacks for diagnostic report for buffer payload.
 * @see MetaStructureHandler.
 * These callbacks are called before and after each buffer payload.
 * @param[in] pdr Address to initialized diagnostic report.
 * @param[in] start_handler Handler to call at the start of a command buffer (can be set to NULL).
 * @param[in] end_handler Handler to call at the end of a command buffer (can be set to NULL).
 *
 * __Example__
 * @code{.c}
 * void _DC_StartBuffer(PCDA cda, const char *title, size_t size)
 * {
 *    printf("<MetaStructure id=\"%s\" size=\"%llu\">",
 *       title, (unsigned long long)size);
 * }
 * void _DC_EndBuffer(PCDA cda, const char *title, size_t size)
 * {
 *    printf("</MetaStructure>");
 * }
 * DC_SetMetaStructureHandler(pdr, _DC_StartBuffer, _DC_EndBuffer);
 * DC_GenerateDiagnosticReport(pdr);
 * // Outputs:
 * // <MetaStructure id="IDENTIFY_CONTROLLER" size="1024"></MetaStructure>
 * @endcode
 */
void DC_SetMetaStructureHandler(PDIAG_REPORT pdr, MetaStructureHandler start_handler, MetaStructureHandler end_handler);

/** Set callback for buffer element.
 * @see MetaPropertyHandler.
 * @see _DIAG_META_DATA.
 * This callback is called for each payload buffer integer/string/buffer/raw element.
 * This callback means the meta data does not have any sub-structures or tables.
 * Should be handled the same way as callbacks from DC_SetMetaPropertyStructurePropertyHandler except this
 * meta-data is for a buffer field instead of a table cell.
 * @param[in] pdr Address to initialized diagnostic report.
 * @param[in] start_handler Handler to call when starting a command buffer data (can be set to NULL).
 * @param[in] end_handler Handler to call when ending a command buffer data (can be set to NULL).
 *
 * __Example__
 * @code{.c}
 * // Copy an n-char Array to a newly allocated [null-terminated] C-string
 * // and escape all needed XML symbols.
 * char * _DC_NewXmlString(const char * string, size_t n)
 *
 * void _DC_StartProperty(PCDA cda, DIAG_META_DATA meta_data)
 * {
 *    char * xml_title = _DC_NewXmlString(meta_data->data_name, SA_strnlen_s(meta_data->data_name, MAX_DATA_NAME_SIZE));
 *    char * xml_desc = meta_data->data_description ? _DC_NewXmlString(meta_data->data_description, SA_strnlen_s(meta_data->data_description, MAX_DATA_DESCRIPTION_SIZE)) : NULL;
 *    if (meta_data->data_type == kDiagMetaDataTypeString)
 *    {
 *       char * xml_value = _DC_NewXmlString(meta_data->data_string_value, meta_data->data_size_bytes);
 *       printf(
 *          "<MetaProperty id=\"%s\" type=\"char []\" value=\"%s\" size=\"%llu\" description=\"%s\">",
 *          xml_title,
 *          xml_value,
 *          (unsigned long long)meta_data->data_size_bytes,
 *          xml_desc ? xml_desc : ""
 *       );
 *       SA_free(xml_value);
 *    }
 *    // else if ...
 *    SA_free(xml_title);
 *    SA_free(xml_desc);
 * }
 * void _DC_EndProperty(PCDA cda, const char *title, size_t size)
 * {
 *    printf("</MetaProperty>");
 * }
 * DC_SetMetaPropertyHandler(pdr, _DC_StartProperty, _DC_EndProperty);
 * DC_GenerateDiagnosticReport(pdr);
 * // Outputs:
 * // <MetaProperty id="ROM Firmware Revision" type="char []" value="1.01" size="4" description=""></MetaProperty>
 * @endcode
 */
void DC_SetMetaPropertyHandler(PDIAG_REPORT pdr, MetaPropertyHandler start_handler, MetaPropertyHandler end_handler);

/** Set callback for buffer element.
 * @see MetaPropertyHandler.
 * @see _DIAG_META_DATA.
 * This callback is called for each payload buffer integer/string/buffer/raw element.
 * This callback means the meta data may have sub-structures or tables following.
 * @param[in] pdr Address to initialized diagnostic report.
 * @param[in] start_handler Handler to call when starting a command buffer data (can be set to NULL).
 * @param[in] end_handler Handler to call when ending a command buffer data (can be set to NULL).
 *
 * __Example__
 * @code{.c}
 * // Copy an n-char Array to a newly allocated [null-terminated] C-string
 * // and escape all needed XML symbols.
 * char * _DC_NewXmlString(const char * string, size_t n)
 *
 * void _DC_StartPropertyWithStruct(PCDA cda, DIAG_META_DATA meta_data)
 * {
 *    char * xml_title = _DC_NewXmlString(meta_data->data_name, SA_strnlen_s(meta_data->data_name, MAX_DATA_NAME_SIZE));
 *    char * xml_desc = meta_data->data_description ? _DC_NewXmlString(meta_data->data_description, SA_strnlen_s(meta_data->data_description, MAX_DATA_DESCRIPTION_SIZE)) : NULL;
 *    if (meta_data->data_type == kDiagMetaDataTypeInteger)
 *    {
 *       printf(
 *          "<MetaPropertyStructure id=\"%s\" type=\"%s\" metaValue=\"%llu\" value=\"0x%0*llx\" size=\"%d\" description=\"%s\">\n",
 *          xml_title,
 *          meta_data->data_size_bytes == 1 ? "BYTE"
 *          : meta_data->data_size_bytes == 2 ? "WORD"
 *          : meta_data->data_size_bytes == 4 ? "DWORD"
 *          : meta_data->data_size_bytes == 8 ? "QWORD"
 *          : "UNKNOWN",
 *          (unsigned long long)meta_data->data_integer_value,
 *          2*meta_data->data_size_bytes, (unsigned long long)meta_data->data_integer_value,
 *          meta_data->data_size_bytes,
 *          xml_desc ? xml_desc : ""
 *          );
 *    }
 *    // else if ...
 *    SA_free(xml_title);
 *    SA_free(xml_desc);
 * }
 * void _DC_EndPropertyWithStuct(PCDA cda, const char *title, size_t size)
 * {
 *    printf("</MetaPropertyStructure>");
 * }
 * DC_SetMetaPropertyHandler(pdr, _DC_StartPropertyWithStruct, _DC_EndPropertyWithStuct);
 * DC_GenerateDiagnosticReport(pdr);
 * // Outputs:
 * // <MetaPropertyStructure id="Swapped Cable Flags" type="integer" metaValue="16" value="0x10" size="1" description="">
 * //   There would be <MetaStructure>'s and <MetaProperty>'s here creating
 * //   a table with additional information on this data
 * //   from DC_SetMetaPropertyStructurePropertiesHandler
 * //   and DC_SetMetaPropertyStructurePropertyHandler
 * // </MetaPropertyStructure>
 * @endcode
 */
void DC_SetMetaPropertyStructureHandler(PDIAG_REPORT pdr, MetaPropertyHandler start_handler, MetaPropertyHandler end_handler);

/** Set start/end callbacks for diagnostic report for a list of sub meta-structures on a property.
 * @see MetaStructuresHandler.
 * This of this as starting the next row of data for an item from @ref DC_SetMetaPropertyStructureHandler.
 * @param[in] pdr Address to initialized diagnostic report.
 * @param[in] start_handler Handler to call at the start of a set property columns (can be set to NULL).
 * @param[in] end_handler Handler to call at the end of a set property columns (can be set to NULL).
 *
 * __Example__
 * @code{.c}
 * void _DC_StartMetaStructureRow(PCDA cda)
 * {
 *    printf("<MetaStructure>\n");
 * }
 * void _DC_EndMetaStructureRow(PCDA cda)
 * {
 *    printf("</MetaStructure>\n");
 * }
 * DC_SetMetaStructuresHandler(pdr, _DC_StartMetaStructureRow, _DC_EndMetaStructureRow);
 * DC_GenerateDiagnosticReport(pdr);
 * // Outputs:
 * // <MetaStructure>
 * //   Properties from DC_SetMetaPropertyStructurePropertyHandler to follow.
 * // </MetaStructure>
 * @endcode
 */
void DC_SetMetaPropertyStructurePropertiesHandler(PDIAG_REPORT pdr, MetaStructuresHandler start_handler, MetaStructuresHandler end_handler);

/** Set start/end callbacks for diagnostic report for a list of sub meta-structures on a property.
 * @see MetaPropertyHandler.
 * @see _DIAG_META_DATA.
 * This callback is called for each payload buffer integer/string/buffer/raw table column.
 * Should be handled the same way as callbacks from DC_SetMetaPropertyHandler except this
 * meta-data is not for a buffer field but instead for a table cell.
 * @param[in] pdr Address to initialized diagnostic report.
 * @param[in] start_handler Handler to call when starting a command buffer data (can be set to NULL).
 * @param[in] end_handler Handler to call when ending a command buffer data (can be set to NULL).
 *
 */
void DC_SetMetaPropertyStructurePropertyHandler(PDIAG_REPORT pdr, MetaPropertyHandler start_handler, MetaPropertyHandler end_handler);

/** @} */ /* Diagnostic Callbacks (Event Handlers) */

#if defined(__cplusplus)
}
#endif

/** @} */ /* diagcore */

#endif /* PDIAGC_H */
