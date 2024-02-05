/** @file pstorc_cda.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: storageCore CDA header file
   Defines structures and functions for manipulating a controller data area (CDA)

*/

#ifndef PSTORC_CDA_H
#define PSTORC_CDA_H

#include "psysc_types.h"

/************************//**
 * @addtogroup storcore
 * @{
 ***************************/

/****************************************//**
 * @defgroup storc_cda CDA
 * @brief
 * @details
 * Create a list of controller data area structures from a list of [device handlers](@ref syscore_device_handler).
 * @code
 * for (handle_index = 0; handle_index < num_handles; handle_index++)
 * {
 *     PCDA test_CDA = (PCDA)SA_malloc(SA_GetCDASize());
 *     if (SA_InitializeCDA(test_CDA, controller_handles[handle_index]) == kInitializeSucceeded)
 *     {
 *         test_CDAs[num_good_handles] = test_CDA;
 *         num_good_handles++;
 *     }
 *     else
 *     {
 *         SA_free(test_CDA);
 *     }
 * }
 * @endcode
 * @{
 *******************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/** Obtain the size of the CDA structure.
 * @return Size of CDA in bytes.
 * @code
 * PCDA cda = (PCDA)SA_malloc(SA_GetCDASize());
 * SA_InitializeCDA(cda, device_descriptor);
 * @endcode
 */
size_t SA_GetCDASize(void);

/** Initialize CDA.
 * @pre `cda` has not yet been initialized.
 * @post CDA can only be reinitialized using @ref SA_ReInitializeCDA.
 * @param[in] cda                Uninitialized controller data area structure.
 * @param[in] controller_handle  @ref syscore_device_handler.
 * @return See [Initialize CDA Returns](@ref storc_cda).
 * @code
 * PCDA cda = (PCDA)SA_malloc(SA_GetCDASize());
 * SA_InitializeCDA(cda, device_descriptor);
 * @endcode
 */
SA_WORD SA_InitializeCDA(PCDA cda, DeviceDescriptor controller_handle);
   /** @name Initialize CDA Returns
    * @outputof SA_InitializeCDA.
    * @{ */
   #define kInitializeSucceeded 0x01 /**< Successfully initialized CDA; @outputof SA_InitializeCDA. */
   #define kInitializeFailed    0x02 /**< Failed to initialize CDA; @outputof SA_InitializeCDA. */

   #define kInitializeFailedInvalidArgument         0x04 /**< Failed to initialize CDA due invalid input; @outputof SA_InitializeCDA. */
   #define kInitializeFailedCommunicationError      0x10 /**< Failed to initialize CDA due to a communication error; @outputof SA_InitializeCDA. */
   #define kInitializeFailedControllerNotSupported  0x20 /**< Failed to initialize CDA for an unsupported controller; @outputof SA_InitializeCDA. */
   #define kInitializeFailedFailedToInitializeLocks 0x40 /**< Failed to initialize internal locks; @outputof SA_InitializeCDA. */
   /** @} */

/** ReInitialize an existing CDA.
 * @pre `cda` has already been initialized using @ref SA_InitializeCDA.
 * @param[in] cda  Previously initialized controller data area.
 * @return Same as @ref SA_InitializeCDA
 * @code
 * PCDA cda = (PCDA)SA_malloc(SA_GetCDASize());
 * SA_InitializeCDA(cda, device_descriptor);
 * SA_ReInitializeCDA(cda);
 * @endcode
 * @details
 * Reinitialize a list of controller data area structures from a list of [device handlers](@ref syscore_device_handler).
 * This operation requires that @ref SA_InitializeCDA has already been performed successfully.
 */
SA_WORD SA_ReInitializeCDA(PCDA cda);

/** Refresh all data for a controller.
 *
 * Unlike @ref SA_ReInitializeCDA, which only invalidates cached data (fast), this
 * function will re-send all commands and fully repopulate (and cache) all structures
 * and fields for a given CDA (slow).
 *
 * This allows clients to have an atomic way to update an entire configuration.
 *
 * @pre @p cda has already been initialized using @ref SA_InitializeCDA.
 * @post The CDA will have cached all known buffers/fields.
 * @post Any outstanding @ref storc_configuration_editable will be dropped and deleted.
 *
 * @param[in] cda Controller data area structure (@ref storc_cda).
 * @return Same as @ref SA_InitializeCDA.
 */
SA_WORD SA_ReenumerateCDA(PCDA cda);

/** UnInitialize an existing CDA.
 * @pre `cda` has already been initialized using @ref SA_InitializeCDA.
 * @param[in] cda  Previously initialized controller data area.
 * @return Same as @ref SA_InitializeCDA
 * @code
 * PCDA cda = (PCDA)SA_malloc(SA_GetCDASize());
 * SA_InitializeCDA(cda, device_descriptor);
 * ...
 * SA_UnInitializeCDA(cda);
 * SA_free(cda);
 * @endcode
 * @details
 * Uninitialize a list of controller data area structures from a list of [device handlers](@ref syscore_device_handler).
 * This operation requires that @ref SA_InitializeCDA has already been performed successfully.
 */
SA_WORD SA_UnInitializeCDA(PCDA cda);

#if defined(__cplusplus)
}
#endif

/** @} */ /* storc_cda */

/** @} */ /* storcore */

#endif /* PSTORC_CDA_H */
