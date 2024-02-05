/** @file psysc_descriptor.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore descriptor header file

*/

#ifndef PSYSC_DESCRIPTOR_H
#define PSYSC_DESCRIPTOR_H

#include "psysc_types.h"

/** @addtogroup syscore
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/** @defgroup syscore_descriptor Descriptor
 * @brief
 * @details
 *
 * sysCore descriptor is of the following format:
 * [Total Length]
 * [Entry Id][Entry Length][Entry Value]
 *
 * @{
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS
 /* The types for each descriptor's id and size fields. Usually you will never
 * need to modify this unless your platform has issues with 2 byte alignment
 * access. If so, change these to your target platform's alignment size (i.e.
 * for 4 byte alignment, change to SA_DWORD.
 */
#if defined(SYSCORE_RAIDSTACK)
#define DESCRIPTOR_ALIGNMENT_TYPE SA_BYTE
#else
#define DESCRIPTOR_ALIGNMENT_TYPE SA_WORD
#endif
#endif

/** Data type of the descriptor's entry length and total length. */
#define DESCRIPTOR_LENGTH_TYPE       DESCRIPTOR_ALIGNMENT_TYPE
/** Data type of descriptor's entry id. */
#define DESCRIPTOR_ID_TYPE           SA_BYTE

/** Get the total length of a descriptor. */
#define DESCRIPTOR_TOTAL_LENGTH(descriptor_base) \
   (*((SC_UNALIGNED DESCRIPTOR_LENGTH_TYPE*)(descriptor_base)))

/** Get the first descriptor entry. */
#define FIRST_DESCRIPTOR_ENTRY(descriptor_base) \
   (void *)(((DESCRIPTOR_LENGTH_TYPE*)(descriptor_base) + 1))

/** Get the descriptor's entry size for the requested @p entry_size.
 * This includes size of entry's id, length and @p entry_size.
 */
#define DESCRIPTOR_FIELD_SIZE(entry_size) \
   (sizeof(DESCRIPTOR_ID_TYPE) + sizeof(DESCRIPTOR_LENGTH_TYPE) + entry_size)

/** Set an entry in the descriptor.
 * @param[in] descriptor_entry      Pointer to location of descriptor's entry.
 * @param[in] descriptor_entry_type Entry Id of type @ref DESCRIPTOR_ID_TYPE.
 * @param[in] descriptor_entry_size Size in bytes of this descriptor entry.
 * @param[in] descriptor_value_ptr  Pointer from where the entry value will be copied.
 */
#define SET_DESCRIPTOR_FIELD_ENTRY(descriptor_entry,descriptor_entry_type,descriptor_entry_size,descriptor_value_ptr) \
   *((SC_UNALIGNED DESCRIPTOR_ID_TYPE*)(descriptor_entry)) = (DESCRIPTOR_ID_TYPE)descriptor_entry_type; \
   *((SC_UNALIGNED DESCRIPTOR_LENGTH_TYPE*)(((SA_BYTE *)(descriptor_entry)) + sizeof(DESCRIPTOR_ID_TYPE))) = \
      (DESCRIPTOR_LENGTH_TYPE)descriptor_entry_size; \
   SA_memcpy_s(((SA_BYTE*)descriptor_entry) + sizeof(DESCRIPTOR_ID_TYPE) + sizeof(DESCRIPTOR_LENGTH_TYPE),\
       (DESCRIPTOR_LENGTH_TYPE)descriptor_entry_size,\
       descriptor_value_ptr,\
       (DESCRIPTOR_LENGTH_TYPE)descriptor_entry_size)

/** Get descriptor entry's id. */
#define DESCRIPTOR_FIELD_ID(descriptor_entry) \
   (*((SC_UNALIGNED DESCRIPTOR_ID_TYPE*)descriptor_entry))
/** Get descriptor entry's length. */
#define DESCRIPTOR_FIELD_LENGTH(descriptor_entry) \
   (*((SC_UNALIGNED DESCRIPTOR_LENGTH_TYPE*)(((SA_BYTE *)(descriptor_entry)) + sizeof(DESCRIPTOR_ID_TYPE))))
/** Get descriptor entry's value. */
#define DESCRIPTOR_FIELD_VALUE(descriptor_entry) \
   ((void*)(((SA_BYTE*)descriptor_entry) + sizeof(DESCRIPTOR_ID_TYPE) + sizeof(DESCRIPTOR_LENGTH_TYPE)))

/** Move to the next descriptor entry's address. */
#define NEXT_DESCRIPTOR_ENTRY(descriptor_entry) \
   (((SA_BYTE *)descriptor_entry) + sizeof(DESCRIPTOR_ID_TYPE) + sizeof(DESCRIPTOR_LENGTH_TYPE) + DESCRIPTOR_FIELD_LENGTH(descriptor_entry))

/** Macro to make sure @p descriptor_entry is a valid descriptor entry in
 * descriptor pointed by @p descriptor_base. */
#define DESCRIPTOR_ENTRY_VALID(descriptor_base,descriptor_entry) \
   ((SA_BYTE *)descriptor_entry < (((SA_BYTE *)descriptor_base) + DESCRIPTOR_TOTAL_LENGTH(descriptor_base)))

/** @} */ /* syscore_descriptor */

#if defined(__cplusplus)
}
#endif

/** @} */ /* syscore */

#endif /* PSYSC_DESCRIPTOR_H */
