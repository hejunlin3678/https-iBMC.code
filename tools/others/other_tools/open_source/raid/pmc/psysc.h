/** @file psysc.h
@copyright Copyright (c) 2021 Microchip Technology Inc. All Rights Reserved.

Description: sysCore header file

*/

#ifndef PSYSC_H
#define PSYSC_H

/* Platform-specific values */
#include "psysc_platform.h"
/* Default type declarations */
#include "psysc_types.h"
/* Definitions for command buffer types and command functions */
#include "psysc_commands.h"
/* Definitions for device discovery types and functions */
#include "psysc_discovery.h"
/* Definitions for functions and constants needed to get information from a device handle */
#include "psysc_info.h"
/* Definitions for debug and logging functions */
#include "psysc_debug.h"
/* Definitions for functions for threads */
#include "psysc_threads.h"
/* Definitions for functions for critical section protection */
#include "psysc_synchronization.h"
/* Definitions for functions for event flags */
#include "psysc_event_flags.h"
/* Definitions for inter process communication functions */
#include "psysc_ipc.h"
/* Definitions for syscore descriptors */
#include "psysc_descriptor.h"

/** @defgroup syscore sysCore
 * @brief
 * @details
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/** Initialize global objects of sysCore.
 * @attention Clients should invoke this before using any of the sysCore module API.
 * @deprecated Parameter `checksum` is not longer used.
 */
void InitializeSysCore(const char *checksum);
/** Uninitialize global objects of sysCore.
 * @attention Clients should invoke this just before the program exits.
 */
void UnInitializeSysCore(void);

#if defined(__cplusplus)
}    /* extern "C" { */
#endif

/** @} */

#endif /* PSYSC_H */
